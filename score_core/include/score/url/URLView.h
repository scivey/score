#pragma once

#include "score/io/ByteStringPiece.h"
#include "score/Optional.h"
#include "score/Try.h"
#include "score/Unit.h"


#include "score/exceptions/exceptions.h"
#include "score/url/URLComponent.h"
#include "score/url/detail/HTTPParserURL.h"

#include "score/vendored/http-parser/http_parser.h"
#include "score/util/option_helpers.h"

namespace score { namespace url {

class URLView {
 public:
  using piece_t = score::Optional<io::ByteStringPiece>;
 protected:
  using HTTPParserURL = score::url::detail::HTTPParserURL;
  using StringPiece = io::ByteStringPiece;

  HTTPParserURL parsed_;
  const char* buff_ {nullptr};
  size_t buffLen_ {0};
  URLView(HTTPParserURL&& httpParser, const char *buff, size_t buffLen);

 public:
  URLView();

  static score::Try<URLView> parse(const char *buff, size_t buffLen);

  piece_t getComponent(URLComponent component) const;
  piece_t host() const;
  piece_t scheme() const;
  piece_t path() const;
  piece_t query() const;
  piece_t userInfo() const;
  piece_t fragment() const;
  score::Optional<int16_t> port() const;

  score::Optional<size_t> fragmentOffset() const;

  template<typename TString,
    typename = decltype(std::declval<TString>().c_str())
  >
  static score::Try<URLView> parse(const TString& anUrl) {
    return parse(anUrl.c_str(), anUrl.size());
  }

  template<typename ...Types>
  auto setTarget(Types&&... args)
    -> decltype(std::declval<HTTPParserURL>().setTarget(std::forward<Types>(args)...)) {
    return parsed_.setTarget(std::forward<Types>(args)...);
  }

};

}} // score::url
