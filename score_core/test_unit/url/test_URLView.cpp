#include <string>
#include <sstream>
#include <functional>
#include <type_traits>

#include <gtest/gtest.h>
#include <folly/Range.h>
#include "score/vendored/http-parser/http_parser.h"
#include "score/exceptions/exceptions.h"
#include "score/url/URLComponent.h"
#include "score/url/detail/HTTPParserURL.h"
#include "score/Optional.h"
#include "score/Try.h"
#include "score/Unit.h"
#include "score/util/option_helpers.h"
#include "score/url/URLView.h"

using namespace std;
using folly::StringPiece;
using score::Optional;
using score::url::URLComponent;
using score::url::httpParserEnumOfURLComponent;
using score::url::detail::HTTPParserURL;
using score::url::URLView;
using score::util::doWithValue;


std::string makeString(const Optional<StringPiece>& strPiece) {
  if (!strPiece.hasValue()) {
    return "";
  }
  std::ostringstream oss;
  oss << strPiece.value();
  return oss.str();
}

TEST(TestParser, TestSanity1) {
  string anUrl = "http://www.google.com/foo?bar=yes";
  auto outcome = URLView::parse(anUrl);
  EXPECT_FALSE(outcome.hasException());
  auto parsed = outcome.value();
  EXPECT_EQ(string{"www.google.com"}, makeString(parsed.host()));
  EXPECT_EQ(string{"/foo"}, makeString(parsed.path()));
  EXPECT_EQ(string{"bar=yes"}, makeString(parsed.query()));
  EXPECT_EQ(string{"http"}, makeString(parsed.scheme()));
}
