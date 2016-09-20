#include <string>
#include <boost/algorithm/string/case_conv.hpp>
#include <glog/logging.h>
#include "score/encodings/LibCharsetDetectorHandle.h"
#include "score/encodings/Encoding.h"
#include "score/encodings/detection.h"
#include "score/macros.h"

using namespace score::encodings;
using std::string;

namespace score { namespace encodings {

const char* detectEncodingStr(const string& text) {
  auto detectorHandle = LibCharsetDetectorHandle::create();
  const char *currentChunk = text.c_str();
  size_t totalSize = text.size();
  size_t chunkSize = 4096;
  size_t offset = 0;
  bool finished = false;
  while (!finished) {
    size_t realChunkSize = chunkSize;
    if (offset + chunkSize > totalSize) {
      realChunkSize = totalSize - offset;
    }
    finished = detectorHandle.consider(currentChunk, realChunkSize);
    if (finished) {
      break;
    }
    if (realChunkSize < chunkSize) {
      break;
    }
    currentChunk += chunkSize;
    offset += chunkSize;
    if (offset >= totalSize) {
      break;
    }
  }
  SCHECK(finished);
  return detectorHandle.csdClose();
}

Optional<Encoding> detectEncoding(const std::string& text) {
  std::string encodingName = detectEncodingStr(text);
  boost::algorithm::to_lower(encodingName);
  auto result = encodingOfString(encodingName);
  if (!result.hasValue()) {
    LOG(INFO) << "unknown encoding: " << encodingName;
  }
  return result;
}

}} // score::encodings