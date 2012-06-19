#ifndef YOBAHACK_COMMON_APPLICATION_H_
#define YOBAHACK_COMMON_APPLICATION_H_

#include <memory>
#include "common/singleton.h"

class Application;

/** Base class for runnable.
 * User should derive this to create his own runnable which can be assigned to Application.
 * \sa Application
 */
class Runnable {
 public:
  Runnable() = default;
  Runnable(const Runnable &other) = delete;
  Runnable(const Runnable &&other) = delete;

 protected:
  friend class Application;

  virtual int Run(int argc, const char **argv) = 0;
  virtual void Terminate(int error_code) noexcept = 0;
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

  int Run(int argc, const char **argv) noexcept;

  void Terminate(int error_code) noexcept;

  void Abort() noexcept;

  /** Returns reference to currently associated application */
  inline Runnable *runnable() noexcept {
    return runnable_.get();
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
