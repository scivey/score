#include <gtest/gtest.h>
#include "score/DefaultValueMap.h"
#include <map>

using namespace std;
using score::DefaultValueMap;

TEST(TestDefaultValueMap, Simple) {
  using map_type = DefaultValueMap<map<string, int>>;
  map_type aMap(10, {
    {"fish", 7},
    {"dogs", 3}
  });
  map<string, int> expected {
    {"fish", 7},
    {"dogs", 3},
    {"cats", 10},
    {"wombats", 10}
  };
  for (auto &item: expected) {
    int val = aMap[item.first];
    EXPECT_EQ(item.second, val);
  }
}

TEST(TestDefaultValueMap, Modification) {
  using map_type = DefaultValueMap<map<string, int>>;
  map_type aMap(10);
  aMap["dogs"] += 5;
  aMap["cats"] += 6;
  aMap["wombats"] += 26;
  aMap.insert(std::make_pair("fish", 2));

  map<string, int> expected {
    {"dogs", 15},
    {"cats", 16},
    {"wombats", 36},
    {"fish", 2},
    {"unrelated", 10}
  };

  for (auto &item: expected) {
    int val = aMap[item.first];
    EXPECT_EQ(item.second, val);
  }
}
