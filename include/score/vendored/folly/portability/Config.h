#pragma once
#include <string>
// this is a stub config file for folly.
// since we don't care about windows, osx
// or old linux distros, we don't really
// need a more complicated `configure` script

#define SCORE_FOLLY_HAVE_BITS_FUNCTEXCEPT_H 1
#define SCORE_FOLLY_HAVE_SCHED_H 1

#define SCORE_FOLLY_HAVE_CPLUS_DEMANGLE_V3_CALLBACK 0
#define SCORE_FOLLY_DEMANGLE_MAX_SYMBOL_SIZE 128
#define SCORE_FOLLY_HAS_RTTI 1

using sf_string_t = std::string;

#define SCORE_FOLLY_NAMESPACE_STD_BEGIN \
  namespace std {


#define SCORE_FOLLY_NAMESPACE_STD_END \
  } // std


