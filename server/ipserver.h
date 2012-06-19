#ifndef YOBAHACK_SERVER_IPSERVER_H_
#define YOBAHACK_SERVER_IPSERVER_H_

#include <atomic>
#include <list>
#include <thread>
#include <exception>
#include <memory>
#include <utility>
#include <functional>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "common/lock_wrapper.h"
#include "common/debug.h"

/** Receives and processes one connection to server.
 * This is an abstract class. User is to inherit this and implement
 * processing calls.
 * You should use async operations and always have one read operation
 * queued. You should also handle socket disconnection, so that connection
 * is disposed.
 * \sa IPServer
 */
template <class Transport> class IPConnection
    : std::enable_shared_from_this<IPConnection<Transport>> {
 public:
  IPConnection(const IPConnection<Transport> &other) = delete;

  /** Connection socket */
  inline typename Transport::socket &socket() noexcept {
    return socket_;
  }

  inline const typename Transport::socket &socket() const noexcept {
    return socket_;
  }

  /** Disconnects client and queues connection for disposal.
   * Thread-safe.
   */
  void Free() noexcept {
    if (closing_.exchange(true)) return;
    PrepareDisconnect();
    socket_.close();
    // Destruction should be done ONLY after we handle all async ops callbacks
    // and from io_service thread.
    // God help you if you destruct this class when unhandled async ops
    // are present.
    socket_.get_io_service().dispatch(std::bind(closed_callback_, Pointer(this)));
  }

  /** Returns true if connection disposal is pending */
  inline bool closing() const noexcept {
    return closing_;
  }

 protected:
  /** Called after connection is established.
   * You should start processing connection from there.
   **/
  virtual void HandleConnected() noexcept = 0;

  /** Called before forced disconnect (used by Free() method) **/
  virtual void PrepareDisconnect() noexcept { }

 private:
  typedef std::shared_ptr<IPConnection<Transport>> Pointer;
  typedef std::function<void(Pointer)> Callback;

  template <class Transport, class Connection> friend class IPServer;

  explicit IPConnection(Callback &&closed_callback) noexcept :
    closed_callback(closed_callback_) { }

  typename Transport::socket socket_;
  std::atomic_bool closing_ = false;
  Callback closed_callback_;
};

// multi-threaded TCP/UDP server based on thread pool
// Connection: class inherited from IPConnection
// Transport: expected to be boost::asio::ip::tcp or udp
/** Multi-threaded IP server based on thread pool.
 * \param Transport expected to be boost::asio::ip tcp or udp
 * \param Connection expected to be class inherited from IPConnection
 * \sa IPConnection
 */
template <class Transport, class Connection> class IPServer {
 public:
  typedef std::list<typename Connection::Pointer> ConnectionList;

  explicit IPServer(typename Transport::endpoint &&endpoint) noexcept :
    acceptor_(io_service_, endpoint),
      connections_(new ConnectionList()) { }

  explicit IPServer(unsigned short port_num) noexcept :
      IPServer(Transport::endpoint(port_num)) { }

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
    for (int i = 0; i < threads_number; ++i) {
      thread thread(bind((size_t(io_service::*)())&io_service::run, &io_service_));
      threads_.push_back(move(thread));
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
  inline typename Transport::endpoint local_endpoint() const noexcept {
    return acceptor_.local_endpoint();
  }

  /** Sets now local endpoint for accepting new connections */
  void set_local_endpoint(const Transport::endpoint &&endpoint) {
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
    boost::shared_lock<boost::shared_mutex> lock(connections_mutex_);
    for (typename Connection::Pointer &connection : *connections_) {
      connection->Free();
    }
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

  /** Returns true if thread pool is working. */
  inline bool working() const noexcept {
    return working_;
  }

  /** Returns wrapper to connections vector which locks it for reading while not destructed. */
  SharedLockWrapper<boost::shared_mutex, ConnectionList> connections() noexcept {
    // Returning non-const vector is not a good idea, user can accidentialy modify it which will
    // break things.
    // TODO: Consider another way.
    return SharedLockWrapper<boost::shared_mutex, ConnectionList>(connections_mutex_, connections_);
  }

 private:
  /** Creates new IPConnection and tries to receive next connection. */
  void AcceptNext() noexcept {
    // if connection is never received, this object will destruct on its own because of shared_ptr
    auto pointer = std::make_shared<IPConnection>(std::bind(&ClosedCallback, this));
    acceptor_.async_accept(pointer->socket(),
                           std::bind(&ConnectedCallback, this, pointer));
  }

  /** Called when connection is established */
  void ConnectedCallback(typename Connection::Pointer &&pointer, boost::system::error_code &&e) {
    if (!e) {
      // without errors? then push connection to list
      boost::unique_lock<boost::shared_mutex> lock(connections_mutex_);
      connections_->push_back(pointer);
      pointer->HandleConnected();
      // receive next connection
      AcceptNext();
    } else {
      // TODO: should handle errors there
      ASSERT(false, e.message());
    }
  }

  /** Called from io_service threads from connection itself */
  void ClosedCallback(typename Connection::Pointer &&pointer) {
    boost::unique_lock<boost::shared_mutex> lock(connections_mutex_);
    connections_->remove(pointer);
  }

  boost::asio::io_service io_service_;
  std::unique_ptr<boost::asio::io_service::work> work_;
  typename Transport::acceptor acceptor_;
  std::list<std::thread> threads_;
  std::unique_ptr<ConnectionList> connections_;
  boost::shared_mutex connections_mutex_;
  int threads_number_ = 2;
  bool working_ = false;
};

template <class Connection> using TCPServer = IPServer<boost::asio::ip::tcp, Connection>;
template <class Connection> using UDPServer = IPServer<boost::asio::ip::udp, Connection>;

#endif // YOBAHACK_SERVER_IPSERVER_H_
