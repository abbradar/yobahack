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

// this works with one connection and is expected to work
// 1) asynchronously
// 2) called from io_service threads
// 3) Free() can be called from any thread
// User is expected to inherit this and implement its own connection handler
template <class T> class IPConnection
    : std::enable_shared_from_this<IPConnection<T>> {
 public:
  typedef std::shared_ptr<IPConnection<T>> Pointer;
  typedef std::function<void(Pointer)> Callback;

  explicit IPConnection(Callback &&closed_callback) noexcept {
    closed_callback_ = closed_callback;
  }

  IPConnection(const IPConnection<T> &other) = delete;

  typename T::socket &socket() noexcept {
    return socket_;
  }

  virtual void HandleConnected() = 0;

  void Free() noexcept {
    if (closing_.exchange(true)) return;
    socket_.close();
    // destruction should be done ONLY after we handle all async ops callbacks
    // and from io_service thread.
    // God help you if you destruct this class when unhandled async ops
    // are present.
    socket_.get_io_service().dispatch(std::bind(closed_callback_, pointer(this)));
  }

  bool closing() noexcept {
    return closing;
  }

 private:
  typename T::socket socket_;
  std::atomic_bool closing_ = false;
  Callback closed_callback_;
};

// multi-threaded TCP/UDP server based on thread pool
// Connection: class inherited from IPConnection
// Transport: expected to be boost::asio::ip::tcp or udp
template <class Transport, class Connection> class IPServer {
 public:
  typedef std::list<typename Connection::Pointer> ConnectionList;

  explicit IPServer(typename Transport::endpoint &&endpoint) noexcept :
    acceptor_(io_service_, endpoint),
      connections_(new ConnectionList()) { }

  IPServer(unsigned short port_num) noexcept :
      IPServer(Transport::endpoint(port_num)) { }

  IPServer(const IPServer &other) = delete;

  void StartService() {
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

  void StopService() {
    if (!working_) return;
    // There is no way to receive callback when all threads are stopped.
    // One way to do this is to implement own "run" function
    // which will execute io_service_.run() and then clean up thread
    // after itself.
    // Other way is to clean threads up on next StartService()
    // TODO (abbradar) Maybe later
    work_.reset();
    io_service_.stop();
    working_ = false;
  }

  bool is_open() noexcept {
    return acceptor_.is_open();
  }


  void StartListening() {
    StartService();
    acceptor_.listen();
    AcceptNext();
  }

  void StopListening() noexcept {
    acceptor_.close();
  }

  void DisconnectAll() {
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

  int threads_number() noexcept {
    return threads_number_;
  }

  void set_threads_number(int value) {
    if (working_) {
      throw std::runtime_error("Service is running");
    }
    if (value <= 0) {
      throw std::out_of_range("Number of threads should be > 0");
    }
    threads_number_ = value;
  }

  bool working() const noexcept {
    return working_;
  }

  // this is sort of expensive getter, because it needs to be thread-safe
  // so it wraps ConnectionList into "mutex lock wrapper"
  SharedLockWrapper<boost::shared_mutex, ConnectionList> connections() noexcept {
    return SharedLockWrapper<boost::shared_mutex, ConnectionList>(connections_mutex_, connections_);
  }

 private:
  // this creates new IPConnection and tries to receive next connection
  void AcceptNext() {
    // if connection is never received, this object will destruct on its own because of shared_ptr
    auto pointer = std::make_shared<IPConnection>(std::bind(&ClosedCallback, this));
    acceptor_.async_accept(pointer->socket(),
                           std::bind(&ConnectedCallback, this, pointer));
  }

  // this is called when connection is established
  void ConnectedCallback(typename Connection::Pointer &&pointer, boost::system::error_code &&e) {
    if (!e) {
      // without errors? then push connection to list
      boost::unique_lock<boost::shared_mutex> lock(connections_mutex_);
      connections_->push_back(pointer);
    }
    // TODO: should handle errors there
    // receive next connection
    AcceptNext();
  }

  // this is called from io_service threads from connection itself
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
