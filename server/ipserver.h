#ifndef IPSERVER_HPP
#define IPSERVER_HPP

#include <atomic>
#include <list>
#include <thread>
#include <exception>
#include <memory>
#include <utility>
#include <functional>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "common/defs.h"
#include "common/lock_wrapper.h"

template <class T> class IPConnection
        : std::enable_shared_from_this<IPConnection<T> >
{
public:
    typedef std::shared_ptr<IPConnection<T> > Pointer;
    typedef std::function<void(Pointer)> Callback;

    IPConnection(Callback &&closed_callback) noexcept
    {
        closed_callback_ = closed_callback;
    }

    IPConnection(const IPConnection<T> &other) = delete;

    typename T::socket &socket() noexcept
    {
      return socket_;
    }

    virtual void HandleConnected() = 0;

    void Free() noexcept
    {
        if (closing_.exchange(true)) return;
        socket_.close();
        // destruction should be done ONLY after we handle all async ops callbacks
        // and from io_service thread.
        // God help you if you destruct this class when unhandled async ops
        // are present.
        socket_.get_io_service().dispatch(std::bind(closed_callback_, pointer(this)));
    }

private:
    typename T::socket socket_;
    std::atomic_bool closing_ = false;
    Callback closed_callback_;
};

template <class T> class IPServer
{
public:
    typedef IPConnection<T> Connection;
    typedef std::list<Connection::Pointer> ConnectionList;

    explicit IPServer(typename T::endpoint &&endpoint) noexcept :
        acceptor_(io_service_, endpoint),
        connections_(new ConnectionList())
    {
    }

    IPServer(unsigned short port_num) noexcept :
        IPServer(T::endpoint(port_num))
    {
    }

    IPServer(const IPServer &other) = delete;

    void StartService()
    {
        if (working_) {
            throw std::runtime_error("Service is already running");
        }
        // wait for threads to finish their previous work
        // see StopService() comments
        for (std::thread &thread : threads_) {
            thread.join();
        }
        threads_.clear();
        work_.reset(new boost::asio::io_service::work(io_service_));
        for (int i = 0; i < threads_number; ++i) {
            std::thread thread(std::bind(&io_service_::run, &io_service_));
            threads_.push_back(std::move(thread));
        }
    }

    void StopService()
    {
        if (!working_) {
            throw std::runtime_error("Service is not running");
        }
        // there is no way to receive callback when all threads are stopped
        // one way to do this is to implement own "run" function
        // which will execute io_service_.run() and then clean up thread
        // after itself
        // maybe later
        work_.reset();
        io_service_.stop();
    }

    bool is_open() noexcept
    {
        return acceptor_.is_open();
    }


    void StartListening()
    {
        if (!working) {
            StartService();
        }
        acceptor_.listen();
        AcceptNext();
    }

    void StopListening() noexcept
    {
        acceptor_.close();
    }

    void DisconnectAll()
    {
        // This can be source of deadlocks, exceptions,
        // nukes and other nasty things.
        boost::shared_lock lock(connections_mutex_);
        for (Connection::Pointer &connection : *connections_) {
            connection->Free();
        }
    }

    int threads_number() noexcept
    {
        return threads_number_;
    }

    void set_threads_number(int value)
    {
        if (working_) {
            throw std::runtime_error("Service is running");
        }
        if (value <= 0) {
            throw std::out_of_range("Number of threads should be > 0");
        }
        threads_number_ = value;
    }

    bool working() const noexcept
    {
        return working_;
    }

    bool stopping() const noexcept
    {
        return stopping_;
    }

    // this is sort of expensive getter, because it needs to be thread-safe
    // so it wraps ConnectionList into "mutex lock wrapper"
    SharedLockWrapper<boost::shared_lock, ConnectionList> connections() noexcept
    {
        return SharedLockWrapper(connections_mutex_, connections_);
    }

private:
    void AcceptNext() {
        auto pointer = std::make_shared<IPConnection>(std::bind(&ClosedCallback, this));
        acceptor_.async_accept(pointer->socket(),
                               std::bind(&ConnectedCallback, pointer));
    }

    void ConnectedCallback(Connection::Pointer &&pointer, boost::system::error_code &&e) {
        if (!e) {
            boost::unique_lock lock(connections_mutex_);
            connections_->push_back(pointer);
        }
        AcceptNext();
    }

    void ClosedCallback(Connection::Pointer &&pointer) {
        boost::unique_lock lock(connections_mutex_);
        connections_->remove(pointer);
    }

    boost::asio::io_service io_service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    typename T::acceptor acceptor_;
    std::list<std::thread> threads_;
    std::unique_ptr<ConnectionList> connections_;
    boost::shared_mutex connections_mutex_;
    int threads_number_ = 2;
    bool working_ = false;
};

typedef IPServer<boost::asio::ip::tcp> TCPServer;
typedef IPServer<boost::asio::ip::udp> UDPServer;

#endif // IPSERVER_HPP
