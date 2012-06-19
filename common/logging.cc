#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "debug.h"
#include "logging.h"

using namespace std;
using namespace logging;
using namespace boost::posix_time;

const char *logging::LevelName(const LogMessageLevel level) {
  switch (level) {
    case kCritical:
      return "Critical";
    case kError:
      return "Error";
    case kWarning:
      return "Warning";
    case kNotice:
      return "Notice";
    case kDebug:
      return "Debug";
    default:
      AssertMsg(false, "Invalid log message level");
  }
  return nullptr;
}

Logger::Logger() noexcept : time_facet_(new TimeFacet("%x %X")) {
  msg_string_.imbue(locale(msg_string_.getloc(), time_facet_));
}

void Logger::Log(LogMessageLevel level, const string msg) noexcept {
  if (level > level_) return;

  if (write_to_stderr_) {
    msg_string_ << LevelName(level)[0] << " [" << second_clock::local_time() << "] " <<
                   name_ << ": " << msg << endl;
    cerr << msg_string_.str();
    msg_string_.str("");
  }

  for (auto &dest : destinations_) {
    dest(level, msg);
  }
}

void Logger::set_level(const LogMessageLevel level) noexcept {
  level_ = level;
}

void Logger::set_write_to_stderr(const bool write_to_stderr) noexcept {
  write_to_stderr_ = write_to_stderr;
}

void Logger::set_name(const std::string name) noexcept {
  name_ = name;
}

void Logger::set_time_facet(TimeFacet *time_facet) noexcept {
  delete time_facet_;
  time_facet_ = time_facet;
  msg_string_.imbue(locale(msg_string_.getloc(), time_facet_));
}
