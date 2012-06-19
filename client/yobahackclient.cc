#include <iostream>
#include "common/logging.h"
#include "yobahackclient.h"

using namespace std;

int YobaHackClient::Run(int argc, const char **argv) {
  logging::Logger::instance().set_name("YobaHack Client");
  cout << "Hello World!" << endl;
  LogError("This is an error!");
  LogNotice("This is a notice!");
  LogWarning("This is a warning!");
  return 0;
}

void YobaHackClient::Terminate(int exit_code) noexcept {
}
