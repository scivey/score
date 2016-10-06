#pragma once

#include <folly/Optional.h>
#include <folly/futures/Try.h>
#include <folly/futures/Unit.h>
#include "score/exceptions/exceptions.h"
#include "score/url/URLComponent.h"
#include "score/vendored/http-parser/http_parser.h"

namespace score { namespace url { namespace detail {

class HTTPParserURL {
 public:
  static const uint16_t kDefaultPort = 80;
  using idx_pair_t = std::pair<uint16_t, uint16_t>;
  enum class State {
    EMPTY, OKAY
  };

 protected:
  State state_ {State::EMPTY};
  http_parser_url parsed_;

 public:
  HTTPParserURL();
  bool valid() const;
  operator bool() const;
  void reset();
  bool hasComponent(URLComponent component) const;
  folly::Optional<idx_pair_t> getComponent(URLComponent component) const;
  int16_t getPort() const;
  folly::Try<folly::Unit> setTarget(const char *buff, size_t buffLen);


  template<typename TString,
    typename = decltype(std::declval<TString>().c_str())
  >
  folly::Try<folly::Unit> setTarget(const TString& aString) {
    return setTarget(aString.c_str(), aString.size());
  }

  static folly::Try<HTTPParserURL> parse(const char *buff, size_t buffLen);


  template<typename TString,
    typename = decltype(std::declval<TString>().c_str())
  >
  static folly::Try<HTTPParserURL> parse(const TString& aString) {
    return parse(aString.c_str(), aString.size());
  }
};


}}} // score::url::detail
