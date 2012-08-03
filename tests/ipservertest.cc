#include "ipservertest.h"

using namespace std;

void TestedIPConnection::HandleConnected()
{
  ReadSome(buffer_, HandleRead);
}

void TestedIPConnection::PrepareDisconnect()
{
}

void TestedIPConnection::HandleRead(const boost::system::error_code &error, std::size_t bytes_transferred)
{
  Write(buffer_, HandleWrite);
}

void TestedIPConnection::HandleWrite(const boost::system::error_code &error, std::size_t bytes_transferred)
{
  ReadSome(buffer_, HandleRead);
}

TEST_F(IPServerTest, OneConnection) {
  
}
