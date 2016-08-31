#pragma once

#include <map>
#include <sstream>
#include <string>
#include <glog/logging.h>
#include "aliens/io/Scstring.h"
#include "aliens/http/HTTPParserBase.h"
#include "aliens/Maybe.h"

namespace aliens { namespace http {

class SimpleHTTPResponseParser: public HTTPParserBase<SimpleHTTPResponseParser> {
 public:
  using string_type = io::Scstring;
  using header_map = std::map<string_type, string_type>;
  using parent_type = HTTPParserBase<SimpleHTTPResponseParser>;
  using string_view_t = typename parent_type::string_view_t;
 protected:
  header_map headers_;
  string_type status_;
  string_type url_;
  string_type body_;
  string_type currentHeader_;
 public:
  SimpleHTTPResponseParser()
    : HTTPParserBase(HTTPParserBase::ParserType::RESPONSE) {}

  int onBody(string_view_t data) {
    data.appendTo(body_);
    return 0;
  }
  int onHeaderField(string_view_t data) {
    data.appendTo(currentHeader_);
    return 0;
  }
  int onHeaderValue(string_view_t data) {
    string_type headerVal;
    data.appendTo(headerVal);
    ADCHECK(!currentHeader_.empty());
    headers_.insert(std::make_pair(currentHeader_, headerVal));
    currentHeader_ = "";
    return 0;
  }
  int onStatus(string_view_t data) {
    data.appendTo(status_);
    return 0;
  }
  int onChunkComplete() {
    return 0;
  }
  int onChunkHeader() {
    return 0;
  }
  int onURL(string_view_t data) {
    data.appendTo(url_);
    return 0;
  }
  int onMessageBegin() {
    return 0;
  }
  int onHeadersComplete() {
    return 0;
  }
  int onMessageComplete() {
    return 0;
  }
  const header_map& getHeaders() const {
    return headers_;
  }
  const string_type& getURL() const {
    return url_;
  }
  const string_type& getStatusString() const {
    return status_;
  }
  const string_type& getBody() const {
    return body_;
  }
  Maybe<string_type> getHeader(const string_type& headerName) const {
    Maybe<string_type> result;
    auto found = headers_.find(headerName);
    if (found != headers_.end()) {
      result.assign(found->second);
    }
    return result;
  }

};

}} // aliens::http
