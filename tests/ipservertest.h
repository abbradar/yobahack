#ifndef YOBAHACK_TESTS_IPSERVERTEST_H_
#define YOBAHACK_TESTS_IPSERVERTEST_H_

#include <atomic>
#include <array>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <gtest/gtest.h>
#include "common/defs.h"
#include "server/ipserver.h"

class TestedIPServer;

class TestedIPConnection : public IPConnection<TestedIPServer> {
 private:
  const size_t kBufferSize = 1024;
  explicit IPConnection(Server *server) noexcept :
    IPConnection<TestedIPServer>(server) {}

  virtual void HandleConnected();

  virtual void PrepareDisconnect() noexcept;

  void HandleRead(const boost::system::error_code &error, std::size_t bytes_transferred);

  void HandleWrite(const boost::system::error_code &error, std::size_t bytes_transferred);

  std::array<kBufferSize> buffer_;
};

typedef IPServer<TestedIPConnection, GameProtocol> TestedIPServer;

class IPServerTest : public testing::Test {
 public:
  typedef IPServer<GameProtocol, TestedIPConnection> TestedIPServer;

 protected:
  TestedIPServer server_;
};

#endif // YOBAHACK_TESTS_IPSERVERTEST_H_
