#ifndef YOBAHACK_COMMON_APPLICATION_H_
#define YOBAHACK_COMMON_APPLICATION_H_

#include <memory>
#include "common/singleton.h"

/** Base class for runnable.
 * User should inherit this to create his own runnable which can be assigned to Application.
 * \sa Application
 */
class Runnable {
 public:
  virtual void Run() = 0;
  virtual void Terminate(int error_code) = 0;
};

/** Handles running application.
 * Singleton which is used to perform some global-scale calls (like graceful stopping) from any place
 * of application, maintain application state, start it and so on.
 * Expected to be created in main(), associated with Runnable, and started.
 * Not thread-safe.
 * \sa Runnable
 */
class Application : public Singleton<Application> {
 public:
  Application(const Application &other) = delete;
  Application(const Application &&other) = delete;

  void Run() noexcept;

  void Terminate(int error_code) noexcept;

  void Abort() noexcept;

  /** Returns reference to currently associated application */
  inline Runnable &runnable() noexcept {
    return *runnable_;
  }

  /** Sets runnable to run.
   * Will generate exception if called after existing runnable is run.
   */
  void set_runnable(Runnable *runnable);

 private:
  friend class Singleton<Application>;

  Application() = default;
  ~Application() = default;

  std::unique_ptr<Runnable> runnable_;
  bool running_;
};

#endif // YOBAHACK_COMMON_APPLICATION_H_
