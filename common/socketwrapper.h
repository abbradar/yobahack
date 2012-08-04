#ifndef SOCKETWRAPPER_H
#define SOCKETWRAPPER_H

#include <functional>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

/** Abstract class that wraps socket operations */
template <class Socket> class SocketWrapper {
 public:
  inline Socket &socket() noexcept {
    return socket_;
  }

 protected:
  SocketWrapper() = default;

  inline void Disconnect() {
    socket_.close();
  }

  template <class MutableBufferSequence> inline void ReadSome(const MutableBufferSequence &buffers,
   std::function<void(const boost::system::error_code &, std::size_t)> func) {
    socket_.async_read_some(buffers, func);
  }

  template <class ConstBufferSequence> inline void WriteSome(const ConstBufferSequence &buffers,
   std::function<void(const boost::system::error_code &, std::size_t)> func) {
    socket_.async_write_some(buffers, func);
  }

  template <class MutableBufferSequence> void Read(const MutableBufferSequence &buffers, const size_t size,
   std::function<void(const boost::system::error_code &, std::size_t)> func) {
    boost::asio::async_read(socket_, buffers, func);
  }

  template <class ConstBufferSequence> void Write(const ConstBufferSequence &buffers, const size_t size,
   std::function<void(const boost::system::error_code &, std::size_t)> func) {
    boost::asio::async_write(socket_, buffers, func);
  }

 private:
  Socket socket_;
};

#endif // SOCKETWRAPPER_H
