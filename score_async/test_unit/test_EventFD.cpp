#include <gtest/gtest.h>
#include "score/async/EventFD.h"

using namespace score;
using namespace score::async;
using namespace std;

TEST(TestEventFD, TestSanity1) {
  auto efd = EventFD::create().value();
  EXPECT_TRUE(efd.getFDNum().value() > 0);
  EXPECT_FALSE(efd.write(17).hasException());
  EXPECT_EQ(17, efd.read().value());
}

TEST(TestEventFD, TestInvalidAfterMove) {
  auto efd = EventFD::create().value();
  EXPECT_TRUE(efd.getFDNum().value() > 0);
  EXPECT_FALSE(efd.write(17).hasException());
  EXPECT_FALSE(efd.read().hasException());

  EventFD usurper {std::move(efd)};

  EXPECT_TRUE(efd.getFDNum().hasException());
  EXPECT_FALSE(usurper.getFDNum().hasException());
  EXPECT_TRUE(efd.write(10).hasException());
  EXPECT_TRUE(efd.read().hasException());

  EXPECT_TRUE(usurper.read().hasException<EventFD::NotReady>());
  EXPECT_FALSE(usurper.write(216).hasException());
  EXPECT_EQ(216, usurper.read().value());
}
