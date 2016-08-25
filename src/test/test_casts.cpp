#include <gtest/gtest.h>
#include <memory>

using namespace std;

struct Point1 {
  int x {0}, y {0};
};

struct Point2 {
  int x {0}, y {0};
};


class Something : public enable_shared_from_this<Something> {
 protected:
  size_t value_ {0};

 public:
  Something(size_t val): value_(val) {}
  size_t getValue() const {
    return value_;
  }
};


TEST(TestSomething, TestWorks) {
  std::weak_ptr<Something> weakPtr;
  EXPECT_TRUE(weakPtr.expired());
  {
    auto shared = std::make_shared<Something>(17);
    EXPECT_EQ(17, shared->getValue());
    EXPECT_TRUE(weakPtr.expired());
    weakPtr = shared;
    EXPECT_FALSE(weakPtr.expired());
    EXPECT_EQ(17, weakPtr.lock()->getValue());
  }
  EXPECT_TRUE(weakPtr.expired());
}