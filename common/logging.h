#ifndef YOBAHACK_COMMON_LOGGING_H_
#define YOBAHACK_COMMON_LOGGING_H_

#include <vector>
#include <memory>
#include <string>
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

  /** Handles logging; receives messages and sends them to various destinations.
   * Singleton which splits logging messages between different destinations.
   * Not thread-safe.
   */
  class Logger : public Singleton<Logger> {
   public:
    typedef std::vector<std::function<void(LogMessageLevel, const std::string &)>> DestinationVector;
    typedef boost::posix_time::time_facet TimeFacet;

    Logger(const Logger &other) = delete;
    Logger(const Logger &&other) = delete;

    /** Sends log message to destinations */
    void Log(LogMessageLevel level, const std::string msg) noexcept;

    /** Holds instances of LoggerDestination class which will receive log messages.
     * Not thread-safe, vector should not be modified if chance of logging call exists.
     */
    inline DestinationVector &destinations() noexcept {
      return destinations_;
    }

    inline const DestinationVector &destinations() const noexcept {
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

    inline const std::string &name() const noexcept {
      return name_;
    }

    void set_name(const std::string &&name) noexcept;

    inline const TimeFacet &time_facet() const noexcept {
      return *time_facet_;
    }

    void set_time_facet(TimeFacet *time_facet) noexcept;

   private:
    friend class Singleton<Logger>;

    Logger() noexcept;
    ~Logger() = default;

    DestinationVector destinations_;
    bool write_to_stderr_ = true;
    LogMessageLevel level_ = kNotice;
    std::string name_ = "";
    std::unique_ptr<TimeFacet>time_facet_;
    std::stringstream msg_string_;
  };
}

// TODO(abbradar) Comment properly

/** Convenience function */
inline void Log(logging::LogMessageLevel level, const std::string msg) {
  logging::Logger::instance().Log(level, msg);
}

/** Convenience function */
inline void LogDebug(const std::string msg) {
  Log(logging::kDebug, msg);
}

/** Convenience function */
inline void LogNotice(const std::string msg) {
  Log(logging::kNotice, msg);
}

/** Convenience function */
inline void LogWarning(const std::string msg) {
  Log(logging::kWarning, msg);
}

/** Convenience function */
inline void LogError(const std::string msg) {
  Log(logging::kError, msg);
}

/** Convenience function */
inline void LogCritical(const std::string msg) {
  Log(logging::kCritical, msg);
}

#endif // YOBAHACK_COMMON_LOGGING_H_
