#include <iostream>
#include <functional>
#include <gtest/gtest.h>
#include <boost/heap/priority_queue.hpp>
#include "../server/eventqueue.h"

class EventQueueTest : public ::testing::Test {
  protected:
    EventQueue eq1;
    EventQueue eq2;
    EventQueue eq3;
    int dummy1;
    int dummy2;
    int dummy3=0;
    void foo1(){dummy1=1;}
    void foo2(){dummy2=2;}
    void foo3(){dummy3=3;}
    std::function<void()> bar1;
    std::function<void()> bar2;
    bar1=&foo1;
    bar2=&foo2;
    virtual void SetUp() {
      eq1.store_function(bar1,0);
      eq1.store_function(bar2,1);
      eq1.tick();
    }
};
