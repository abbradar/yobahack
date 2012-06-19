#include <iostream>
#include "common/logging.h"
#include "yobahackserver.h"

using namespace std;

int YobaHackServer::Run(int argc, const char **argv) {
  logging::Logger::instance().set_name("YobaHack Server");
  cout << "Hello World!" << endl;
  LogError("This is an error!");
  LogNotice("This is a notice!");
  LogWarning("This is a warning!");
  return 0;
}

void YobaHackServer::Terminate(int exit_code) noexcept {
}
