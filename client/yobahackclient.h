#ifndef YOBAHACK_CLIENT_YOBAHACKCLIENT_H_
#define YOBAHACK_CLIENT_YOBAHACKCLIENT_H_

#include "common/application.h"

class YobaHackClient : public Runnable
{
 public:
  YobaHackClient() = default;
  YobaHackClient(const YobaHackClient &other) = delete;
  YobaHackClient(const YobaHackClient &&other) = delete;

 private:
  ~YobaHackClient() = default;

  virtual int Run(int argc, const char **argv);
  virtual void Terminate(int error_code) noexcept;
};

#endif // YOBAHACK_CLIENT_YOBAHACKCLIENT_H_
