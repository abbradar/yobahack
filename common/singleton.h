#ifndef YOBAHACK_COMMON_SINGLETON_H_
#define YOBAHACK_COMMON_SINGLETON_H_

/** Basic class for implementing singletons.
 * You should inherit it from your class and friend it.
 * Use private constructor and remove copy/move contructors from your derived class.
 */
template <class Derived> class Singleton {
 public:
  Singleton(const Singleton &other) = delete;
  Singleton(const Singleton &&other) = delete;

  /** Returns single instance of Logger */
  static inline Derived &instance() noexcept {
    static Derived singleton;
    return singleton;
  }

 protected:
  Singleton() = default;
};

#endif // YOBAHACK_COMMON_SINGLETON_H_
