#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <atomic>
#include <list>
#include <thread>
#include <exception>
#include <memory>
#include <functional>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "common/defs.h"
#include "common/lock_wrapper.h"

template <class T> class IPConnection
        : std::enable_shared_from_this<IPConnection>
{
public:
    typedef std::shared_ptr<IPConnection> pointer;
    typedef std::function<void, pointer> callback;

    IPConnection(callback &&connected_callback, callback &&closed_callback)
    {
        connected_callback_ = connected_callback;
        closed_callback_ = closed_callback;
    }

    // you should not close socket again in your destructor
    // destructor can be called in two cases:
    // 1. after graceful socket close while delete'ing object in callback
    // 2. while explicitly delete'ing all objects in server (during forceful stop)
    ~IPConnection()
    {
        closing_ = true;
        socket_.close();
    }

    IPConnection(const IPConnection &other) = delete;

    T::socket &socket()
    {
      return socket_;
    }

    void PreHandleConnected()
    {
        connected_callback_(pointer(this));
        HandleConnected();
    }

    virtual void GracefulStop() = 0;

    void Free()
    {
        if (closing_) return;
        closing_ = true;
        socket_.close();
        closed_callback_(pointer(this));
    }

protected:
    virtual void HandleConnected() = 0;

private:
    T::socket socket_;
    bool closing_;
    std::function<void, pointer> connected_callback_, closed_callback_;
};

template <class T> class IPServer
{
public:
    typedef std::list<IPConnection::pointer> connection_list;

    explicit IPServer(T::endpoint &&endpoint) :
        acceptor_(io_service, endpoint),
        connections_(new connection_list())
    {
    }

    IPServer(unsigned short port_num) :
        IPServer(io_service, T::endpoint(port_num))
    {
    }

    IPServer(const IPServer &other) = delete;

    ~IPServer()
    {
        delete connections_;
    }

    void Listen()
    {
        if (!working) {
            throw std::runtime_error("Server is already running");
        }

    }

    void Stop()
    {

    }

    void AsyncGracefulClose(void (*callback)())
    {
        if (!working_) {
            throw std::runtime_error("Server is not running");
        }
        if (stopping_) {
            throw std::runtime_error("Server is already stopping");
        }
        stop_callback_ = callback;
        stopping_ = true;
        acceptor_.close();
        while (threads_.size()) {
            threads_.back().join();
            threads_.pop_back();
        }
        working_ = false;
        stopping_ = false;
    }

    int threads_number()
    {
        return threads_number_;
    }

    void set_threads_number(int value)
    {
        if (working_) {
            throw std::runtime_error("Server is running");
        }
        if (value <= 0) {
            throw std::out_of_range("Number of threads should be > 0");
        }
        threads_number_ = value;
    }

    bool working() const
    {
        return working_;
    }

    bool stopping() const
    {
        return stopping_;
    }

    // this is sort of expensive getter, because it needs to be thread-safe
    SharedLockWrapper<boost::shared_lock, connection_list> connections()
    {
        return SharedLockWrapper(connections_mutex_, connections_);
    }

private:
    void HandleAccept()
    {

    }

    boost::asio::io_service io_service_;
    T::acceptor acceptor_;
    std::mutex stop_mutex_;
    std::list<std::thread> threads_;
    connection_list *connections_;
    int threads_number_ = 2;
    std::atomic_bool stopping_ = false, working_ = false;
    std::atomic<void (*)()> stop_callback_ = nullptr;
    boost::shared_mutex connections_mutex_;
};

#endif // TCPSERVER_HPP
