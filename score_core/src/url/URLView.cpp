#include "score/url/URLView.h"
#include "score/util/option_helpers.h"

using score::url::detail::HTTPParserURL;

namespace score { namespace url {

using piece_t = URLView::piece_t;

URLView::URLView(HTTPParserURL&& httpParser, const char *buff, size_t buffLen)
  : parsed_(httpParser), buff_(buff), buffLen_(buffLen) {}

URLView::URLView(){}

score::Try<URLView> URLView::parse(const char *buff, size_t buffLen) {
  auto parsed = HTTPParserURL::parse(buff, buffLen);
  if (parsed.hasException()) {
    return score::Try<URLView>{
      std::move(parsed.exception())
    };
  }
  return score::Try<URLView> {
    URLView(std::move(parsed.value()), buff, buffLen)
  };
}

piece_t URLView::getComponent(URLComponent component) const {
  return util::doWithValue(parsed_.getComponent(component),
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

score::Optional<size_t> URLView::fragmentOffset() const {
  score::Optional<size_t> result;
  auto frag = fragment();
  if (!frag.hasValue()) {
    return result;
  }
  uintptr_t fragPtr = (uintptr_t) frag.value().data();
  uintptr_t basePtr = (uintptr_t) buff_;
  result.assign((fragPtr - basePtr) - 1);
  return result;
}

score::Optional<int16_t> URLView::port() const {
  score::Optional<int16_t> result;
  if (parsed_.valid()) {
    result.assign(parsed_.getPort());
  }
  return result;
}

}} // score::url
