#ifndef YOBAHACK_CLIENT_IPCLIENT_H_
#define YOBAHACK_CLIENT_IPCLIENT_H_

#include <thread>
#include <memory>
#include <boost/asio.hpp>
#include "common/socketwrapper.h"

/** Asynchronous IP socket client.
 * Needs to be inherited with some virtual methods defined.
 * Works in the other thread, some methods are not thread-safe.
 */
template <class Protocol> class IPClient :
    public SocketWrapper<typename Protocol::socket>
{
public:
  IPClient();

  /** Starts asynchronous connection to endpoint.
   * If we are already connected, closes existing connection.
   * Not thread-safe.
   */
  void Connect(typename Protocol::endpoint &&endpoint) {
    using namespace std;

    if (socket()) disconnect();
    if (thread_) {
      thread_->join();
    }
    socket().reset(new typename Protocol::socket(io_service_));
    boost::system::error_code error;
    socket()->async_connect(endpoint, std::bind(&HandlePreConnected, this));
    thread_.reset(new thread(bind((size_t(io_service::*)())&io_service::run, &io_service_)));
  }

  /** Starts asynchronous disconnection.
   * Does nothing if we are not connected.
   * Not thread-safe.
   */
  void Disconnect() {
    // Same problems as in IPServer; we cannot receive any callback
    // when io_service will really stop working (to clean up)
    // and I'm too lazy to write my own Run()
    // TODO(abbradar) maybe later
    if (!socket_) return;
    PrepareDisconnect();
    socket()->close();
  }

  inline bool connected() const noexcept {
    if (socket()) {
      return socket().is_open();
    } else {
      return false;
    }
  }

protected:
  virtual void HandleConnected() = 0;
  virtual void PrepareDisconnect() {};
  virtual void HandleError(boost::system::error_code &e) {};

private:
  void HandlePreConnected(boost::system::error_code &e) {
    if (!e) {
      HandleConnected();
    } else {
      HandleError(e);
    }
  }

  boost::asio::io_service io_service_;
  std::unique_ptr<std::thread> thread_;
};

#endif // YOBAHACK_CLIENT_IPCLIENT_H_
