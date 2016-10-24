#pragma once

#include "score/vendored/http-parser/http_parser.h"

namespace score { namespace url {

enum class URLComponent {
  SCHEME, HOST, PORT, PATH, QUERY, FRAGMENT, USERINFO
};

http_parser_url_fields httpParserEnumOfURLComponent(URLComponent component);

}} // score::url
