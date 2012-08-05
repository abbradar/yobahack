#ifndef YOBAHACK_TESTS_IPSERVERTEST_H_
#define YOBAHACK_TESTS_IPSERVERTEST_H_

#include <atomic>
#include <array>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <gtest/gtest.h>
#include "common/defs.h"
#include "server/ipserver.h"

class TestedIPConnection : public IPConnection<TestedIPConnection, GameProtocol> {
 private:
  static const size_t kBufferSize = 1024;

  explicit TestedIPConnection(boost::asio::io_service &io_service, ServerType *server) noexcept :
    IPConnection<TestedIPConnection, GameProtocol>(io_service, server), stop_(false) {}

  virtual void HandleConnected() noexcept;

  virtual void PrepareDisconnect() noexcept;

  void HandleRead(const boost::system::error_code &error, std::size_t bytes_transferred) noexcept;

  void HandleWrite(const boost::system::error_code &error, std::size_t bytes_transferred) noexcept;

  std::array<char, kBufferSize> buffer_;
  bool stop_;

  friend class IPServer<TestedIPConnection, GameProtocol>;
};

class IPServerTest : public testing::Test {
 public:
  typedef IPServer<TestedIPConnection, GameProtocol> TestedIPServer;

  IPServerTest() : server_(typename GameProtocol::endpoint(GameProtocol::v4(), kGamePort)) { }

 protected:
  TestedIPServer server_;
};

#endif // YOBAHACK_TESTS_IPSERVERTEST_H_
