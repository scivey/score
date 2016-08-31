#pragma once

// this is a stub config file for folly.
// since we don't care about windows, osx
// or old linux distros, we don't really
// need a more complicated `configure` script

#define SCORE_FOLLY_HAVE_BITS_FUNCTEXCEPT_H 1
#define SCORE_FOLLY_HAVE_SCHED_H 1

#define SCORE_FOLLY_NAMESPACE_STD_BEGIN \
  namespace std {


#define SCORE_FOLLY_NAMESPACE_STD_END \
  } // std


