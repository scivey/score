#pragma once
#include <string>
#include "score/io/ByteStringPiece.h"

struct UText;

namespace score { namespace nlp {

// UTF8UTextRef wraps icu's UText with RAII semantics.
// It keeps a non-owned reference to a char* buffer,
// which should be utf8-encoded, and allows a subset
// of ICU methods to operate directly on that buffer
// without copying to a UTF16-encoded `icu::UnicodeString`.

class UTF8UTextRef {
 protected:
  const char *buff_ {nullptr};
  size_t buffLen_ {0};
  UText *utextHandle_ {nullptr};

  // takes ownership of Utexthandle
  UTF8UTextRef(const char*, size_t, UText*);

  UTF8UTextRef(const UTF8UTextRef&) = delete;
  UTF8UTextRef& operator=(const UTF8UTextRef&) = delete;

  void maybeClose();
 public:
  UTF8UTextRef();
  UTF8UTextRef(UTF8UTextRef &&other);
  UTF8UTextRef& operator=(UTF8UTextRef &&other);
  bool valid() const;
  operator bool() const;
  size_t size() const;
  static UTF8UTextRef fromUTF8(const char*, size_t);

  template<typename TStr>
  static UTF8UTextRef fromUTF8(const TStr &strRef) {
    return fromUTF8(strRef.c_str(), strRef.size());
  }

  void assign(const char*, size_t);

  UTF8UTextRef copyRef() const;

  template<typename TStr>
  void assign(const TStr &strRef) {
    assign(strRef.c_str(), strRef.size());
  }

  UText* getUText();
  const char* data() const;

  io::ByteStringPiece toByteStringPiece() const;

  ~UTF8UTextRef();
};


namespace detail {
  UText* openUtext(const char*, size_t);
}


}} // score::nlp