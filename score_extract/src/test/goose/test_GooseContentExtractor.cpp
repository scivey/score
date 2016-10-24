#include <gtest/gtest.h>
#include <string>
#include "score/fs/fs.h"
#include "score_nlp/Language.h"
#include "score_extract/goose/GooseContentExtractor.h"

using namespace std;
using namespace score::extract::goose;
using score::nlp::Language;
using score::fs::readFileSync;

const string kFilePath = "data/test/html/jezebel1.txt";

TEST(TestGooseContentExtractor, TestBasic) {
  auto fileData = readFileSync(kFilePath);
  GooseContentExtractor extractor;
  auto extracted = extractor.extract(fileData, Language::EN);
  EXPECT_TRUE(extracted.find("Taylor") != string::npos);
  EXPECT_TRUE(extracted.find("Avril") != string::npos);
}

