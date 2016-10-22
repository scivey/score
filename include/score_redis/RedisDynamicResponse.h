#pragma once

#include <string>
#include <vector>
#include "score/Try.h"

struct redisReply;

namespace score { namespace redis {

class RedisDynamicResponse {
 public:
  enum class ResponseType {
    STATUS, ERROR, INTEGER, NIL, STRING, ARRAY
  };
  using response_vector_t = std::vector<RedisDynamicResponse>;
  using try_array_t = score::Try<response_vector_t>;
  using string_t = std::string;
  using try_string_t = score::Try<string_t>;
  using try_int_t = score::Try<int64_t>;
 protected:
  redisReply *hiredisReply_ {nullptr};
  string_t toStringUnchecked();
 public:
  RedisDynamicResponse(redisReply *redisRep);
  RedisDynamicResponse(const RedisDynamicResponse &other);
  RedisDynamicResponse& operator=(const RedisDynamicResponse &other);

  bool isType(ResponseType resType) const;
  score::Try<const char*> getTypeString() const;
  score::Try<ResponseType> getType() const;
  bool isNil() const;
  try_string_t getString();
  try_string_t getStatusString();
  try_string_t getErrorString();
  try_int_t getInt();
  try_array_t getArray();

  void pprintTo(std::ostream &oss);
  string_t pprint();
};

namespace detail {
RedisDynamicResponse::ResponseType responseTypeOfIntExcept(int);
score::Try<RedisDynamicResponse::ResponseType> responseTypeOfInt(int);
const char* stringOfResponseType(RedisDynamicResponse::ResponseType);
}

}} // score::redis
