#include "score/url/URLComponent.h"

namespace score { namespace url {

http_parser_url_fields httpParserEnumOfURLComponent(URLComponent component) {
  switch (component) {
    case URLComponent::SCHEME:    return UF_SCHEMA;
    case URLComponent::HOST:      return UF_HOST;
    case URLComponent::PORT:      return UF_PORT;
    case URLComponent::PATH:      return UF_PATH;
    case URLComponent::QUERY:     return UF_QUERY;
    case URLComponent::FRAGMENT:  return UF_FRAGMENT;
    case URLComponent::USERINFO:  return UF_USERINFO;
  }
}


}} // score::url
