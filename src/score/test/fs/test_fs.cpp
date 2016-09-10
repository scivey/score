#include <gtest/gtest.h>
#include <string>
#include "score/fs/fs.h"

using namespace std;
namespace fs = score::fs;

TEST(TestFs, TestReadFileSync) {
  string result;
  EXPECT_TRUE(fs::readFileSync("CMakeLists.txt", result));
  EXPECT_TRUE(result.find("score") != string::npos);
}
