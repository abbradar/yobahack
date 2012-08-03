#include <iostream>
#include <functional>
#include <gtest/gtest.h>
#include "../server/eventqueue.h"

class EventQueueTest : public ::testing::Test {
  protected:
    virtual void SetUp() {
      //write something here
    }
  EventQueue eq1;
  EventQueue eq2;
  EventQueue eq3;
};
