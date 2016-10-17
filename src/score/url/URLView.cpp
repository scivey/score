#include "score/url/URLView.h"
#include "score/folly_util/option_helpers.h"

using score::url::detail::HTTPParserURL;
using folly::Optional;
using folly::StringPiece;
using folly::Try;

namespace score { namespace url {

using piece_t = URLView::piece_t;

URLView::URLView(HTTPParserURL&& httpParser, const char *buff, size_t buffLen)
  : parsed_(httpParser), buff_(buff), buffLen_(buffLen) {}

URLView::URLView(){}

folly::Try<URLView> URLView::parse(const char *buff, size_t buffLen) {
  auto parsed = HTTPParserURL::parse(buff, buffLen);
  if (parsed.hasException()) {
    return folly::Try<URLView>{
      std::move(parsed.exception())
    };
  }
  return folly::Try<URLView> {
    URLView(std::move(parsed.value()), buff, buffLen)
  };
}

piece_t URLView::getComponent(URLComponent component) const {
  return folly_util::doWithValue(parsed_.getComponent(component),
    [this](HTTPParserURL::idx_pair_t idxPair) {
      return StringPiece(
        buff_ + idxPair.first,
        idxPair.second
      );
    }
  );
}

piece_t URLView::host() const {
  return getComponent(URLComponent::HOST);
}

piece_t URLView::scheme() const {
  return getComponent(URLComponent::SCHEME);
}

piece_t URLView::path() const {
  return getComponent(URLComponent::PATH);
}

piece_t URLView::query() const {
  return getComponent(URLComponent::QUERY);
}

piece_t URLView::userInfo() const {
  return getComponent(URLComponent::USERINFO);
}

piece_t URLView::fragment() const {
  return getComponent(URLComponent::FRAGMENT);
}

folly::Optional<size_t> URLView::fragmentOffset() const {
  folly::Optional<size_t> result;
  auto frag = fragment();
  if (!frag.hasValue()) {
    return result;
  }
  uintptr_t fragPtr = (uintptr_t) frag.value().data();
  uintptr_t basePtr = (uintptr_t) buff_;
  result.assign((fragPtr - basePtr) - 1);
  return result;
}

folly::Optional<int16_t> URLView::port() const {
  folly::Optional<int16_t> result;
  if (parsed_.valid()) {
    result.assign(parsed_.getPort());
  }
  return result;
}

}} // score::url
