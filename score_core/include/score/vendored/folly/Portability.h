/*
 * Copyright 2016 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string.h>

#include <cstddef>

#include "score/vendored/folly/portability/Config.h"
#include "score/vendored/folly/CPortability.h"

#if SCORE_FOLLY_HAVE_SCHED_H
 #include <sched.h>
#endif

// Unaligned loads and stores
namespace score { namespace vendored { namespace folly {
#if SCORE_FOLLY_HAVE_UNALIGNED_ACCESS
constexpr bool kHasUnalignedAccess = true;
#else
constexpr bool kHasUnalignedAccess = false;
#endif
}}} // score::vendored::folly

// compiler specific attribute translation
// msvc should come first, so if clang is in msvc mode it gets the right defines

#if defined(__clang__) || defined(__GNUC__)
# define SCORE_FOLLY_ALIGNED(size) __attribute__((__aligned__(size)))
#elif defined(_MSC_VER)
# define SCORE_FOLLY_ALIGNED(size) __declspec(align(size))
#else
# error Cannot define SCORE_FOLLY_ALIGNED on this platform
#endif
#define SCORE_FOLLY_ALIGNED_MAX SCORE_FOLLY_ALIGNED(alignof(std::max_align_t))

// NOTE: this will only do checking in msvc with versions that support /analyze
#if _MSC_VER
# ifdef _USE_ATTRIBUTES_FOR_SAL
#    undef _USE_ATTRIBUTES_FOR_SAL
# endif
/* nolint */
# define _USE_ATTRIBUTES_FOR_SAL 1
# include <sal.h>
# define SCORE_FOLLY_PRINTF_FORMAT _Printf_format_string_
# define SCORE_FOLLY_PRINTF_FORMAT_ATTR(format_param, dots_param) /**/
#else
# define SCORE_FOLLY_PRINTF_FORMAT /**/
# define SCORE_FOLLY_PRINTF_FORMAT_ATTR(format_param, dots_param) \
  __attribute__((__format__(__printf__, format_param, dots_param)))
#endif

// deprecated
#if defined(__clang__) || defined(__GNUC__)
# define SCORE_FOLLY_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#elif defined(_MSC_VER)
# define SCORE_FOLLY_DEPRECATED(msg) __declspec(deprecated(msg))
#else
# define SCORE_FOLLY_DEPRECATED(msg)
#endif

// noinline
#ifdef _MSC_VER
# define SCORE_FOLLY_NOINLINE __declspec(noinline)
#elif defined(__clang__) || defined(__GNUC__)
# define SCORE_FOLLY_NOINLINE __attribute__((__noinline__))
#else
# define SCORE_FOLLY_NOINLINE
#endif

// always inline
#ifdef _MSC_VER
# define SCORE_FOLLY_ALWAYS_INLINE __forceinline
#elif defined(__clang__) || defined(__GNUC__)
# define SCORE_FOLLY_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
# define SCORE_FOLLY_ALWAYS_INLINE inline
#endif

// warn unused result
#if defined(_MSC_VER) && (_MSC_VER >= 1700)
#define SCORE_FOLLY_WARN_UNUSED_RESULT _Check_return_
#elif defined(__clang__) || defined(__GNUC__)
#define SCORE_FOLLY_WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#else
#define SCORE_FOLLY_WARN_UNUSED_RESULT
#endif

// target
#ifdef _MSC_VER
# define SCORE_FOLLY_TARGET_ATTRIBUTE(target)
#else
# define SCORE_FOLLY_TARGET_ATTRIBUTE(target) __attribute__((__target__(target)))
#endif

// detection for 64 bit
#if defined(__x86_64__) || defined(_M_X64)
# define SCORE_FOLLY_X64 1
#else
# define SCORE_FOLLY_X64 0
#endif

#if defined(__aarch64__)
# define SCORE_FOLLY_A64 1
#else
# define SCORE_FOLLY_A64 0
#endif

#if defined (__powerpc64__)
# define SCORE_FOLLY_PPC64 1
#else
# define SCORE_FOLLY_PPC64 0
#endif

// packing is very ugly in msvc
#ifdef _MSC_VER
# define SCORE_FOLLY_PACK_ATTR /**/
# define SCORE_FOLLY_PACK_PUSH __pragma(pack(push, 1))
# define SCORE_FOLLY_PACK_POP __pragma(pack(pop))
#elif defined(__clang__) || defined(__GNUC__)
# define SCORE_FOLLY_PACK_ATTR __attribute__((__packed__))
# define SCORE_FOLLY_PACK_PUSH /**/
# define SCORE_FOLLY_PACK_POP /**/
#else
# define SCORE_FOLLY_PACK_ATTR /**/
# define SCORE_FOLLY_PACK_PUSH /**/
# define SCORE_FOLLY_PACK_POP /**/
#endif

// Generalize warning push/pop.
#if defined(_MSC_VER)
# define SCORE_FOLLY_PUSH_WARNING __pragma(warning(push))
# define SCORE_FOLLY_POP_WARNING __pragma(warning(pop))
// Disable the GCC warnings.
# define SCORE_FOLLY_GCC_DISABLE_WARNING(warningName)
# define SCORE_FOLLY_MSVC_DISABLE_WARNING(warningNumber) __pragma(warning(disable: warningNumber))
#elif defined(__clang__) || defined(__GNUC__)
# define SCORE_FOLLY_PUSH_WARNING _Pragma("GCC diagnostic push")
# define SCORE_FOLLY_POP_WARNING _Pragma("GCC diagnostic pop")
#define SCORE_FOLLY_GCC_DISABLE_WARNING_INTERNAL3(warningName) #warningName
#define SCORE_FOLLY_GCC_DISABLE_WARNING_INTERNAL2(warningName) \
  SCORE_FOLLY_GCC_DISABLE_WARNING_INTERNAL3(warningName)
#define SCORE_FOLLY_GCC_DISABLE_WARNING(warningName)                       \
  _Pragma(SCORE_FOLLY_GCC_DISABLE_WARNING_INTERNAL2(GCC diagnostic ignored \
          SCORE_FOLLY_GCC_DISABLE_WARNING_INTERNAL3(-W##warningName)))
// Disable the MSVC warnings.
# define SCORE_FOLLY_MSVC_DISABLE_WARNING(warningNumber)
#else
# define SCORE_FOLLY_PUSH_WARNING
# define SCORE_FOLLY_POP_WARNING
# define SCORE_FOLLY_GCC_DISABLE_WARNING(warningName)
# define SCORE_FOLLY_MSVC_DISABLE_WARNING(warningNumber)
#endif

// portable version check
#ifndef __GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
/* nolint */
#  define __GNUC_PREREQ(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= \
                                   ((maj) << 16) + (min))
# else
/* nolint */
#  define __GNUC_PREREQ(maj, min) 0
# endif
#endif

#if defined(__GNUC__) && !defined(__APPLE__) && !__GNUC_PREREQ(4,9)
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56019
// gcc 4.8.x incorrectly placed max_align_t in the root namespace
// Alias it into std (where it's found in 4.9 and later)
namespace std { typedef ::max_align_t max_align_t; }
#endif

// portable version check for clang
#ifndef __CLANG_PREREQ
# if defined __clang__ && defined __clang_major__ && defined __clang_minor__
/* nolint */
#  define __CLANG_PREREQ(maj, min) \
    ((__clang_major__ << 16) + __clang_minor__ >= ((maj) << 16) + (min))
# else
/* nolint */
#  define __CLANG_PREREQ(maj, min) 0
# endif
#endif

/* Platform specific TLS support
 * gcc implements __thread
 * msvc implements __declspec(thread)
 * the semantics are the same
 * (but remember __thread has different semantics when using emutls (ex. apple))
 */
#if defined(_MSC_VER)
# define SCORE_FOLLY_TLS __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
# define SCORE_FOLLY_TLS __thread
#else
# error cannot define platform specific thread local storage
#endif

#if SCORE_FOLLY_MOBILE
#undef SCORE_FOLLY_TLS
#endif

// It turns out that GNU libstdc++ and LLVM libc++ differ on how they implement
// the 'std' namespace; the latter uses inline namespaces. Wrap this decision
// up in a macro to make forward-declarations easier.
#if SCORE_FOLLY_USE_LIBCPP
#include <__config>
#define SCORE_FOLLY_NAMESPACE_STD_BEGIN     _LIBCPP_BEGIN_NAMESPACE_STD
#define SCORE_FOLLY_NAMESPACE_STD_END       _LIBCPP_END_NAMESPACE_STD
#else
#define SCORE_FOLLY_NAMESPACE_STD_BEGIN     namespace std {
#define SCORE_FOLLY_NAMESPACE_STD_END       }
#endif

// If the new c++ ABI is used, __cxx11 inline namespace needs to be added to
// some types, e.g. std::list.
#if _GLIBCXX_USE_CXX11_ABI
#define SCORE_FOLLY_GLIBCXX_NAMESPACE_CXX11_BEGIN \
  inline _GLIBCXX_BEGIN_NAMESPACE_CXX11
# define SCORE_FOLLY_GLIBCXX_NAMESPACE_CXX11_END   _GLIBCXX_END_NAMESPACE_CXX11
#else
# define SCORE_FOLLY_GLIBCXX_NAMESPACE_CXX11_BEGIN
# define SCORE_FOLLY_GLIBCXX_NAMESPACE_CXX11_END
#endif


// Debug
namespace folly {
#ifdef NDEBUG
constexpr auto kIsDebug = false;
#else
constexpr auto kIsDebug = true;
#endif
}

// Endianness
namespace folly {
#ifdef _MSC_VER
// It's MSVC, so we just have to guess ... and allow an override
#ifdef SCORE_FOLLY_ENDIAN_BE
constexpr auto kIsLittleEndian = false;
#else
constexpr auto kIsLittleEndian = true;
#endif
#else
constexpr auto kIsLittleEndian = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
#endif
constexpr auto kIsBigEndian = !kIsLittleEndian;
}

#ifndef SCORE_FOLLY_SSE
# if defined(__SSE4_2__)
#  define SCORE_FOLLY_SSE 4
#  define SCORE_FOLLY_SSE_MINOR 2
# elif defined(__SSE4_1__)
#  define SCORE_FOLLY_SSE 4
#  define SCORE_FOLLY_SSE_MINOR 1
# elif defined(__SSE4__)
#  define SCORE_FOLLY_SSE 4
#  define SCORE_FOLLY_SSE_MINOR 0
# elif defined(__SSE3__)
#  define SCORE_FOLLY_SSE 3
#  define SCORE_FOLLY_SSE_MINOR 0
# elif defined(__SSE2__)
#  define SCORE_FOLLY_SSE 2
#  define SCORE_FOLLY_SSE_MINOR 0
# elif defined(__SSE__)
#  define SCORE_FOLLY_SSE 1
#  define SCORE_FOLLY_SSE_MINOR 0
# else
#  define SCORE_FOLLY_SSE 0
#  define SCORE_FOLLY_SSE_MINOR 0
# endif
#endif

#define SCORE_FOLLY_SSE_PREREQ(major, minor) \
  (SCORE_FOLLY_SSE > major || SCORE_FOLLY_SSE == major && SCORE_FOLLY_SSE_MINOR >= minor)

#if SCORE_FOLLY_UNUSUAL_GFLAGS_NAMESPACE
namespace SCORE_FOLLY_GFLAGS_NAMESPACE { }
namespace gflags {
using namespace SCORE_FOLLY_GFLAGS_NAMESPACE;
}  // namespace gflags
#endif

// for TARGET_OS_IPHONE
#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

// RTTI may not be enabled for this compilation unit.
#if defined(__GXX_RTTI) || defined(__cpp_rtti) || \
    (defined(_MSC_VER) && defined(_CPPRTTI))
# define SCORE_FOLLY_HAS_RTTI 1
#endif

#if defined(__APPLE__) || defined(_MSC_VER)
#define SCORE_FOLLY_STATIC_CTOR_PRIORITY_MAX
#else
// 101 is the highest priority allowed by the init_priority attribute.
// This priority is already used by JEMalloc and other memory allocators so
// we will take the next one.
#define SCORE_FOLLY_STATIC_CTOR_PRIORITY_MAX __attribute__((__init_priority__(102)))
#endif
namespace score { namespace vendored { namespace folly {

#if defined(__linux__) && !SCORE_FOLLY_MOBILE
constexpr auto kIsLinux = true;
#else
constexpr auto kIsLinux = false;
#endif
}}} // score::vendored::folly
