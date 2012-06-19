#include <cstdlib>
#include <exception>
#include <sstream>
#include "common/logging.h"
#include "application.h"

using namespace std;

void Application::Run() noexcept
{
  if (running_) {
    throw runtime_error("Application is already started.");
  }
  if (!runnable_) {
    throw runtime_error("Runnable is not assigned.");
  }
  running_ = true;
  try {
    runnable_->Run();
  } catch (exception &e) {
    stringstream ss;
    ss << "Unhandled exception from Run(): " << e.what();
    LogCritical(ss.str());
    Abort();
  }
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