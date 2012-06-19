#include "yobahackserver.h"

int main(int argc, const char **argv) {
  Application::instance().set_runnable(new YobaHackServer());
  return Application::instance().Run(argc, argv);
}
