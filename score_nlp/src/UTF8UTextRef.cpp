#include "score/nlp/UTF8UTextRef.h"
#include "score/macros.h"
#include <unicode/utext.h>
using namespace std;

namespace score { namespace nlp {

namespace detail {
  UText* openUText(const char *buff, size_t buffLen) {
    UErrorCode status = U_ZERO_ERROR;
    UText *utext = nullptr;
    utext = utext_openUTF8(utext, buff, buffLen, &status);
    SCHECK(U_SUCCESS(status));
    return utext;
  }
}

UTF8UTextRef::UTF8UTextRef(){}

UTF8UTextRef::UTF8UTextRef(const char *buff, size_t buffLen, UText *utextHandle)
  : buff_(buff), buffLen_(buffLen), utextHandle_(utextHandle) {}

UTF8UTextRef::UTF8UTextRef(UTF8UTextRef &&other)
  : buff_(other.buff_), buffLen_(other.buffLen_), utextHandle_(other.utextHandle_) {
  other.buff_ = nullptr;
  other.buffLen_ = 0;
  other.utextHandle_ = nullptr;
}

UTF8UTextRef& UTF8UTextRef::operator=(UTF8UTextRef &&other) {
  const char* swapBuff = buff_;
  buff_ = other.buff_;
  other.buff_ = swapBuff;
  size_t swapLen = buffLen_;
  buffLen_ = other.buffLen_;
  other.buffLen_ = swapLen;
  UText *swapUtext = utextHandle_;
  utextHandle_ = other.utextHandle_;
  other.utextHandle_ = swapUtext;
  return *this;
}

UTF8UTextRef UTF8UTextRef::fromUTF8(const char *buff, size_t buffLen) {
  UErrorCode status = U_ZERO_ERROR;
  UText *utext = nullptr;
  utext = utext_openUTF8(utext, buff, buffLen, &status);
  SCHECK(U_SUCCESS(status));
  return UTF8UTextRef(buff, buffLen, utext);
}

bool UTF8UTextRef::valid() const {
  return !!utextHandle_;
}

UTF8UTextRef::operator bool() const {
  return valid();
}

size_t UTF8UTextRef::size() const {
  return buffLen_;
}


void UTF8UTextRef::maybeClose() {
  if (utextHandle_) {
    utext_close(utextHandle_);
    utextHandle_ = nullptr;
  }
  buff_ = nullptr;
  buffLen_ = 0;
}

void UTF8UTextRef::assign(const char *buff, size_t buffLen) {
  maybeClose();
  buff_ = buff;
  buffLen_ = buffLen;
  utextHandle_ = detail::openUText(buff, buffLen);
}

UTF8UTextRef::~UTF8UTextRef() {
  maybeClose();
}

UText* UTF8UTextRef::getUText() {
  SDCHECK(valid());
  return utextHandle_;
}


UTF8UTextRef UTF8UTextRef::copyRef() const {
  return UTF8UTextRef::fromUTF8(buff_, buffLen_);
}

io::ByteStringPiece UTF8UTextRef::toByteStringPiece() const {
  return io::ByteStringPiece {buff_, buffLen_};
}


}} // score::nlp