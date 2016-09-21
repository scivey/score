#include <gtest/gtest.h>
#include "score/encodings/IConverter.h"
#include "score/encodings/Encoding.h"
#include "score/encodings/IConvOptions.h"


using namespace std;
using namespace score::encodings;

TEST(TestIConverter, TestIdentity) {
  IConvOptions options;
  options.fromEncoding = Encoding::GB18030;
  options.toEncoding = Encoding::UTF_8;
  auto converter = IConverter::create(options);
  string expectedUtf8 = "\xe5\xb7\xa5\xe5\x95\x86\xe7\xbd\x91\xe7\x9b\x91";
  string gb18030Text = "\xb9\xa4\xc9\xcc\xcd\xf8\xbc\xe0";
  auto result = converter.convert(gb18030Text);
  EXPECT_EQ(expectedUtf8, result);
}

TEST(TestIConverter, TestWorkiness) {
  IConvOptions options;
  options.fromEncoding = Encoding::GB18030;
  options.toEncoding = Encoding::UTF_8;
  auto converter = IConverter::create(options);
  string expectedUtf8 = "\xe5\xb7\xa5\xe5\x95\x86\xe7\xbd\x91\xe7\x9b\x91";
  string gb18030Text = "\xb9\xa4\xc9\xcc\xcd\xf8\xbc\xe0";
  auto result = converter.convert(gb18030Text);
  EXPECT_EQ(expectedUtf8, result);
}

