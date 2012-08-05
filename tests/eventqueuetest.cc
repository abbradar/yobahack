#include "eventqueuetest.h"

void FlagSet(bool &flag) {
  flag = true;
}

TEST_F(EventQueueTest, FirstTickTest) {
  bool flag = false;
  eq_.Push(std::bind(&FlagSet, flag), 0);
  eq_.Tick();
  ASSERT_TRUE(flag);
}
