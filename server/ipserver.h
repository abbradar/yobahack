#ifndef YOBAHACK_SERVER_IPSERVER_H_
#define YOBAHACK_SERVER_IPSERVER_H_

#include <atomic>
#include <list>
#include <thread>
#include <exception>
#include <cstdint>
#include <memory>
#include <utility>
#include <functional>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "common/sharedlockwrapper.h"
#include "common/socketwrapper.h"
#include "common/debug.h"
#include "common/logging.h"

template <class Connection, class Protocol = boost::asio::ip::tcp>
 class IPServer;

/** Receives and processes one connection to server.
 * This is an abstract class. User is to inherit this and implement
 * processing calls.
 * You should use async operations and always have one read operation
 * queued. You should also handle socket disconnection, so that connection
 * is disposed.
 * \sa IPServer
 */
template <class Protocol, class Descedant>
 class IPConnection : public SocketWrapper<typename Protocol::socket> {
 public:
  typedef IPServer<Descedant, Protocol> ServerType;

  IPConnection(const IPConnection<Protocol, Descedant> &other) = delete;

  /** Disconnects client and queues connection for disposal.
   * Thread-safe.
   */
  void Free() noexcept {
    if (closing_.exchange(true)) return;
    PrepareDisconnect();
    try {
      Disconnect();
    } catch (const std::exception &e) {
      LogWarning(e.what());
    }
    // Destruction should be done ONLY after we handle all async ops callbacks
    // and from io_service thread.
    // God help you if you destruct this class when unhandled async ops
    // are present.
    socket().get_io_service().dispatch(std::bind(ServerType::CloseConnection, server_, this));
  }

  /** Returns true if connection disposal is pending */
  inline bool closing() const noexcept {
    return closing_;
  }

  inline ServerType *server() const noexcept {
    return server_;
  }

 protected:  
  explicit IPConnection(boost::asio::io_service &io_service, ServerType *server) noexcept :
    SocketWrapper<typename Protocol::socket>(io_service), server_(server), closing_(false) { }

  /** Called after connection is established.
   * You should start processing connection from there.
   **/
  virtual void HandleConnected() noexcept = 0;

  /** Called before forced disconnect (used by Free() method) **/
  virtual void PrepareDisconnect() noexcept { }

 private:
  friend class IPServer<Descedant, Protocol>;

  ServerType *server_;
  std::atomic_bool closing_;
};

/** Multi-threaded IP server based on thread pool.
 * \param Protocol expected to be boost::asio::ip tcp or udp
 * \param Connection expected to be class inherited from IPConnection
 * \sa IPConnection
 */
template <class Connection, class Protocol> class IPServer {
 public:
  typedef std::unique_ptr<Connection> ConnectionPointer;
  typedef std::list<ConnectionPointer> ConnectionList;

  explicit IPServer(const typename Protocol::endpoint &&endpoint) noexcept :
    acceptor_(io_service_, endpoint),
      connections_(new ConnectionList()) { }

  IPServer(const IPServer &other) = delete;

  /** Starts thread pool which processes I/O.
   * If pool is already started, does nothing.
   * Not thread-safe.
   */
  void StartService() noexcept {
    using namespace boost::asio;
    using namespace std;

    if (working_) return;
    // wait for threads to finish their previous work
    // see StopService() comments
    for (thread &thread : threads_) {
      thread.join();
    }
    threads_.clear();
    // this thing prevents io_service from going out from run() loop
    work_.reset(new io_service::work(io_service_));
    // spawn threads for io_service::run event loop
    for (int i = 0; i < threads_number_; ++i) {
      threads_.push_back(thread(bind((size_t(io_service::*)())&io_service::run, &io_service_)));
    }
    working_ = true;
  }

  /** Aborts all operations and sends thread pool signal to stop.
   * If pool is stopped, does nothing.
   * Not thread-safe.
   */
  void StopService() noexcept {
    if (!working_) return;
    // There is no way to receive callback when all threads are stopped.
    // One way to do this is to implement our own "run" function
    // which will execute io_service_.run() and then clean up thread
    // after itself.
    // Other way is to clean threads up on next StartService()
    // TODO (abbradar) Maybe later
    StopListening();
    DisconnectAll();
    work_.reset();
    working_ = false;
  }

  /** Returns true if we are accepting new connections */
  inline bool is_open() const noexcept {
    return acceptor_.is_open();
  }

  /** Return acceptor's local endpoint */
  inline typename Protocol::endpoint local_endpoint() const noexcept {
    return acceptor_.local_endpoint();
  }

  /** Sets now local endpoint for accepting new connections */
  void set_local_endpoint(const typename Protocol::endpoint &&endpoint) {
    boost::system::error_code ec;
    acceptor_.bind(endpoint, ec);
    if (ec) throw std::runtime_error(ec.message());
  }

  /** Starts thread pool if not started and starts accepting new connections.
   * If we are already accepting connections, does nothing.
   */
  void StartListening() noexcept {
    if (acceptor_.is_open()) return;
    StartService();
    acceptor_.listen();
    AcceptNext();
  }

  /** Stops listening for new connections.
   * If we are stopped already, does nothing.
   */
  void StopListening() noexcept {
    acceptor_.close();
  }

  /** Disconnects all clients from server.
   * If */
  void DisconnectAll() noexcept {
    // This can be source of deadlocks, exceptions,
    // nukes and other nasty things.
    // We try to lock connections list and send Free() to all connections
    // so they *should* be destroyed from another thread.
    // In fact we block all io_service threads while working here
    connections_.Lock();
    for (ConnectionPointer &connection : *connections_) {
      connection->Free();
    }
    connections_.Unlock();
  }

  /** Returns number of threads in the thread pool. */
  inline int threads_number() const noexcept {
    return threads_number_;
  }

  /** Sets number of threads in the thread pool.
   * If server is already working, throws exception.
   * Not thread-safe.
   */
  void set_threads_number(const int value) {
    if (working_) {
      throw std::runtime_error("Service is running");
    }
    if (value <= 0) {
      throw std::out_of_range("Number of threads should be > 0");
    }
    threads_number_ = value;
  }

  /** Close and dispose of connection by pointer */
  void CloseConnection(Connection *pointer) {
    connections_.Lock();
    connections_->remove_if([pointer](const typename Connection::Pointer &p) -> bool {
                              if (p == pointer) {
                                if (p->closing())
                                  return true;
                                else
                                  p->Free();
                              }
                              return false;
                            });
    connections_.Unlock();
  }

  /** Returns true if thread pool is working. */
  inline bool working() const noexcept {
    return working_;
  }

  /** Returns wrapper to connections vector */
  inline SharedLockWrapper<ConnectionList> &connections() noexcept {
    return connections_;
  }

 private:
  /** Creates new IPConnection and tries to receive next connection. */
  void AcceptNext() noexcept {
    auto pointer = std::unique_ptr<Connection>(new Connection(io_service_, this));
    // if connection is never received, this object will destruct on its own because of unique_ptr
    acceptor_.async_accept(pointer->socket(),
                           std::bind(&HandleConnected,this,
                                     std::move(pointer)));
  }

  /** Called when connection is established */
  void HandleConnected(ConnectionPointer &pointer, boost::system::error_code &&e) {
    if (!e) {
      // without errors? then push connection to list
      connections_.Lock();
      connections_->push_back(std::move(pointer));
      pointer->HandleConnected();
      connections_.Unlock();
      // receive next connection
      AcceptNext();
    } else {
      // TODO: should handle errors there
      AssertMsg(false, e.message());
    }
  }

  boost::asio::io_service io_service_;
  std::unique_ptr<boost::asio::io_service::work> work_;
  typename Protocol::acceptor acceptor_;
  std::list<std::thread> threads_;
  SharedLockWrapper<ConnectionList> connections_;
  int threads_number_ = 2;
  bool working_ = false;
};

#endif // YOBAHACK_SERVER_IPSERVER_H_
