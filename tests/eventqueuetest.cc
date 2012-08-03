#include "eventqueuetest.h"

TEST_F(EventQueueTest, wat) {
  EXPECT_EQ(1,1);
}

TEST_F(EventQueueTest, FirstTickTest) {
  EXPECT_EQ(1,dummy1);
}

TEST_F(EventQueueTest, SecondTickTest) {
  EXPECT_EQ(2,dummy2);
}
