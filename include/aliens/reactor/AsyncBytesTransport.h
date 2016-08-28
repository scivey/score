#pragma once

namespace aliens { namespace reactor {

class AsyncBytesTransport {
 public:
  class ReadHandler {
   public:
    virtual void getReadBuffer(void** buffOut, size_t* lenOut) = 0;
    virtual void readDataAvailable(size_t len) = 0;
    virtual void readEOF() = 0;
    virtual void readError(const std::exception &err) = 0;
    virtual ~ReadHandler() = default;
  };

  class WriteCallback {
   public:
    virtual void writeSuccess() = 0;
    virtual void writeError(size_t nr, const std::exception &err) = 0;
    virtual ~WriteCallback() = default;
  };
  virtual void setReadHandler(ReadHandler*) = 0;
  virtual ReadHandler* getReadHandler() = 0;
  virtual void write(WriteCallback* cb, void* buff, size_t buffLen) = 0;
  virtual ~AsyncBytesTransport() = default;
};

}} // aliens::reactor