#pragma once
#include <errno.h>

#include "score/vendored/folly/Likely.h"
#include "score/macros/assertions.h"

#define SCORE_XSTR(x) #x
#define SCORE_STR(x) SCORE_XSTR(x)

#define SCORE_THROW0(cls_name) \
    throw cls_name(SCORE_XSTR(cls_name))

#define SCORE_UNUSED(x) ((void) x);

#define SCORE_DISABLE_COPY_AND_ASSIGN(cls) \
  cls(const cls&) = delete; \
  cls& operator=(const cls&) = delete;


#define SCORE_DISABLE_MOVE(cls) \
  cls(cls&&) = delete; \
  cls& operator=(cls&&) = delete;

