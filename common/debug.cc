#include <sstream>
#include "common/logging.h"
#include "common/application.h"
#include "debug.h"

#if DEBUG_LEVEL != 0
namespace boost
{
  void assertion_failed_msg(const char *expr, const char *msg, const char *function, const char *file, long line) noexcept {
    // Assertions are sent for logging, then application is aborted.
    // We assume assertion fail is a critical error.
    std::stringstream ss;
    ss << "Assertion failed in " << file << ":" << line << " (" << function << ")";
    if (msg) {
      ss << ": " << msg;
    }
    LogCritical(ss.str());
    Application::instance().Abort();
  }

  void assertion_failed(const char *expr, const char *function, const char *file, long line) noexcept {
    assertion_failed_msg(expr, NULL, function, file, line);
  }
}
#endif
