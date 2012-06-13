#ifndef LOCK_WRAPPER_H
#define LOCK_WRAPPER_H

#include <boost/thread.hpp>

template <class Lock, class Lockable, class Wrapped> class LockWrapper
{
    public:
    explicit SharedWrapper(Lockable &lockable, Wrapped *wrapped)
        : lock_(lockable),
          wrapped_(wrapped)
    {
    }

    const Wrapped operator*()
    {
        return *wrapped_;
    }

    private:
    Wrapped *wrapped_;
    Lock lock_;
};

template <class Lockable, class Wrapped> typedef LockWrapper<boost::shared_lock, Lockable, const Wrapped> SharedLockWrapper;
template <class Lockable, class Wrapped> typedef LockWrapper<boost::unique_lock, Lockable, Wrapped> UniqueLockWrapper;

#endif // LOCK_WRAPPER_H
