#ifndef SOCKETWRAPPER_H
#define SOCKETWRAPPER_H

#include <functional>
#include <boost/system/error_code.hpp>

/** Abstract class that wraps socket operations and adds guaranteed R/W */
template <class Socket> class SocketWrapper {
 public:
  inline typename Socket &socket() noexcept {
    return socket_;
  }
 protected:
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
    // TODO(abbradar) make static_assert for MutableBufferSequence
    if (size == 0) {
      // we has to insure that callback will be called later
      socket_.get_io_service().dispatch(std::bind(&HandleRead, this, boost::system::error_code(), 0));
    } else {
      HandleHalfRead(&*buffers.begin(), size, size, func, boost::system::error_code(), 0);
    }
  }

  template <class ConstBufferSequence> void Write(const ConstBufferSequence &buffers, const size_t size,
   std::function<void(const boost::system::error_code &, std::size_t)> func) {
    if (size == 0) {
      socket_.get_io_service().dispatch(std::bind(&HandleWrite, this, boost::system::error_code(), 0));
    } else {
      HandleHalfWrite(&*buffers.begin(), size, size, func, boost::system::error_code(), 0);
    }
  }

 private:
  void HandleHalfRead(const char *begin, std::size_t total, std::size_t remained,
   std::function<void(const boost::system::error_code &, std::size_t)> func,
   const boost::system::error_code &error, std::size_t bytes_transferred) {
    // I used a very dirty hack there:
    // BufferSequence must be a consistent sequence of POD, so I can use pointer magic.
    remained -= bytes_transferred;
    if (error || !remained) {
      func(error, bytes, total - remained);
      return;
    }
    begin += bytes_transferred;
    socket_.async_read_some(boost::asio::buffer(begin, remained),
                            std::bind(&HandleHalfRead, this, begin, total, remained, func));
  }

  void HandleHalfWrite(const char *begin, std::size_t total, std::size_t remained,
   std::function<void(const boost::system::error_code &, std::size_t)> func,
   const boost::system::error_code &error, std::size_t bytes_transferred) {
    remained -= bytes_transferred;
    if (error || !remained) {
      func(error, bytes, total - remained);
      return;
    }
    begin += bytes_transferred;
    socket_.async_write_some(boost::asio::buffer(begin, remained),
                            std::bind(&HandleHalfWrite, this, begin, total, remained, func));
  }

  Socket socket_;
};

#endif // SOCKETWRAPPER_H
