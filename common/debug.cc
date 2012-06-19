#include "debug.h"

#if DEBUG_LEVEL != 0
namespace boost
{
  void assertion_failed_msg(const char *expr, const char *msg, const char *function, const char *file, int line) noexcept {
    // Assertions are sent for logging, then application is aborted.
    // We assume assertion fail is a critical error.
    std::stringstream ss;
    ss << "Assertion failed in " << file << ":" << function << " (line " << line << ")";
    if (msg) {
      ss << ": " << msg;
    }
    LogCritical(ss.str());
    Application::instance().Abort();
  }

  void assertion_failed(const char *expr, const char *function, const char *file, int line) noexcept {
    assertion_failed_msg(expr, NULL, function, file, line);
  }
}
#endif
