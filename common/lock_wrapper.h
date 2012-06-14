#ifndef LOCK_WRAPPER_H
#define LOCK_WRAPPER_H

#include <boost/thread.hpp>

template <class Lock, class Lockable, class Wrapped> class LockWrapper
{
    public:
    explicit LockWrapper(Lockable &lockable, Wrapped *wrapped) noexcept
        : lock_(lockable),
          wrapped_(wrapped)
    {
    }

    LockWrapper(const LockWrapper &other) = delete;

    Wrapped operator*() noexcept
    {
        return *wrapped_;
    }

    Wrapped operator->() noexcept
    {
        return *wrapped_;
    }

    private:
    Wrapped *wrapped_;
    Lock lock_;
};

template <class Lockable, class Wrapped> using SharedLockWrapper = LockWrapper<boost::shared_lock, Lockable, const Wrapped>;
template <class Lockable, class Wrapped> using UniqueLockWrapper = LockWrapper<boost::unique_lock, Lockable, Wrapped>;

#endif // LOCK_WRAPPER_H
