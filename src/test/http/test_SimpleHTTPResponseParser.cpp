#include <gtest/gtest.h>
#include "score/http/SimpleHTTPResponseParser.h"
#include "score/io/string_utils.h"

using namespace score::http;
using namespace score::io;
using namespace std;

static const vector<string> requestParts {
  "HTTP/1.1 200 OK",
  "Content-Type: text/xml; charset=utf-8",
  "Content-Length: 4",
  "Accept-Ranges: bytes",
  "Date: Wed, 31 Aug 2016 02:08:50 GMT",
  "",
  "DATA"
};

TEST(TestSimpleHTTPResponseParser, TestSanity) {
  SimpleHTTPResponseParser parser;
  string responseStr = joinWith("\r\n", requestParts);
  size_t nr = parser.feed(responseStr);
  EXPECT_TRUE(parser.isComplete());
  EXPECT_EQ(responseStr.size(), nr);
  EXPECT_EQ("OK", parser.getStatusString().copyToStdString());

  std::map<string, string> expectedHeaders {
    {"Content-Type", "text/xml; charset=utf-8"},
    {"Content-Length", "4"},
    {"Accept-Ranges", "bytes"},
    {"Date", "Wed, 31 Aug 2016 02:08:50 GMT"}
  };
  for (auto &header: expectedHeaders) {
    auto headerVal = parser.getHeader(header.first);
    EXPECT_TRUE(headerVal.hasValue());
    EXPECT_EQ(header.second, headerVal.value().copyToStdString());
  }
}
