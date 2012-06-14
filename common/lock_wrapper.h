#ifndef YOBAHACK_COMMON_LOCK_WRAPPER_H_
#define YOBAHACK_COMMON_LOCK_WRAPPER_H_

// this class locks given Lockable with Lock until destructed and gives access to Wrapped
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

#endif // YOBAHACK_COMMON_LOCK_WRAPPER_H_
