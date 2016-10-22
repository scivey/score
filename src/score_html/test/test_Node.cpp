#include <gtest/gtest.h>
#include <glog/logging.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <vector>
#include "score_html/HTMLDoc.h"
#include "score/io/string_utils.h"
#include "score_html/vendored/gumbo-parser/gumbo.h"

using namespace std;
using namespace score::html;
using score::io::joinWith;

TEST(TestNode, TestDFS1) {
  vector<string> doc {
    "<!doctype html>",
    "<html>",
    "<body>",
    "  <div>",
    "    <p>one</p>",
    "    <span>",
    "      <p>two</p>",
    "    </span>",
    "  </div>",
    "  <div>",
    "    <p>three</p>",
    "    <p>four</p>"
    "  </div>",
    "<p>five</p>",
    "</body>",
    "</html>"
  };
  auto fulldoc = joinWith("", doc);
  auto dom = HTMLDoc::create(fulldoc);
  vector<string> foundText;
  dom.root().dfs([&foundText](const Node& node) {
    if (node.isElement() && node.hasTag(Tag::P)) {
      foundText.push_back(node.getText());
    }
  });
  vector<string> expected {
    "one", "two", "three", "four", "five"
  };
  EXPECT_EQ(expected, foundText);
}


TEST(TestNode, TestChildElementCount) {
  vector<string> doc {
    "<!doctype html>",
    "<html>",
    "<body>",
    "  <h1>Heading</h1>",
    "  <div class='bar'>",
    "    <p>x</p>",
    "    <p>x</p>",
    "  </div>",
    "  <div class='foo'>",
    "    <p>x</p>",
    "    <p>x</p>",
    "    <p>x</p>",
    "    <p>x</p>",
    "    <p>x</p>",
    "  </div>",
    "</body>",
    "</html>"
  };
  auto fulldoc = joinWith("\n", doc);
  auto dom = HTMLDoc::create(fulldoc);
  auto root = dom.root();
  Node barDiv, fooDiv;
  root.dfs([&barDiv, &fooDiv](const Node& node, std::function<void()> escape) {
    if (node.hasTag(Tag::DIV) && node.hasAttr("class")) {
      auto clsName = node.getAttr("class");
      if (clsName == "bar") {
        barDiv = node;
      } else if (clsName == "foo") {
        fooDiv = node;
      } else {
        throw std::runtime_error("unexpected DIV class: " + clsName);
      }
      if (barDiv && fooDiv) {
        escape();
      }
    }
  });
  EXPECT_TRUE(!!barDiv);
  EXPECT_TRUE(!!fooDiv);
  EXPECT_EQ(2, barDiv.childElementCount());
  EXPECT_EQ(5, fooDiv.childElementCount());

  for (auto& aNode: {barDiv, fooDiv}) {
    EXPECT_TRUE(
      aNode.childCount() > aNode.childElementCount()
    ) << "some children should be white space or text.";
  }
}


struct SimpleMethodTestCtx {
  HTMLDoc doc;
  shared_ptr<string> docText;
  SimpleMethodTestCtx(HTMLDoc&& hdoc, shared_ptr<string> text)
    : doc(std::move(hdoc)), docText(text) {}

  static SimpleMethodTestCtx create() {
    std::vector<string> docParts {
      "<!doctype html>",
      "<html>",
      "<body>",
      "  <h1>Heading </h1>", // whitespace inside h1 is intentional
      "  <div class='bar'>",
      "    <p>a</p>",
      "    <p>b</p>",
      "  </div>",
      "  <div class='foo'>",
      "    <p>c</p>",
      "    <p class='special-p'>d</p>",
      "    <p>e</p>",
      "    <p>f</p>",
      "    <p>g</p>",
      "  </div>",
      "  <article class='classy-article'>",
      "    Article Text",
      "  </article>",
      "</body>",
      "</html>"
    };

    vector<string> strippedParts;
    for (auto& line: docParts) {
      strippedParts.push_back(score::io::trimWhitespace(line));
    }
    auto fulldoc = std::make_shared<string>(joinWith("\n", strippedParts));
    auto doc = HTMLDoc::create(*fulldoc.get());
    return SimpleMethodTestCtx(std::move(doc), fulldoc);
  }
};


TEST(TestNode, TestGetAttr1) {
  auto ctx = SimpleMethodTestCtx::create();
  Node article;
  ctx.doc.root().dfs([&article](const Node& node, function<void()> escape) {
    if (node.isElement() && node.hasTag(Tag::ARTICLE)) {
      article = node;
      escape();
    }
  });
  EXPECT_TRUE(!!article);
  EXPECT_TRUE(article.isElement() && article.hasTag(Tag::ARTICLE));
  EXPECT_TRUE(article.hasAttr("class"));
  string className = article.getAttr("class");
  EXPECT_EQ("classy-article", className);
}


TEST(TestNode, TestGetText1) {
  auto ctx = SimpleMethodTestCtx::create();
  auto heading = ctx.doc.root().dfFindFirst([](const Node& node) {
    return node.isElement() && node.hasTag(Tag::H1);
  });
  EXPECT_TRUE(!!heading);
  EXPECT_TRUE(heading.isElement() && heading.hasTag(Tag::H1));
  EXPECT_EQ("Heading ", heading.getText());
  EXPECT_EQ("Heading", heading.getTrimmedText());

}

TEST(TestNode, TestGetText2) {
  auto ctx = SimpleMethodTestCtx::create();
  auto barDiv = ctx.doc.root().dfFindFirst([](const Node& node) {
    if (node.isElement() && node.hasTag(Tag::DIV)) {
      if (node.hasAttr("class")) {
        string className = node.getAttr("class");
        return className == "bar";
      }
    }
    return false;
  });
  EXPECT_TRUE(!!barDiv);
  EXPECT_TRUE(barDiv.isElement() && barDiv.hasTag(Tag::DIV));
  EXPECT_EQ("bar", barDiv.getAttr("class"));
  EXPECT_EQ("a b", barDiv.getTrimmedText());
}