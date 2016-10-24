#include "score/encodings/IConverter.h"
#include "score/encodings/HeapBufferHandle.h"

#include "score/macros.h"

namespace score { namespace encodings {

IConverter::IConverter(IConverter &&other)
  : handle_(std::move(other.handle_)) {}

IConverter& IConverter::operator=(IConverter &&other) {
  IConvHandle tempHandle = std::move(handle_);
  handle_ = std::move(other.handle_);
  if (tempHandle.isValid()) {
    other.handle_ = std::move(tempHandle);
  }
  return *this;
}

IConverter::IConverter(IConvHandle &&handle)
  : handle_(std::forward<IConvHandle>(handle)) {}

IConverter IConverter::create(const IConvOptions &options) {
  return IConverter(IConvHandle::create(options));
}

std::string IConverter::convert(const std::string &text) {
  SDCHECK(handle_.isValid());
  if (text.size() == 0) {
    return "";
  }
  const size_t outBuffSize = text.size() * 4;
  auto buff = HeapBufferHandle<char>::create(outBuffSize);
  IConvFeedParams params;
  params.inBuff = (char*) text.c_str();
  params.inBytesLeft = text.size();
  params.outBuff = buff.data();
  params.outBytesLeft = buff.totalCapacity();
  handle_.feed(&params);
  SCHECK(params.inBytesLeft == 0);
  size_t nWritten = buff.totalCapacity() - params.outBytesLeft;
  std::string result = buff.data();
  if (nWritten > 0) {
    result.resize(nWritten);
  }
  return result;
}


}} // expr2::encodings

