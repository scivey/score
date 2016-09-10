#include <gtest/gtest.h>
#include <glog/logging.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include "score/html/HtmlDom.h"
#include "score/html/MetaTags.h"
#include "score/io/string_utils.h"
#include "score/vendored/gumbo-parser/gumbo.h"

using namespace std;
using namespace score::html;
using score::io::joinWith;

static const vector<string> doc1 {
  "<!doctype html>",
  "<html>",
  "<head>",
  "<meta property='anything1' content='cats'/>",
  "<meta name='anything2' content='dogs'/>",
  "<meta itemprop='datePublished' datetime='2016-01-28'/>",
  "<title>Something</title>"
  "</head>",
  "<body>",
  "<p>Para1</p>",
  "<p>Para2</p>",
  "<div><p>Para3</p></div>",
  "</body>",
  "</html>"
};

static const map<string, string> doc1Expectations {
  {"anything1", "cats"},
  {"anything2", "dogs"},
  {"datePublished", "2016-01-28"}
};

TEST(TestMetaTags, BasicTest1) {
  auto fulldoc = joinWith("\n", doc1);
  auto dom = HtmlDom::create(fulldoc);
  auto metas = MetaTags::extract(dom.root());
  EXPECT_EQ(doc1Expectations.size(), metas.size());
  for (auto &item: doc1Expectations) {
    EXPECT_TRUE(metas.has(item.first));
    string val = metas.get(item.first);
    EXPECT_EQ(item.second, val);
  }
}

TEST(TestMetaTags, BasicTest2) {
  auto fulldoc = joinWith("\n", doc1);
  auto dom = HtmlDom::create(fulldoc);
  auto head = dom.root().dfFindFirst(Node::nodeHasTag(Tag::HEAD));
  auto metas = MetaTags::extract(head);
  EXPECT_EQ(doc1Expectations.size(), metas.size());
  for (auto &item: doc1Expectations) {
    EXPECT_TRUE(metas.has(item.first));
    string val = metas.get(item.first);
    EXPECT_EQ(item.second, val);
  }
}

TEST(TestMetaTags, TestNoMetas) {
  auto fulldoc = joinWith("\n", doc1);
  auto dom = HtmlDom::create(fulldoc);
  auto body = dom.root().dfFindFirst(Node::nodeHasTag(Tag::BODY));
  auto metas = MetaTags::extract(body);
  EXPECT_EQ(0, metas.size());
}