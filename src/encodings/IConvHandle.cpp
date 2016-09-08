#include "score/encodings/IConvHandle.h"
#include <iconv.h>
#include <glog/logging.h>
namespace score { namespace encodings {

IConvHandle::IConvHandle(IConvHandle &&other)
  : handle_(other.handle_), options_(other.options_) {
  other.handle_ = nullptr;
}

IConvHandle& IConvHandle::operator=(IConvHandle &&other) {
  void *swapHandle = handle_;
  IConvOptions swapOptions = options_;
  handle_ = other.handle_;
  options_ = other.options_;
  other.handle_ = swapHandle;
  other.options_ = swapOptions;
  return *this;

}

IConvHandle::IConvHandle(const IConvOptions &options)
  : options_(options) {}
IConvHandle::IConvHandle(void *handle, const IConvOptions &options)
  : handle_(handle), options_(options) {}

IConvHandle IConvHandle::create(const IConvOptions &options) {
  IConvHandle instance(options);
  auto toStr = stringOfEncoding(options.toEncoding);
  auto fromStr = stringOfEncoding(options.fromEncoding);
  instance.handle_ = iconv_open(toStr.c_str(), fromStr.c_str());
  if ( ( (int64_t) instance.handle_ ) <= 0 ) {
    THROW_ICONV_ERR(errno);
  }
  return instance;
}

const IConvOptions& IConvHandle::getOptions() const {
  return options_;
}

void IConvHandle::maybeClose() {
  if (isValid()) {
    iconv_close(handle_);
    handle_ = nullptr;
  }
}

void IConvHandle::feed(IConvFeedParams *params) {
  size_t nr = iconv(handle_,
    &params->inBuff, &params->inBytesLeft,
    &params->outBuff, &params->outBytesLeft
  );
  const size_t kIConvErrorValue = (size_t)(-1);
  if (nr == kIConvErrorValue) {
    THROW_ICONV_ERR(errno);
  }
}

bool IConvHandle::isValid() const {
  return handle_ != nullptr;
}

IConvHandle::~IConvHandle() {
  maybeClose();
}

}} // score::encodings

