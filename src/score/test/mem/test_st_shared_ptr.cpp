#include <gtest/gtest.h>
#include <thread>
#include "score/mem/st_shared_ptr.h"
#include "score/test_support/Noisy.h"

using namespace std;
namespace mem = score::mem;
using Noisy = score::test_support::Noisy<88751>;

struct StPtrTestContext {
  StPtrTestContext(){
    Noisy::resetReport();
    EXPECT_EQ(0, Noisy::nCreated());
    EXPECT_EQ(0, Noisy::nDestroyed());
  }
  ~StPtrTestContext() {
    Noisy::resetReport();
  }
};

#define EXPECT_CREATED_DESTROYED(n_created, n_destroyed) do { \
      EXPECT_EQ(n_created, Noisy::nCreated()); \
      EXPECT_EQ(n_destroyed, Noisy::nDestroyed()); \
  } while(0)



TEST(Test_st_shared_ptr, TestSanity1) {
  StPtrTestContext ctx;
  {
    auto n1 = mem::make_st_shared<Noisy>();
    auto id1 = n1->getId();
    EXPECT_CREATED_DESTROYED(1, 0);
    mem::st_shared_ptr<Noisy> n2 = n1;
    EXPECT_EQ(id1, n1->getId());
    EXPECT_EQ(id1, n2->getId());
    EXPECT_CREATED_DESTROYED(1, 0);
    EXPECT_EQ(n1.get(), n2.get());
    auto n3 = mem::make_st_shared<Noisy>();
    EXPECT_CREATED_DESTROYED(2, 0);
    EXPECT_NE(n3->getId(), n1->getId());
    EXPECT_NE(n3->getId(), n2->getId());
    n2 = n3;
    EXPECT_EQ(n3->getId(), n2->getId());
    EXPECT_NE(id1, n2->getId());
    EXPECT_EQ(id1, n1->getId());
  }
  EXPECT_CREATED_DESTROYED(2, 2);
}

TEST(Test_st_shared_ptr, TestSanity2) {
  StPtrTestContext ctx;
  {
    auto n1 = mem::make_st_shared<Noisy>();
    auto id1 = n1->getId();
    EXPECT_CREATED_DESTROYED(1, 0);
    {
      decltype(n1) n2 = n1;
      EXPECT_CREATED_DESTROYED(1, 0);
      EXPECT_EQ(id1, n2->getId());
      EXPECT_EQ(n1->getId(), n2->getId());
    }
    EXPECT_CREATED_DESTROYED(1, 0);
    EXPECT_EQ(id1, n1->getId());
  }
  EXPECT_CREATED_DESTROYED(1, 1);
}

TEST(Test_st_shared_ptr, TestClosure) {
  StPtrTestContext ctx;
  static const size_t kDefaultId = 999;
  {
    std::function<void()> func;
    bool wasCalled {false};
    size_t savedId {kDefaultId};
    size_t compareId {kDefaultId};
    EXPECT_CREATED_DESTROYED(0, 0);
    {
      auto n1 = mem::make_st_shared<Noisy>();
      EXPECT_CREATED_DESTROYED(1, 0);
      savedId = n1->getId();
      EXPECT_NE(kDefaultId, savedId);
      func = [n1, &wasCalled, &compareId](){
        wasCalled = true;
        compareId = n1->getId();
      };
      EXPECT_CREATED_DESTROYED(1, 0);
    }
    EXPECT_CREATED_DESTROYED(1, 0);
    EXPECT_FALSE(wasCalled);
    func();
    EXPECT_TRUE(wasCalled);
    EXPECT_EQ(savedId, compareId);
    EXPECT_CREATED_DESTROYED(1, 0);
  }
  EXPECT_CREATED_DESTROYED(1, 1);
}

