#include <gtest/gtest.h>
#include "score/MoveWrapper.h"
#include "score/test_support/Noisy.h"


using namespace score;
using namespace std;

using Noisy = score::test_support::Noisy<9515>;

TEST(TestMoveWrapper, TestAssumptions) {
  Noisy::resetReport();
  Noisy n1;
  EXPECT_EQ(1, Noisy::nCreated());
  EXPECT_EQ(0, Noisy::getMoves().size());
  Noisy n2;
  EXPECT_EQ(2, Noisy::nCreated());
  EXPECT_EQ(0, Noisy::getMoves().size());

  Noisy n3 {std::move(n1)};
  EXPECT_EQ(2, Noisy::nCreated());
  auto moves = Noisy::getMoves();
  std::map<size_t, size_t> expectedMoves {
    {n3.getId(), 1}
  };
  EXPECT_EQ(expectedMoves, moves);

  Noisy n4 {std::move(n3)};
  EXPECT_EQ(2, Noisy::nCreated());
  expectedMoves = {
    {n4.getId(), 2}
  };
  EXPECT_EQ(expectedMoves, Noisy::getMoves());
}

TEST(TestMoveWrapper, TestWorks) {
  Noisy::resetReport();
  using func_t = std::function<void()>;
  EXPECT_EQ(0, Noisy::nCreated());
  Noisy n1;
  EXPECT_EQ(1, Noisy::nCreated());
  EXPECT_EQ(0, Noisy::nTotalMoves());
  auto mwrapped = makeMoveWrapper(n1);
  EXPECT_EQ(1, Noisy::nCreated());
  EXPECT_EQ(1, Noisy::nTotalMoves());

  bool called {false};
  func_t fn([mwrapped, &called]() {
    MoveWrapper<Noisy> moved = mwrapped;
    EXPECT_EQ(4, Noisy::nTotalMoves());
    Noisy unwrapped = moved.move();
    called = true;
  });
  EXPECT_EQ(3, Noisy::nTotalMoves());
  EXPECT_EQ(1, Noisy::nCreated());
  fn();
  EXPECT_TRUE(called);
  EXPECT_EQ(5, Noisy::nTotalMoves());
  EXPECT_EQ(1, Noisy::nCreated());

}