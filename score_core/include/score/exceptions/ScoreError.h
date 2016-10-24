#pragma once
#include <stdexcept>
#include "score/format.h"


namespace score { namespace exceptions {


class ScoreError: public std::runtime_error {
 public:
  template<typename T>
  ScoreError(const T& msg): std::runtime_error(msg) {}

  template<typename TVal1>
  ScoreError(const char *fmt, const TVal1& val1)
    : std::runtime_error(score::tryFormat(fmt, val1)) {}

  template<typename TVal1, typename...TVals>
  ScoreError(const char *fmt, const TVal1& val1, TVals&&... vals)
    : std::runtime_error(score::tryFormat(fmt, val1, std::forward<TVals>(vals)...)) {}

  ScoreError(const char*);
};

}} // score::exceptions

#define SCORE_DECLARE_EXCEPTION(cls_name, base_cls) \
    class cls_name : public base_cls { \
     public: \
      template<typename ...Types> \
      cls_name(Types&&... args): base_cls(std::forward<Types>(args)...) {} \
    };