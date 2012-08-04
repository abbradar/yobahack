#include "eventqueuetest.h"

int dummy1;
int dummy2;

void foo1(){dummy1=1;}

TEST_F(EventQueueTest, wat) {
  EXPECT_EQ(1,1);
}

TEST_F(EventQueueTest, FirstTickTest) {
  dummy1=0;
  eq1.store_function(foo1,0);
  eq1.tick();
  EXPECT_EQ(1,dummy1);
}

TEST_F(EventQueueTest, SecondTickTest) {
  int dummy2=2;
  EXPECT_EQ(2,dummy2);
}
