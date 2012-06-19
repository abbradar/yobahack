#ifndef YOBAHACK_SERVER_YOBAHACKSERVER_H_
#define YOBAHACK_SERVER_YOBAHACKSERVER_H_

#include "common/application.h"

class YobaHackServer : public Runnable
{
 public:
  YobaHackServer() = default;
  YobaHackServer(const YobaHackServer &other) = delete;
  YobaHackServer(const YobaHackServer &&other) = delete;

 private:
  ~YobaHackServer() = default;

  virtual int Run(int argc, const char **argv);
  virtual void Terminate(int error_code) noexcept;
};

#endif // YOBAHACK_SERVER_YOBAHACKSERVER_H_
