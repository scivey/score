#include "score/encodings/EncodingNormalizer.h"
#include "score/encodings/IConverter.h"
#include "score/encodings/IConvOptions.h"
#include "score/encodings/detection.h"

namespace score { namespace encodings {

EncodingNormalizer::EncodingNormalizer(Encoding target)
  : targetEncoding_(target) {}

std::string EncodingNormalizer::normalize(const std::string &text) {
  auto maybeEncoding = detectEncoding(text);
  CHECK(maybeEncoding.hasValue());
  if (maybeEncoding.value() == targetEncoding_) {
    return text;
  }
  IConvOptions opts;
  opts.toEncoding = targetEncoding_;
  opts.fromEncoding = maybeEncoding.value();
  auto converter = IConverter::create(opts);
  return converter.convert(text);
}

}} // score::encodings
