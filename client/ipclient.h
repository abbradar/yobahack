#ifndef YOBAHACK_CLIENT_IPCLIENT_H_
#define YOBAHACK_CLIENT_IPCLIENT_H_

#include <thread>
#include <memory>
#include <boost/asio.hpp>

// asynchronous IP socket client
// needs to be inherited with some virtual methods defined
// works in the other thread
template <class Transport> class IPClient
{
public:
  IPClient();

  void connect(typename Transport::endpoint &&endpoint) {
    if (connected()) disconnect();
    boost::system::error_code error;
    //socket.connect()
  }

  bool connected() const noexcept {
    return !service_.stopped();
  }

private:
  boost::asio::io_service service_;
  std::unique_ptr<typename Transport::socket> socket_;
  std::unique_ptr<std::thread> thread_;
};

#endif // YOBAHACK_CLIENT_IPCLIENT_H_
