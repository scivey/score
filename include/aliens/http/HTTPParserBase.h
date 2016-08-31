#include <tuple>
#include <map>
#include "aliens/io/BytesView.h"
#include "aliens/macros.h"
#include "aliens/vendored/http-parser/http_parser.h"


#pragma once

namespace aliens { namespace http {

template<typename TImpl>
class HTTPParserBase {
 public:
  using string_view_t = aliens::io::BytesView<char>;
  enum class ParserType {
    REQUEST, RESPONSE, BOTH
  };


 protected:
  static http_parser_type cParserTypeOfParserType(ParserType ptype) {
    switch(ptype) {
      case ParserType::REQUEST:
        return HTTP_REQUEST;
      case ParserType::RESPONSE:
        return HTTP_RESPONSE;
      case ParserType::BOTH:
        return HTTP_BOTH;
    }
    // not reached
    return HTTP_BOTH;
  }

  http_parser_settings parserSettings_;
  http_parser parser_;
  bool isComplete_ {false};
  ParserType parserType_;

  TImpl* getThis() const {
    return static_cast<TImpl*>(this);
  }
  void initialize() {
    isComplete_ = false;
    http_parser_settings_init(&parserSettings_);
    parserSettings_.on_message_begin = HTTPParserBase::pOnMessageBegin;
    parserSettings_.on_url = HTTPParserBase::pOnURL;
    parserSettings_.on_status = HTTPParserBase::pOnStatus;
    parserSettings_.on_header_field = HTTPParserBase::pOnHeaderField;
    parserSettings_.on_header_value = HTTPParserBase::pOnHeaderValue;
    parserSettings_.on_headers_complete = HTTPParserBase::pOnHeadersComplete;
    parserSettings_.on_body = HTTPParserBase::pOnBody;
    parserSettings_.on_chunk_header = HTTPParserBase::pOnChunkHeader;
    parserSettings_.on_chunk_complete = HTTPParserBase::pOnChunkComplete;
    parserSettings_.on_message_complete = HTTPParserBase::pOnMessageComplete;
    http_parser_init(&parser_, cParserTypeOfParserType(parserType_));
    parser_.data = (void*) this;
  }
  static TImpl* instanceFromParser(http_parser *parser) {
    return static_cast<TImpl*>(parser->data);
  }

  // http_cb methods
  static int pOnMessageBegin(http_parser *parser) {
    return instanceFromParser(parser)->onMessageBegin();
  }
  static int pOnMessageComplete(http_parser *parser) {
    auto instance = instanceFromParser(parser);
    instance->isComplete_ = true;
    return instance->onMessageComplete();
  }
  static int pOnChunkHeader(http_parser *parser) {
    return instanceFromParser(parser)->onChunkHeader();
  }
  static int pOnChunkComplete(http_parser *parser) {
    return instanceFromParser(parser)->onChunkComplete();
  }
  static int pOnHeadersComplete(http_parser *parser) {
    return instanceFromParser(parser)->onHeadersComplete();
  }

  // http_data_cb methods
  static int pOnHeaderField(http_parser *parser, const char* data, size_t dataLen) {
    return instanceFromParser(parser)->onHeaderField(string_view_t(data, dataLen));
  }
  static int pOnHeaderValue(http_parser *parser, const char* data, size_t dataLen) {
    return instanceFromParser(parser)->onHeaderValue(string_view_t(data, dataLen));
  }
  static int pOnBody(http_parser *parser, const char* data, size_t dataLen) {
    return instanceFromParser(parser)->onBody(string_view_t(data, dataLen));
  }
  static int pOnURL(http_parser *parser, const char* data, size_t dataLen) {
    return instanceFromParser(parser)->onURL(string_view_t(data, dataLen));
  }
  static int pOnStatus(http_parser *parser, const char* data, size_t dataLen) {
    return instanceFromParser(parser)->onStatus(string_view_t(data, dataLen));
  }
 public:

  HTTPParserBase(ParserType parserType): parserType_(parserType) {
    initialize();
  }
  bool isComplete() const {
    return isComplete_;
  }
  size_t feed(const char *data, size_t dataLen) {
    return http_parser_execute(&parser_, &parserSettings_, data, dataLen);
  }

  template<typename TString>
  size_t feed(const TString &responseStr) {
    return feed(responseStr.data(), responseStr.size());
  }
};

}} // aliens::http
