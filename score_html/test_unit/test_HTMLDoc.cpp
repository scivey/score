#include <gtest/gtest.h>
#include <glog/logging.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include "score/html/HTMLDoc.h"
#include "score/io/string_utils.h"
#include "score/html/vendored/gumbo-parser/gumbo.h"

using namespace std;
using namespace score::html;
using score::io::joinWith;

TEST(TestHTMLDoc, TestSanity1) {
  vector<string> doc {
    "<!doctype html>",
    "<html>",
    "<body>",
    "<h1>Heading</h1>",
    "<p>some text</p>",
    "</body>",
    "</html>"
  };
  auto fulldoc = joinWith("\n", doc);
  auto dom = HTMLDoc::create(fulldoc);
  auto root = dom.root();
  EXPECT_TRUE(root.childCount() > 0);
  auto text = dom.root().getText();
  EXPECT_TRUE(text.find("Heading") != string::npos);
  EXPECT_TRUE(text.find("some text") != string::npos);
}
