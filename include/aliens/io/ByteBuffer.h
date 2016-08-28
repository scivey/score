#pragma once
#include <string>
#include <cstring>
namespace aliens {

namespace detail {
std::string buffToString(char *buff, size_t buffLen);
}

class ByteBuffer {
 public:
  virtual ~ByteBuffer() = default;
  virtual void* data() = 0;
  virtual void zero() = 0;
  virtual size_t len() = 0;
  virtual size_t capacity() = 0;
  virtual size_t spaceRemaining() = 0;

  FixedBuffer() {
    zero();
  }
  char* body() {
    return (char*) data_;
  }
  void zero() {
    memset((char*) data_, '\0', NBytes);
  }
  size_t currentLen() const {
    return strlen(data_);
  }
  size_t remaining() const {
    return NBytes - currentLen();
  }
  size_t capacity() const {
    return NBytes;
  }
  std::string copyToString() {
    return detail::buffToString(body(), currentLen());
  }
  void fillWith(const std::string &text) {
    if (text.size() == 0) {
      data_[0] = '\0';
    } else {
      auto nbytes = std::min(NBytes-1, text.size());
      memcpy((void*) data_, (void*) text.c_str(), nbytes);
      data_[nbytes] = '\0';
    }
  }
};

using Buffer = FixedBuffer<1024>;

} // aliens
