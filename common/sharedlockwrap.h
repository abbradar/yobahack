#ifndef YOBAHACK_COMMON_LOCK_WRAPPER_H_
#define YOBAHACK_COMMON_LOCK_WRAPPER_H_

#include <memory>
#include <boost/thread.hpp>
#include "common/debug.h"

/** Wraps around given thread-unsafe object and provides shared access between threads.
 If DEBUG_LEVEL is greater or equal 4, checks ownership of lock on each access. */
template <class Wrapped> class SharedLockWrap
{
 public:
  typedef std::unique_ptr<Wrapped> Pointer;

  SharedLockWrap() = default;

  explicit SharedLockWrap(Pointer &&wrapped) noexcept
    : wrapped_(wrapped) {}

  SharedLockWrap(const SharedLockWrap &other) = delete;
  SharedLockWrap(const SharedLockWrap &&other) = delete;

  inline const Pointer &shared() const noexcept {
#if DEBUG_LEVEL >= 4
    AssertMsg(GetLockState() < kShared, "Shared ownership is not acquired.");
#endif
    return wrapped_;
  }

  inline Pointer &exclusive() noexcept {
#if DEBUG_LEVEL >= 4
    AssertMsg(GetLockState() < kLocked, "Exclusive ownership is not acquired.");
#endif
    return wrapped_;
  }

  inline Pointer operator ->() const noexcept {
    return shared();
  }

  inline Wrapped &operator *() const noexcept {
    return *(shared().get());
  }

  inline void LockShared() {
#if DEBUG_LEVEL >= 4
    AssertMsg(GetLockState() != kUnlocked, "Lock is already acquired.");
#endif
    mutex_.lock_shared();
#if DEBUG_LEVEL >= 4
    *lock_state_.get() = kLocked;
#endif
  }

  inline void Lock() {
#if DEBUG_LEVEL >= 4
    AssertMsg(GetLockState() != kUnlocked, "Lock is already acquired.");
#endif
    mutex_.lock();
#if DEBUG_LEVEL >= 4
    *lock_state_.get() = kLocked;
#endif
  }

  inline bool TryLock() {
    bool result = mutex_.try_lock();
#if DEBUG_LEVEL >= 4
    if (result) {
      SetLockState(kLocked);
    }
#endif
    return result;
  }

  inline bool TryLockShared() {
    bool result = mutex_.try_lock_shared();
#if DEBUG_LEVEL >= 4
    if (result) {
      SetLockState(kShared);
    }
#endif
    return result;
  }

  inline bool TimedLock(const boost::system_time &abs_time) {
    bool result = mutex_.timed_lock(abs_time);
#if DEBUG_LEVEL >= 4
    if (result) {
      SetLockState(kLocked);
    }
#endif
    return result;
  }

  inline bool TimedLockShared(const boost::system_time &abs_time) {
    bool result = mutex_.timed_lock_shared(abs_time);
#if DEBUG_LEVEL >= 4
    if (result) {
      SetLockState(kShared);
    }
#endif
    return result;
  }

  inline void Unlock() {
#if DEBUG_LEVEL >= 4
    AssertMsg(GetLockState() != kLocked, "Lock is not acquired or of other type.");
#endif
    mutex_.unlock();
#if DEBUG_LEVEL >= 4
    *lock_state_.get() = kUnlocked;
#endif
  }

  inline void UnlockShared() {
#if DEBUG_LEVEL >= 4
    AssertMsg(GetLockState() != kShared, "Lock is not acquired or of other type.");
#endif
    mutex_.unlock_shared();
#if DEBUG_LEVEL >= 4
    *lock_state_.get() = kUnlocked;
#endif
  }

 private:
#if DEBUG_LEVEL >= 4
  enum LockState {
    kUnlocked = 0, kShared = 1, kLocked = 2,
  };
#endif

  std::unique_ptr<Wrapped> wrapped_ = nullptr;
  boost::shared_mutex mutex_;
#if DEBUG_LEVEL >= 4
  boost::thread_specific_ptr<LockState> lock_state_;
#endif

#if DEBUG_LEVEL >= 4
  LockState GetLockState() noexcept {
    if (!lock_state_.get()) {
      lock_state_.reset(new LockState(kUnlocked));
    }
    return *lock_state_.get();
  }

  void SetLockState(const LockState &lock_state) noexcept {
    if (!lock_state_.get()) {
      lock_state_.reset(new LockState(lock_state));
    } else {
      *lock_state_.get() = lock_state;
    }
  }
#endif
};

#endif // YOBAHACK_COMMON_LOCK_WRAPPER_H_
