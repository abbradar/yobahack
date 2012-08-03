#include <cstdlib>
#include <exception>
#include <string>
#include "common/logging.h"
#include "application.h"

using namespace std;

int Application::Run(int argc, const char **argv) noexcept
{
  if (!runnable_) {
    throw runtime_error("Runnable is not assigned.");
  }
  if (running_.exchange(true)) {
    throw runtime_error("Application is already started.");
  }
  try {
    return runnable_->Run(argc, argv);
  } catch (exception &e) {
    string s("Unhandled exception: ");
    s.append(e.what());
    LogCritical(s.c_str());
    Abort();
  }
  return 1;
}

void Application::Terminate(int error_code) noexcept
{
  if (running_) {
    runnable_->Terminate(error_code);
  }
  exit(error_code);
}

void Application::Abort() noexcept
{
  // TODO(abbradar) maybe better handling there, and maybe not
  abort();
}

void Application::set_runnable(Runnable *runnable)
{
  if (running_) {
    throw runtime_error("Application is already started.");
  }
  runnable_.reset(runnable);
}
