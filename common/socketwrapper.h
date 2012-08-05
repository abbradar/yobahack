#ifndef SOCKETWRAPPER_H
#define SOCKETWRAPPER_H

#include <functional>
#include <exception>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

/** Class that wraps socket operations */
template <class Socket> class SocketWrapper {
 public:
  inline Socket &socket() noexcept {
    return socket_;
  }

 protected:
  explicit SocketWrapper(boost::asio::io_service &io_service) noexcept : socket_(io_service) {}

  inline void Disconnect() {
    boost::system::error_code e;
   socket_.close(e);
    if (e) throw std::runtime_error("Disconnection from socket failed: " + e.message());
  }

  template <class MutableBufferSequence, class ReadHandler> inline void ReadSome(const MutableBufferSequence &buffers,
   ReadHandler func) noexcept {
    socket_.async_read_some(buffers, func);
  }

  template <class ConstBufferSequence, class WriteHandler> inline void WriteSome(const ConstBufferSequence &buffers,
   WriteHandler func) noexcept {
    socket_.async_write_some(buffers, func);
  }

  template <class MutableBufferSequence, class ReadHandler> void Read(const MutableBufferSequence &buffers,
   ReadHandler func) noexcept {
    boost::asio::async_read(socket_, buffers, func);
  }

  template <class ConstBufferSequence, class WriteHandler> void Write(const ConstBufferSequence &buffers,
   WriteHandler func) noexcept {
    boost::asio::async_write(socket_, buffers, func);
  }

 private:
  Socket socket_;
};

#endif // SOCKETWRAPPER_H
