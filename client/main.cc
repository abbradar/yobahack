#include "yobahackclient.h"

int main(int argc, const char **argv) {
  Application::instance().set_runnable(new YobaHackClient());
  return Application::instance().Run(argc, argv);
}
