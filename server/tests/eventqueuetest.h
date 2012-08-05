#include <iostream>
#include <functional>
#include <gtest/gtest.h>
#include <boost/heap/priority_queue.hpp>
#include "../server/eventqueue.h"

class EventQueueTest : public ::testing::Test {
  protected:
    EventQueue eq_;
};
