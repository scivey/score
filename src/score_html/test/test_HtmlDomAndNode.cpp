#include <gtest/gtest.h>
#include <glog/logging.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include "score_html/HtmlDom.h"
#include "score/io/string_utils.h"
#include "score_html/vendored/gumbo-parser/gumbo.h"

using namespace std;
using namespace score::html;
using score::io::joinWith;

TEST(TestHtmlDomAndNode, TestBasicChildCount) {
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
  auto dom = HtmlDom::create(fulldoc);
  auto root = dom.root();
  size_t children = root.childCount();
  EXPECT_EQ(2, children);
}

TEST(TestHtmlDomAndNode, TestGetText) {
  vector<string> doc {
    "<!doctype html>",
    "<html>",
    "<body>",
    "<h1>Heading</h1>",
    "<p>some text</p>",
    "</body>",
    "</html>"
  };
  auto fulldoc = joinWith("", doc);
  auto dom = HtmlDom::create(fulldoc);
  auto text = dom.root().getText();
  EXPECT_TRUE(text.find("Heading") != string::npos);
  EXPECT_TRUE(text.find("some text") != string::npos);
}