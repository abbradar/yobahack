#ifndef YOBAHACK_COMMON_LOGGING_H_
#define YOBAHACK_COMMON_LOGGING_H_

#include <vector>
#include <memory>
#include <functional>
#include <sstream>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include "common/singleton.h"

namespace logging {

/** Levels of log message severity */
enum LogMessageLevel {
  kCritical, ///< After this application should typically crash.
  kError, ///< Bad, but maintainable, error.
  kWarning, ///< Something strange or bad.
  kNotice, ///< Typically just information; like "player connected"
  kDebug, ///< Debug information
};

/** Returns name of givel LogMessageLevel */
const char *LevelName(const LogMessageLevel level);

/** Handles logging; receives messages and sends them to various destinations.
 * Singleton which splits logging messages between different destinations.
 * Not thread-safe.
 */
class Logger : public Singleton<Logger> {
 public:
  typedef std::vector<std::function<void(LogMessageLevel, const char *)>> DestinationVector;
  typedef boost::posix_time::time_facet TimeFacet;

  Logger(const Logger &other) = delete;
  Logger(const Logger &&other) = delete;

  /** Sends log message to destinations */
  void Log(LogMessageLevel level, const char *) noexcept;

  /** Holds instances of LoggerDestination class which will receive log messages.
   * Not thread-safe, vector should not be modified if chance of logging call exists.
   */
  inline DestinationVector &destinations() noexcept {
    return destinations_;
  }

  inline LogMessageLevel level() const noexcept {
    return level_;
  }

  void set_level(const LogMessageLevel level) noexcept;

  inline bool write_to_stderr() const noexcept {
    return write_to_stderr_;
  }

  void set_write_to_stderr(bool write_to_stderr) noexcept;

  inline const char *name() const noexcept {
    return name_;
  }

  void set_name(const char *) noexcept;

  inline const TimeFacet &time_facet() const noexcept {
    return *time_facet_;
  }

  void set_time_facet(TimeFacet *time_facet) noexcept;

 private:
  friend class Singleton<Logger>;

  Logger() = default;
  ~Logger() = default;

  DestinationVector destinations_;
  bool write_to_stderr_ = true;
  LogMessageLevel level_ = kNotice;
  const char *name_ = "";
  TimeFacet *time_facet_;
  std::stringstream time_format_;
};

}

// TODO(abbradar) Comment properly

/** Convenience function */
inline void Log(logging::LogMessageLevel level, const char *msg) {
  logging::Logger::instance().Log(level, msg);
}

/** Convenience function */
inline void LogDebug(const char *msg) {
  Log(logging::kDebug, msg);
}

/** Convenience function */
inline void LogNotice(const char *msg) {
  Log(logging::kNotice, msg);
}

/** Convenience function */
inline void LogWarning(const char *msg) {
  Log(logging::kWarning, msg);
}

/** Convenience function */
inline void LogError(const char *msg) {
  Log(logging::kError, msg);
}

/** Convenience function */
inline void LogCritical(const char *msg) {
  Log(logging::kCritical, msg);
}

#endif // YOBAHACK_COMMON_LOGGING_H_
