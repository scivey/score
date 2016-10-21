#include "score/url/detail/HTTPParserURL.h"
#include "score/url/URLError.h"

namespace score { namespace url { namespace detail {

using idx_pair_t = HTTPParserURL::idx_pair_t;

HTTPParserURL::HTTPParserURL() {
  reset();
}

bool HTTPParserURL::valid() const {
  return state_ == State::OKAY;
}

HTTPParserURL::operator bool() const {
  return valid();
}

void HTTPParserURL::reset() {
  state_ = State::EMPTY;
  http_parser_url_init(&parsed_);
}

bool HTTPParserURL::hasComponent(URLComponent component) const {
  auto i = (int) httpParserEnumOfURLComponent(component);
  return ((parsed_.field_set & (1 << i)) != 0);
}

score::Optional<idx_pair_t> HTTPParserURL::getComponent(URLComponent component) const {
  score::Optional<idx_pair_t> idxOption;
  if (!hasComponent(component)) {
    return idxOption;
  }
  auto i = (int) httpParserEnumOfURLComponent(component);
  auto idxPair = std::make_pair(
    parsed_.field_data[i].off,
    parsed_.field_data[i].len
  );
  if (idxPair.second > 0) {
    idxOption.assign(idxPair);
  }
  return idxOption;
}

int16_t HTTPParserURL::getPort() const {
  if (!valid() || parsed_.port == 0) {
    return kDefaultPort;
  }
  return parsed_.port;
}

score::Try<score::Unit> HTTPParserURL::setTarget(
    const char *buff, size_t buffLen) {
  int isConnect = 0;
  reset();
  int rc = http_parser_parse_url(buff, buffLen, isConnect, &parsed_);
  if (rc != 0) {
    reset();
    return score::Try<score::Unit>{
      score::make_exception_wrapper<URLParseError>("URLParseError")
    };
  }
  return score::Try<score::Unit>{score::Unit{}};
}

score::Try<HTTPParserURL> HTTPParserURL::parse(
    const char *buff, size_t buffLen) {
  HTTPParserURL instance;
  auto outcome = instance.setTarget(buff, buffLen);
  if (outcome.hasException()) {
    return score::Try<HTTPParserURL>{ std::move(outcome.exception()) };
  }
  return score::Try<HTTPParserURL>(std::move(instance));
}


}}} // score::url::detail
