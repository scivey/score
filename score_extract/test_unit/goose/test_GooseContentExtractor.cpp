#include <gtest/gtest.h>
#include <string>
#include "score/fs/fs.h"
#include "score/nlp/Language.h"
#include "score/extract/goose/GooseContentExtractor.h"

using namespace std;
using namespace score::extract::goose;
using score::nlp::Language;
using score::fs::readFileSync;


#define EXPECT_EXTRACTION(lang, file_path, good_strings, bad_strings) do { \
    auto fileData = readFileSync(file_path); \
    GooseContentExtractor extractor; \
    auto extracted = extractor.extract(fileData, lang); \
    for (const std::string& expected: good_strings) { \
      EXPECT_TRUE(extracted.find(expected) != std::string::npos); \
    } \
    for (const std::string& unexpected: bad_strings) { \
      EXPECT_TRUE(extracted.find(unexpected) == std::string::npos); \
    } \
  } while(0)


TEST(TestGooseContentExtractor, TestBasic1) {
  const string kFilePath = "data/test/html/jezebel1.txt";
  vector<string> expected {"Taylor", "Avril"};
  vector<string> notExpected;
  EXPECT_EXTRACTION(Language::EN, kFilePath, expected, notExpected);
}

TEST(TestGooseContentExtractor, TestBasic2) {
  const string kFilePath = "data/test/html/businessweek2.txt";
  vector<string> expected {
    "significantly higher for our human interest content."
  };
  vector<string> notExpected {
    "Product Reviews",
    "Oracle Eyes Industry-Specific Software Buys",
    "Which Is America's Best City?"
  };
  EXPECT_EXTRACTION(Language::EN, kFilePath, expected, notExpected);
}


TEST(TestGooseContentExtractor, TestBasic3) {
  const string kFilePath = "data/test/html/huffpo1.txt";
  vector<string> expected {
    string{"The Fed's policymaking body voted 9 to 1 earlier this"
    " week to maintain its current zero-interest-rate policy."}
  };
  vector<string> notExpected {
    "HUFFPOST TECH"
  };
  EXPECT_EXTRACTION(Language::EN, kFilePath, expected, notExpected);
}
