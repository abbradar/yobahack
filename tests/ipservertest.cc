#include <string>
#include <array>
#include <chrono>
#include <boost/asio.hpp>
#include "ipservertest.h"

using namespace std;
using namespace std::chrono;

void TestedIPConnection::HandleConnected()
{
  ReadSome(boost::asio::buffer(buffer_), HandleRead);
}

void TestedIPConnection::PrepareDisconnect()
{
}

void TestedIPConnection::HandleRead(const boost::system::error_code &error, std::size_t bytes_transferred)
{
  if (buffer_[bytes_transferred - 1] == '\n') stop_ = true;
  Write(boost::asio::buffer(buffer_, bytes_transferred), HandleWrite);
}

void TestedIPConnection::HandleWrite(const boost::system::error_code &error, std::size_t bytes_transferred)
{
  if (stop_) Disconnect();
  else ReadSome(boost::asio::buffer(buffer_), HandleRead);
}

TEST_F(IPServerTest, OneConnection) {
  boost::asio::io_service io_service;
  GameProtocol::socket socket(io_service);
  GameProtocol::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"), kGamePort);
  string str = "TEST";
  float timeout = 0.1;
  server_.StartListening();
  ASSERT_EQ(boost::asio::connect(socket, endpoint), endpoint);
  boost::system::error_code error;
  boost::asio::write(socket, boost::asio::buffer(str), error);
  ASSERT_FALSE(error);
  
  string buff;
  buff.reserve(str.size());
  steady_clock::time_point start_p = steady_clock::now();
  while (buff.size() < str.size()) {
    steady_clock::time_point cur_p = steady_clock::now();
    duration<float> time_span = duration_cast<duration<float>>(cur_p - start_p);
    EXPECT_LT(time_span.count(), timeout);
    ASSERT_LT(time_span.count(), timeout * 10);
    string curr;
    curr.reserve(curr.size());
    socket_.read_some(boost::asio::buffer(curr), error);
    ASSERT_FALSE(error);
    buff += curr;
  }
  ASSERT_EQ(buff, str);
  boost::asio::write(socket, boost::asio::buffer("\n", 1), error);
  ASSERT_FALSE(error);
  start_p = steady_clock::now();
  while (!error) {
    steady_clock::time_point cur_p = steady_clock::now();
    duration<float> time_span = duration_cast<duration<float>>(cur_p - start_p);
    EXPECT_LT(time_span.count(), timeout);
    ASSERT_LT(time_span.count(), timeout * 10);
    socket_.read_some(boost::asio::buffer(buff), error);
  }
  ASSERT_EQ(error, boost::asio::error::eof);
}

