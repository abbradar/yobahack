#include <iostream>
#include "yobahackclient.h"

using namespace std;

int YobaHackClient::Run(int argc, const char **argv) {
  cout << "Hello World!" << endl;
  return 0;
}

void YobaHackClient::Terminate(int exit_code) noexcept {
}
