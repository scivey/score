# Encoding Normalization

```c++
#include <string>
#include <cassert>
#include <score/encodings/EncodingNormalizer.h>

using score::encodings::EncodingNormalizer;
using score::encodings::Encoding;

int main() {
  std::string someGb18030Text = "\xb9\xa4\xc9\xcc\xcd\xf8\xbc\xe0";
  std::string expectedUtf8 = "\xe5\xb7\xa5\xe5\x95\x86\xe7\xbd\x91\xe7\x9b\x91";
  EncodingNormalizer normalizer { Encoding::UTF8 };
  auto asUtf8 = normalizer.normalize(someGb18030Text);
  assert(asUtf8 == expectedUtf8);
}

```
