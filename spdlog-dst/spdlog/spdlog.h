

// cubao/spdlog-amalgamate: spdlog-src/spdlog/spdlog.h
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

// spdlog main header file.
// see example.cpp for usage example

#ifndef SPDLOG_H
#define SPDLOG_H

#pragma once

// #include <spdlog/common.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/tweakme.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



///////////////////////////////////////////////////////////////////////////////
//
// Edit this file to squeeze more performance, and to customize supported
// features
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Under Linux, the much faster CLOCK_REALTIME_COARSE clock can be used.
// This clock is less accurate - can be off by dozens of millis - depending on
// the kernel HZ.
// Uncomment to use it instead of the regular clock.
//
// #define SPDLOG_CLOCK_COARSE
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment if source location logging is not needed.
// This will prevent spdlog from using __FILE__, __LINE__ and SPDLOG_FUNCTION
//
// #define SPDLOG_NO_SOURCE_LOC
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment if thread id logging is not needed (i.e. no %t in the log pattern).
// This will prevent spdlog from querying the thread id on each log call.
//
// WARNING: If the log pattern contains thread id (i.e, %t) while this flag is
// on, zero will be logged as thread id.
//
// #define SPDLOG_NO_THREAD_ID
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to prevent spdlog from using thread local storage.
//
// WARNING: if your program forks, UNCOMMENT this flag to prevent undefined
// thread ids in the children logs.
//
// #define SPDLOG_NO_TLS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to avoid spdlog's usage of atomic log levels
// Use only if your code never modifies a logger's log levels concurrently by
// different threads.
//
// #define SPDLOG_NO_ATOMIC_LEVELS
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to enable usage of wchar_t for file names on Windows.
//
// #define SPDLOG_WCHAR_FILENAMES
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to override default eol ("\n" or "\r\n" under Linux/Windows)
//
// #define SPDLOG_EOL ";-)\n"
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to override default folder separators ("/" or "\\/" under
// Linux/Windows). Each character in the string is treated as a different
// separator.
//
// #define SPDLOG_FOLDER_SEPS "\\"
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to use your own copy of the fmt library instead of spdlog's copy.
// In this case spdlog will try to include <fmt/format.h> so set your -I flag
// accordingly.
//
// #define SPDLOG_FMT_EXTERNAL
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to use C++20 std::format instead of fmt. This removes compile
// time checking of format strings, but doesn't depend on the fmt library.
//
// #define SPDLOG_USE_STD_FORMAT
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to enable wchar_t support (convert to utf8)
//
// #define SPDLOG_WCHAR_TO_UTF8_SUPPORT
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to prevent child processes from inheriting log file descriptors
//
// #define SPDLOG_PREVENT_CHILD_FD
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to customize level names (e.g. "MY TRACE")
//
// #define SPDLOG_LEVEL_NAMES { "MY TRACE", "MY DEBUG", "MY INFO", "MY WARNING", "MY ERROR", "MY CRITICAL", "OFF" }
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to customize short level names (e.g. "MT")
// These can be longer than one character.
//
// #define SPDLOG_SHORT_LEVEL_NAMES { "T", "D", "I", "W", "E", "C", "O" }
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment to disable default logger creation.
// This might save some (very) small initialization time if no default logger is needed.
//
// #define SPDLOG_DISABLE_DEFAULT_LOGGER
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment and set to compile time level with zero cost (default is INFO).
// Macros like SPDLOG_DEBUG(..), SPDLOG_INFO(..)  will expand to empty statements if not enabled
//
// #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Uncomment (and change if desired) macro to use for function names.
// This is compiler dependent.
// __PRETTY_FUNCTION__ might be nicer in clang/gcc, and __FUNCTION__ in msvc.
// Defaults to __FUNCTION__ (should work on all compilers) if not defined.
//
// #ifdef __PRETTY_FUNCTION__
// # define SPDLOG_FUNCTION __PRETTY_FUNCTION__
// #else
// # define SPDLOG_FUNCTION __FUNCTION__
// #endif
///////////////////////////////////////////////////////////////////////////////

// #include <spdlog/details/null_mutex.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#include <atomic>
#include <utility>
// null, no cost dummy "mutex" and dummy "atomic" int

namespace spdlog {
namespace details {
struct null_mutex
{
    void lock() const {}
    void unlock() const {}
};

struct null_atomic_int
{
    int value;
    null_atomic_int() = default;

    explicit null_atomic_int(int new_value)
        : value(new_value)
    {}

    int load(std::memory_order = std::memory_order_relaxed) const
    {
        return value;
    }

    void store(int new_value, std::memory_order = std::memory_order_relaxed)
    {
        value = new_value;
    }

    int exchange(int new_value, std::memory_order = std::memory_order_relaxed)
    {
        std::swap(new_value, value);
        return new_value; // return value before the call
    }
};

} // namespace details
} // namespace spdlog


#include <atomic>
#include <chrono>
#include <initializer_list>
#include <memory>
#include <exception>
#include <string>
#include <type_traits>
#include <functional>
#include <cstdio>

#ifdef SPDLOG_USE_STD_FORMAT
#    include <string_view>
#endif

#ifdef SPDLOG_COMPILED_LIB
#    undef SPDLOG_HEADER_ONLY
#    if defined(SPDLOG_SHARED_LIB)
#        if defined(_WIN32)
#            ifdef spdlog_EXPORTS
#                define SPDLOG_API __declspec(dllexport)
#            else // !spdlog_EXPORTS
#                define SPDLOG_API __declspec(dllimport)
#            endif
#        else // !defined(_WIN32)
#            define SPDLOG_API __attribute__((visibility("default")))
#        endif
#    else // !defined(SPDLOG_SHARED_LIB)
#        define SPDLOG_API
#    endif
#    define SPDLOG_INLINE
#else // !defined(SPDLOG_COMPILED_LIB)
#    define SPDLOG_API
#    define SPDLOG_HEADER_ONLY
#    define SPDLOG_INLINE inline
#endif // #ifdef SPDLOG_COMPILED_LIB

// #include <spdlog/fmt/fmt.h>
//
// Copyright(c) 2016-2018 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//



//
// Include a bundled header-only copy of fmtlib or an external one.
// By default spdlog include its own copy.
//

#if defined(SPDLOG_USE_STD_FORMAT) // SPDLOG_USE_STD_FORMAT is defined - use std::format
#    include <format>
#elif !defined(SPDLOG_FMT_EXTERNAL)
#    if !defined(SPDLOG_COMPILED_LIB) && !defined(FMT_HEADER_ONLY)
#        define FMT_HEADER_ONLY
#    endif
#    ifndef FMT_USE_WINDOWS_H
#        define FMT_USE_WINDOWS_H 0
#    endif
// enable the 'n' flag in for backward compatibility with fmt 6.x
#    define FMT_DEPRECATED_N_SPECIFIER
// #    include <spdlog/fmt/bundled/core.h>
// Formatting library for C++ - the core API for char/UTF-8
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_CORE_H_
#define FMT_CORE_H_

#include <cstddef>  // std::byte
#include <cstdio>   // std::FILE
#include <cstring>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>

// The fmt library version in the form major * 10000 + minor * 100 + patch.
#define FMT_VERSION 80101

#if defined(__clang__) && !defined(__ibmxl__)
#  define FMT_CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#else
#  define FMT_CLANG_VERSION 0
#endif

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER) && \
    !defined(__NVCOMPILER)
#  define FMT_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#  define FMT_GCC_VERSION 0
#endif

#ifndef FMT_GCC_PRAGMA
// Workaround _Pragma bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59884.
#  if FMT_GCC_VERSION >= 504
#    define FMT_GCC_PRAGMA(arg) _Pragma(arg)
#  else
#    define FMT_GCC_PRAGMA(arg)
#  endif
#endif

#ifdef __ICL
#  define FMT_ICC_VERSION __ICL
#elif defined(__INTEL_COMPILER)
#  define FMT_ICC_VERSION __INTEL_COMPILER
#else
#  define FMT_ICC_VERSION 0
#endif

#ifdef __NVCC__
#  define FMT_NVCC __NVCC__
#else
#  define FMT_NVCC 0
#endif

#ifdef _MSC_VER
#  define FMT_MSC_VER _MSC_VER
#  define FMT_MSC_WARNING(...) __pragma(warning(__VA_ARGS__))
#else
#  define FMT_MSC_VER 0
#  define FMT_MSC_WARNING(...)
#endif

#ifdef __has_feature
#  define FMT_HAS_FEATURE(x) __has_feature(x)
#else
#  define FMT_HAS_FEATURE(x) 0
#endif

#if defined(__has_include) &&                             \
    (!defined(__INTELLISENSE__) || FMT_MSC_VER > 1900) && \
    (!FMT_ICC_VERSION || FMT_ICC_VERSION >= 1600)
#  define FMT_HAS_INCLUDE(x) __has_include(x)
#else
#  define FMT_HAS_INCLUDE(x) 0
#endif

#ifdef __has_cpp_attribute
#  define FMT_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define FMT_HAS_CPP_ATTRIBUTE(x) 0
#endif

#ifdef _MSVC_LANG
#  define FMT_CPLUSPLUS _MSVC_LANG
#else
#  define FMT_CPLUSPLUS __cplusplus
#endif

#define FMT_HAS_CPP14_ATTRIBUTE(attribute) \
  (FMT_CPLUSPLUS >= 201402L && FMT_HAS_CPP_ATTRIBUTE(attribute))

#define FMT_HAS_CPP17_ATTRIBUTE(attribute) \
  (FMT_CPLUSPLUS >= 201703L && FMT_HAS_CPP_ATTRIBUTE(attribute))

// Check if relaxed C++14 constexpr is supported.
// GCC doesn't allow throw in constexpr until version 6 (bug 67371).
#ifndef FMT_USE_CONSTEXPR
#  define FMT_USE_CONSTEXPR                                           \
    (FMT_HAS_FEATURE(cxx_relaxed_constexpr) || FMT_MSC_VER >= 1912 || \
     (FMT_GCC_VERSION >= 600 && __cplusplus >= 201402L)) &&           \
        !FMT_NVCC && !FMT_ICC_VERSION
#endif
#if FMT_USE_CONSTEXPR
#  define FMT_CONSTEXPR constexpr
#  define FMT_CONSTEXPR_DECL constexpr
#else
#  define FMT_CONSTEXPR
#  define FMT_CONSTEXPR_DECL
#endif

#if ((__cplusplus >= 202002L) &&                              \
     (!defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE > 9)) || \
    (__cplusplus >= 201709L && FMT_GCC_VERSION >= 1002)
#  define FMT_CONSTEXPR20 constexpr
#else
#  define FMT_CONSTEXPR20
#endif

// Check if constexpr std::char_traits<>::compare,length is supported.
#if defined(__GLIBCXX__)
#  if __cplusplus >= 201703L && defined(_GLIBCXX_RELEASE) && \
      _GLIBCXX_RELEASE >= 7  // GCC 7+ libstdc++ has _GLIBCXX_RELEASE.
#    define FMT_CONSTEXPR_CHAR_TRAITS constexpr
#  endif
#elif defined(_LIBCPP_VERSION) && __cplusplus >= 201703L && \
    _LIBCPP_VERSION >= 4000
#  define FMT_CONSTEXPR_CHAR_TRAITS constexpr
#elif FMT_MSC_VER >= 1914 && _MSVC_LANG >= 201703L
#  define FMT_CONSTEXPR_CHAR_TRAITS constexpr
#endif
#ifndef FMT_CONSTEXPR_CHAR_TRAITS
#  define FMT_CONSTEXPR_CHAR_TRAITS
#endif

// Check if exceptions are disabled.
#ifndef FMT_EXCEPTIONS
#  if (defined(__GNUC__) && !defined(__EXCEPTIONS)) || \
      FMT_MSC_VER && !_HAS_EXCEPTIONS
#    define FMT_EXCEPTIONS 0
#  else
#    define FMT_EXCEPTIONS 1
#  endif
#endif

// Define FMT_USE_NOEXCEPT to make fmt use noexcept (C++11 feature).
#ifndef FMT_USE_NOEXCEPT
#  define FMT_USE_NOEXCEPT 0
#endif

#if FMT_USE_NOEXCEPT || FMT_HAS_FEATURE(cxx_noexcept) || \
    FMT_GCC_VERSION >= 408 || FMT_MSC_VER >= 1900
#  define FMT_DETECTED_NOEXCEPT noexcept
#  define FMT_HAS_CXX11_NOEXCEPT 1
#else
#  define FMT_DETECTED_NOEXCEPT throw()
#  define FMT_HAS_CXX11_NOEXCEPT 0
#endif

#ifndef FMT_NOEXCEPT
#  if FMT_EXCEPTIONS || FMT_HAS_CXX11_NOEXCEPT
#    define FMT_NOEXCEPT FMT_DETECTED_NOEXCEPT
#  else
#    define FMT_NOEXCEPT
#  endif
#endif

// [[noreturn]] is disabled on MSVC and NVCC because of bogus unreachable code
// warnings.
#if FMT_EXCEPTIONS && FMT_HAS_CPP_ATTRIBUTE(noreturn) && !FMT_MSC_VER && \
    !FMT_NVCC
#  define FMT_NORETURN [[noreturn]]
#else
#  define FMT_NORETURN
#endif

#if __cplusplus == 201103L || __cplusplus == 201402L
#  if defined(__INTEL_COMPILER) || defined(__PGI)
#    define FMT_FALLTHROUGH
#  elif defined(__clang__)
#    define FMT_FALLTHROUGH [[clang::fallthrough]]
#  elif FMT_GCC_VERSION >= 700 && \
      (!defined(__EDG_VERSION__) || __EDG_VERSION__ >= 520)
#    define FMT_FALLTHROUGH [[gnu::fallthrough]]
#  else
#    define FMT_FALLTHROUGH
#  endif
#elif FMT_HAS_CPP17_ATTRIBUTE(fallthrough)
#  define FMT_FALLTHROUGH [[fallthrough]]
#else
#  define FMT_FALLTHROUGH
#endif

#ifndef FMT_NODISCARD
#  if FMT_HAS_CPP17_ATTRIBUTE(nodiscard)
#    define FMT_NODISCARD [[nodiscard]]
#  else
#    define FMT_NODISCARD
#  endif
#endif

#ifndef FMT_USE_FLOAT
#  define FMT_USE_FLOAT 1
#endif
#ifndef FMT_USE_DOUBLE
#  define FMT_USE_DOUBLE 1
#endif
#ifndef FMT_USE_LONG_DOUBLE
#  define FMT_USE_LONG_DOUBLE 1
#endif

#ifndef FMT_INLINE
#  if FMT_GCC_VERSION || FMT_CLANG_VERSION
#    define FMT_INLINE inline __attribute__((always_inline))
#  else
#    define FMT_INLINE inline
#  endif
#endif

#ifndef FMT_DEPRECATED
#  if FMT_HAS_CPP14_ATTRIBUTE(deprecated) || FMT_MSC_VER >= 1900
#    define FMT_DEPRECATED [[deprecated]]
#  else
#    if (defined(__GNUC__) && !defined(__LCC__)) || defined(__clang__)
#      define FMT_DEPRECATED __attribute__((deprecated))
#    elif FMT_MSC_VER
#      define FMT_DEPRECATED __declspec(deprecated)
#    else
#      define FMT_DEPRECATED /* deprecated */
#    endif
#  endif
#endif

#ifndef FMT_BEGIN_NAMESPACE
#  define FMT_BEGIN_NAMESPACE \
    namespace fmt {           \
    inline namespace v8 {
#  define FMT_END_NAMESPACE \
    }                       \
    }
#endif

#ifndef FMT_MODULE_EXPORT
#  define FMT_MODULE_EXPORT
#  define FMT_MODULE_EXPORT_BEGIN
#  define FMT_MODULE_EXPORT_END
#  define FMT_BEGIN_DETAIL_NAMESPACE namespace detail {
#  define FMT_END_DETAIL_NAMESPACE }
#endif

#if !defined(FMT_HEADER_ONLY) && defined(_WIN32)
#  define FMT_CLASS_API FMT_MSC_WARNING(suppress : 4275)
#  ifdef FMT_EXPORT
#    define FMT_API __declspec(dllexport)
#  elif defined(FMT_SHARED)
#    define FMT_API __declspec(dllimport)
#  endif
#else
#  define FMT_CLASS_API
#  if defined(FMT_EXPORT) || defined(FMT_SHARED)
#    if defined(__GNUC__) || defined(__clang__)
#      define FMT_API __attribute__((visibility("default")))
#    endif
#  endif
#endif
#ifndef FMT_API
#  define FMT_API
#endif

// libc++ supports string_view in pre-c++17.
#if (FMT_HAS_INCLUDE(<string_view>) &&                       \
     (__cplusplus > 201402L || defined(_LIBCPP_VERSION))) || \
    (defined(_MSVC_LANG) && _MSVC_LANG > 201402L && _MSC_VER >= 1910)
#  include <string_view>
#  define FMT_USE_STRING_VIEW
#elif FMT_HAS_INCLUDE("experimental/string_view") && __cplusplus >= 201402L
#  include <experimental/string_view>
#  define FMT_USE_EXPERIMENTAL_STRING_VIEW
#endif

#ifndef FMT_UNICODE
#  define FMT_UNICODE !FMT_MSC_VER
#endif

#ifndef FMT_CONSTEVAL
#  if ((FMT_GCC_VERSION >= 1000 || FMT_CLANG_VERSION >= 1101) &&      \
       __cplusplus > 201703L && !defined(__apple_build_version__)) || \
      (defined(__cpp_consteval) &&                                    \
       (!FMT_MSC_VER || _MSC_FULL_VER >= 193030704))
// consteval is broken in MSVC before VS2022 and Apple clang 13.
#    define FMT_CONSTEVAL consteval
#    define FMT_HAS_CONSTEVAL
#  else
#    define FMT_CONSTEVAL
#  endif
#endif

#ifndef FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
#  if defined(__cpp_nontype_template_args) &&                \
      ((FMT_GCC_VERSION >= 903 && __cplusplus >= 201709L) || \
       __cpp_nontype_template_args >= 201911L)
#    define FMT_USE_NONTYPE_TEMPLATE_PARAMETERS 1
#  else
#    define FMT_USE_NONTYPE_TEMPLATE_PARAMETERS 0
#  endif
#endif

// Enable minimal optimizations for more compact code in debug mode.
FMT_GCC_PRAGMA("GCC push_options")
#ifndef __OPTIMIZE__
FMT_GCC_PRAGMA("GCC optimize(\"Og\")")
#endif

FMT_BEGIN_NAMESPACE
FMT_MODULE_EXPORT_BEGIN

// Implementations of enable_if_t and other metafunctions for older systems.
template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;
template <bool B, typename T, typename F>
using conditional_t = typename std::conditional<B, T, F>::type;
template <bool B> using bool_constant = std::integral_constant<bool, B>;
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
template <typename T>
using remove_const_t = typename std::remove_const<T>::type;
template <typename T>
using remove_cvref_t = typename std::remove_cv<remove_reference_t<T>>::type;
template <typename T> struct type_identity { using type = T; };
template <typename T> using type_identity_t = typename type_identity<T>::type;

struct monostate {
  constexpr monostate() {}
};

// An enable_if helper to be used in template parameters which results in much
// shorter symbols: https://godbolt.org/z/sWw4vP. Extra parentheses are needed
// to workaround a bug in MSVC 2019 (see #1140 and #1186).
#ifdef FMT_DOC
#  define FMT_ENABLE_IF(...)
#else
#  define FMT_ENABLE_IF(...) enable_if_t<(__VA_ARGS__), int> = 0
#endif

FMT_BEGIN_DETAIL_NAMESPACE

// Suppress "unused variable" warnings with the method described in
// https://herbsutter.com/2009/10/18/mailbag-shutting-up-compiler-warnings/.
// (void)var does not work on many Intel compilers.
template <typename... T> FMT_CONSTEXPR void ignore_unused(const T&...) {}

constexpr FMT_INLINE auto is_constant_evaluated(bool default_value = false)
    FMT_NOEXCEPT -> bool {
#ifdef __cpp_lib_is_constant_evaluated
  ignore_unused(default_value);
  return std::is_constant_evaluated();
#else
  return default_value;
#endif
}

// A function to suppress "conditional expression is constant" warnings.
template <typename T> constexpr FMT_INLINE auto const_check(T value) -> T {
  return value;
}

FMT_NORETURN FMT_API void assert_fail(const char* file, int line,
                                      const char* message);

#ifndef FMT_ASSERT
#  ifdef NDEBUG
// FMT_ASSERT is not empty to avoid -Werror=empty-body.
#    define FMT_ASSERT(condition, message) \
      ::fmt::detail::ignore_unused((condition), (message))
#  else
#    define FMT_ASSERT(condition, message)                                    \
      ((condition) /* void() fails with -Winvalid-constexpr on clang 4.0.1 */ \
           ? (void)0                                                          \
           : ::fmt::detail::assert_fail(__FILE__, __LINE__, (message)))
#  endif
#endif

#ifdef __cpp_lib_byte
using byte = std::byte;
#else
enum class byte : unsigned char {};
#endif

#if defined(FMT_USE_STRING_VIEW)
template <typename Char> using std_string_view = std::basic_string_view<Char>;
#elif defined(FMT_USE_EXPERIMENTAL_STRING_VIEW)
template <typename Char>
using std_string_view = std::experimental::basic_string_view<Char>;
#else
template <typename T> struct std_string_view {};
#endif

#ifdef FMT_USE_INT128
// Do nothing.
#elif defined(__SIZEOF_INT128__) && !FMT_NVCC && \
    !(FMT_CLANG_VERSION && FMT_MSC_VER)
#  define FMT_USE_INT128 1
using int128_t = __int128_t;
using uint128_t = __uint128_t;
template <typename T> inline auto convert_for_visit(T value) -> T {
  return value;
}
#else
#  define FMT_USE_INT128 0
#endif
#if !FMT_USE_INT128
enum class int128_t {};
enum class uint128_t {};
// Reduce template instantiations.
template <typename T> inline auto convert_for_visit(T) -> monostate {
  return {};
}
#endif

// Casts a nonnegative integer to unsigned.
template <typename Int>
FMT_CONSTEXPR auto to_unsigned(Int value) ->
    typename std::make_unsigned<Int>::type {
  FMT_ASSERT(value >= 0, "negative value");
  return static_cast<typename std::make_unsigned<Int>::type>(value);
}

FMT_MSC_WARNING(suppress : 4566) constexpr unsigned char micro[] = "\u00B5";

constexpr auto is_utf8() -> bool {
  // Avoid buggy sign extensions in MSVC's constant evaluation mode.
  // https://developercommunity.visualstudio.com/t/C-difference-in-behavior-for-unsigned/1233612
  using uchar = unsigned char;
  return FMT_UNICODE || (sizeof(micro) == 3 && uchar(micro[0]) == 0xC2 &&
                         uchar(micro[1]) == 0xB5);
}
FMT_END_DETAIL_NAMESPACE

/**
  An implementation of ``std::basic_string_view`` for pre-C++17. It provides a
  subset of the API. ``fmt::basic_string_view`` is used for format strings even
  if ``std::string_view`` is available to prevent issues when a library is
  compiled with a different ``-std`` option than the client code (which is not
  recommended).
 */
template <typename Char> class basic_string_view {
 private:
  const Char* data_;
  size_t size_;

 public:
  using value_type = Char;
  using iterator = const Char*;

  constexpr basic_string_view() FMT_NOEXCEPT : data_(nullptr), size_(0) {}

  /** Constructs a string reference object from a C string and a size. */
  constexpr basic_string_view(const Char* s, size_t count) FMT_NOEXCEPT
      : data_(s),
        size_(count) {}

  /**
    \rst
    Constructs a string reference object from a C string computing
    the size with ``std::char_traits<Char>::length``.
    \endrst
   */
  FMT_CONSTEXPR_CHAR_TRAITS
  FMT_INLINE
  basic_string_view(const Char* s)
      : data_(s),
        size_(detail::const_check(std::is_same<Char, char>::value &&
                                  !detail::is_constant_evaluated(true))
                  ? std::strlen(reinterpret_cast<const char*>(s))
                  : std::char_traits<Char>::length(s)) {}

  /** Constructs a string reference from a ``std::basic_string`` object. */
  template <typename Traits, typename Alloc>
  FMT_CONSTEXPR basic_string_view(
      const std::basic_string<Char, Traits, Alloc>& s) FMT_NOEXCEPT
      : data_(s.data()),
        size_(s.size()) {}

  template <typename S, FMT_ENABLE_IF(std::is_same<
                                      S, detail::std_string_view<Char>>::value)>
  FMT_CONSTEXPR basic_string_view(S s) FMT_NOEXCEPT : data_(s.data()),
                                                      size_(s.size()) {}

  /** Returns a pointer to the string data. */
  constexpr auto data() const FMT_NOEXCEPT -> const Char* { return data_; }

  /** Returns the string size. */
  constexpr auto size() const FMT_NOEXCEPT -> size_t { return size_; }

  constexpr auto begin() const FMT_NOEXCEPT -> iterator { return data_; }
  constexpr auto end() const FMT_NOEXCEPT -> iterator { return data_ + size_; }

  constexpr auto operator[](size_t pos) const FMT_NOEXCEPT -> const Char& {
    return data_[pos];
  }

  FMT_CONSTEXPR void remove_prefix(size_t n) FMT_NOEXCEPT {
    data_ += n;
    size_ -= n;
  }

  // Lexicographically compare this string reference to other.
  FMT_CONSTEXPR_CHAR_TRAITS auto compare(basic_string_view other) const -> int {
    size_t str_size = size_ < other.size_ ? size_ : other.size_;
    int result = std::char_traits<Char>::compare(data_, other.data_, str_size);
    if (result == 0)
      result = size_ == other.size_ ? 0 : (size_ < other.size_ ? -1 : 1);
    return result;
  }

  FMT_CONSTEXPR_CHAR_TRAITS friend auto operator==(basic_string_view lhs,
                                                   basic_string_view rhs)
      -> bool {
    return lhs.compare(rhs) == 0;
  }
  friend auto operator!=(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) != 0;
  }
  friend auto operator<(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) < 0;
  }
  friend auto operator<=(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) <= 0;
  }
  friend auto operator>(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) > 0;
  }
  friend auto operator>=(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) >= 0;
  }
};

using string_view = basic_string_view<char>;

/** Specifies if ``T`` is a character type. Can be specialized by users. */
template <typename T> struct is_char : std::false_type {};
template <> struct is_char<char> : std::true_type {};

// Returns a string view of `s`.
template <typename Char, FMT_ENABLE_IF(is_char<Char>::value)>
FMT_INLINE auto to_string_view(const Char* s) -> basic_string_view<Char> {
  return s;
}
template <typename Char, typename Traits, typename Alloc>
inline auto to_string_view(const std::basic_string<Char, Traits, Alloc>& s)
    -> basic_string_view<Char> {
  return s;
}
template <typename Char>
constexpr auto to_string_view(basic_string_view<Char> s)
    -> basic_string_view<Char> {
  return s;
}
template <typename Char,
          FMT_ENABLE_IF(!std::is_empty<detail::std_string_view<Char>>::value)>
inline auto to_string_view(detail::std_string_view<Char> s)
    -> basic_string_view<Char> {
  return s;
}

// A base class for compile-time strings. It is defined in the fmt namespace to
// make formatting functions visible via ADL, e.g. format(FMT_STRING("{}"), 42).
struct compile_string {};

template <typename S>
struct is_compile_string : std::is_base_of<compile_string, S> {};

template <typename S, FMT_ENABLE_IF(is_compile_string<S>::value)>
constexpr auto to_string_view(const S& s)
    -> basic_string_view<typename S::char_type> {
  return basic_string_view<typename S::char_type>(s);
}

FMT_BEGIN_DETAIL_NAMESPACE

void to_string_view(...);
using fmt::to_string_view;

// Specifies whether S is a string type convertible to fmt::basic_string_view.
// It should be a constexpr function but MSVC 2017 fails to compile it in
// enable_if and MSVC 2015 fails to compile it as an alias template.
template <typename S>
struct is_string : std::is_class<decltype(to_string_view(std::declval<S>()))> {
};

template <typename S, typename = void> struct char_t_impl {};
template <typename S> struct char_t_impl<S, enable_if_t<is_string<S>::value>> {
  using result = decltype(to_string_view(std::declval<S>()));
  using type = typename result::value_type;
};

// Reports a compile-time error if S is not a valid format string.
template <typename..., typename S, FMT_ENABLE_IF(!is_compile_string<S>::value)>
FMT_INLINE void check_format_string(const S&) {
#ifdef FMT_ENFORCE_COMPILE_STRING
  static_assert(is_compile_string<S>::value,
                "FMT_ENFORCE_COMPILE_STRING requires all format strings to use "
                "FMT_STRING.");
#endif
}
template <typename..., typename S, FMT_ENABLE_IF(is_compile_string<S>::value)>
void check_format_string(S);

FMT_NORETURN FMT_API void throw_format_error(const char* message);

struct error_handler {
  constexpr error_handler() = default;
  constexpr error_handler(const error_handler&) = default;

  // This function is intentionally not constexpr to give a compile-time error.
  FMT_NORETURN FMT_API void on_error(const char* message);
};
FMT_END_DETAIL_NAMESPACE

/** String's character type. */
template <typename S> using char_t = typename detail::char_t_impl<S>::type;

/**
  \rst
  Parsing context consisting of a format string range being parsed and an
  argument counter for automatic indexing.
  You can use the ``format_parse_context`` type alias for ``char`` instead.
  \endrst
 */
template <typename Char, typename ErrorHandler = detail::error_handler>
class basic_format_parse_context : private ErrorHandler {
 private:
  basic_string_view<Char> format_str_;
  int next_arg_id_;

 public:
  using char_type = Char;
  using iterator = typename basic_string_view<Char>::iterator;

  explicit constexpr basic_format_parse_context(
      basic_string_view<Char> format_str, ErrorHandler eh = {},
      int next_arg_id = 0)
      : ErrorHandler(eh), format_str_(format_str), next_arg_id_(next_arg_id) {}

  /**
    Returns an iterator to the beginning of the format string range being
    parsed.
   */
  constexpr auto begin() const FMT_NOEXCEPT -> iterator {
    return format_str_.begin();
  }

  /**
    Returns an iterator past the end of the format string range being parsed.
   */
  constexpr auto end() const FMT_NOEXCEPT -> iterator {
    return format_str_.end();
  }

  /** Advances the begin iterator to ``it``. */
  FMT_CONSTEXPR void advance_to(iterator it) {
    format_str_.remove_prefix(detail::to_unsigned(it - begin()));
  }

  /**
    Reports an error if using the manual argument indexing; otherwise returns
    the next argument index and switches to the automatic indexing.
   */
  FMT_CONSTEXPR auto next_arg_id() -> int {
    // Don't check if the argument id is valid to avoid overhead and because it
    // will be checked during formatting anyway.
    if (next_arg_id_ >= 0) return next_arg_id_++;
    on_error("cannot switch from manual to automatic argument indexing");
    return 0;
  }

  /**
    Reports an error if using the automatic argument indexing; otherwise
    switches to the manual indexing.
   */
  FMT_CONSTEXPR void check_arg_id(int) {
    if (next_arg_id_ > 0)
      on_error("cannot switch from automatic to manual argument indexing");
    else
      next_arg_id_ = -1;
  }

  FMT_CONSTEXPR void check_arg_id(basic_string_view<Char>) {}

  FMT_CONSTEXPR void on_error(const char* message) {
    ErrorHandler::on_error(message);
  }

  constexpr auto error_handler() const -> ErrorHandler { return *this; }
};

using format_parse_context = basic_format_parse_context<char>;

template <typename Context> class basic_format_arg;
template <typename Context> class basic_format_args;
template <typename Context> class dynamic_format_arg_store;

// A formatter for objects of type T.
template <typename T, typename Char = char, typename Enable = void>
struct formatter {
  // A deleted default constructor indicates a disabled formatter.
  formatter() = delete;
};

// Specifies if T has an enabled formatter specialization. A type can be
// formattable even if it doesn't have a formatter e.g. via a conversion.
template <typename T, typename Context>
using has_formatter =
    std::is_constructible<typename Context::template formatter_type<T>>;

// Checks whether T is a container with contiguous storage.
template <typename T> struct is_contiguous : std::false_type {};
template <typename Char>
struct is_contiguous<std::basic_string<Char>> : std::true_type {};

class appender;

FMT_BEGIN_DETAIL_NAMESPACE

template <typename Context, typename T>
constexpr auto has_const_formatter_impl(T*)
    -> decltype(typename Context::template formatter_type<T>().format(
                    std::declval<const T&>(), std::declval<Context&>()),
                true) {
  return true;
}
template <typename Context>
constexpr auto has_const_formatter_impl(...) -> bool {
  return false;
}
template <typename T, typename Context>
constexpr auto has_const_formatter() -> bool {
  return has_const_formatter_impl<Context>(static_cast<T*>(nullptr));
}

// Extracts a reference to the container from back_insert_iterator.
template <typename Container>
inline auto get_container(std::back_insert_iterator<Container> it)
    -> Container& {
  using bi_iterator = std::back_insert_iterator<Container>;
  struct accessor : bi_iterator {
    accessor(bi_iterator iter) : bi_iterator(iter) {}
    using bi_iterator::container;
  };
  return *accessor(it).container;
}

template <typename Char, typename InputIt, typename OutputIt>
FMT_CONSTEXPR auto copy_str(InputIt begin, InputIt end, OutputIt out)
    -> OutputIt {
  while (begin != end) *out++ = static_cast<Char>(*begin++);
  return out;
}

template <typename Char, typename T, typename U,
          FMT_ENABLE_IF(
              std::is_same<remove_const_t<T>, U>::value&& is_char<U>::value)>
FMT_CONSTEXPR auto copy_str(T* begin, T* end, U* out) -> U* {
  if (is_constant_evaluated()) return copy_str<Char, T*, U*>(begin, end, out);
  auto size = to_unsigned(end - begin);
  memcpy(out, begin, size * sizeof(U));
  return out + size;
}

/**
  \rst
  A contiguous memory buffer with an optional growing ability. It is an internal
  class and shouldn't be used directly, only via `~fmt::basic_memory_buffer`.
  \endrst
 */
template <typename T> class buffer {
 private:
  T* ptr_;
  size_t size_;
  size_t capacity_;

 protected:
  // Don't initialize ptr_ since it is not accessed to save a few cycles.
  FMT_MSC_WARNING(suppress : 26495)
  buffer(size_t sz) FMT_NOEXCEPT : size_(sz), capacity_(sz) {}

  FMT_CONSTEXPR20 buffer(T* p = nullptr, size_t sz = 0,
                         size_t cap = 0) FMT_NOEXCEPT : ptr_(p),
                                                        size_(sz),
                                                        capacity_(cap) {}

  FMT_CONSTEXPR20 ~buffer() = default;
  buffer(buffer&&) = default;

  /** Sets the buffer data and capacity. */
  FMT_CONSTEXPR void set(T* buf_data, size_t buf_capacity) FMT_NOEXCEPT {
    ptr_ = buf_data;
    capacity_ = buf_capacity;
  }

  /** Increases the buffer capacity to hold at least *capacity* elements. */
  virtual FMT_CONSTEXPR20 void grow(size_t capacity) = 0;

 public:
  using value_type = T;
  using const_reference = const T&;

  buffer(const buffer&) = delete;
  void operator=(const buffer&) = delete;

  auto begin() FMT_NOEXCEPT -> T* { return ptr_; }
  auto end() FMT_NOEXCEPT -> T* { return ptr_ + size_; }

  auto begin() const FMT_NOEXCEPT -> const T* { return ptr_; }
  auto end() const FMT_NOEXCEPT -> const T* { return ptr_ + size_; }

  /** Returns the size of this buffer. */
  constexpr auto size() const FMT_NOEXCEPT -> size_t { return size_; }

  /** Returns the capacity of this buffer. */
  constexpr auto capacity() const FMT_NOEXCEPT -> size_t { return capacity_; }

  /** Returns a pointer to the buffer data. */
  FMT_CONSTEXPR auto data() FMT_NOEXCEPT -> T* { return ptr_; }

  /** Returns a pointer to the buffer data. */
  FMT_CONSTEXPR auto data() const FMT_NOEXCEPT -> const T* { return ptr_; }

  /** Clears this buffer. */
  void clear() { size_ = 0; }

  // Tries resizing the buffer to contain *count* elements. If T is a POD type
  // the new elements may not be initialized.
  FMT_CONSTEXPR20 void try_resize(size_t count) {
    try_reserve(count);
    size_ = count <= capacity_ ? count : capacity_;
  }

  // Tries increasing the buffer capacity to *new_capacity*. It can increase the
  // capacity by a smaller amount than requested but guarantees there is space
  // for at least one additional element either by increasing the capacity or by
  // flushing the buffer if it is full.
  FMT_CONSTEXPR20 void try_reserve(size_t new_capacity) {
    if (new_capacity > capacity_) grow(new_capacity);
  }

  FMT_CONSTEXPR20 void push_back(const T& value) {
    try_reserve(size_ + 1);
    ptr_[size_++] = value;
  }

  /** Appends data to the end of the buffer. */
  template <typename U> void append(const U* begin, const U* end);

  template <typename I> FMT_CONSTEXPR auto operator[](I index) -> T& {
    return ptr_[index];
  }
  template <typename I>
  FMT_CONSTEXPR auto operator[](I index) const -> const T& {
    return ptr_[index];
  }
};

struct buffer_traits {
  explicit buffer_traits(size_t) {}
  auto count() const -> size_t { return 0; }
  auto limit(size_t size) -> size_t { return size; }
};

class fixed_buffer_traits {
 private:
  size_t count_ = 0;
  size_t limit_;

 public:
  explicit fixed_buffer_traits(size_t limit) : limit_(limit) {}
  auto count() const -> size_t { return count_; }
  auto limit(size_t size) -> size_t {
    size_t n = limit_ > count_ ? limit_ - count_ : 0;
    count_ += size;
    return size < n ? size : n;
  }
};

// A buffer that writes to an output iterator when flushed.
template <typename OutputIt, typename T, typename Traits = buffer_traits>
class iterator_buffer final : public Traits, public buffer<T> {
 private:
  OutputIt out_;
  enum { buffer_size = 256 };
  T data_[buffer_size];

 protected:
  FMT_CONSTEXPR20 void grow(size_t) override {
    if (this->size() == buffer_size) flush();
  }

  void flush() {
    auto size = this->size();
    this->clear();
    out_ = copy_str<T>(data_, data_ + this->limit(size), out_);
  }

 public:
  explicit iterator_buffer(OutputIt out, size_t n = buffer_size)
      : Traits(n), buffer<T>(data_, 0, buffer_size), out_(out) {}
  iterator_buffer(iterator_buffer&& other)
      : Traits(other), buffer<T>(data_, 0, buffer_size), out_(other.out_) {}
  ~iterator_buffer() { flush(); }

  auto out() -> OutputIt {
    flush();
    return out_;
  }
  auto count() const -> size_t { return Traits::count() + this->size(); }
};

template <typename T>
class iterator_buffer<T*, T, fixed_buffer_traits> final
    : public fixed_buffer_traits,
      public buffer<T> {
 private:
  T* out_;
  enum { buffer_size = 256 };
  T data_[buffer_size];

 protected:
  FMT_CONSTEXPR20 void grow(size_t) override {
    if (this->size() == this->capacity()) flush();
  }

  void flush() {
    size_t n = this->limit(this->size());
    if (this->data() == out_) {
      out_ += n;
      this->set(data_, buffer_size);
    }
    this->clear();
  }

 public:
  explicit iterator_buffer(T* out, size_t n = buffer_size)
      : fixed_buffer_traits(n), buffer<T>(out, 0, n), out_(out) {}
  iterator_buffer(iterator_buffer&& other)
      : fixed_buffer_traits(other),
        buffer<T>(std::move(other)),
        out_(other.out_) {
    if (this->data() != out_) {
      this->set(data_, buffer_size);
      this->clear();
    }
  }
  ~iterator_buffer() { flush(); }

  auto out() -> T* {
    flush();
    return out_;
  }
  auto count() const -> size_t {
    return fixed_buffer_traits::count() + this->size();
  }
};

template <typename T> class iterator_buffer<T*, T> final : public buffer<T> {
 protected:
  FMT_CONSTEXPR20 void grow(size_t) override {}

 public:
  explicit iterator_buffer(T* out, size_t = 0) : buffer<T>(out, 0, ~size_t()) {}

  auto out() -> T* { return &*this->end(); }
};

// A buffer that writes to a container with the contiguous storage.
template <typename Container>
class iterator_buffer<std::back_insert_iterator<Container>,
                      enable_if_t<is_contiguous<Container>::value,
                                  typename Container::value_type>>
    final : public buffer<typename Container::value_type> {
 private:
  Container& container_;

 protected:
  FMT_CONSTEXPR20 void grow(size_t capacity) override {
    container_.resize(capacity);
    this->set(&container_[0], capacity);
  }

 public:
  explicit iterator_buffer(Container& c)
      : buffer<typename Container::value_type>(c.size()), container_(c) {}
  explicit iterator_buffer(std::back_insert_iterator<Container> out, size_t = 0)
      : iterator_buffer(get_container(out)) {}
  auto out() -> std::back_insert_iterator<Container> {
    return std::back_inserter(container_);
  }
};

// A buffer that counts the number of code units written discarding the output.
template <typename T = char> class counting_buffer final : public buffer<T> {
 private:
  enum { buffer_size = 256 };
  T data_[buffer_size];
  size_t count_ = 0;

 protected:
  FMT_CONSTEXPR20 void grow(size_t) override {
    if (this->size() != buffer_size) return;
    count_ += this->size();
    this->clear();
  }

 public:
  counting_buffer() : buffer<T>(data_, 0, buffer_size) {}

  auto count() -> size_t { return count_ + this->size(); }
};

template <typename T>
using buffer_appender = conditional_t<std::is_same<T, char>::value, appender,
                                      std::back_insert_iterator<buffer<T>>>;

// Maps an output iterator to a buffer.
template <typename T, typename OutputIt>
auto get_buffer(OutputIt out) -> iterator_buffer<OutputIt, T> {
  return iterator_buffer<OutputIt, T>(out);
}

template <typename Buffer>
auto get_iterator(Buffer& buf) -> decltype(buf.out()) {
  return buf.out();
}
template <typename T> auto get_iterator(buffer<T>& buf) -> buffer_appender<T> {
  return buffer_appender<T>(buf);
}

template <typename T, typename Char = char, typename Enable = void>
struct fallback_formatter {
  fallback_formatter() = delete;
};

// Specifies if T has an enabled fallback_formatter specialization.
template <typename T, typename Char>
using has_fallback_formatter =
    std::is_constructible<fallback_formatter<T, Char>>;

struct view {};

template <typename Char, typename T> struct named_arg : view {
  const Char* name;
  const T& value;
  named_arg(const Char* n, const T& v) : name(n), value(v) {}
};

template <typename Char> struct named_arg_info {
  const Char* name;
  int id;
};

template <typename T, typename Char, size_t NUM_ARGS, size_t NUM_NAMED_ARGS>
struct arg_data {
  // args_[0].named_args points to named_args_ to avoid bloating format_args.
  // +1 to workaround a bug in gcc 7.5 that causes duplicated-branches warning.
  T args_[1 + (NUM_ARGS != 0 ? NUM_ARGS : +1)];
  named_arg_info<Char> named_args_[NUM_NAMED_ARGS];

  template <typename... U>
  arg_data(const U&... init) : args_{T(named_args_, NUM_NAMED_ARGS), init...} {}
  arg_data(const arg_data& other) = delete;
  auto args() const -> const T* { return args_ + 1; }
  auto named_args() -> named_arg_info<Char>* { return named_args_; }
};

template <typename T, typename Char, size_t NUM_ARGS>
struct arg_data<T, Char, NUM_ARGS, 0> {
  // +1 to workaround a bug in gcc 7.5 that causes duplicated-branches warning.
  T args_[NUM_ARGS != 0 ? NUM_ARGS : +1];

  template <typename... U>
  FMT_CONSTEXPR FMT_INLINE arg_data(const U&... init) : args_{init...} {}
  FMT_CONSTEXPR FMT_INLINE auto args() const -> const T* { return args_; }
  FMT_CONSTEXPR FMT_INLINE auto named_args() -> std::nullptr_t {
    return nullptr;
  }
};

template <typename Char>
inline void init_named_args(named_arg_info<Char>*, int, int) {}

template <typename T> struct is_named_arg : std::false_type {};
template <typename T> struct is_statically_named_arg : std::false_type {};

template <typename T, typename Char>
struct is_named_arg<named_arg<Char, T>> : std::true_type {};

template <typename Char, typename T, typename... Tail,
          FMT_ENABLE_IF(!is_named_arg<T>::value)>
void init_named_args(named_arg_info<Char>* named_args, int arg_count,
                     int named_arg_count, const T&, const Tail&... args) {
  init_named_args(named_args, arg_count + 1, named_arg_count, args...);
}

template <typename Char, typename T, typename... Tail,
          FMT_ENABLE_IF(is_named_arg<T>::value)>
void init_named_args(named_arg_info<Char>* named_args, int arg_count,
                     int named_arg_count, const T& arg, const Tail&... args) {
  named_args[named_arg_count++] = {arg.name, arg_count};
  init_named_args(named_args, arg_count + 1, named_arg_count, args...);
}

template <typename... Args>
FMT_CONSTEXPR FMT_INLINE void init_named_args(std::nullptr_t, int, int,
                                              const Args&...) {}

template <bool B = false> constexpr auto count() -> size_t { return B ? 1 : 0; }
template <bool B1, bool B2, bool... Tail> constexpr auto count() -> size_t {
  return (B1 ? 1 : 0) + count<B2, Tail...>();
}

template <typename... Args> constexpr auto count_named_args() -> size_t {
  return count<is_named_arg<Args>::value...>();
}

template <typename... Args>
constexpr auto count_statically_named_args() -> size_t {
  return count<is_statically_named_arg<Args>::value...>();
}

enum class type {
  none_type,
  // Integer types should go first,
  int_type,
  uint_type,
  long_long_type,
  ulong_long_type,
  int128_type,
  uint128_type,
  bool_type,
  char_type,
  last_integer_type = char_type,
  // followed by floating-point types.
  float_type,
  double_type,
  long_double_type,
  last_numeric_type = long_double_type,
  cstring_type,
  string_type,
  pointer_type,
  custom_type
};

// Maps core type T to the corresponding type enum constant.
template <typename T, typename Char>
struct type_constant : std::integral_constant<type, type::custom_type> {};

#define FMT_TYPE_CONSTANT(Type, constant) \
  template <typename Char>                \
  struct type_constant<Type, Char>        \
      : std::integral_constant<type, type::constant> {}

FMT_TYPE_CONSTANT(int, int_type);
FMT_TYPE_CONSTANT(unsigned, uint_type);
FMT_TYPE_CONSTANT(long long, long_long_type);
FMT_TYPE_CONSTANT(unsigned long long, ulong_long_type);
FMT_TYPE_CONSTANT(int128_t, int128_type);
FMT_TYPE_CONSTANT(uint128_t, uint128_type);
FMT_TYPE_CONSTANT(bool, bool_type);
FMT_TYPE_CONSTANT(Char, char_type);
FMT_TYPE_CONSTANT(float, float_type);
FMT_TYPE_CONSTANT(double, double_type);
FMT_TYPE_CONSTANT(long double, long_double_type);
FMT_TYPE_CONSTANT(const Char*, cstring_type);
FMT_TYPE_CONSTANT(basic_string_view<Char>, string_type);
FMT_TYPE_CONSTANT(const void*, pointer_type);

constexpr bool is_integral_type(type t) {
  return t > type::none_type && t <= type::last_integer_type;
}

constexpr bool is_arithmetic_type(type t) {
  return t > type::none_type && t <= type::last_numeric_type;
}

struct unformattable {};
struct unformattable_char : unformattable {};
struct unformattable_const : unformattable {};
struct unformattable_pointer : unformattable {};

template <typename Char> struct string_value {
  const Char* data;
  size_t size;
};

template <typename Char> struct named_arg_value {
  const named_arg_info<Char>* data;
  size_t size;
};

template <typename Context> struct custom_value {
  using parse_context = typename Context::parse_context_type;
  void* value;
  void (*format)(void* arg, parse_context& parse_ctx, Context& ctx);
};

// A formatting argument value.
template <typename Context> class value {
 public:
  using char_type = typename Context::char_type;

  union {
    monostate no_value;
    int int_value;
    unsigned uint_value;
    long long long_long_value;
    unsigned long long ulong_long_value;
    int128_t int128_value;
    uint128_t uint128_value;
    bool bool_value;
    char_type char_value;
    float float_value;
    double double_value;
    long double long_double_value;
    const void* pointer;
    string_value<char_type> string;
    custom_value<Context> custom;
    named_arg_value<char_type> named_args;
  };

  constexpr FMT_INLINE value() : no_value() {}
  constexpr FMT_INLINE value(int val) : int_value(val) {}
  constexpr FMT_INLINE value(unsigned val) : uint_value(val) {}
  constexpr FMT_INLINE value(long long val) : long_long_value(val) {}
  constexpr FMT_INLINE value(unsigned long long val) : ulong_long_value(val) {}
  FMT_INLINE value(int128_t val) : int128_value(val) {}
  FMT_INLINE value(uint128_t val) : uint128_value(val) {}
  constexpr FMT_INLINE value(float val) : float_value(val) {}
  constexpr FMT_INLINE value(double val) : double_value(val) {}
  FMT_INLINE value(long double val) : long_double_value(val) {}
  constexpr FMT_INLINE value(bool val) : bool_value(val) {}
  constexpr FMT_INLINE value(char_type val) : char_value(val) {}
  FMT_CONSTEXPR FMT_INLINE value(const char_type* val) {
    string.data = val;
    if (is_constant_evaluated()) string.size = {};
  }
  FMT_CONSTEXPR FMT_INLINE value(basic_string_view<char_type> val) {
    string.data = val.data();
    string.size = val.size();
  }
  FMT_INLINE value(const void* val) : pointer(val) {}
  FMT_INLINE value(const named_arg_info<char_type>* args, size_t size)
      : named_args{args, size} {}

  template <typename T> FMT_CONSTEXPR FMT_INLINE value(T& val) {
    using value_type = remove_cvref_t<T>;
    custom.value = const_cast<value_type*>(&val);
    // Get the formatter type through the context to allow different contexts
    // have different extension points, e.g. `formatter<T>` for `format` and
    // `printf_formatter<T>` for `printf`.
    custom.format = format_custom_arg<
        value_type,
        conditional_t<has_formatter<value_type, Context>::value,
                      typename Context::template formatter_type<value_type>,
                      fallback_formatter<value_type, char_type>>>;
  }
  value(unformattable);
  value(unformattable_char);
  value(unformattable_const);
  value(unformattable_pointer);

 private:
  // Formats an argument of a custom type, such as a user-defined class.
  template <typename T, typename Formatter>
  static void format_custom_arg(void* arg,
                                typename Context::parse_context_type& parse_ctx,
                                Context& ctx) {
    auto f = Formatter();
    parse_ctx.advance_to(f.parse(parse_ctx));
    using qualified_type =
        conditional_t<has_const_formatter<T, Context>(), const T, T>;
    ctx.advance_to(f.format(*static_cast<qualified_type*>(arg), ctx));
  }
};

template <typename Context, typename T>
FMT_CONSTEXPR auto make_arg(const T& value) -> basic_format_arg<Context>;

// To minimize the number of types we need to deal with, long is translated
// either to int or to long long depending on its size.
enum { long_short = sizeof(long) == sizeof(int) };
using long_type = conditional_t<long_short, int, long long>;
using ulong_type = conditional_t<long_short, unsigned, unsigned long long>;

// Maps formatting arguments to core types.
// arg_mapper reports errors by returning unformattable instead of using
// static_assert because it's used in the is_formattable trait.
template <typename Context> struct arg_mapper {
  using char_type = typename Context::char_type;

  FMT_CONSTEXPR FMT_INLINE auto map(signed char val) -> int { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(unsigned char val) -> unsigned {
    return val;
  }
  FMT_CONSTEXPR FMT_INLINE auto map(short val) -> int { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(unsigned short val) -> unsigned {
    return val;
  }
  FMT_CONSTEXPR FMT_INLINE auto map(int val) -> int { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(unsigned val) -> unsigned { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(long val) -> long_type { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(unsigned long val) -> ulong_type {
    return val;
  }
  FMT_CONSTEXPR FMT_INLINE auto map(long long val) -> long long { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(unsigned long long val)
      -> unsigned long long {
    return val;
  }
  FMT_CONSTEXPR FMT_INLINE auto map(int128_t val) -> int128_t { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(uint128_t val) -> uint128_t { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(bool val) -> bool { return val; }

  template <typename T, FMT_ENABLE_IF(std::is_same<T, char>::value ||
                                      std::is_same<T, char_type>::value)>
  FMT_CONSTEXPR FMT_INLINE auto map(T val) -> char_type {
    return val;
  }
  template <typename T, enable_if_t<(std::is_same<T, wchar_t>::value ||
#ifdef __cpp_char8_t
                                     std::is_same<T, char8_t>::value ||
#endif
                                     std::is_same<T, char16_t>::value ||
                                     std::is_same<T, char32_t>::value) &&
                                        !std::is_same<T, char_type>::value,
                                    int> = 0>
  FMT_CONSTEXPR FMT_INLINE auto map(T) -> unformattable_char {
    return {};
  }

  FMT_CONSTEXPR FMT_INLINE auto map(float val) -> float { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(double val) -> double { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(long double val) -> long double {
    return val;
  }

  FMT_CONSTEXPR FMT_INLINE auto map(char_type* val) -> const char_type* {
    return val;
  }
  FMT_CONSTEXPR FMT_INLINE auto map(const char_type* val) -> const char_type* {
    return val;
  }
  template <typename T,
            FMT_ENABLE_IF(is_string<T>::value && !std::is_pointer<T>::value &&
                          std::is_same<char_type, char_t<T>>::value)>
  FMT_CONSTEXPR FMT_INLINE auto map(const T& val)
      -> basic_string_view<char_type> {
    return to_string_view(val);
  }
  template <typename T,
            FMT_ENABLE_IF(is_string<T>::value && !std::is_pointer<T>::value &&
                          !std::is_same<char_type, char_t<T>>::value)>
  FMT_CONSTEXPR FMT_INLINE auto map(const T&) -> unformattable_char {
    return {};
  }
  template <typename T,
            FMT_ENABLE_IF(
                std::is_constructible<basic_string_view<char_type>, T>::value &&
                !is_string<T>::value && !has_formatter<T, Context>::value &&
                !has_fallback_formatter<T, char_type>::value)>
  FMT_CONSTEXPR FMT_INLINE auto map(const T& val)
      -> basic_string_view<char_type> {
    return basic_string_view<char_type>(val);
  }
  template <
      typename T,
      FMT_ENABLE_IF(
          std::is_constructible<std_string_view<char_type>, T>::value &&
          !std::is_constructible<basic_string_view<char_type>, T>::value &&
          !is_string<T>::value && !has_formatter<T, Context>::value &&
          !has_fallback_formatter<T, char_type>::value)>
  FMT_CONSTEXPR FMT_INLINE auto map(const T& val)
      -> basic_string_view<char_type> {
    return std_string_view<char_type>(val);
  }

  using cstring_result = conditional_t<std::is_same<char_type, char>::value,
                                       const char*, unformattable_pointer>;

  FMT_DEPRECATED FMT_CONSTEXPR FMT_INLINE auto map(const signed char* val)
      -> cstring_result {
    return map(reinterpret_cast<const char*>(val));
  }
  FMT_DEPRECATED FMT_CONSTEXPR FMT_INLINE auto map(const unsigned char* val)
      -> cstring_result {
    return map(reinterpret_cast<const char*>(val));
  }
  FMT_DEPRECATED FMT_CONSTEXPR FMT_INLINE auto map(signed char* val)
      -> cstring_result {
    return map(reinterpret_cast<const char*>(val));
  }
  FMT_DEPRECATED FMT_CONSTEXPR FMT_INLINE auto map(unsigned char* val)
      -> cstring_result {
    return map(reinterpret_cast<const char*>(val));
  }

  FMT_CONSTEXPR FMT_INLINE auto map(void* val) -> const void* { return val; }
  FMT_CONSTEXPR FMT_INLINE auto map(const void* val) -> const void* {
    return val;
  }
  FMT_CONSTEXPR FMT_INLINE auto map(std::nullptr_t val) -> const void* {
    return val;
  }

  // We use SFINAE instead of a const T* parameter to avoid conflicting with
  // the C array overload.
  template <
      typename T,
      FMT_ENABLE_IF(
          std::is_member_pointer<T>::value ||
          std::is_function<typename std::remove_pointer<T>::type>::value ||
          (std::is_convertible<const T&, const void*>::value &&
           !std::is_convertible<const T&, const char_type*>::value))>
  FMT_CONSTEXPR auto map(const T&) -> unformattable_pointer {
    return {};
  }

  template <typename T, std::size_t N,
            FMT_ENABLE_IF(!std::is_same<T, wchar_t>::value)>
  FMT_CONSTEXPR FMT_INLINE auto map(const T (&values)[N]) -> const T (&)[N] {
    return values;
  }

  template <typename T,
            FMT_ENABLE_IF(
                std::is_enum<T>::value&& std::is_convertible<T, int>::value &&
                !has_formatter<T, Context>::value &&
                !has_fallback_formatter<T, char_type>::value)>
  FMT_CONSTEXPR FMT_INLINE auto map(const T& val)
      -> decltype(std::declval<arg_mapper>().map(
          static_cast<typename std::underlying_type<T>::type>(val))) {
    return map(static_cast<typename std::underlying_type<T>::type>(val));
  }

  FMT_CONSTEXPR FMT_INLINE auto map(detail::byte val) -> unsigned {
    return map(static_cast<unsigned char>(val));
  }

  template <typename T, typename U = remove_cvref_t<T>>
  struct formattable
      : bool_constant<has_const_formatter<U, Context>() ||
                      !std::is_const<remove_reference_t<T>>::value ||
                      has_fallback_formatter<U, char_type>::value> {};

#if FMT_MSC_VER != 0 && FMT_MSC_VER < 1910
  // Workaround a bug in MSVC.
  template <typename T> FMT_CONSTEXPR FMT_INLINE auto do_map(T&& val) -> T& {
    return val;
  }
#else
  template <typename T, FMT_ENABLE_IF(formattable<T>::value)>
  FMT_CONSTEXPR FMT_INLINE auto do_map(T&& val) -> T& {
    return val;
  }
  template <typename T, FMT_ENABLE_IF(!formattable<T>::value)>
  FMT_CONSTEXPR FMT_INLINE auto do_map(T&&) -> unformattable_const {
    return {};
  }
#endif

  template <typename T, typename U = remove_cvref_t<T>,
            FMT_ENABLE_IF(!is_string<U>::value && !is_char<U>::value &&
                          !std::is_array<U>::value &&
                          (has_formatter<U, Context>::value ||
                           has_fallback_formatter<U, char_type>::value))>
  FMT_CONSTEXPR FMT_INLINE auto map(T&& val)
      -> decltype(this->do_map(std::forward<T>(val))) {
    return do_map(std::forward<T>(val));
  }

  template <typename T, FMT_ENABLE_IF(is_named_arg<T>::value)>
  FMT_CONSTEXPR FMT_INLINE auto map(const T& named_arg)
      -> decltype(std::declval<arg_mapper>().map(named_arg.value)) {
    return map(named_arg.value);
  }

  auto map(...) -> unformattable { return {}; }
};

// A type constant after applying arg_mapper<Context>.
template <typename T, typename Context>
using mapped_type_constant =
    type_constant<decltype(arg_mapper<Context>().map(std::declval<const T&>())),
                  typename Context::char_type>;

enum { packed_arg_bits = 4 };
// Maximum number of arguments with packed types.
enum { max_packed_args = 62 / packed_arg_bits };
enum : unsigned long long { is_unpacked_bit = 1ULL << 63 };
enum : unsigned long long { has_named_args_bit = 1ULL << 62 };

FMT_END_DETAIL_NAMESPACE

// An output iterator that appends to a buffer.
// It is used to reduce symbol sizes for the common case.
class appender : public std::back_insert_iterator<detail::buffer<char>> {
  using base = std::back_insert_iterator<detail::buffer<char>>;

  template <typename T>
  friend auto get_buffer(appender out) -> detail::buffer<char>& {
    return detail::get_container(out);
  }

 public:
  using std::back_insert_iterator<detail::buffer<char>>::back_insert_iterator;
  appender(base it) FMT_NOEXCEPT : base(it) {}
  using _Unchecked_type = appender;  // Mark iterator as checked.

  auto operator++() FMT_NOEXCEPT -> appender& { return *this; }

  auto operator++(int) FMT_NOEXCEPT -> appender { return *this; }
};

// A formatting argument. It is a trivially copyable/constructible type to
// allow storage in basic_memory_buffer.
template <typename Context> class basic_format_arg {
 private:
  detail::value<Context> value_;
  detail::type type_;

  template <typename ContextType, typename T>
  friend FMT_CONSTEXPR auto detail::make_arg(const T& value)
      -> basic_format_arg<ContextType>;

  template <typename Visitor, typename Ctx>
  friend FMT_CONSTEXPR auto visit_format_arg(Visitor&& vis,
                                             const basic_format_arg<Ctx>& arg)
      -> decltype(vis(0));

  friend class basic_format_args<Context>;
  friend class dynamic_format_arg_store<Context>;

  using char_type = typename Context::char_type;

  template <typename T, typename Char, size_t NUM_ARGS, size_t NUM_NAMED_ARGS>
  friend struct detail::arg_data;

  basic_format_arg(const detail::named_arg_info<char_type>* args, size_t size)
      : value_(args, size) {}

 public:
  class handle {
   public:
    explicit handle(detail::custom_value<Context> custom) : custom_(custom) {}

    void format(typename Context::parse_context_type& parse_ctx,
                Context& ctx) const {
      custom_.format(custom_.value, parse_ctx, ctx);
    }

   private:
    detail::custom_value<Context> custom_;
  };

  constexpr basic_format_arg() : type_(detail::type::none_type) {}

  constexpr explicit operator bool() const FMT_NOEXCEPT {
    return type_ != detail::type::none_type;
  }

  auto type() const -> detail::type { return type_; }

  auto is_integral() const -> bool { return detail::is_integral_type(type_); }
  auto is_arithmetic() const -> bool {
    return detail::is_arithmetic_type(type_);
  }
};

/**
  \rst
  Visits an argument dispatching to the appropriate visit method based on
  the argument type. For example, if the argument type is ``double`` then
  ``vis(value)`` will be called with the value of type ``double``.
  \endrst
 */
template <typename Visitor, typename Context>
FMT_CONSTEXPR FMT_INLINE auto visit_format_arg(
    Visitor&& vis, const basic_format_arg<Context>& arg) -> decltype(vis(0)) {
  switch (arg.type_) {
  case detail::type::none_type:
    break;
  case detail::type::int_type:
    return vis(arg.value_.int_value);
  case detail::type::uint_type:
    return vis(arg.value_.uint_value);
  case detail::type::long_long_type:
    return vis(arg.value_.long_long_value);
  case detail::type::ulong_long_type:
    return vis(arg.value_.ulong_long_value);
  case detail::type::int128_type:
    return vis(detail::convert_for_visit(arg.value_.int128_value));
  case detail::type::uint128_type:
    return vis(detail::convert_for_visit(arg.value_.uint128_value));
  case detail::type::bool_type:
    return vis(arg.value_.bool_value);
  case detail::type::char_type:
    return vis(arg.value_.char_value);
  case detail::type::float_type:
    return vis(arg.value_.float_value);
  case detail::type::double_type:
    return vis(arg.value_.double_value);
  case detail::type::long_double_type:
    return vis(arg.value_.long_double_value);
  case detail::type::cstring_type:
    return vis(arg.value_.string.data);
  case detail::type::string_type:
    using sv = basic_string_view<typename Context::char_type>;
    return vis(sv(arg.value_.string.data, arg.value_.string.size));
  case detail::type::pointer_type:
    return vis(arg.value_.pointer);
  case detail::type::custom_type:
    return vis(typename basic_format_arg<Context>::handle(arg.value_.custom));
  }
  return vis(monostate());
}

FMT_BEGIN_DETAIL_NAMESPACE

template <typename Char, typename InputIt>
auto copy_str(InputIt begin, InputIt end, appender out) -> appender {
  get_container(out).append(begin, end);
  return out;
}

#if FMT_GCC_VERSION && FMT_GCC_VERSION < 500
// A workaround for gcc 4.8 to make void_t work in a SFINAE context.
template <typename... Ts> struct void_t_impl { using type = void; };
template <typename... Ts>
using void_t = typename detail::void_t_impl<Ts...>::type;
#else
template <typename...> using void_t = void;
#endif

template <typename It, typename T, typename Enable = void>
struct is_output_iterator : std::false_type {};

template <typename It, typename T>
struct is_output_iterator<
    It, T,
    void_t<typename std::iterator_traits<It>::iterator_category,
           decltype(*std::declval<It>() = std::declval<T>())>>
    : std::true_type {};

template <typename OutputIt>
struct is_back_insert_iterator : std::false_type {};
template <typename Container>
struct is_back_insert_iterator<std::back_insert_iterator<Container>>
    : std::true_type {};

template <typename OutputIt>
struct is_contiguous_back_insert_iterator : std::false_type {};
template <typename Container>
struct is_contiguous_back_insert_iterator<std::back_insert_iterator<Container>>
    : is_contiguous<Container> {};
template <>
struct is_contiguous_back_insert_iterator<appender> : std::true_type {};

// A type-erased reference to an std::locale to avoid heavy <locale> include.
class locale_ref {
 private:
  const void* locale_;  // A type-erased pointer to std::locale.

 public:
  constexpr locale_ref() : locale_(nullptr) {}
  template <typename Locale> explicit locale_ref(const Locale& loc);

  explicit operator bool() const FMT_NOEXCEPT { return locale_ != nullptr; }

  template <typename Locale> auto get() const -> Locale;
};

template <typename> constexpr auto encode_types() -> unsigned long long {
  return 0;
}

template <typename Context, typename Arg, typename... Args>
constexpr auto encode_types() -> unsigned long long {
  return static_cast<unsigned>(mapped_type_constant<Arg, Context>::value) |
         (encode_types<Context, Args...>() << packed_arg_bits);
}

template <typename Context, typename T>
FMT_CONSTEXPR auto make_arg(const T& value) -> basic_format_arg<Context> {
  basic_format_arg<Context> arg;
  arg.type_ = mapped_type_constant<T, Context>::value;
  arg.value_ = arg_mapper<Context>().map(value);
  return arg;
}

// The type template parameter is there to avoid an ODR violation when using
// a fallback formatter in one translation unit and an implicit conversion in
// another (not recommended).
template <bool IS_PACKED, typename Context, type, typename T,
          FMT_ENABLE_IF(IS_PACKED)>
FMT_CONSTEXPR FMT_INLINE auto make_arg(T&& val) -> value<Context> {
  const auto& arg = arg_mapper<Context>().map(std::forward<T>(val));

  constexpr bool formattable_char =
      !std::is_same<decltype(arg), const unformattable_char&>::value;
  static_assert(formattable_char, "Mixing character types is disallowed.");

  constexpr bool formattable_const =
      !std::is_same<decltype(arg), const unformattable_const&>::value;
  static_assert(formattable_const, "Cannot format a const argument.");

  // Formatting of arbitrary pointers is disallowed. If you want to output
  // a pointer cast it to "void *" or "const void *". In particular, this
  // forbids formatting of "[const] volatile char *" which is printed as bool
  // by iostreams.
  constexpr bool formattable_pointer =
      !std::is_same<decltype(arg), const unformattable_pointer&>::value;
  static_assert(formattable_pointer,
                "Formatting of non-void pointers is disallowed.");

  constexpr bool formattable =
      !std::is_same<decltype(arg), const unformattable&>::value;
  static_assert(
      formattable,
      "Cannot format an argument. To make type T formattable provide a "
      "formatter<T> specialization: https://fmt.dev/latest/api.html#udt");
  return {arg};
}

template <bool IS_PACKED, typename Context, type, typename T,
          FMT_ENABLE_IF(!IS_PACKED)>
inline auto make_arg(const T& value) -> basic_format_arg<Context> {
  return make_arg<Context>(value);
}
FMT_END_DETAIL_NAMESPACE

// Formatting context.
template <typename OutputIt, typename Char> class basic_format_context {
 public:
  /** The character type for the output. */
  using char_type = Char;

 private:
  OutputIt out_;
  basic_format_args<basic_format_context> args_;
  detail::locale_ref loc_;

 public:
  using iterator = OutputIt;
  using format_arg = basic_format_arg<basic_format_context>;
  using parse_context_type = basic_format_parse_context<Char>;
  template <typename T> using formatter_type = formatter<T, char_type>;

  basic_format_context(basic_format_context&&) = default;
  basic_format_context(const basic_format_context&) = delete;
  void operator=(const basic_format_context&) = delete;
  /**
   Constructs a ``basic_format_context`` object. References to the arguments are
   stored in the object so make sure they have appropriate lifetimes.
   */
  constexpr basic_format_context(
      OutputIt out, basic_format_args<basic_format_context> ctx_args,
      detail::locale_ref loc = detail::locale_ref())
      : out_(out), args_(ctx_args), loc_(loc) {}

  constexpr auto arg(int id) const -> format_arg { return args_.get(id); }
  FMT_CONSTEXPR auto arg(basic_string_view<char_type> name) -> format_arg {
    return args_.get(name);
  }
  FMT_CONSTEXPR auto arg_id(basic_string_view<char_type> name) -> int {
    return args_.get_id(name);
  }
  auto args() const -> const basic_format_args<basic_format_context>& {
    return args_;
  }

  FMT_CONSTEXPR auto error_handler() -> detail::error_handler { return {}; }
  void on_error(const char* message) { error_handler().on_error(message); }

  // Returns an iterator to the beginning of the output range.
  FMT_CONSTEXPR auto out() -> iterator { return out_; }

  // Advances the begin iterator to ``it``.
  void advance_to(iterator it) {
    if (!detail::is_back_insert_iterator<iterator>()) out_ = it;
  }

  FMT_CONSTEXPR auto locale() -> detail::locale_ref { return loc_; }
};

template <typename Char>
using buffer_context =
    basic_format_context<detail::buffer_appender<Char>, Char>;
using format_context = buffer_context<char>;

// Workaround an alias issue: https://stackoverflow.com/q/62767544/471164.
#define FMT_BUFFER_CONTEXT(Char) \
  basic_format_context<detail::buffer_appender<Char>, Char>

template <typename T, typename Char = char>
using is_formattable = bool_constant<
    !std::is_base_of<detail::unformattable,
                     decltype(detail::arg_mapper<buffer_context<Char>>().map(
                         std::declval<T>()))>::value &&
    !detail::has_fallback_formatter<T, Char>::value>;

/**
  \rst
  An array of references to arguments. It can be implicitly converted into
  `~fmt::basic_format_args` for passing into type-erased formatting functions
  such as `~fmt::vformat`.
  \endrst
 */
template <typename Context, typename... Args>
class format_arg_store
#if FMT_GCC_VERSION && FMT_GCC_VERSION < 409
    // Workaround a GCC template argument substitution bug.
    : public basic_format_args<Context>
#endif
{
 private:
  static const size_t num_args = sizeof...(Args);
  static const size_t num_named_args = detail::count_named_args<Args...>();
  static const bool is_packed = num_args <= detail::max_packed_args;

  using value_type = conditional_t<is_packed, detail::value<Context>,
                                   basic_format_arg<Context>>;

  detail::arg_data<value_type, typename Context::char_type, num_args,
                   num_named_args>
      data_;

  friend class basic_format_args<Context>;

  static constexpr unsigned long long desc =
      (is_packed ? detail::encode_types<Context, Args...>()
                 : detail::is_unpacked_bit | num_args) |
      (num_named_args != 0
           ? static_cast<unsigned long long>(detail::has_named_args_bit)
           : 0);

 public:
  template <typename... T>
  FMT_CONSTEXPR FMT_INLINE format_arg_store(T&&... args)
      :
#if FMT_GCC_VERSION && FMT_GCC_VERSION < 409
        basic_format_args<Context>(*this),
#endif
        data_{detail::make_arg<
            is_packed, Context,
            detail::mapped_type_constant<remove_cvref_t<T>, Context>::value>(
            std::forward<T>(args))...} {
    detail::init_named_args(data_.named_args(), 0, 0, args...);
  }
};

/**
  \rst
  Constructs a `~fmt::format_arg_store` object that contains references to
  arguments and can be implicitly converted to `~fmt::format_args`. `Context`
  can be omitted in which case it defaults to `~fmt::context`.
  See `~fmt::arg` for lifetime considerations.
  \endrst
 */
template <typename Context = format_context, typename... Args>
constexpr auto make_format_args(Args&&... args)
    -> format_arg_store<Context, remove_cvref_t<Args>...> {
  return {std::forward<Args>(args)...};
}

/**
  \rst
  Returns a named argument to be used in a formatting function.
  It should only be used in a call to a formatting function or
  `dynamic_format_arg_store::push_back`.

  **Example**::

    fmt::print("Elapsed time: {s:.2f} seconds", fmt::arg("s", 1.23));
  \endrst
 */
template <typename Char, typename T>
inline auto arg(const Char* name, const T& arg) -> detail::named_arg<Char, T> {
  static_assert(!detail::is_named_arg<T>(), "nested named arguments");
  return {name, arg};
}

/**
  \rst
  A view of a collection of formatting arguments. To avoid lifetime issues it
  should only be used as a parameter type in type-erased functions such as
  ``vformat``::

    void vlog(string_view format_str, format_args args);  // OK
    format_args args = make_format_args(42);  // Error: dangling reference
  \endrst
 */
template <typename Context> class basic_format_args {
 public:
  using size_type = int;
  using format_arg = basic_format_arg<Context>;

 private:
  // A descriptor that contains information about formatting arguments.
  // If the number of arguments is less or equal to max_packed_args then
  // argument types are passed in the descriptor. This reduces binary code size
  // per formatting function call.
  unsigned long long desc_;
  union {
    // If is_packed() returns true then argument values are stored in values_;
    // otherwise they are stored in args_. This is done to improve cache
    // locality and reduce compiled code size since storing larger objects
    // may require more code (at least on x86-64) even if the same amount of
    // data is actually copied to stack. It saves ~10% on the bloat test.
    const detail::value<Context>* values_;
    const format_arg* args_;
  };

  constexpr auto is_packed() const -> bool {
    return (desc_ & detail::is_unpacked_bit) == 0;
  }
  auto has_named_args() const -> bool {
    return (desc_ & detail::has_named_args_bit) != 0;
  }

  FMT_CONSTEXPR auto type(int index) const -> detail::type {
    int shift = index * detail::packed_arg_bits;
    unsigned int mask = (1 << detail::packed_arg_bits) - 1;
    return static_cast<detail::type>((desc_ >> shift) & mask);
  }

  constexpr FMT_INLINE basic_format_args(unsigned long long desc,
                                         const detail::value<Context>* values)
      : desc_(desc), values_(values) {}
  constexpr basic_format_args(unsigned long long desc, const format_arg* args)
      : desc_(desc), args_(args) {}

 public:
  constexpr basic_format_args() : desc_(0), args_(nullptr) {}

  /**
   \rst
   Constructs a `basic_format_args` object from `~fmt::format_arg_store`.
   \endrst
   */
  template <typename... Args>
  constexpr FMT_INLINE basic_format_args(
      const format_arg_store<Context, Args...>& store)
      : basic_format_args(format_arg_store<Context, Args...>::desc,
                          store.data_.args()) {}

  /**
   \rst
   Constructs a `basic_format_args` object from
   `~fmt::dynamic_format_arg_store`.
   \endrst
   */
  constexpr FMT_INLINE basic_format_args(
      const dynamic_format_arg_store<Context>& store)
      : basic_format_args(store.get_types(), store.data()) {}

  /**
   \rst
   Constructs a `basic_format_args` object from a dynamic set of arguments.
   \endrst
   */
  constexpr basic_format_args(const format_arg* args, int count)
      : basic_format_args(detail::is_unpacked_bit | detail::to_unsigned(count),
                          args) {}

  /** Returns the argument with the specified id. */
  FMT_CONSTEXPR auto get(int id) const -> format_arg {
    format_arg arg;
    if (!is_packed()) {
      if (id < max_size()) arg = args_[id];
      return arg;
    }
    if (id >= detail::max_packed_args) return arg;
    arg.type_ = type(id);
    if (arg.type_ == detail::type::none_type) return arg;
    arg.value_ = values_[id];
    return arg;
  }

  template <typename Char>
  auto get(basic_string_view<Char> name) const -> format_arg {
    int id = get_id(name);
    return id >= 0 ? get(id) : format_arg();
  }

  template <typename Char>
  auto get_id(basic_string_view<Char> name) const -> int {
    if (!has_named_args()) return -1;
    const auto& named_args =
        (is_packed() ? values_[-1] : args_[-1].value_).named_args;
    for (size_t i = 0; i < named_args.size; ++i) {
      if (named_args.data[i].name == name) return named_args.data[i].id;
    }
    return -1;
  }

  auto max_size() const -> int {
    unsigned long long max_packed = detail::max_packed_args;
    return static_cast<int>(is_packed() ? max_packed
                                        : desc_ & ~detail::is_unpacked_bit);
  }
};

/** An alias to ``basic_format_args<format_context>``. */
// A separate type would result in shorter symbols but break ABI compatibility
// between clang and gcc on ARM (#1919).
using format_args = basic_format_args<format_context>;

// We cannot use enum classes as bit fields because of a gcc bug
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61414.
namespace align {
enum type { none, left, right, center, numeric };
}
using align_t = align::type;
namespace sign {
enum type { none, minus, plus, space };
}
using sign_t = sign::type;

FMT_BEGIN_DETAIL_NAMESPACE

// Workaround an array initialization issue in gcc 4.8.
template <typename Char> struct fill_t {
 private:
  enum { max_size = 4 };
  Char data_[max_size] = {Char(' '), Char(0), Char(0), Char(0)};
  unsigned char size_ = 1;

 public:
  FMT_CONSTEXPR void operator=(basic_string_view<Char> s) {
    auto size = s.size();
    if (size > max_size) return throw_format_error("invalid fill");
    for (size_t i = 0; i < size; ++i) data_[i] = s[i];
    size_ = static_cast<unsigned char>(size);
  }

  constexpr auto size() const -> size_t { return size_; }
  constexpr auto data() const -> const Char* { return data_; }

  FMT_CONSTEXPR auto operator[](size_t index) -> Char& { return data_[index]; }
  FMT_CONSTEXPR auto operator[](size_t index) const -> const Char& {
    return data_[index];
  }
};
FMT_END_DETAIL_NAMESPACE

enum class presentation_type : unsigned char {
  none,
  // Integer types should go first,
  dec,             // 'd'
  oct,             // 'o'
  hex_lower,       // 'x'
  hex_upper,       // 'X'
  bin_lower,       // 'b'
  bin_upper,       // 'B'
  hexfloat_lower,  // 'a'
  hexfloat_upper,  // 'A'
  exp_lower,       // 'e'
  exp_upper,       // 'E'
  fixed_lower,     // 'f'
  fixed_upper,     // 'F'
  general_lower,   // 'g'
  general_upper,   // 'G'
  chr,             // 'c'
  string,          // 's'
  pointer          // 'p'
};

// Format specifiers for built-in and string types.
template <typename Char> struct basic_format_specs {
  int width;
  int precision;
  presentation_type type;
  align_t align : 4;
  sign_t sign : 3;
  bool alt : 1;  // Alternate form ('#').
  bool localized : 1;
  detail::fill_t<Char> fill;

  constexpr basic_format_specs()
      : width(0),
        precision(-1),
        type(presentation_type::none),
        align(align::none),
        sign(sign::none),
        alt(false),
        localized(false) {}
};

using format_specs = basic_format_specs<char>;

FMT_BEGIN_DETAIL_NAMESPACE

enum class arg_id_kind { none, index, name };

// An argument reference.
template <typename Char> struct arg_ref {
  FMT_CONSTEXPR arg_ref() : kind(arg_id_kind::none), val() {}

  FMT_CONSTEXPR explicit arg_ref(int index)
      : kind(arg_id_kind::index), val(index) {}
  FMT_CONSTEXPR explicit arg_ref(basic_string_view<Char> name)
      : kind(arg_id_kind::name), val(name) {}

  FMT_CONSTEXPR auto operator=(int idx) -> arg_ref& {
    kind = arg_id_kind::index;
    val.index = idx;
    return *this;
  }

  arg_id_kind kind;
  union value {
    FMT_CONSTEXPR value(int id = 0) : index{id} {}
    FMT_CONSTEXPR value(basic_string_view<Char> n) : name(n) {}

    int index;
    basic_string_view<Char> name;
  } val;
};

// Format specifiers with width and precision resolved at formatting rather
// than parsing time to allow re-using the same parsed specifiers with
// different sets of arguments (precompilation of format strings).
template <typename Char>
struct dynamic_format_specs : basic_format_specs<Char> {
  arg_ref<Char> width_ref;
  arg_ref<Char> precision_ref;
};

struct auto_id {};

// A format specifier handler that sets fields in basic_format_specs.
template <typename Char> class specs_setter {
 protected:
  basic_format_specs<Char>& specs_;

 public:
  explicit FMT_CONSTEXPR specs_setter(basic_format_specs<Char>& specs)
      : specs_(specs) {}

  FMT_CONSTEXPR specs_setter(const specs_setter& other)
      : specs_(other.specs_) {}

  FMT_CONSTEXPR void on_align(align_t align) { specs_.align = align; }
  FMT_CONSTEXPR void on_fill(basic_string_view<Char> fill) {
    specs_.fill = fill;
  }
  FMT_CONSTEXPR void on_sign(sign_t s) { specs_.sign = s; }
  FMT_CONSTEXPR void on_hash() { specs_.alt = true; }
  FMT_CONSTEXPR void on_localized() { specs_.localized = true; }

  FMT_CONSTEXPR void on_zero() {
    if (specs_.align == align::none) specs_.align = align::numeric;
    specs_.fill[0] = Char('0');
  }

  FMT_CONSTEXPR void on_width(int width) { specs_.width = width; }
  FMT_CONSTEXPR void on_precision(int precision) {
    specs_.precision = precision;
  }
  FMT_CONSTEXPR void end_precision() {}

  FMT_CONSTEXPR void on_type(presentation_type type) { specs_.type = type; }
};

// Format spec handler that saves references to arguments representing dynamic
// width and precision to be resolved at formatting time.
template <typename ParseContext>
class dynamic_specs_handler
    : public specs_setter<typename ParseContext::char_type> {
 public:
  using char_type = typename ParseContext::char_type;

  FMT_CONSTEXPR dynamic_specs_handler(dynamic_format_specs<char_type>& specs,
                                      ParseContext& ctx)
      : specs_setter<char_type>(specs), specs_(specs), context_(ctx) {}

  FMT_CONSTEXPR dynamic_specs_handler(const dynamic_specs_handler& other)
      : specs_setter<char_type>(other),
        specs_(other.specs_),
        context_(other.context_) {}

  template <typename Id> FMT_CONSTEXPR void on_dynamic_width(Id arg_id) {
    specs_.width_ref = make_arg_ref(arg_id);
  }

  template <typename Id> FMT_CONSTEXPR void on_dynamic_precision(Id arg_id) {
    specs_.precision_ref = make_arg_ref(arg_id);
  }

  FMT_CONSTEXPR void on_error(const char* message) {
    context_.on_error(message);
  }

 private:
  dynamic_format_specs<char_type>& specs_;
  ParseContext& context_;

  using arg_ref_type = arg_ref<char_type>;

  FMT_CONSTEXPR auto make_arg_ref(int arg_id) -> arg_ref_type {
    context_.check_arg_id(arg_id);
    return arg_ref_type(arg_id);
  }

  FMT_CONSTEXPR auto make_arg_ref(auto_id) -> arg_ref_type {
    return arg_ref_type(context_.next_arg_id());
  }

  FMT_CONSTEXPR auto make_arg_ref(basic_string_view<char_type> arg_id)
      -> arg_ref_type {
    context_.check_arg_id(arg_id);
    basic_string_view<char_type> format_str(
        context_.begin(), to_unsigned(context_.end() - context_.begin()));
    return arg_ref_type(arg_id);
  }
};

template <typename Char> constexpr bool is_ascii_letter(Char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Converts a character to ASCII. Returns a number > 127 on conversion failure.
template <typename Char, FMT_ENABLE_IF(std::is_integral<Char>::value)>
constexpr auto to_ascii(Char value) -> Char {
  return value;
}
template <typename Char, FMT_ENABLE_IF(std::is_enum<Char>::value)>
constexpr auto to_ascii(Char value) ->
    typename std::underlying_type<Char>::type {
  return value;
}

template <typename Char>
FMT_CONSTEXPR auto code_point_length(const Char* begin) -> int {
  if (const_check(sizeof(Char) != 1)) return 1;
  auto lengths =
      "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\0\0\0\0\0\0\0\0\2\2\2\2\3\3\4";
  int len = lengths[static_cast<unsigned char>(*begin) >> 3];

  // Compute the pointer to the next character early so that the next
  // iteration can start working on the next character. Neither Clang
  // nor GCC figure out this reordering on their own.
  return len + !len;
}

// Return the result via the out param to workaround gcc bug 77539.
template <bool IS_CONSTEXPR, typename T, typename Ptr = const T*>
FMT_CONSTEXPR auto find(Ptr first, Ptr last, T value, Ptr& out) -> bool {
  for (out = first; out != last; ++out) {
    if (*out == value) return true;
  }
  return false;
}

template <>
inline auto find<false, char>(const char* first, const char* last, char value,
                              const char*& out) -> bool {
  out = static_cast<const char*>(
      std::memchr(first, value, to_unsigned(last - first)));
  return out != nullptr;
}

// Parses the range [begin, end) as an unsigned integer. This function assumes
// that the range is non-empty and the first character is a digit.
template <typename Char>
FMT_CONSTEXPR auto parse_nonnegative_int(const Char*& begin, const Char* end,
                                         int error_value) noexcept -> int {
  FMT_ASSERT(begin != end && '0' <= *begin && *begin <= '9', "");
  unsigned value = 0, prev = 0;
  auto p = begin;
  do {
    prev = value;
    value = value * 10 + unsigned(*p - '0');
    ++p;
  } while (p != end && '0' <= *p && *p <= '9');
  auto num_digits = p - begin;
  begin = p;
  if (num_digits <= std::numeric_limits<int>::digits10)
    return static_cast<int>(value);
  // Check for overflow.
  const unsigned max = to_unsigned((std::numeric_limits<int>::max)());
  return num_digits == std::numeric_limits<int>::digits10 + 1 &&
                 prev * 10ull + unsigned(p[-1] - '0') <= max
             ? static_cast<int>(value)
             : error_value;
}

// Parses fill and alignment.
template <typename Char, typename Handler>
FMT_CONSTEXPR auto parse_align(const Char* begin, const Char* end,
                               Handler&& handler) -> const Char* {
  FMT_ASSERT(begin != end, "");
  auto align = align::none;
  auto p = begin + code_point_length(begin);
  if (p >= end) p = begin;
  for (;;) {
    switch (to_ascii(*p)) {
    case '<':
      align = align::left;
      break;
    case '>':
      align = align::right;
      break;
    case '^':
      align = align::center;
      break;
    default:
      break;
    }
    if (align != align::none) {
      if (p != begin) {
        auto c = *begin;
        if (c == '{')
          return handler.on_error("invalid fill character '{'"), begin;
        handler.on_fill(basic_string_view<Char>(begin, to_unsigned(p - begin)));
        begin = p + 1;
      } else
        ++begin;
      handler.on_align(align);
      break;
    } else if (p == begin) {
      break;
    }
    p = begin;
  }
  return begin;
}

template <typename Char> FMT_CONSTEXPR bool is_name_start(Char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || '_' == c;
}

template <typename Char, typename IDHandler>
FMT_CONSTEXPR auto do_parse_arg_id(const Char* begin, const Char* end,
                                   IDHandler&& handler) -> const Char* {
  FMT_ASSERT(begin != end, "");
  Char c = *begin;
  if (c >= '0' && c <= '9') {
    int index = 0;
    if (c != '0')
      index =
          parse_nonnegative_int(begin, end, (std::numeric_limits<int>::max)());
    else
      ++begin;
    if (begin == end || (*begin != '}' && *begin != ':'))
      handler.on_error("invalid format string");
    else
      handler(index);
    return begin;
  }
  if (!is_name_start(c)) {
    handler.on_error("invalid format string");
    return begin;
  }
  auto it = begin;
  do {
    ++it;
  } while (it != end && (is_name_start(c = *it) || ('0' <= c && c <= '9')));
  handler(basic_string_view<Char>(begin, to_unsigned(it - begin)));
  return it;
}

template <typename Char, typename IDHandler>
FMT_CONSTEXPR FMT_INLINE auto parse_arg_id(const Char* begin, const Char* end,
                                           IDHandler&& handler) -> const Char* {
  Char c = *begin;
  if (c != '}' && c != ':') return do_parse_arg_id(begin, end, handler);
  handler();
  return begin;
}

template <typename Char, typename Handler>
FMT_CONSTEXPR auto parse_width(const Char* begin, const Char* end,
                               Handler&& handler) -> const Char* {
  using detail::auto_id;
  struct width_adapter {
    Handler& handler;

    FMT_CONSTEXPR void operator()() { handler.on_dynamic_width(auto_id()); }
    FMT_CONSTEXPR void operator()(int id) { handler.on_dynamic_width(id); }
    FMT_CONSTEXPR void operator()(basic_string_view<Char> id) {
      handler.on_dynamic_width(id);
    }
    FMT_CONSTEXPR void on_error(const char* message) {
      if (message) handler.on_error(message);
    }
  };

  FMT_ASSERT(begin != end, "");
  if ('0' <= *begin && *begin <= '9') {
    int width = parse_nonnegative_int(begin, end, -1);
    if (width != -1)
      handler.on_width(width);
    else
      handler.on_error("number is too big");
  } else if (*begin == '{') {
    ++begin;
    if (begin != end) begin = parse_arg_id(begin, end, width_adapter{handler});
    if (begin == end || *begin != '}')
      return handler.on_error("invalid format string"), begin;
    ++begin;
  }
  return begin;
}

template <typename Char, typename Handler>
FMT_CONSTEXPR auto parse_precision(const Char* begin, const Char* end,
                                   Handler&& handler) -> const Char* {
  using detail::auto_id;
  struct precision_adapter {
    Handler& handler;

    FMT_CONSTEXPR void operator()() { handler.on_dynamic_precision(auto_id()); }
    FMT_CONSTEXPR void operator()(int id) { handler.on_dynamic_precision(id); }
    FMT_CONSTEXPR void operator()(basic_string_view<Char> id) {
      handler.on_dynamic_precision(id);
    }
    FMT_CONSTEXPR void on_error(const char* message) {
      if (message) handler.on_error(message);
    }
  };

  ++begin;
  auto c = begin != end ? *begin : Char();
  if ('0' <= c && c <= '9') {
    auto precision = parse_nonnegative_int(begin, end, -1);
    if (precision != -1)
      handler.on_precision(precision);
    else
      handler.on_error("number is too big");
  } else if (c == '{') {
    ++begin;
    if (begin != end)
      begin = parse_arg_id(begin, end, precision_adapter{handler});
    if (begin == end || *begin++ != '}')
      return handler.on_error("invalid format string"), begin;
  } else {
    return handler.on_error("missing precision specifier"), begin;
  }
  handler.end_precision();
  return begin;
}

template <typename Char>
FMT_CONSTEXPR auto parse_presentation_type(Char type) -> presentation_type {
  switch (to_ascii(type)) {
  case 'd':
    return presentation_type::dec;
  case 'o':
    return presentation_type::oct;
  case 'x':
    return presentation_type::hex_lower;
  case 'X':
    return presentation_type::hex_upper;
  case 'b':
    return presentation_type::bin_lower;
  case 'B':
    return presentation_type::bin_upper;
  case 'a':
    return presentation_type::hexfloat_lower;
  case 'A':
    return presentation_type::hexfloat_upper;
  case 'e':
    return presentation_type::exp_lower;
  case 'E':
    return presentation_type::exp_upper;
  case 'f':
    return presentation_type::fixed_lower;
  case 'F':
    return presentation_type::fixed_upper;
  case 'g':
    return presentation_type::general_lower;
  case 'G':
    return presentation_type::general_upper;
  case 'c':
    return presentation_type::chr;
  case 's':
    return presentation_type::string;
  case 'p':
    return presentation_type::pointer;
  default:
    return presentation_type::none;
  }
}

// Parses standard format specifiers and sends notifications about parsed
// components to handler.
template <typename Char, typename SpecHandler>
FMT_CONSTEXPR FMT_INLINE auto parse_format_specs(const Char* begin,
                                                 const Char* end,
                                                 SpecHandler&& handler)
    -> const Char* {
  if (1 < end - begin && begin[1] == '}' && is_ascii_letter(*begin) &&
      *begin != 'L') {
    presentation_type type = parse_presentation_type(*begin++);
    if (type == presentation_type::none)
      handler.on_error("invalid type specifier");
    handler.on_type(type);
    return begin;
  }

  if (begin == end) return begin;

  begin = parse_align(begin, end, handler);
  if (begin == end) return begin;

  // Parse sign.
  switch (to_ascii(*begin)) {
  case '+':
    handler.on_sign(sign::plus);
    ++begin;
    break;
  case '-':
    handler.on_sign(sign::minus);
    ++begin;
    break;
  case ' ':
    handler.on_sign(sign::space);
    ++begin;
    break;
  default:
    break;
  }
  if (begin == end) return begin;

  if (*begin == '#') {
    handler.on_hash();
    if (++begin == end) return begin;
  }

  // Parse zero flag.
  if (*begin == '0') {
    handler.on_zero();
    if (++begin == end) return begin;
  }

  begin = parse_width(begin, end, handler);
  if (begin == end) return begin;

  // Parse precision.
  if (*begin == '.') {
    begin = parse_precision(begin, end, handler);
    if (begin == end) return begin;
  }

  if (*begin == 'L') {
    handler.on_localized();
    ++begin;
  }

  // Parse type.
  if (begin != end && *begin != '}') {
    presentation_type type = parse_presentation_type(*begin++);
    if (type == presentation_type::none)
      handler.on_error("invalid type specifier");
    handler.on_type(type);
  }
  return begin;
}

template <typename Char, typename Handler>
FMT_CONSTEXPR auto parse_replacement_field(const Char* begin, const Char* end,
                                           Handler&& handler) -> const Char* {
  struct id_adapter {
    Handler& handler;
    int arg_id;

    FMT_CONSTEXPR void operator()() { arg_id = handler.on_arg_id(); }
    FMT_CONSTEXPR void operator()(int id) { arg_id = handler.on_arg_id(id); }
    FMT_CONSTEXPR void operator()(basic_string_view<Char> id) {
      arg_id = handler.on_arg_id(id);
    }
    FMT_CONSTEXPR void on_error(const char* message) {
      if (message) handler.on_error(message);
    }
  };

  ++begin;
  if (begin == end) return handler.on_error("invalid format string"), end;
  if (*begin == '}') {
    handler.on_replacement_field(handler.on_arg_id(), begin);
  } else if (*begin == '{') {
    handler.on_text(begin, begin + 1);
  } else {
    auto adapter = id_adapter{handler, 0};
    begin = parse_arg_id(begin, end, adapter);
    Char c = begin != end ? *begin : Char();
    if (c == '}') {
      handler.on_replacement_field(adapter.arg_id, begin);
    } else if (c == ':') {
      begin = handler.on_format_specs(adapter.arg_id, begin + 1, end);
      if (begin == end || *begin != '}')
        return handler.on_error("unknown format specifier"), end;
    } else {
      return handler.on_error("missing '}' in format string"), end;
    }
  }
  return begin + 1;
}

template <bool IS_CONSTEXPR, typename Char, typename Handler>
FMT_CONSTEXPR FMT_INLINE void parse_format_string(
    basic_string_view<Char> format_str, Handler&& handler) {
  // Workaround a name-lookup bug in MSVC's modules implementation.
  using detail::find;

  auto begin = format_str.data();
  auto end = begin + format_str.size();
  if (end - begin < 32) {
    // Use a simple loop instead of memchr for small strings.
    const Char* p = begin;
    while (p != end) {
      auto c = *p++;
      if (c == '{') {
        handler.on_text(begin, p - 1);
        begin = p = parse_replacement_field(p - 1, end, handler);
      } else if (c == '}') {
        if (p == end || *p != '}')
          return handler.on_error("unmatched '}' in format string");
        handler.on_text(begin, p);
        begin = ++p;
      }
    }
    handler.on_text(begin, end);
    return;
  }
  struct writer {
    FMT_CONSTEXPR void operator()(const Char* pbegin, const Char* pend) {
      if (pbegin == pend) return;
      for (;;) {
        const Char* p = nullptr;
        if (!find<IS_CONSTEXPR>(pbegin, pend, Char('}'), p))
          return handler_.on_text(pbegin, pend);
        ++p;
        if (p == pend || *p != '}')
          return handler_.on_error("unmatched '}' in format string");
        handler_.on_text(pbegin, p);
        pbegin = p + 1;
      }
    }
    Handler& handler_;
  } write{handler};
  while (begin != end) {
    // Doing two passes with memchr (one for '{' and another for '}') is up to
    // 2.5x faster than the naive one-pass implementation on big format strings.
    const Char* p = begin;
    if (*begin != '{' && !find<IS_CONSTEXPR>(begin + 1, end, Char('{'), p))
      return write(begin, end);
    write(begin, p);
    begin = parse_replacement_field(p, end, handler);
  }
}

template <typename T, typename ParseContext>
FMT_CONSTEXPR auto parse_format_specs(ParseContext& ctx)
    -> decltype(ctx.begin()) {
  using char_type = typename ParseContext::char_type;
  using context = buffer_context<char_type>;
  using mapped_type = conditional_t<
      mapped_type_constant<T, context>::value != type::custom_type,
      decltype(arg_mapper<context>().map(std::declval<const T&>())), T>;
  auto f = conditional_t<has_formatter<mapped_type, context>::value,
                         formatter<mapped_type, char_type>,
                         fallback_formatter<T, char_type>>();
  return f.parse(ctx);
}

// A parse context with extra argument id checks. It is only used at compile
// time because adding checks at runtime would introduce substantial overhead
// and would be redundant since argument ids are checked when arguments are
// retrieved anyway.
template <typename Char, typename ErrorHandler = error_handler>
class compile_parse_context
    : public basic_format_parse_context<Char, ErrorHandler> {
 private:
  int num_args_;
  using base = basic_format_parse_context<Char, ErrorHandler>;

 public:
  explicit FMT_CONSTEXPR compile_parse_context(
      basic_string_view<Char> format_str,
      int num_args = (std::numeric_limits<int>::max)(), ErrorHandler eh = {})
      : base(format_str, eh), num_args_(num_args) {}

  FMT_CONSTEXPR auto next_arg_id() -> int {
    int id = base::next_arg_id();
    if (id >= num_args_) this->on_error("argument not found");
    return id;
  }

  FMT_CONSTEXPR void check_arg_id(int id) {
    base::check_arg_id(id);
    if (id >= num_args_) this->on_error("argument not found");
  }
  using base::check_arg_id;
};

template <typename ErrorHandler>
FMT_CONSTEXPR void check_int_type_spec(presentation_type type,
                                       ErrorHandler&& eh) {
  if (type > presentation_type::bin_upper && type != presentation_type::chr)
    eh.on_error("invalid type specifier");
}

// Checks char specs and returns true if the type spec is char (and not int).
template <typename Char, typename ErrorHandler = error_handler>
FMT_CONSTEXPR auto check_char_specs(const basic_format_specs<Char>& specs,
                                    ErrorHandler&& eh = {}) -> bool {
  if (specs.type != presentation_type::none &&
      specs.type != presentation_type::chr) {
    check_int_type_spec(specs.type, eh);
    return false;
  }
  if (specs.align == align::numeric || specs.sign != sign::none || specs.alt)
    eh.on_error("invalid format specifier for char");
  return true;
}

// A floating-point presentation format.
enum class float_format : unsigned char {
  general,  // General: exponent notation or fixed point based on magnitude.
  exp,      // Exponent notation with the default precision of 6, e.g. 1.2e-3.
  fixed,    // Fixed point with the default precision of 6, e.g. 0.0012.
  hex
};

struct float_specs {
  int precision;
  float_format format : 8;
  sign_t sign : 8;
  bool upper : 1;
  bool locale : 1;
  bool binary32 : 1;
  bool fallback : 1;
  bool showpoint : 1;
};

template <typename ErrorHandler = error_handler, typename Char>
FMT_CONSTEXPR auto parse_float_type_spec(const basic_format_specs<Char>& specs,
                                         ErrorHandler&& eh = {})
    -> float_specs {
  auto result = float_specs();
  result.showpoint = specs.alt;
  result.locale = specs.localized;
  switch (specs.type) {
  case presentation_type::none:
    result.format = float_format::general;
    break;
  case presentation_type::general_upper:
    result.upper = true;
    FMT_FALLTHROUGH;
  case presentation_type::general_lower:
    result.format = float_format::general;
    break;
  case presentation_type::exp_upper:
    result.upper = true;
    FMT_FALLTHROUGH;
  case presentation_type::exp_lower:
    result.format = float_format::exp;
    result.showpoint |= specs.precision != 0;
    break;
  case presentation_type::fixed_upper:
    result.upper = true;
    FMT_FALLTHROUGH;
  case presentation_type::fixed_lower:
    result.format = float_format::fixed;
    result.showpoint |= specs.precision != 0;
    break;
  case presentation_type::hexfloat_upper:
    result.upper = true;
    FMT_FALLTHROUGH;
  case presentation_type::hexfloat_lower:
    result.format = float_format::hex;
    break;
  default:
    eh.on_error("invalid type specifier");
    break;
  }
  return result;
}

template <typename ErrorHandler = error_handler>
FMT_CONSTEXPR auto check_cstring_type_spec(presentation_type type,
                                           ErrorHandler&& eh = {}) -> bool {
  if (type == presentation_type::none || type == presentation_type::string)
    return true;
  if (type != presentation_type::pointer) eh.on_error("invalid type specifier");
  return false;
}

template <typename ErrorHandler = error_handler>
FMT_CONSTEXPR void check_string_type_spec(presentation_type type,
                                          ErrorHandler&& eh = {}) {
  if (type != presentation_type::none && type != presentation_type::string)
    eh.on_error("invalid type specifier");
}

template <typename ErrorHandler>
FMT_CONSTEXPR void check_pointer_type_spec(presentation_type type,
                                           ErrorHandler&& eh) {
  if (type != presentation_type::none && type != presentation_type::pointer)
    eh.on_error("invalid type specifier");
}

// A parse_format_specs handler that checks if specifiers are consistent with
// the argument type.
template <typename Handler> class specs_checker : public Handler {
 private:
  detail::type arg_type_;

  FMT_CONSTEXPR void require_numeric_argument() {
    if (!is_arithmetic_type(arg_type_))
      this->on_error("format specifier requires numeric argument");
  }

 public:
  FMT_CONSTEXPR specs_checker(const Handler& handler, detail::type arg_type)
      : Handler(handler), arg_type_(arg_type) {}

  FMT_CONSTEXPR void on_align(align_t align) {
    if (align == align::numeric) require_numeric_argument();
    Handler::on_align(align);
  }

  FMT_CONSTEXPR void on_sign(sign_t s) {
    require_numeric_argument();
    if (is_integral_type(arg_type_) && arg_type_ != type::int_type &&
        arg_type_ != type::long_long_type && arg_type_ != type::char_type) {
      this->on_error("format specifier requires signed argument");
    }
    Handler::on_sign(s);
  }

  FMT_CONSTEXPR void on_hash() {
    require_numeric_argument();
    Handler::on_hash();
  }

  FMT_CONSTEXPR void on_localized() {
    require_numeric_argument();
    Handler::on_localized();
  }

  FMT_CONSTEXPR void on_zero() {
    require_numeric_argument();
    Handler::on_zero();
  }

  FMT_CONSTEXPR void end_precision() {
    if (is_integral_type(arg_type_) || arg_type_ == type::pointer_type)
      this->on_error("precision not allowed for this argument type");
  }
};

constexpr int invalid_arg_index = -1;

#if FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
template <int N, typename T, typename... Args, typename Char>
constexpr auto get_arg_index_by_name(basic_string_view<Char> name) -> int {
  if constexpr (detail::is_statically_named_arg<T>()) {
    if (name == T::name) return N;
  }
  if constexpr (sizeof...(Args) > 0)
    return get_arg_index_by_name<N + 1, Args...>(name);
  (void)name;  // Workaround an MSVC bug about "unused" parameter.
  return invalid_arg_index;
}
#endif

template <typename... Args, typename Char>
FMT_CONSTEXPR auto get_arg_index_by_name(basic_string_view<Char> name) -> int {
#if FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
  if constexpr (sizeof...(Args) > 0)
    return get_arg_index_by_name<0, Args...>(name);
#endif
  (void)name;
  return invalid_arg_index;
}

template <typename Char, typename ErrorHandler, typename... Args>
class format_string_checker {
 private:
  using parse_context_type = compile_parse_context<Char, ErrorHandler>;
  enum { num_args = sizeof...(Args) };

  // Format specifier parsing function.
  using parse_func = const Char* (*)(parse_context_type&);

  parse_context_type context_;
  parse_func parse_funcs_[num_args > 0 ? num_args : 1];

 public:
  explicit FMT_CONSTEXPR format_string_checker(
      basic_string_view<Char> format_str, ErrorHandler eh)
      : context_(format_str, num_args, eh),
        parse_funcs_{&parse_format_specs<Args, parse_context_type>...} {}

  FMT_CONSTEXPR void on_text(const Char*, const Char*) {}

  FMT_CONSTEXPR auto on_arg_id() -> int { return context_.next_arg_id(); }
  FMT_CONSTEXPR auto on_arg_id(int id) -> int {
    return context_.check_arg_id(id), id;
  }
  FMT_CONSTEXPR auto on_arg_id(basic_string_view<Char> id) -> int {
#if FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
    auto index = get_arg_index_by_name<Args...>(id);
    if (index == invalid_arg_index) on_error("named argument is not found");
    return context_.check_arg_id(index), index;
#else
    (void)id;
    on_error("compile-time checks for named arguments require C++20 support");
    return 0;
#endif
  }

  FMT_CONSTEXPR void on_replacement_field(int, const Char*) {}

  FMT_CONSTEXPR auto on_format_specs(int id, const Char* begin, const Char*)
      -> const Char* {
    context_.advance_to(context_.begin() + (begin - &*context_.begin()));
    // id >= 0 check is a workaround for gcc 10 bug (#2065).
    return id >= 0 && id < num_args ? parse_funcs_[id](context_) : begin;
  }

  FMT_CONSTEXPR void on_error(const char* message) {
    context_.on_error(message);
  }
};

template <typename... Args, typename S,
          enable_if_t<(is_compile_string<S>::value), int>>
void check_format_string(S format_str) {
  FMT_CONSTEXPR auto s = to_string_view(format_str);
  using checker = format_string_checker<typename S::char_type, error_handler,
                                        remove_cvref_t<Args>...>;
  FMT_CONSTEXPR bool invalid_format =
      (parse_format_string<true>(s, checker(s, {})), true);
  ignore_unused(invalid_format);
}

template <typename Char>
void vformat_to(
    buffer<Char>& buf, basic_string_view<Char> fmt,
    basic_format_args<FMT_BUFFER_CONTEXT(type_identity_t<Char>)> args,
    locale_ref loc = {});

FMT_API void vprint_mojibake(std::FILE*, string_view, format_args);
#ifndef _WIN32
inline void vprint_mojibake(std::FILE*, string_view, format_args) {}
#endif
FMT_END_DETAIL_NAMESPACE

// A formatter specialization for the core types corresponding to detail::type
// constants.
template <typename T, typename Char>
struct formatter<T, Char,
                 enable_if_t<detail::type_constant<T, Char>::value !=
                             detail::type::custom_type>> {
 private:
  detail::dynamic_format_specs<Char> specs_;

 public:
  // Parses format specifiers stopping either at the end of the range or at the
  // terminating '}'.
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    auto begin = ctx.begin(), end = ctx.end();
    if (begin == end) return begin;
    using handler_type = detail::dynamic_specs_handler<ParseContext>;
    auto type = detail::type_constant<T, Char>::value;
    auto checker =
        detail::specs_checker<handler_type>(handler_type(specs_, ctx), type);
    auto it = detail::parse_format_specs(begin, end, checker);
    auto eh = ctx.error_handler();
    switch (type) {
    case detail::type::none_type:
      FMT_ASSERT(false, "invalid argument type");
      break;
    case detail::type::bool_type:
      if (specs_.type == presentation_type::none ||
          specs_.type == presentation_type::string) {
        break;
      }
      FMT_FALLTHROUGH;
    case detail::type::int_type:
    case detail::type::uint_type:
    case detail::type::long_long_type:
    case detail::type::ulong_long_type:
    case detail::type::int128_type:
    case detail::type::uint128_type:
      detail::check_int_type_spec(specs_.type, eh);
      break;
    case detail::type::char_type:
      detail::check_char_specs(specs_, eh);
      break;
    case detail::type::float_type:
      if (detail::const_check(FMT_USE_FLOAT))
        detail::parse_float_type_spec(specs_, eh);
      else
        FMT_ASSERT(false, "float support disabled");
      break;
    case detail::type::double_type:
      if (detail::const_check(FMT_USE_DOUBLE))
        detail::parse_float_type_spec(specs_, eh);
      else
        FMT_ASSERT(false, "double support disabled");
      break;
    case detail::type::long_double_type:
      if (detail::const_check(FMT_USE_LONG_DOUBLE))
        detail::parse_float_type_spec(specs_, eh);
      else
        FMT_ASSERT(false, "long double support disabled");
      break;
    case detail::type::cstring_type:
      detail::check_cstring_type_spec(specs_.type, eh);
      break;
    case detail::type::string_type:
      detail::check_string_type_spec(specs_.type, eh);
      break;
    case detail::type::pointer_type:
      detail::check_pointer_type_spec(specs_.type, eh);
      break;
    case detail::type::custom_type:
      // Custom format specifiers are checked in parse functions of
      // formatter specializations.
      break;
    }
    return it;
  }

  template <typename FormatContext>
  FMT_CONSTEXPR auto format(const T& val, FormatContext& ctx) const
      -> decltype(ctx.out());
};

template <typename Char> struct basic_runtime { basic_string_view<Char> str; };

/** A compile-time format string. */
template <typename Char, typename... Args> class basic_format_string {
 private:
  basic_string_view<Char> str_;

 public:
  template <typename S,
            FMT_ENABLE_IF(
                std::is_convertible<const S&, basic_string_view<Char>>::value)>
  FMT_CONSTEVAL FMT_INLINE basic_format_string(const S& s) : str_(s) {
    static_assert(
        detail::count<
            (std::is_base_of<detail::view, remove_reference_t<Args>>::value &&
             std::is_reference<Args>::value)...>() == 0,
        "passing views as lvalues is disallowed");
#ifdef FMT_HAS_CONSTEVAL
    if constexpr (detail::count_named_args<Args...>() ==
                  detail::count_statically_named_args<Args...>()) {
      using checker = detail::format_string_checker<Char, detail::error_handler,
                                                    remove_cvref_t<Args>...>;
      detail::parse_format_string<true>(str_, checker(s, {}));
    }
#else
    detail::check_format_string<Args...>(s);
#endif
  }
  basic_format_string(basic_runtime<Char> r) : str_(r.str) {}

  FMT_INLINE operator basic_string_view<Char>() const { return str_; }
};

#if FMT_GCC_VERSION && FMT_GCC_VERSION < 409
// Workaround broken conversion on older gcc.
template <typename... Args> using format_string = string_view;
template <typename S> auto runtime(const S& s) -> basic_string_view<char_t<S>> {
  return s;
}
#else
template <typename... Args>
using format_string = basic_format_string<char, type_identity_t<Args>...>;
/**
  \rst
  Creates a runtime format string.

  **Example**::

    // Check format string at runtime instead of compile-time.
    fmt::print(fmt::runtime("{:d}"), "I am not a number");
  \endrst
 */
template <typename S> auto runtime(const S& s) -> basic_runtime<char_t<S>> {
  return {{s}};
}
#endif

FMT_API auto vformat(string_view fmt, format_args args) -> std::string;

/**
  \rst
  Formats ``args`` according to specifications in ``fmt`` and returns the result
  as a string.

  **Example**::

    #include <fmt/core.h>
    std::string message = fmt::format("The answer is {}.", 42);
  \endrst
*/
template <typename... T>
FMT_NODISCARD FMT_INLINE auto format(format_string<T...> fmt, T&&... args)
    -> std::string {
  return vformat(fmt, fmt::make_format_args(args...));
}

/** Formats a string and writes the output to ``out``. */
template <typename OutputIt,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
auto vformat_to(OutputIt out, string_view fmt, format_args args) -> OutputIt {
  using detail::get_buffer;
  auto&& buf = get_buffer<char>(out);
  detail::vformat_to(buf, fmt, args, {});
  return detail::get_iterator(buf);
}

/**
 \rst
 Formats ``args`` according to specifications in ``fmt``, writes the result to
 the output iterator ``out`` and returns the iterator past the end of the output
 range. `format_to` does not append a terminating null character.

 **Example**::

   auto out = std::vector<char>();
   fmt::format_to(std::back_inserter(out), "{}", 42);
 \endrst
 */
template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
FMT_INLINE auto format_to(OutputIt out, format_string<T...> fmt, T&&... args)
    -> OutputIt {
  return vformat_to(out, fmt, fmt::make_format_args(args...));
}

template <typename OutputIt> struct format_to_n_result {
  /** Iterator past the end of the output range. */
  OutputIt out;
  /** Total (not truncated) output size. */
  size_t size;
};

template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
auto vformat_to_n(OutputIt out, size_t n, string_view fmt, format_args args)
    -> format_to_n_result<OutputIt> {
  using traits = detail::fixed_buffer_traits;
  auto buf = detail::iterator_buffer<OutputIt, char, traits>(out, n);
  detail::vformat_to(buf, fmt, args, {});
  return {buf.out(), buf.count()};
}

/**
  \rst
  Formats ``args`` according to specifications in ``fmt``, writes up to ``n``
  characters of the result to the output iterator ``out`` and returns the total
  (not truncated) output size and the iterator past the end of the output range.
  `format_to_n` does not append a terminating null character.
  \endrst
 */
template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
FMT_INLINE auto format_to_n(OutputIt out, size_t n, format_string<T...> fmt,
                            T&&... args) -> format_to_n_result<OutputIt> {
  return vformat_to_n(out, n, fmt, fmt::make_format_args(args...));
}

/** Returns the number of chars in the output of ``format(fmt, args...)``. */
template <typename... T>
FMT_NODISCARD FMT_INLINE auto formatted_size(format_string<T...> fmt,
                                             T&&... args) -> size_t {
  auto buf = detail::counting_buffer<>();
  detail::vformat_to(buf, string_view(fmt), fmt::make_format_args(args...), {});
  return buf.count();
}

FMT_API void vprint(string_view fmt, format_args args);
FMT_API void vprint(std::FILE* f, string_view fmt, format_args args);

/**
  \rst
  Formats ``args`` according to specifications in ``fmt`` and writes the output
  to ``stdout``.

  **Example**::

    fmt::print("Elapsed time: {0:.2f} seconds", 1.23);
  \endrst
 */
template <typename... T>
FMT_INLINE void print(format_string<T...> fmt, T&&... args) {
  const auto& vargs = fmt::make_format_args(args...);
  return detail::is_utf8() ? vprint(fmt, vargs)
                           : detail::vprint_mojibake(stdout, fmt, vargs);
}

/**
  \rst
  Formats ``args`` according to specifications in ``fmt`` and writes the
  output to the file ``f``.

  **Example**::

    fmt::print(stderr, "Don't {}!", "panic");
  \endrst
 */
template <typename... T>
FMT_INLINE void print(std::FILE* f, format_string<T...> fmt, T&&... args) {
  const auto& vargs = fmt::make_format_args(args...);
  return detail::is_utf8() ? vprint(f, fmt, vargs)
                           : detail::vprint_mojibake(f, fmt, vargs);
}

FMT_MODULE_EXPORT_END
FMT_GCC_PRAGMA("GCC pop_options")
FMT_END_NAMESPACE

#ifdef FMT_HEADER_ONLY
// #  include "format.h"
/*
 Formatting library for C++

 Copyright (c) 2012 - present, Victor Zverovich

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 --- Optional exception to the license ---

 As an exception, if, as a result of your compiling your source code, portions
 of this Software are embedded into a machine-executable object form of such
 source code, you may redistribute such embedded portions in such object form
 without including the above copyright and permission notices.
 */

#ifndef FMT_FORMAT_H_
#define FMT_FORMAT_H_

#include <cmath>         // std::signbit
#include <cstdint>       // uint32_t
#include <limits>        // std::numeric_limits
#include <memory>        // std::uninitialized_copy
#include <stdexcept>     // std::runtime_error
#include <system_error>  // std::system_error
#include <utility>       // std::swap

#ifdef __cpp_lib_bit_cast
#  include <bit>  // std::bitcast
#endif

// #include "core.h"


#if FMT_GCC_VERSION
#  define FMT_GCC_VISIBILITY_HIDDEN __attribute__((visibility("hidden")))
#else
#  define FMT_GCC_VISIBILITY_HIDDEN
#endif

#ifdef __NVCC__
#  define FMT_CUDA_VERSION (__CUDACC_VER_MAJOR__ * 100 + __CUDACC_VER_MINOR__)
#else
#  define FMT_CUDA_VERSION 0
#endif

#ifdef __has_builtin
#  define FMT_HAS_BUILTIN(x) __has_builtin(x)
#else
#  define FMT_HAS_BUILTIN(x) 0
#endif

#if FMT_GCC_VERSION || FMT_CLANG_VERSION
#  define FMT_NOINLINE __attribute__((noinline))
#else
#  define FMT_NOINLINE
#endif

#if FMT_MSC_VER
#  define FMT_MSC_DEFAULT = default
#else
#  define FMT_MSC_DEFAULT
#endif

#ifndef FMT_THROW
#  if FMT_EXCEPTIONS
#    if FMT_MSC_VER || FMT_NVCC
FMT_BEGIN_NAMESPACE
namespace detail {
template <typename Exception> inline void do_throw(const Exception& x) {
  // Silence unreachable code warnings in MSVC and NVCC because these
  // are nearly impossible to fix in a generic code.
  volatile bool b = true;
  if (b) throw x;
}
}  // namespace detail
FMT_END_NAMESPACE
#      define FMT_THROW(x) detail::do_throw(x)
#    else
#      define FMT_THROW(x) throw x
#    endif
#  else
#    define FMT_THROW(x)               \
      do {                             \
        FMT_ASSERT(false, (x).what()); \
      } while (false)
#  endif
#endif

#if FMT_EXCEPTIONS
#  define FMT_TRY try
#  define FMT_CATCH(x) catch (x)
#else
#  define FMT_TRY if (true)
#  define FMT_CATCH(x) if (false)
#endif

#ifndef FMT_MAYBE_UNUSED
#  if FMT_HAS_CPP17_ATTRIBUTE(maybe_unused)
#    define FMT_MAYBE_UNUSED [[maybe_unused]]
#  else
#    define FMT_MAYBE_UNUSED
#  endif
#endif

// Workaround broken [[deprecated]] in the Intel, PGI and NVCC compilers.
#if FMT_ICC_VERSION || defined(__PGI) || FMT_NVCC
#  define FMT_DEPRECATED_ALIAS
#else
#  define FMT_DEPRECATED_ALIAS FMT_DEPRECATED
#endif

#ifndef FMT_USE_USER_DEFINED_LITERALS
// EDG based compilers (Intel, NVIDIA, Elbrus, etc), GCC and MSVC support UDLs.
#  if (FMT_HAS_FEATURE(cxx_user_literals) || FMT_GCC_VERSION >= 407 || \
       FMT_MSC_VER >= 1900) &&                                         \
      (!defined(__EDG_VERSION__) || __EDG_VERSION__ >= /* UDL feature */ 480)
#    define FMT_USE_USER_DEFINED_LITERALS 1
#  else
#    define FMT_USE_USER_DEFINED_LITERALS 0
#  endif
#endif

// Defining FMT_REDUCE_INT_INSTANTIATIONS to 1, will reduce the number of
// integer formatter template instantiations to just one by only using the
// largest integer type. This results in a reduction in binary size but will
// cause a decrease in integer formatting performance.
#if !defined(FMT_REDUCE_INT_INSTANTIATIONS)
#  define FMT_REDUCE_INT_INSTANTIATIONS 0
#endif

// __builtin_clz is broken in clang with Microsoft CodeGen:
// https://github.com/fmtlib/fmt/issues/519.
#if !FMT_MSC_VER
#  if FMT_HAS_BUILTIN(__builtin_clz) || FMT_GCC_VERSION || FMT_ICC_VERSION
#    define FMT_BUILTIN_CLZ(n) __builtin_clz(n)
#  endif
#  if FMT_HAS_BUILTIN(__builtin_clzll) || FMT_GCC_VERSION || FMT_ICC_VERSION
#    define FMT_BUILTIN_CLZLL(n) __builtin_clzll(n)
#  endif
#endif

// __builtin_ctz is broken in Intel Compiler Classic on Windows:
// https://github.com/fmtlib/fmt/issues/2510.
#ifndef __ICL
#  if FMT_HAS_BUILTIN(__builtin_ctz) || FMT_GCC_VERSION || FMT_ICC_VERSION
#    define FMT_BUILTIN_CTZ(n) __builtin_ctz(n)
#  endif
#  if FMT_HAS_BUILTIN(__builtin_ctzll) || FMT_GCC_VERSION || FMT_ICC_VERSION
#    define FMT_BUILTIN_CTZLL(n) __builtin_ctzll(n)
#  endif
#endif

#if FMT_MSC_VER
#  include <intrin.h>  // _BitScanReverse[64], _BitScanForward[64], _umul128
#endif

// Some compilers masquerade as both MSVC and GCC-likes or otherwise support
// __builtin_clz and __builtin_clzll, so only define FMT_BUILTIN_CLZ using the
// MSVC intrinsics if the clz and clzll builtins are not available.
#if FMT_MSC_VER && !defined(FMT_BUILTIN_CLZLL) && !defined(FMT_BUILTIN_CTZLL)
FMT_BEGIN_NAMESPACE
namespace detail {
// Avoid Clang with Microsoft CodeGen's -Wunknown-pragmas warning.
#  if !defined(__clang__)
#    pragma intrinsic(_BitScanForward)
#    pragma intrinsic(_BitScanReverse)
#    if defined(_WIN64)
#      pragma intrinsic(_BitScanForward64)
#      pragma intrinsic(_BitScanReverse64)
#    endif
#  endif

inline auto clz(uint32_t x) -> int {
  unsigned long r = 0;
  _BitScanReverse(&r, x);
  FMT_ASSERT(x != 0, "");
  // Static analysis complains about using uninitialized data
  // "r", but the only way that can happen is if "x" is 0,
  // which the callers guarantee to not happen.
  FMT_MSC_WARNING(suppress : 6102)
  return 31 ^ static_cast<int>(r);
}
#  define FMT_BUILTIN_CLZ(n) detail::clz(n)

inline auto clzll(uint64_t x) -> int {
  unsigned long r = 0;
#  ifdef _WIN64
  _BitScanReverse64(&r, x);
#  else
  // Scan the high 32 bits.
  if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32))) return 63 ^ (r + 32);
  // Scan the low 32 bits.
  _BitScanReverse(&r, static_cast<uint32_t>(x));
#  endif
  FMT_ASSERT(x != 0, "");
  FMT_MSC_WARNING(suppress : 6102)  // Suppress a bogus static analysis warning.
  return 63 ^ static_cast<int>(r);
}
#  define FMT_BUILTIN_CLZLL(n) detail::clzll(n)

inline auto ctz(uint32_t x) -> int {
  unsigned long r = 0;
  _BitScanForward(&r, x);
  FMT_ASSERT(x != 0, "");
  FMT_MSC_WARNING(suppress : 6102)  // Suppress a bogus static analysis warning.
  return static_cast<int>(r);
}
#  define FMT_BUILTIN_CTZ(n) detail::ctz(n)

inline auto ctzll(uint64_t x) -> int {
  unsigned long r = 0;
  FMT_ASSERT(x != 0, "");
  FMT_MSC_WARNING(suppress : 6102)  // Suppress a bogus static analysis warning.
#  ifdef _WIN64
  _BitScanForward64(&r, x);
#  else
  // Scan the low 32 bits.
  if (_BitScanForward(&r, static_cast<uint32_t>(x))) return static_cast<int>(r);
  // Scan the high 32 bits.
  _BitScanForward(&r, static_cast<uint32_t>(x >> 32));
  r += 32;
#  endif
  return static_cast<int>(r);
}
#  define FMT_BUILTIN_CTZLL(n) detail::ctzll(n)
}  // namespace detail
FMT_END_NAMESPACE
#endif

#ifdef FMT_HEADER_ONLY
#  define FMT_HEADER_ONLY_CONSTEXPR20 FMT_CONSTEXPR20
#else
#  define FMT_HEADER_ONLY_CONSTEXPR20
#endif

FMT_BEGIN_NAMESPACE
namespace detail {

template <typename Streambuf> class formatbuf : public Streambuf {
 private:
  using char_type = typename Streambuf::char_type;
  using streamsize = decltype(std::declval<Streambuf>().sputn(nullptr, 0));
  using int_type = typename Streambuf::int_type;
  using traits_type = typename Streambuf::traits_type;

  buffer<char_type>& buffer_;

 public:
  explicit formatbuf(buffer<char_type>& buf) : buffer_(buf) {}

 protected:
  // The put area is always empty. This makes the implementation simpler and has
  // the advantage that the streambuf and the buffer are always in sync and
  // sputc never writes into uninitialized memory. A disadvantage is that each
  // call to sputc always results in a (virtual) call to overflow. There is no
  // disadvantage here for sputn since this always results in a call to xsputn.

  auto overflow(int_type ch) -> int_type override {
    if (!traits_type::eq_int_type(ch, traits_type::eof()))
      buffer_.push_back(static_cast<char_type>(ch));
    return ch;
  }

  auto xsputn(const char_type* s, streamsize count) -> streamsize override {
    buffer_.append(s, s + count);
    return count;
  }
};

// Implementation of std::bit_cast for pre-C++20.
template <typename To, typename From>
FMT_CONSTEXPR20 auto bit_cast(const From& from) -> To {
  static_assert(sizeof(To) == sizeof(From), "size mismatch");
#ifdef __cpp_lib_bit_cast
  if (is_constant_evaluated()) return std::bit_cast<To>(from);
#endif
  auto to = To();
  std::memcpy(&to, &from, sizeof(to));
  return to;
}

inline auto is_big_endian() -> bool {
#ifdef _WIN32
  return false;
#elif defined(__BIG_ENDIAN__)
  return true;
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__)
  return __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__;
#else
  struct bytes {
    char data[sizeof(int)];
  };
  return bit_cast<bytes>(1).data[0] == 0;
#endif
}

// A fallback implementation of uintptr_t for systems that lack it.
struct fallback_uintptr {
  unsigned char value[sizeof(void*)];

  fallback_uintptr() = default;
  explicit fallback_uintptr(const void* p) {
    *this = bit_cast<fallback_uintptr>(p);
    if (const_check(is_big_endian())) {
      for (size_t i = 0, j = sizeof(void*) - 1; i < j; ++i, --j)
        std::swap(value[i], value[j]);
    }
  }
};
#ifdef UINTPTR_MAX
using uintptr_t = ::uintptr_t;
inline auto to_uintptr(const void* p) -> uintptr_t {
  return bit_cast<uintptr_t>(p);
}
#else
using uintptr_t = fallback_uintptr;
inline auto to_uintptr(const void* p) -> fallback_uintptr {
  return fallback_uintptr(p);
}
#endif

// Returns the largest possible value for type T. Same as
// std::numeric_limits<T>::max() but shorter and not affected by the max macro.
template <typename T> constexpr auto max_value() -> T {
  return (std::numeric_limits<T>::max)();
}
template <typename T> constexpr auto num_bits() -> int {
  return std::numeric_limits<T>::digits;
}
// std::numeric_limits<T>::digits may return 0 for 128-bit ints.
template <> constexpr auto num_bits<int128_t>() -> int { return 128; }
template <> constexpr auto num_bits<uint128_t>() -> int { return 128; }
template <> constexpr auto num_bits<fallback_uintptr>() -> int {
  return static_cast<int>(sizeof(void*) *
                          std::numeric_limits<unsigned char>::digits);
}

FMT_INLINE void assume(bool condition) {
  (void)condition;
#if FMT_HAS_BUILTIN(__builtin_assume)
  __builtin_assume(condition);
#endif
}

// An approximation of iterator_t for pre-C++20 systems.
template <typename T>
using iterator_t = decltype(std::begin(std::declval<T&>()));
template <typename T> using sentinel_t = decltype(std::end(std::declval<T&>()));

// A workaround for std::string not having mutable data() until C++17.
template <typename Char>
inline auto get_data(std::basic_string<Char>& s) -> Char* {
  return &s[0];
}
template <typename Container>
inline auto get_data(Container& c) -> typename Container::value_type* {
  return c.data();
}

#if defined(_SECURE_SCL) && _SECURE_SCL
// Make a checked iterator to avoid MSVC warnings.
template <typename T> using checked_ptr = stdext::checked_array_iterator<T*>;
template <typename T>
constexpr auto make_checked(T* p, size_t size) -> checked_ptr<T> {
  return {p, size};
}
#else
template <typename T> using checked_ptr = T*;
template <typename T> constexpr auto make_checked(T* p, size_t) -> T* {
  return p;
}
#endif

// Attempts to reserve space for n extra characters in the output range.
// Returns a pointer to the reserved range or a reference to it.
template <typename Container, FMT_ENABLE_IF(is_contiguous<Container>::value)>
#if FMT_CLANG_VERSION >= 307 && !FMT_ICC_VERSION
__attribute__((no_sanitize("undefined")))
#endif
inline auto
reserve(std::back_insert_iterator<Container> it, size_t n)
    -> checked_ptr<typename Container::value_type> {
  Container& c = get_container(it);
  size_t size = c.size();
  c.resize(size + n);
  return make_checked(get_data(c) + size, n);
}

template <typename T>
inline auto reserve(buffer_appender<T> it, size_t n) -> buffer_appender<T> {
  buffer<T>& buf = get_container(it);
  buf.try_reserve(buf.size() + n);
  return it;
}

template <typename Iterator>
constexpr auto reserve(Iterator& it, size_t) -> Iterator& {
  return it;
}

template <typename OutputIt>
using reserve_iterator =
    remove_reference_t<decltype(reserve(std::declval<OutputIt&>(), 0))>;

template <typename T, typename OutputIt>
constexpr auto to_pointer(OutputIt, size_t) -> T* {
  return nullptr;
}
template <typename T> auto to_pointer(buffer_appender<T> it, size_t n) -> T* {
  buffer<T>& buf = get_container(it);
  auto size = buf.size();
  if (buf.capacity() < size + n) return nullptr;
  buf.try_resize(size + n);
  return buf.data() + size;
}

template <typename Container, FMT_ENABLE_IF(is_contiguous<Container>::value)>
inline auto base_iterator(std::back_insert_iterator<Container>& it,
                          checked_ptr<typename Container::value_type>)
    -> std::back_insert_iterator<Container> {
  return it;
}

template <typename Iterator>
constexpr auto base_iterator(Iterator, Iterator it) -> Iterator {
  return it;
}

// <algorithm> is spectacularly slow to compile in C++20 so use a simple fill_n
// instead (#1998).
template <typename OutputIt, typename Size, typename T>
FMT_CONSTEXPR auto fill_n(OutputIt out, Size count, const T& value)
    -> OutputIt {
  for (Size i = 0; i < count; ++i) *out++ = value;
  return out;
}
template <typename T, typename Size>
FMT_CONSTEXPR20 auto fill_n(T* out, Size count, char value) -> T* {
  if (is_constant_evaluated()) {
    return fill_n<T*, Size, T>(out, count, value);
  }
  std::memset(out, value, to_unsigned(count));
  return out + count;
}

#ifdef __cpp_char8_t
using char8_type = char8_t;
#else
enum char8_type : unsigned char {};
#endif

template <typename OutChar, typename InputIt, typename OutputIt>
FMT_CONSTEXPR FMT_NOINLINE auto copy_str_noinline(InputIt begin, InputIt end,
                                                  OutputIt out) -> OutputIt {
  return copy_str<OutChar>(begin, end, out);
}

// A public domain branchless UTF-8 decoder by Christopher Wellons:
// https://github.com/skeeto/branchless-utf8
/* Decode the next character, c, from s, reporting errors in e.
 *
 * Since this is a branchless decoder, four bytes will be read from the
 * buffer regardless of the actual length of the next character. This
 * means the buffer _must_ have at least three bytes of zero padding
 * following the end of the data stream.
 *
 * Errors are reported in e, which will be non-zero if the parsed
 * character was somehow invalid: invalid byte sequence, non-canonical
 * encoding, or a surrogate half.
 *
 * The function returns a pointer to the next character. When an error
 * occurs, this pointer will be a guess that depends on the particular
 * error, but it will always advance at least one byte.
 */
FMT_CONSTEXPR inline auto utf8_decode(const char* s, uint32_t* c, int* e)
    -> const char* {
  constexpr const int masks[] = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
  constexpr const uint32_t mins[] = {4194304, 0, 128, 2048, 65536};
  constexpr const int shiftc[] = {0, 18, 12, 6, 0};
  constexpr const int shifte[] = {0, 6, 4, 2, 0};

  int len = code_point_length(s);
  const char* next = s + len;

  // Assume a four-byte character and load four bytes. Unused bits are
  // shifted out.
  *c = uint32_t(s[0] & masks[len]) << 18;
  *c |= uint32_t(s[1] & 0x3f) << 12;
  *c |= uint32_t(s[2] & 0x3f) << 6;
  *c |= uint32_t(s[3] & 0x3f) << 0;
  *c >>= shiftc[len];

  // Accumulate the various error conditions.
  using uchar = unsigned char;
  *e = (*c < mins[len]) << 6;       // non-canonical encoding
  *e |= ((*c >> 11) == 0x1b) << 7;  // surrogate half?
  *e |= (*c > 0x10FFFF) << 8;       // out of range?
  *e |= (uchar(s[1]) & 0xc0) >> 2;
  *e |= (uchar(s[2]) & 0xc0) >> 4;
  *e |= uchar(s[3]) >> 6;
  *e ^= 0x2a;  // top two bits of each tail byte correct?
  *e >>= shifte[len];

  return next;
}

constexpr uint32_t invalid_code_point = ~uint32_t();

// Invokes f(cp, sv) for every code point cp in s with sv being the string view
// corresponding to the code point. cp is invalid_code_point on error.
template <typename F>
FMT_CONSTEXPR void for_each_codepoint(string_view s, F f) {
  auto decode = [f](const char* buf_ptr, const char* ptr) {
    auto cp = uint32_t();
    auto error = 0;
    auto end = utf8_decode(buf_ptr, &cp, &error);
    bool result = f(error ? invalid_code_point : cp,
                    string_view(ptr, to_unsigned(end - buf_ptr)));
    return result ? end : nullptr;
  };
  auto p = s.data();
  const size_t block_size = 4;  // utf8_decode always reads blocks of 4 chars.
  if (s.size() >= block_size) {
    for (auto end = p + s.size() - block_size + 1; p < end;) {
      p = decode(p, p);
      if (!p) return;
    }
  }
  if (auto num_chars_left = s.data() + s.size() - p) {
    char buf[2 * block_size - 1] = {};
    copy_str<char>(p, p + num_chars_left, buf);
    const char* buf_ptr = buf;
    do {
      auto end = decode(buf_ptr, p);
      if (!end) return;
      p += end - buf_ptr;
      buf_ptr = end;
    } while (buf_ptr - buf < num_chars_left);
  }
}

template <typename Char>
inline auto compute_width(basic_string_view<Char> s) -> size_t {
  return s.size();
}

// Computes approximate display width of a UTF-8 string.
FMT_CONSTEXPR inline size_t compute_width(string_view s) {
  size_t num_code_points = 0;
  // It is not a lambda for compatibility with C++14.
  struct count_code_points {
    size_t* count;
    FMT_CONSTEXPR auto operator()(uint32_t cp, string_view) const -> bool {
      *count += detail::to_unsigned(
          1 +
          (cp >= 0x1100 &&
           (cp <= 0x115f ||  // Hangul Jamo init. consonants
            cp == 0x2329 ||  // LEFT-POINTING ANGLE BRACKET
            cp == 0x232a ||  // RIGHT-POINTING ANGLE BRACKET
            // CJK ... Yi except IDEOGRAPHIC HALF FILL SPACE:
            (cp >= 0x2e80 && cp <= 0xa4cf && cp != 0x303f) ||
            (cp >= 0xac00 && cp <= 0xd7a3) ||    // Hangul Syllables
            (cp >= 0xf900 && cp <= 0xfaff) ||    // CJK Compatibility Ideographs
            (cp >= 0xfe10 && cp <= 0xfe19) ||    // Vertical Forms
            (cp >= 0xfe30 && cp <= 0xfe6f) ||    // CJK Compatibility Forms
            (cp >= 0xff00 && cp <= 0xff60) ||    // Fullwidth Forms
            (cp >= 0xffe0 && cp <= 0xffe6) ||    // Fullwidth Forms
            (cp >= 0x20000 && cp <= 0x2fffd) ||  // CJK
            (cp >= 0x30000 && cp <= 0x3fffd) ||
            // Miscellaneous Symbols and Pictographs + Emoticons:
            (cp >= 0x1f300 && cp <= 0x1f64f) ||
            // Supplemental Symbols and Pictographs:
            (cp >= 0x1f900 && cp <= 0x1f9ff))));
      return true;
    }
  };
  for_each_codepoint(s, count_code_points{&num_code_points});
  return num_code_points;
}

inline auto compute_width(basic_string_view<char8_type> s) -> size_t {
  return compute_width(basic_string_view<char>(
      reinterpret_cast<const char*>(s.data()), s.size()));
}

template <typename Char>
inline auto code_point_index(basic_string_view<Char> s, size_t n) -> size_t {
  size_t size = s.size();
  return n < size ? n : size;
}

// Calculates the index of the nth code point in a UTF-8 string.
inline auto code_point_index(basic_string_view<char8_type> s, size_t n)
    -> size_t {
  const char8_type* data = s.data();
  size_t num_code_points = 0;
  for (size_t i = 0, size = s.size(); i != size; ++i) {
    if ((data[i] & 0xc0) != 0x80 && ++num_code_points > n) return i;
  }
  return s.size();
}

template <typename T, bool = std::is_floating_point<T>::value>
struct is_fast_float : bool_constant<std::numeric_limits<T>::is_iec559 &&
                                     sizeof(T) <= sizeof(double)> {};
template <typename T> struct is_fast_float<T, false> : std::false_type {};

#ifndef FMT_USE_FULL_CACHE_DRAGONBOX
#  define FMT_USE_FULL_CACHE_DRAGONBOX 0
#endif

template <typename T>
template <typename U>
void buffer<T>::append(const U* begin, const U* end) {
  while (begin != end) {
    auto count = to_unsigned(end - begin);
    try_reserve(size_ + count);
    auto free_cap = capacity_ - size_;
    if (free_cap < count) count = free_cap;
    std::uninitialized_copy_n(begin, count, make_checked(ptr_ + size_, count));
    size_ += count;
    begin += count;
  }
}

template <typename T, typename Enable = void>
struct is_locale : std::false_type {};
template <typename T>
struct is_locale<T, void_t<decltype(T::classic())>> : std::true_type {};
}  // namespace detail

FMT_MODULE_EXPORT_BEGIN

// The number of characters to store in the basic_memory_buffer object itself
// to avoid dynamic memory allocation.
enum { inline_buffer_size = 500 };

/**
  \rst
  A dynamically growing memory buffer for trivially copyable/constructible types
  with the first ``SIZE`` elements stored in the object itself.

  You can use the ``memory_buffer`` type alias for ``char`` instead.

  **Example**::

     auto out = fmt::memory_buffer();
     format_to(std::back_inserter(out), "The answer is {}.", 42);

  This will append the following output to the ``out`` object:

  .. code-block:: none

     The answer is 42.

  The output can be converted to an ``std::string`` with ``to_string(out)``.
  \endrst
 */
template <typename T, size_t SIZE = inline_buffer_size,
          typename Allocator = std::allocator<T>>
class basic_memory_buffer final : public detail::buffer<T> {
 private:
  T store_[SIZE];

  // Don't inherit from Allocator avoid generating type_info for it.
  Allocator alloc_;

  // Deallocate memory allocated by the buffer.
  FMT_CONSTEXPR20 void deallocate() {
    T* data = this->data();
    if (data != store_) alloc_.deallocate(data, this->capacity());
  }

 protected:
  FMT_CONSTEXPR20 void grow(size_t size) override;

 public:
  using value_type = T;
  using const_reference = const T&;

  FMT_CONSTEXPR20 explicit basic_memory_buffer(
      const Allocator& alloc = Allocator())
      : alloc_(alloc) {
    this->set(store_, SIZE);
    if (detail::is_constant_evaluated()) {
      detail::fill_n(store_, SIZE, T{});
    }
  }
  FMT_CONSTEXPR20 ~basic_memory_buffer() { deallocate(); }

 private:
  // Move data from other to this buffer.
  FMT_CONSTEXPR20 void move(basic_memory_buffer& other) {
    alloc_ = std::move(other.alloc_);
    T* data = other.data();
    size_t size = other.size(), capacity = other.capacity();
    if (data == other.store_) {
      this->set(store_, capacity);
      if (detail::is_constant_evaluated()) {
        detail::copy_str<T>(other.store_, other.store_ + size,
                            detail::make_checked(store_, capacity));
      } else {
        std::uninitialized_copy(other.store_, other.store_ + size,
                                detail::make_checked(store_, capacity));
      }
    } else {
      this->set(data, capacity);
      // Set pointer to the inline array so that delete is not called
      // when deallocating.
      other.set(other.store_, 0);
    }
    this->resize(size);
  }

 public:
  /**
    \rst
    Constructs a :class:`fmt::basic_memory_buffer` object moving the content
    of the other object to it.
    \endrst
   */
  FMT_CONSTEXPR20 basic_memory_buffer(basic_memory_buffer&& other)
      FMT_NOEXCEPT {
    move(other);
  }

  /**
    \rst
    Moves the content of the other ``basic_memory_buffer`` object to this one.
    \endrst
   */
  auto operator=(basic_memory_buffer&& other) FMT_NOEXCEPT
      -> basic_memory_buffer& {
    FMT_ASSERT(this != &other, "");
    deallocate();
    move(other);
    return *this;
  }

  // Returns a copy of the allocator associated with this buffer.
  auto get_allocator() const -> Allocator { return alloc_; }

  /**
    Resizes the buffer to contain *count* elements. If T is a POD type new
    elements may not be initialized.
   */
  FMT_CONSTEXPR20 void resize(size_t count) { this->try_resize(count); }

  /** Increases the buffer capacity to *new_capacity*. */
  void reserve(size_t new_capacity) { this->try_reserve(new_capacity); }

  // Directly append data into the buffer
  using detail::buffer<T>::append;
  template <typename ContiguousRange>
  void append(const ContiguousRange& range) {
    append(range.data(), range.data() + range.size());
  }
};

template <typename T, size_t SIZE, typename Allocator>
FMT_CONSTEXPR20 void basic_memory_buffer<T, SIZE, Allocator>::grow(
    size_t size) {
#ifdef FMT_FUZZ
  if (size > 5000) throw std::runtime_error("fuzz mode - won't grow that much");
#endif
  const size_t max_size = std::allocator_traits<Allocator>::max_size(alloc_);
  size_t old_capacity = this->capacity();
  size_t new_capacity = old_capacity + old_capacity / 2;
  if (size > new_capacity)
    new_capacity = size;
  else if (new_capacity > max_size)
    new_capacity = size > max_size ? size : max_size;
  T* old_data = this->data();
  T* new_data =
      std::allocator_traits<Allocator>::allocate(alloc_, new_capacity);
  // The following code doesn't throw, so the raw pointer above doesn't leak.
  std::uninitialized_copy(old_data, old_data + this->size(),
                          detail::make_checked(new_data, new_capacity));
  this->set(new_data, new_capacity);
  // deallocate must not throw according to the standard, but even if it does,
  // the buffer already uses the new storage and will deallocate it in
  // destructor.
  if (old_data != store_) alloc_.deallocate(old_data, old_capacity);
}

using memory_buffer = basic_memory_buffer<char>;

template <typename T, size_t SIZE, typename Allocator>
struct is_contiguous<basic_memory_buffer<T, SIZE, Allocator>> : std::true_type {
};

namespace detail {
FMT_API void print(std::FILE*, string_view);
}

/** A formatting error such as invalid format string. */
FMT_CLASS_API
class FMT_API format_error : public std::runtime_error {
 public:
  explicit format_error(const char* message) : std::runtime_error(message) {}
  explicit format_error(const std::string& message)
      : std::runtime_error(message) {}
  format_error(const format_error&) = default;
  format_error& operator=(const format_error&) = default;
  format_error(format_error&&) = default;
  format_error& operator=(format_error&&) = default;
  ~format_error() FMT_NOEXCEPT override FMT_MSC_DEFAULT;
};

/**
  \rst
  Constructs a `~fmt::format_arg_store` object that contains references
  to arguments and can be implicitly converted to `~fmt::format_args`.
  If ``fmt`` is a compile-time string then `make_args_checked` checks
  its validity at compile time.
  \endrst
 */
template <typename... Args, typename S, typename Char = char_t<S>>
FMT_INLINE auto make_args_checked(const S& fmt,
                                  const remove_reference_t<Args>&... args)
    -> format_arg_store<buffer_context<Char>, remove_reference_t<Args>...> {
  static_assert(
      detail::count<(
              std::is_base_of<detail::view, remove_reference_t<Args>>::value &&
              std::is_reference<Args>::value)...>() == 0,
      "passing views as lvalues is disallowed");
  detail::check_format_string<Args...>(fmt);
  return {args...};
}

// compile-time support
namespace detail_exported {
#if FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
template <typename Char, size_t N> struct fixed_string {
  constexpr fixed_string(const Char (&str)[N]) {
    detail::copy_str<Char, const Char*, Char*>(static_cast<const Char*>(str),
                                               str + N, data);
  }
  Char data[N]{};
};
#endif

// Converts a compile-time string to basic_string_view.
template <typename Char, size_t N>
constexpr auto compile_string_to_view(const Char (&s)[N])
    -> basic_string_view<Char> {
  // Remove trailing NUL character if needed. Won't be present if this is used
  // with a raw character array (i.e. not defined as a string).
  return {s, N - (std::char_traits<Char>::to_int_type(s[N - 1]) == 0 ? 1 : 0)};
}
template <typename Char>
constexpr auto compile_string_to_view(detail::std_string_view<Char> s)
    -> basic_string_view<Char> {
  return {s.data(), s.size()};
}
}  // namespace detail_exported

FMT_BEGIN_DETAIL_NAMESPACE

template <typename T> struct is_integral : std::is_integral<T> {};
template <> struct is_integral<int128_t> : std::true_type {};
template <> struct is_integral<uint128_t> : std::true_type {};

template <typename T>
using is_signed =
    std::integral_constant<bool, std::numeric_limits<T>::is_signed ||
                                     std::is_same<T, int128_t>::value>;

// Returns true if value is negative, false otherwise.
// Same as `value < 0` but doesn't produce warnings if T is an unsigned type.
template <typename T, FMT_ENABLE_IF(is_signed<T>::value)>
FMT_CONSTEXPR auto is_negative(T value) -> bool {
  return value < 0;
}
template <typename T, FMT_ENABLE_IF(!is_signed<T>::value)>
FMT_CONSTEXPR auto is_negative(T) -> bool {
  return false;
}

template <typename T, FMT_ENABLE_IF(std::is_floating_point<T>::value)>
FMT_CONSTEXPR auto is_supported_floating_point(T) -> uint16_t {
  return (std::is_same<T, float>::value && FMT_USE_FLOAT) ||
         (std::is_same<T, double>::value && FMT_USE_DOUBLE) ||
         (std::is_same<T, long double>::value && FMT_USE_LONG_DOUBLE);
}

// Smallest of uint32_t, uint64_t, uint128_t that is large enough to
// represent all values of an integral type T.
template <typename T>
using uint32_or_64_or_128_t =
    conditional_t<num_bits<T>() <= 32 && !FMT_REDUCE_INT_INSTANTIATIONS,
                  uint32_t,
                  conditional_t<num_bits<T>() <= 64, uint64_t, uint128_t>>;
template <typename T>
using uint64_or_128_t = conditional_t<num_bits<T>() <= 64, uint64_t, uint128_t>;

#define FMT_POWERS_OF_10(factor)                                             \
  factor * 10, (factor)*100, (factor)*1000, (factor)*10000, (factor)*100000, \
      (factor)*1000000, (factor)*10000000, (factor)*100000000,               \
      (factor)*1000000000

// Converts value in the range [0, 100) to a string.
constexpr const char* digits2(size_t value) {
  // GCC generates slightly better code when value is pointer-size.
  return &"0001020304050607080910111213141516171819"
         "2021222324252627282930313233343536373839"
         "4041424344454647484950515253545556575859"
         "6061626364656667686970717273747576777879"
         "8081828384858687888990919293949596979899"[value * 2];
}

// Sign is a template parameter to workaround a bug in gcc 4.8.
template <typename Char, typename Sign> constexpr Char sign(Sign s) {
#if !FMT_GCC_VERSION || FMT_GCC_VERSION >= 604
  static_assert(std::is_same<Sign, sign_t>::value, "");
#endif
  return static_cast<Char>("\0-+ "[s]);
}

template <typename T> FMT_CONSTEXPR auto count_digits_fallback(T n) -> int {
  int count = 1;
  for (;;) {
    // Integer division is slow so do it for a group of four digits instead
    // of for every digit. The idea comes from the talk by Alexandrescu
    // "Three Optimization Tips for C++". See speed-test for a comparison.
    if (n < 10) return count;
    if (n < 100) return count + 1;
    if (n < 1000) return count + 2;
    if (n < 10000) return count + 3;
    n /= 10000u;
    count += 4;
  }
}
#if FMT_USE_INT128
FMT_CONSTEXPR inline auto count_digits(uint128_t n) -> int {
  return count_digits_fallback(n);
}
#endif

#ifdef FMT_BUILTIN_CLZLL
// It is a separate function rather than a part of count_digits to workaround
// the lack of static constexpr in constexpr functions.
inline auto do_count_digits(uint64_t n) -> int {
  // This has comparable performance to the version by Kendall Willets
  // (https://github.com/fmtlib/format-benchmark/blob/master/digits10)
  // but uses smaller tables.
  // Maps bsr(n) to ceil(log10(pow(2, bsr(n) + 1) - 1)).
  static constexpr uint8_t bsr2log10[] = {
      1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
      6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10,
      10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
      15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20};
  auto t = bsr2log10[FMT_BUILTIN_CLZLL(n | 1) ^ 63];
  static constexpr const uint64_t zero_or_powers_of_10[] = {
      0, 0, FMT_POWERS_OF_10(1U), FMT_POWERS_OF_10(1000000000ULL),
      10000000000000000000ULL};
  return t - (n < zero_or_powers_of_10[t]);
}
#endif

// Returns the number of decimal digits in n. Leading zeros are not counted
// except for n == 0 in which case count_digits returns 1.
FMT_CONSTEXPR20 inline auto count_digits(uint64_t n) -> int {
#ifdef FMT_BUILTIN_CLZLL
  if (!is_constant_evaluated()) {
    return do_count_digits(n);
  }
#endif
  return count_digits_fallback(n);
}

// Counts the number of digits in n. BITS = log2(radix).
template <int BITS, typename UInt>
FMT_CONSTEXPR auto count_digits(UInt n) -> int {
#ifdef FMT_BUILTIN_CLZ
  if (num_bits<UInt>() == 32)
    return (FMT_BUILTIN_CLZ(static_cast<uint32_t>(n) | 1) ^ 31) / BITS + 1;
#endif
  // Lambda avoids unreachable code warnings from NVHPC.
  return [](UInt m) {
    int num_digits = 0;
    do {
      ++num_digits;
    } while ((m >>= BITS) != 0);
    return num_digits;
  }(n);
}

template <> auto count_digits<4>(detail::fallback_uintptr n) -> int;

#ifdef FMT_BUILTIN_CLZ
// It is a separate function rather than a part of count_digits to workaround
// the lack of static constexpr in constexpr functions.
FMT_INLINE auto do_count_digits(uint32_t n) -> int {
// An optimization by Kendall Willets from https://bit.ly/3uOIQrB.
// This increments the upper 32 bits (log10(T) - 1) when >= T is added.
#  define FMT_INC(T) (((sizeof(#  T) - 1ull) << 32) - T)
  static constexpr uint64_t table[] = {
      FMT_INC(0),          FMT_INC(0),          FMT_INC(0),           // 8
      FMT_INC(10),         FMT_INC(10),         FMT_INC(10),          // 64
      FMT_INC(100),        FMT_INC(100),        FMT_INC(100),         // 512
      FMT_INC(1000),       FMT_INC(1000),       FMT_INC(1000),        // 4096
      FMT_INC(10000),      FMT_INC(10000),      FMT_INC(10000),       // 32k
      FMT_INC(100000),     FMT_INC(100000),     FMT_INC(100000),      // 256k
      FMT_INC(1000000),    FMT_INC(1000000),    FMT_INC(1000000),     // 2048k
      FMT_INC(10000000),   FMT_INC(10000000),   FMT_INC(10000000),    // 16M
      FMT_INC(100000000),  FMT_INC(100000000),  FMT_INC(100000000),   // 128M
      FMT_INC(1000000000), FMT_INC(1000000000), FMT_INC(1000000000),  // 1024M
      FMT_INC(1000000000), FMT_INC(1000000000)                        // 4B
  };
  auto inc = table[FMT_BUILTIN_CLZ(n | 1) ^ 31];
  return static_cast<int>((n + inc) >> 32);
}
#endif

// Optional version of count_digits for better performance on 32-bit platforms.
FMT_CONSTEXPR20 inline auto count_digits(uint32_t n) -> int {
#ifdef FMT_BUILTIN_CLZ
  if (!is_constant_evaluated()) {
    return do_count_digits(n);
  }
#endif
  return count_digits_fallback(n);
}

template <typename Int> constexpr auto digits10() FMT_NOEXCEPT -> int {
  return std::numeric_limits<Int>::digits10;
}
template <> constexpr auto digits10<int128_t>() FMT_NOEXCEPT -> int {
  return 38;
}
template <> constexpr auto digits10<uint128_t>() FMT_NOEXCEPT -> int {
  return 38;
}

template <typename Char> struct thousands_sep_result {
  std::string grouping;
  Char thousands_sep;
};

template <typename Char>
FMT_API auto thousands_sep_impl(locale_ref loc) -> thousands_sep_result<Char>;
template <typename Char>
inline auto thousands_sep(locale_ref loc) -> thousands_sep_result<Char> {
  auto result = thousands_sep_impl<char>(loc);
  return {result.grouping, Char(result.thousands_sep)};
}
template <>
inline auto thousands_sep(locale_ref loc) -> thousands_sep_result<wchar_t> {
  return thousands_sep_impl<wchar_t>(loc);
}

template <typename Char>
FMT_API auto decimal_point_impl(locale_ref loc) -> Char;
template <typename Char> inline auto decimal_point(locale_ref loc) -> Char {
  return Char(decimal_point_impl<char>(loc));
}
template <> inline auto decimal_point(locale_ref loc) -> wchar_t {
  return decimal_point_impl<wchar_t>(loc);
}

// Compares two characters for equality.
template <typename Char> auto equal2(const Char* lhs, const char* rhs) -> bool {
  return lhs[0] == Char(rhs[0]) && lhs[1] == Char(rhs[1]);
}
inline auto equal2(const char* lhs, const char* rhs) -> bool {
  return memcmp(lhs, rhs, 2) == 0;
}

// Copies two characters from src to dst.
template <typename Char>
FMT_CONSTEXPR20 FMT_INLINE void copy2(Char* dst, const char* src) {
  if (!is_constant_evaluated() && sizeof(Char) == sizeof(char)) {
    memcpy(dst, src, 2);
    return;
  }
  *dst++ = static_cast<Char>(*src++);
  *dst = static_cast<Char>(*src);
}

template <typename Iterator> struct format_decimal_result {
  Iterator begin;
  Iterator end;
};

// Formats a decimal unsigned integer value writing into out pointing to a
// buffer of specified size. The caller must ensure that the buffer is large
// enough.
template <typename Char, typename UInt>
FMT_CONSTEXPR20 auto format_decimal(Char* out, UInt value, int size)
    -> format_decimal_result<Char*> {
  FMT_ASSERT(size >= count_digits(value), "invalid digit count");
  out += size;
  Char* end = out;
  while (value >= 100) {
    // Integer division is slow so do it for a group of two digits instead
    // of for every digit. The idea comes from the talk by Alexandrescu
    // "Three Optimization Tips for C++". See speed-test for a comparison.
    out -= 2;
    copy2(out, digits2(static_cast<size_t>(value % 100)));
    value /= 100;
  }
  if (value < 10) {
    *--out = static_cast<Char>('0' + value);
    return {out, end};
  }
  out -= 2;
  copy2(out, digits2(static_cast<size_t>(value)));
  return {out, end};
}

template <typename Char, typename UInt, typename Iterator,
          FMT_ENABLE_IF(!std::is_pointer<remove_cvref_t<Iterator>>::value)>
inline auto format_decimal(Iterator out, UInt value, int size)
    -> format_decimal_result<Iterator> {
  // Buffer is large enough to hold all digits (digits10 + 1).
  Char buffer[digits10<UInt>() + 1];
  auto end = format_decimal(buffer, value, size).end;
  return {out, detail::copy_str_noinline<Char>(buffer, end, out)};
}

template <unsigned BASE_BITS, typename Char, typename UInt>
FMT_CONSTEXPR auto format_uint(Char* buffer, UInt value, int num_digits,
                               bool upper = false) -> Char* {
  buffer += num_digits;
  Char* end = buffer;
  do {
    const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    unsigned digit = (value & ((1 << BASE_BITS) - 1));
    *--buffer = static_cast<Char>(BASE_BITS < 4 ? static_cast<char>('0' + digit)
                                                : digits[digit]);
  } while ((value >>= BASE_BITS) != 0);
  return end;
}

template <unsigned BASE_BITS, typename Char>
auto format_uint(Char* buffer, detail::fallback_uintptr n, int num_digits,
                 bool = false) -> Char* {
  auto char_digits = std::numeric_limits<unsigned char>::digits / 4;
  int start = (num_digits + char_digits - 1) / char_digits - 1;
  if (int start_digits = num_digits % char_digits) {
    unsigned value = n.value[start--];
    buffer = format_uint<BASE_BITS>(buffer, value, start_digits);
  }
  for (; start >= 0; --start) {
    unsigned value = n.value[start];
    buffer += char_digits;
    auto p = buffer;
    for (int i = 0; i < char_digits; ++i) {
      unsigned digit = (value & ((1 << BASE_BITS) - 1));
      *--p = static_cast<Char>("0123456789abcdef"[digit]);
      value >>= BASE_BITS;
    }
  }
  return buffer;
}

template <unsigned BASE_BITS, typename Char, typename It, typename UInt>
inline auto format_uint(It out, UInt value, int num_digits, bool upper = false)
    -> It {
  if (auto ptr = to_pointer<Char>(out, to_unsigned(num_digits))) {
    format_uint<BASE_BITS>(ptr, value, num_digits, upper);
    return out;
  }
  // Buffer should be large enough to hold all digits (digits / BASE_BITS + 1).
  char buffer[num_bits<UInt>() / BASE_BITS + 1];
  format_uint<BASE_BITS>(buffer, value, num_digits, upper);
  return detail::copy_str_noinline<Char>(buffer, buffer + num_digits, out);
}

// A converter from UTF-8 to UTF-16.
class utf8_to_utf16 {
 private:
  basic_memory_buffer<wchar_t> buffer_;

 public:
  FMT_API explicit utf8_to_utf16(string_view s);
  operator basic_string_view<wchar_t>() const { return {&buffer_[0], size()}; }
  auto size() const -> size_t { return buffer_.size() - 1; }
  auto c_str() const -> const wchar_t* { return &buffer_[0]; }
  auto str() const -> std::wstring { return {&buffer_[0], size()}; }
};

namespace dragonbox {

// Type-specific information that Dragonbox uses.
template <class T> struct float_info;

template <> struct float_info<float> {
  using carrier_uint = uint32_t;
  static const int significand_bits = 23;
  static const int exponent_bits = 8;
  static const int min_exponent = -126;
  static const int max_exponent = 127;
  static const int exponent_bias = -127;
  static const int decimal_digits = 9;
  static const int kappa = 1;
  static const int big_divisor = 100;
  static const int small_divisor = 10;
  static const int min_k = -31;
  static const int max_k = 46;
  static const int cache_bits = 64;
  static const int divisibility_check_by_5_threshold = 39;
  static const int case_fc_pm_half_lower_threshold = -1;
  static const int case_fc_pm_half_upper_threshold = 6;
  static const int case_fc_lower_threshold = -2;
  static const int case_fc_upper_threshold = 6;
  static const int case_shorter_interval_left_endpoint_lower_threshold = 2;
  static const int case_shorter_interval_left_endpoint_upper_threshold = 3;
  static const int shorter_interval_tie_lower_threshold = -35;
  static const int shorter_interval_tie_upper_threshold = -35;
  static const int max_trailing_zeros = 7;
};

template <> struct float_info<double> {
  using carrier_uint = uint64_t;
  static const int significand_bits = 52;
  static const int exponent_bits = 11;
  static const int min_exponent = -1022;
  static const int max_exponent = 1023;
  static const int exponent_bias = -1023;
  static const int decimal_digits = 17;
  static const int kappa = 2;
  static const int big_divisor = 1000;
  static const int small_divisor = 100;
  static const int min_k = -292;
  static const int max_k = 326;
  static const int cache_bits = 128;
  static const int divisibility_check_by_5_threshold = 86;
  static const int case_fc_pm_half_lower_threshold = -2;
  static const int case_fc_pm_half_upper_threshold = 9;
  static const int case_fc_lower_threshold = -4;
  static const int case_fc_upper_threshold = 9;
  static const int case_shorter_interval_left_endpoint_lower_threshold = 2;
  static const int case_shorter_interval_left_endpoint_upper_threshold = 3;
  static const int shorter_interval_tie_lower_threshold = -77;
  static const int shorter_interval_tie_upper_threshold = -77;
  static const int max_trailing_zeros = 16;
};

template <typename T> struct decimal_fp {
  using significand_type = typename float_info<T>::carrier_uint;
  significand_type significand;
  int exponent;
};

template <typename T>
FMT_API auto to_decimal(T x) FMT_NOEXCEPT -> decimal_fp<T>;
}  // namespace dragonbox

template <typename T>
constexpr auto exponent_mask() ->
    typename dragonbox::float_info<T>::carrier_uint {
  using uint = typename dragonbox::float_info<T>::carrier_uint;
  return ((uint(1) << dragonbox::float_info<T>::exponent_bits) - 1)
         << dragonbox::float_info<T>::significand_bits;
}

// Writes the exponent exp in the form "[+-]d{2,3}" to buffer.
template <typename Char, typename It>
FMT_CONSTEXPR auto write_exponent(int exp, It it) -> It {
  FMT_ASSERT(-10000 < exp && exp < 10000, "exponent out of range");
  if (exp < 0) {
    *it++ = static_cast<Char>('-');
    exp = -exp;
  } else {
    *it++ = static_cast<Char>('+');
  }
  if (exp >= 100) {
    const char* top = digits2(to_unsigned(exp / 100));
    if (exp >= 1000) *it++ = static_cast<Char>(top[0]);
    *it++ = static_cast<Char>(top[1]);
    exp %= 100;
  }
  const char* d = digits2(to_unsigned(exp));
  *it++ = static_cast<Char>(d[0]);
  *it++ = static_cast<Char>(d[1]);
  return it;
}

template <typename T>
FMT_HEADER_ONLY_CONSTEXPR20 auto format_float(T value, int precision,
                                              float_specs specs,
                                              buffer<char>& buf) -> int;

// Formats a floating-point number with snprintf.
template <typename T>
auto snprintf_float(T value, int precision, float_specs specs,
                    buffer<char>& buf) -> int;

template <typename T> constexpr auto promote_float(T value) -> T {
  return value;
}
constexpr auto promote_float(float value) -> double {
  return static_cast<double>(value);
}

template <typename OutputIt, typename Char>
FMT_NOINLINE FMT_CONSTEXPR auto fill(OutputIt it, size_t n,
                                     const fill_t<Char>& fill) -> OutputIt {
  auto fill_size = fill.size();
  if (fill_size == 1) return detail::fill_n(it, n, fill[0]);
  auto data = fill.data();
  for (size_t i = 0; i < n; ++i)
    it = copy_str<Char>(data, data + fill_size, it);
  return it;
}

// Writes the output of f, padded according to format specifications in specs.
// size: output size in code units.
// width: output display width in (terminal) column positions.
template <align::type align = align::left, typename OutputIt, typename Char,
          typename F>
FMT_CONSTEXPR auto write_padded(OutputIt out,
                                const basic_format_specs<Char>& specs,
                                size_t size, size_t width, F&& f) -> OutputIt {
  static_assert(align == align::left || align == align::right, "");
  unsigned spec_width = to_unsigned(specs.width);
  size_t padding = spec_width > width ? spec_width - width : 0;
  // Shifts are encoded as string literals because static constexpr is not
  // supported in constexpr functions.
  auto* shifts = align == align::left ? "\x1f\x1f\x00\x01" : "\x00\x1f\x00\x01";
  size_t left_padding = padding >> shifts[specs.align];
  size_t right_padding = padding - left_padding;
  auto it = reserve(out, size + padding * specs.fill.size());
  if (left_padding != 0) it = fill(it, left_padding, specs.fill);
  it = f(it);
  if (right_padding != 0) it = fill(it, right_padding, specs.fill);
  return base_iterator(out, it);
}

template <align::type align = align::left, typename OutputIt, typename Char,
          typename F>
constexpr auto write_padded(OutputIt out, const basic_format_specs<Char>& specs,
                            size_t size, F&& f) -> OutputIt {
  return write_padded<align>(out, specs, size, size, f);
}

template <align::type align = align::left, typename Char, typename OutputIt>
FMT_CONSTEXPR auto write_bytes(OutputIt out, string_view bytes,
                               const basic_format_specs<Char>& specs)
    -> OutputIt {
  return write_padded<align>(
      out, specs, bytes.size(), [bytes](reserve_iterator<OutputIt> it) {
        const char* data = bytes.data();
        return copy_str<Char>(data, data + bytes.size(), it);
      });
}

template <typename Char, typename OutputIt, typename UIntPtr>
auto write_ptr(OutputIt out, UIntPtr value,
               const basic_format_specs<Char>* specs) -> OutputIt {
  int num_digits = count_digits<4>(value);
  auto size = to_unsigned(num_digits) + size_t(2);
  auto write = [=](reserve_iterator<OutputIt> it) {
    *it++ = static_cast<Char>('0');
    *it++ = static_cast<Char>('x');
    return format_uint<4, Char>(it, value, num_digits);
  };
  return specs ? write_padded<align::right>(out, *specs, size, write)
               : base_iterator(out, write(reserve(out, size)));
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write_char(OutputIt out, Char value,
                              const basic_format_specs<Char>& specs)
    -> OutputIt {
  return write_padded(out, specs, 1, [=](reserve_iterator<OutputIt> it) {
    *it++ = value;
    return it;
  });
}
template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, Char value,
                         const basic_format_specs<Char>& specs,
                         locale_ref loc = {}) -> OutputIt {
  return check_char_specs(specs)
             ? write_char(out, value, specs)
             : write(out, static_cast<int>(value), specs, loc);
}

// Data for write_int that doesn't depend on output iterator type. It is used to
// avoid template code bloat.
template <typename Char> struct write_int_data {
  size_t size;
  size_t padding;

  FMT_CONSTEXPR write_int_data(int num_digits, unsigned prefix,
                               const basic_format_specs<Char>& specs)
      : size((prefix >> 24) + to_unsigned(num_digits)), padding(0) {
    if (specs.align == align::numeric) {
      auto width = to_unsigned(specs.width);
      if (width > size) {
        padding = width - size;
        size = width;
      }
    } else if (specs.precision > num_digits) {
      size = (prefix >> 24) + to_unsigned(specs.precision);
      padding = to_unsigned(specs.precision - num_digits);
    }
  }
};

// Writes an integer in the format
//   <left-padding><prefix><numeric-padding><digits><right-padding>
// where <digits> are written by write_digits(it).
// prefix contains chars in three lower bytes and the size in the fourth byte.
template <typename OutputIt, typename Char, typename W>
FMT_CONSTEXPR FMT_INLINE auto write_int(OutputIt out, int num_digits,
                                        unsigned prefix,
                                        const basic_format_specs<Char>& specs,
                                        W write_digits) -> OutputIt {
  // Slightly faster check for specs.width == 0 && specs.precision == -1.
  if ((specs.width | (specs.precision + 1)) == 0) {
    auto it = reserve(out, to_unsigned(num_digits) + (prefix >> 24));
    if (prefix != 0) {
      for (unsigned p = prefix & 0xffffff; p != 0; p >>= 8)
        *it++ = static_cast<Char>(p & 0xff);
    }
    return base_iterator(out, write_digits(it));
  }
  auto data = write_int_data<Char>(num_digits, prefix, specs);
  return write_padded<align::right>(
      out, specs, data.size, [=](reserve_iterator<OutputIt> it) {
        for (unsigned p = prefix & 0xffffff; p != 0; p >>= 8)
          *it++ = static_cast<Char>(p & 0xff);
        it = detail::fill_n(it, data.padding, static_cast<Char>('0'));
        return write_digits(it);
      });
}

template <typename Char> class digit_grouping {
 private:
  thousands_sep_result<Char> sep_;

  struct next_state {
    std::string::const_iterator group;
    int pos;
  };
  next_state initial_state() const { return {sep_.grouping.begin(), 0}; }

  // Returns the next digit group separator position.
  int next(next_state& state) const {
    if (!sep_.thousands_sep) return max_value<int>();
    if (state.group == sep_.grouping.end())
      return state.pos += sep_.grouping.back();
    if (*state.group <= 0 || *state.group == max_value<char>())
      return max_value<int>();
    state.pos += *state.group++;
    return state.pos;
  }

 public:
  explicit digit_grouping(locale_ref loc, bool localized = true) {
    if (localized)
      sep_ = thousands_sep<Char>(loc);
    else
      sep_.thousands_sep = Char();
  }
  explicit digit_grouping(thousands_sep_result<Char> sep) : sep_(sep) {}

  Char separator() const { return sep_.thousands_sep; }

  int count_separators(int num_digits) const {
    int count = 0;
    auto state = initial_state();
    while (num_digits > next(state)) ++count;
    return count;
  }

  // Applies grouping to digits and write the output to out.
  template <typename Out, typename C>
  Out apply(Out out, basic_string_view<C> digits) const {
    auto num_digits = static_cast<int>(digits.size());
    auto separators = basic_memory_buffer<int>();
    separators.push_back(0);
    auto state = initial_state();
    while (int i = next(state)) {
      if (i >= num_digits) break;
      separators.push_back(i);
    }
    for (int i = 0, sep_index = static_cast<int>(separators.size() - 1);
         i < num_digits; ++i) {
      if (num_digits - i == separators[sep_index]) {
        *out++ = separator();
        --sep_index;
      }
      *out++ = static_cast<Char>(digits[to_unsigned(i)]);
    }
    return out;
  }
};

template <typename OutputIt, typename UInt, typename Char>
auto write_int_localized(OutputIt out, UInt value, unsigned prefix,
                         const basic_format_specs<Char>& specs,
                         const digit_grouping<Char>& grouping) -> OutputIt {
  static_assert(std::is_same<uint64_or_128_t<UInt>, UInt>::value, "");
  int num_digits = count_digits(value);
  char digits[40];
  format_decimal(digits, value, num_digits);
  unsigned size = to_unsigned((prefix != 0 ? 1 : 0) + num_digits +
                              grouping.count_separators(num_digits));
  return write_padded<align::right>(
      out, specs, size, size, [&](reserve_iterator<OutputIt> it) {
        if (prefix != 0) *it++ = static_cast<Char>(prefix);
        return grouping.apply(it, string_view(digits, to_unsigned(num_digits)));
      });
}

template <typename OutputIt, typename UInt, typename Char>
auto write_int_localized(OutputIt& out, UInt value, unsigned prefix,
                         const basic_format_specs<Char>& specs, locale_ref loc)
    -> bool {
  auto grouping = digit_grouping<Char>(loc);
  out = write_int_localized(out, value, prefix, specs, grouping);
  return true;
}

FMT_CONSTEXPR inline void prefix_append(unsigned& prefix, unsigned value) {
  prefix |= prefix != 0 ? value << 8 : value;
  prefix += (1u + (value > 0xff ? 1 : 0)) << 24;
}

template <typename UInt> struct write_int_arg {
  UInt abs_value;
  unsigned prefix;
};

template <typename T>
FMT_CONSTEXPR auto make_write_int_arg(T value, sign_t sign)
    -> write_int_arg<uint32_or_64_or_128_t<T>> {
  auto prefix = 0u;
  auto abs_value = static_cast<uint32_or_64_or_128_t<T>>(value);
  if (is_negative(value)) {
    prefix = 0x01000000 | '-';
    abs_value = 0 - abs_value;
  } else {
    constexpr const unsigned prefixes[4] = {0, 0, 0x1000000u | '+',
                                            0x1000000u | ' '};
    prefix = prefixes[sign];
  }
  return {abs_value, prefix};
}

template <typename Char, typename OutputIt, typename T>
FMT_CONSTEXPR FMT_INLINE auto write_int(OutputIt out, write_int_arg<T> arg,
                                        const basic_format_specs<Char>& specs,
                                        locale_ref loc) -> OutputIt {
  static_assert(std::is_same<T, uint32_or_64_or_128_t<T>>::value, "");
  auto abs_value = arg.abs_value;
  auto prefix = arg.prefix;
  switch (specs.type) {
  case presentation_type::none:
  case presentation_type::dec: {
    if (specs.localized &&
        write_int_localized(out, static_cast<uint64_or_128_t<T>>(abs_value),
                            prefix, specs, loc)) {
      return out;
    }
    auto num_digits = count_digits(abs_value);
    return write_int(
        out, num_digits, prefix, specs, [=](reserve_iterator<OutputIt> it) {
          return format_decimal<Char>(it, abs_value, num_digits).end;
        });
  }
  case presentation_type::hex_lower:
  case presentation_type::hex_upper: {
    bool upper = specs.type == presentation_type::hex_upper;
    if (specs.alt)
      prefix_append(prefix, unsigned(upper ? 'X' : 'x') << 8 | '0');
    int num_digits = count_digits<4>(abs_value);
    return write_int(
        out, num_digits, prefix, specs, [=](reserve_iterator<OutputIt> it) {
          return format_uint<4, Char>(it, abs_value, num_digits, upper);
        });
  }
  case presentation_type::bin_lower:
  case presentation_type::bin_upper: {
    bool upper = specs.type == presentation_type::bin_upper;
    if (specs.alt)
      prefix_append(prefix, unsigned(upper ? 'B' : 'b') << 8 | '0');
    int num_digits = count_digits<1>(abs_value);
    return write_int(out, num_digits, prefix, specs,
                     [=](reserve_iterator<OutputIt> it) {
                       return format_uint<1, Char>(it, abs_value, num_digits);
                     });
  }
  case presentation_type::oct: {
    int num_digits = count_digits<3>(abs_value);
    // Octal prefix '0' is counted as a digit, so only add it if precision
    // is not greater than the number of digits.
    if (specs.alt && specs.precision <= num_digits && abs_value != 0)
      prefix_append(prefix, '0');
    return write_int(out, num_digits, prefix, specs,
                     [=](reserve_iterator<OutputIt> it) {
                       return format_uint<3, Char>(it, abs_value, num_digits);
                     });
  }
  case presentation_type::chr:
    return write_char(out, static_cast<Char>(abs_value), specs);
  default:
    throw_format_error("invalid type specifier");
  }
  return out;
}
template <typename Char, typename OutputIt, typename T>
FMT_CONSTEXPR FMT_NOINLINE auto write_int_noinline(
    OutputIt out, write_int_arg<T> arg, const basic_format_specs<Char>& specs,
    locale_ref loc) -> OutputIt {
  return write_int(out, arg, specs, loc);
}
template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        std::is_same<OutputIt, buffer_appender<Char>>::value)>
FMT_CONSTEXPR FMT_INLINE auto write(OutputIt out, T value,
                                    const basic_format_specs<Char>& specs,
                                    locale_ref loc) -> OutputIt {
  return write_int_noinline(out, make_write_int_arg(value, specs.sign), specs,
                            loc);
}
// An inlined version of write used in format string compilation.
template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        !std::is_same<OutputIt, buffer_appender<Char>>::value)>
FMT_CONSTEXPR FMT_INLINE auto write(OutputIt out, T value,
                                    const basic_format_specs<Char>& specs,
                                    locale_ref loc) -> OutputIt {
  return write_int(out, make_write_int_arg(value, specs.sign), specs, loc);
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> s,
                         const basic_format_specs<Char>& specs) -> OutputIt {
  auto data = s.data();
  auto size = s.size();
  if (specs.precision >= 0 && to_unsigned(specs.precision) < size)
    size = code_point_index(s, to_unsigned(specs.precision));
  auto width =
      specs.width != 0 ? compute_width(basic_string_view<Char>(data, size)) : 0;
  return write_padded(out, specs, size, width,
                      [=](reserve_iterator<OutputIt> it) {
                        return copy_str<Char>(data, data + size, it);
                      });
}
template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out,
                         basic_string_view<type_identity_t<Char>> s,
                         const basic_format_specs<Char>& specs, locale_ref)
    -> OutputIt {
  check_string_type_spec(specs.type);
  return write(out, s, specs);
}
template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, const Char* s,
                         const basic_format_specs<Char>& specs, locale_ref)
    -> OutputIt {
  return check_cstring_type_spec(specs.type)
             ? write(out, basic_string_view<Char>(s), specs, {})
             : write_ptr<Char>(out, to_uintptr(s), &specs);
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR20 auto write_nonfinite(OutputIt out, bool isinf,
                                     basic_format_specs<Char> specs,
                                     const float_specs& fspecs) -> OutputIt {
  auto str =
      isinf ? (fspecs.upper ? "INF" : "inf") : (fspecs.upper ? "NAN" : "nan");
  constexpr size_t str_size = 3;
  auto sign = fspecs.sign;
  auto size = str_size + (sign ? 1 : 0);
  // Replace '0'-padding with space for non-finite values.
  const bool is_zero_fill =
      specs.fill.size() == 1 && *specs.fill.data() == static_cast<Char>('0');
  if (is_zero_fill) specs.fill[0] = static_cast<Char>(' ');
  return write_padded(out, specs, size, [=](reserve_iterator<OutputIt> it) {
    if (sign) *it++ = detail::sign<Char>(sign);
    return copy_str<Char>(str, str + str_size, it);
  });
}

// A decimal floating-point number significand * pow(10, exp).
struct big_decimal_fp {
  const char* significand;
  int significand_size;
  int exponent;
};

constexpr auto get_significand_size(const big_decimal_fp& fp) -> int {
  return fp.significand_size;
}
template <typename T>
inline auto get_significand_size(const dragonbox::decimal_fp<T>& fp) -> int {
  return count_digits(fp.significand);
}

template <typename Char, typename OutputIt>
constexpr auto write_significand(OutputIt out, const char* significand,
                                 int significand_size) -> OutputIt {
  return copy_str<Char>(significand, significand + significand_size, out);
}
template <typename Char, typename OutputIt, typename UInt>
inline auto write_significand(OutputIt out, UInt significand,
                              int significand_size) -> OutputIt {
  return format_decimal<Char>(out, significand, significand_size).end;
}
template <typename Char, typename OutputIt, typename T, typename Grouping>
FMT_CONSTEXPR20 auto write_significand(OutputIt out, T significand,
                                       int significand_size, int exponent,
                                       const Grouping& grouping) -> OutputIt {
  if (!grouping.separator()) {
    out = write_significand<Char>(out, significand, significand_size);
    return detail::fill_n(out, exponent, static_cast<Char>('0'));
  }
  auto buffer = memory_buffer();
  write_significand<char>(appender(buffer), significand, significand_size);
  detail::fill_n(appender(buffer), exponent, '0');
  return grouping.apply(out, string_view(buffer.data(), buffer.size()));
}

template <typename Char, typename UInt,
          FMT_ENABLE_IF(std::is_integral<UInt>::value)>
inline auto write_significand(Char* out, UInt significand, int significand_size,
                              int integral_size, Char decimal_point) -> Char* {
  if (!decimal_point)
    return format_decimal(out, significand, significand_size).end;
  out += significand_size + 1;
  Char* end = out;
  int floating_size = significand_size - integral_size;
  for (int i = floating_size / 2; i > 0; --i) {
    out -= 2;
    copy2(out, digits2(significand % 100));
    significand /= 100;
  }
  if (floating_size % 2 != 0) {
    *--out = static_cast<Char>('0' + significand % 10);
    significand /= 10;
  }
  *--out = decimal_point;
  format_decimal(out - integral_size, significand, integral_size);
  return end;
}

template <typename OutputIt, typename UInt, typename Char,
          FMT_ENABLE_IF(!std::is_pointer<remove_cvref_t<OutputIt>>::value)>
inline auto write_significand(OutputIt out, UInt significand,
                              int significand_size, int integral_size,
                              Char decimal_point) -> OutputIt {
  // Buffer is large enough to hold digits (digits10 + 1) and a decimal point.
  Char buffer[digits10<UInt>() + 2];
  auto end = write_significand(buffer, significand, significand_size,
                               integral_size, decimal_point);
  return detail::copy_str_noinline<Char>(buffer, end, out);
}

template <typename OutputIt, typename Char>
FMT_CONSTEXPR auto write_significand(OutputIt out, const char* significand,
                                     int significand_size, int integral_size,
                                     Char decimal_point) -> OutputIt {
  out = detail::copy_str_noinline<Char>(significand,
                                        significand + integral_size, out);
  if (!decimal_point) return out;
  *out++ = decimal_point;
  return detail::copy_str_noinline<Char>(significand + integral_size,
                                         significand + significand_size, out);
}

template <typename OutputIt, typename Char, typename T, typename Grouping>
FMT_CONSTEXPR20 auto write_significand(OutputIt out, T significand,
                                       int significand_size, int integral_size,
                                       Char decimal_point,
                                       const Grouping& grouping) -> OutputIt {
  if (!grouping.separator()) {
    return write_significand(out, significand, significand_size, integral_size,
                             decimal_point);
  }
  auto buffer = basic_memory_buffer<Char>();
  write_significand(buffer_appender<Char>(buffer), significand,
                    significand_size, integral_size, decimal_point);
  grouping.apply(
      out, basic_string_view<Char>(buffer.data(), to_unsigned(integral_size)));
  return detail::copy_str_noinline<Char>(buffer.data() + integral_size,
                                         buffer.end(), out);
}

template <typename OutputIt, typename DecimalFP, typename Char,
          typename Grouping = digit_grouping<Char>>
FMT_CONSTEXPR20 auto do_write_float(OutputIt out, const DecimalFP& fp,
                                    const basic_format_specs<Char>& specs,
                                    float_specs fspecs, locale_ref loc)
    -> OutputIt {
  auto significand = fp.significand;
  int significand_size = get_significand_size(fp);
  constexpr Char zero = static_cast<Char>('0');
  auto sign = fspecs.sign;
  size_t size = to_unsigned(significand_size) + (sign ? 1 : 0);
  using iterator = reserve_iterator<OutputIt>;

  Char decimal_point =
      fspecs.locale ? detail::decimal_point<Char>(loc) : static_cast<Char>('.');

  int output_exp = fp.exponent + significand_size - 1;
  auto use_exp_format = [=]() {
    if (fspecs.format == float_format::exp) return true;
    if (fspecs.format != float_format::general) return false;
    // Use the fixed notation if the exponent is in [exp_lower, exp_upper),
    // e.g. 0.0001 instead of 1e-04. Otherwise use the exponent notation.
    const int exp_lower = -4, exp_upper = 16;
    return output_exp < exp_lower ||
           output_exp >= (fspecs.precision > 0 ? fspecs.precision : exp_upper);
  };
  if (use_exp_format()) {
    int num_zeros = 0;
    if (fspecs.showpoint) {
      num_zeros = fspecs.precision - significand_size;
      if (num_zeros < 0) num_zeros = 0;
      size += to_unsigned(num_zeros);
    } else if (significand_size == 1) {
      decimal_point = Char();
    }
    auto abs_output_exp = output_exp >= 0 ? output_exp : -output_exp;
    int exp_digits = 2;
    if (abs_output_exp >= 100) exp_digits = abs_output_exp >= 1000 ? 4 : 3;

    size += to_unsigned((decimal_point ? 1 : 0) + 2 + exp_digits);
    char exp_char = fspecs.upper ? 'E' : 'e';
    auto write = [=](iterator it) {
      if (sign) *it++ = detail::sign<Char>(sign);
      // Insert a decimal point after the first digit and add an exponent.
      it = write_significand(it, significand, significand_size, 1,
                             decimal_point);
      if (num_zeros > 0) it = detail::fill_n(it, num_zeros, zero);
      *it++ = static_cast<Char>(exp_char);
      return write_exponent<Char>(output_exp, it);
    };
    return specs.width > 0 ? write_padded<align::right>(out, specs, size, write)
                           : base_iterator(out, write(reserve(out, size)));
  }

  int exp = fp.exponent + significand_size;
  if (fp.exponent >= 0) {
    // 1234e5 -> 123400000[.0+]
    size += to_unsigned(fp.exponent);
    int num_zeros = fspecs.precision - exp;
#ifdef FMT_FUZZ
    if (num_zeros > 5000)
      throw std::runtime_error("fuzz mode - avoiding excessive cpu use");
#endif
    if (fspecs.showpoint) {
      if (num_zeros <= 0 && fspecs.format != float_format::fixed) num_zeros = 1;
      if (num_zeros > 0) size += to_unsigned(num_zeros) + 1;
    }
    auto grouping = Grouping(loc, fspecs.locale);
    size += to_unsigned(grouping.count_separators(significand_size));
    return write_padded<align::right>(out, specs, size, [&](iterator it) {
      if (sign) *it++ = detail::sign<Char>(sign);
      it = write_significand<Char>(it, significand, significand_size,
                                   fp.exponent, grouping);
      if (!fspecs.showpoint) return it;
      *it++ = decimal_point;
      return num_zeros > 0 ? detail::fill_n(it, num_zeros, zero) : it;
    });
  } else if (exp > 0) {
    // 1234e-2 -> 12.34[0+]
    int num_zeros = fspecs.showpoint ? fspecs.precision - significand_size : 0;
    size += 1 + to_unsigned(num_zeros > 0 ? num_zeros : 0);
    auto grouping = Grouping(loc, fspecs.locale);
    size += to_unsigned(grouping.count_separators(significand_size));
    return write_padded<align::right>(out, specs, size, [&](iterator it) {
      if (sign) *it++ = detail::sign<Char>(sign);
      it = write_significand(it, significand, significand_size, exp,
                             decimal_point, grouping);
      return num_zeros > 0 ? detail::fill_n(it, num_zeros, zero) : it;
    });
  }
  // 1234e-6 -> 0.001234
  int num_zeros = -exp;
  if (significand_size == 0 && fspecs.precision >= 0 &&
      fspecs.precision < num_zeros) {
    num_zeros = fspecs.precision;
  }
  bool pointy = num_zeros != 0 || significand_size != 0 || fspecs.showpoint;
  size += 1 + (pointy ? 1 : 0) + to_unsigned(num_zeros);
  return write_padded<align::right>(out, specs, size, [&](iterator it) {
    if (sign) *it++ = detail::sign<Char>(sign);
    *it++ = zero;
    if (!pointy) return it;
    *it++ = decimal_point;
    it = detail::fill_n(it, num_zeros, zero);
    return write_significand<Char>(it, significand, significand_size);
  });
}

template <typename Char> class fallback_digit_grouping {
 public:
  constexpr fallback_digit_grouping(locale_ref, bool) {}

  constexpr Char separator() const { return Char(); }

  constexpr int count_separators(int) const { return 0; }

  template <typename Out, typename C>
  constexpr Out apply(Out out, basic_string_view<C>) const {
    return out;
  }
};

template <typename OutputIt, typename DecimalFP, typename Char>
FMT_CONSTEXPR20 auto write_float(OutputIt out, const DecimalFP& fp,
                                 const basic_format_specs<Char>& specs,
                                 float_specs fspecs, locale_ref loc)
    -> OutputIt {
  if (is_constant_evaluated()) {
    return do_write_float<OutputIt, DecimalFP, Char,
                          fallback_digit_grouping<Char>>(out, fp, specs, fspecs,
                                                         loc);
  } else {
    return do_write_float(out, fp, specs, fspecs, loc);
  }
}

template <typename T, FMT_ENABLE_IF(std::is_floating_point<T>::value)>
FMT_CONSTEXPR20 bool isinf(T value) {
  if (is_constant_evaluated()) {
#if defined(__cpp_if_constexpr)
    if constexpr (std::numeric_limits<double>::is_iec559) {
      auto bits = detail::bit_cast<uint64_t>(static_cast<double>(value));
      constexpr auto significand_bits =
          dragonbox::float_info<double>::significand_bits;
      return (bits & exponent_mask<double>()) &&
             !(bits & ((uint64_t(1) << significand_bits) - 1));
    }
#endif
  }
  return std::isinf(value);
}

template <typename T, FMT_ENABLE_IF(std::is_floating_point<T>::value)>
FMT_CONSTEXPR20 bool isfinite(T value) {
  if (is_constant_evaluated()) {
#if defined(__cpp_if_constexpr)
    if constexpr (std::numeric_limits<double>::is_iec559) {
      auto bits = detail::bit_cast<uint64_t>(static_cast<double>(value));
      return (bits & exponent_mask<double>()) != exponent_mask<double>();
    }
#endif
  }
  return std::isfinite(value);
}

template <typename T, FMT_ENABLE_IF(std::is_floating_point<T>::value)>
FMT_INLINE FMT_CONSTEXPR bool signbit(T value) {
  if (is_constant_evaluated()) {
#ifdef __cpp_if_constexpr
    if constexpr (std::numeric_limits<double>::is_iec559) {
      auto bits = detail::bit_cast<uint64_t>(static_cast<double>(value));
      return (bits & (uint64_t(1) << (num_bits<uint64_t>() - 1))) != 0;
    }
#endif
  }
  return std::signbit(value);
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(std::is_floating_point<T>::value)>
FMT_CONSTEXPR20 auto write(OutputIt out, T value,
                           basic_format_specs<Char> specs, locale_ref loc = {})
    -> OutputIt {
  if (const_check(!is_supported_floating_point(value))) return out;
  float_specs fspecs = parse_float_type_spec(specs);
  fspecs.sign = specs.sign;
  if (detail::signbit(value)) {  // value < 0 is false for NaN so use signbit.
    fspecs.sign = sign::minus;
    value = -value;
  } else if (fspecs.sign == sign::minus) {
    fspecs.sign = sign::none;
  }

  if (!detail::isfinite(value))
    return write_nonfinite(out, detail::isinf(value), specs, fspecs);

  if (specs.align == align::numeric && fspecs.sign) {
    auto it = reserve(out, 1);
    *it++ = detail::sign<Char>(fspecs.sign);
    out = base_iterator(out, it);
    fspecs.sign = sign::none;
    if (specs.width != 0) --specs.width;
  }

  memory_buffer buffer;
  if (fspecs.format == float_format::hex) {
    if (fspecs.sign) buffer.push_back(detail::sign<char>(fspecs.sign));
    snprintf_float(promote_float(value), specs.precision, fspecs, buffer);
    return write_bytes<align::right>(out, {buffer.data(), buffer.size()},
                                     specs);
  }
  int precision = specs.precision >= 0 || specs.type == presentation_type::none
                      ? specs.precision
                      : 6;
  if (fspecs.format == float_format::exp) {
    if (precision == max_value<int>())
      throw_format_error("number is too big");
    else
      ++precision;
  }
  if (const_check(std::is_same<T, float>())) fspecs.binary32 = true;
  if (!is_fast_float<T>()) fspecs.fallback = true;
  int exp = format_float(promote_float(value), precision, fspecs, buffer);
  fspecs.precision = precision;
  auto fp = big_decimal_fp{buffer.data(), static_cast<int>(buffer.size()), exp};
  return write_float(out, fp, specs, fspecs, loc);
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_fast_float<T>::value)>
FMT_CONSTEXPR20 auto write(OutputIt out, T value) -> OutputIt {
  if (is_constant_evaluated()) {
    return write(out, value, basic_format_specs<Char>());
  }

  if (const_check(!is_supported_floating_point(value))) return out;

  using floaty = conditional_t<std::is_same<T, long double>::value, double, T>;
  using uint = typename dragonbox::float_info<floaty>::carrier_uint;
  auto bits = bit_cast<uint>(value);

  auto fspecs = float_specs();
  if (detail::signbit(value)) {
    fspecs.sign = sign::minus;
    value = -value;
  }

  constexpr auto specs = basic_format_specs<Char>();
  uint mask = exponent_mask<floaty>();
  if ((bits & mask) == mask)
    return write_nonfinite(out, std::isinf(value), specs, fspecs);

  auto dec = dragonbox::to_decimal(static_cast<floaty>(value));
  return write_float(out, dec, specs, fspecs, {});
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(std::is_floating_point<T>::value &&
                        !is_fast_float<T>::value)>
inline auto write(OutputIt out, T value) -> OutputIt {
  return write(out, value, basic_format_specs<Char>());
}

template <typename Char, typename OutputIt>
auto write(OutputIt out, monostate, basic_format_specs<Char> = {},
           locale_ref = {}) -> OutputIt {
  FMT_ASSERT(false, "");
  return out;
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> value)
    -> OutputIt {
  auto it = reserve(out, value.size());
  it = copy_str_noinline<Char>(value.begin(), value.end(), it);
  return base_iterator(out, it);
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_string<T>::value)>
constexpr auto write(OutputIt out, const T& value) -> OutputIt {
  return write<Char>(out, to_string_view(value));
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        !std::is_same<T, Char>::value)>
FMT_CONSTEXPR auto write(OutputIt out, T value) -> OutputIt {
  auto abs_value = static_cast<uint32_or_64_or_128_t<T>>(value);
  bool negative = is_negative(value);
  // Don't do -abs_value since it trips unsigned-integer-overflow sanitizer.
  if (negative) abs_value = ~abs_value + 1;
  int num_digits = count_digits(abs_value);
  auto size = (negative ? 1 : 0) + static_cast<size_t>(num_digits);
  auto it = reserve(out, size);
  if (auto ptr = to_pointer<Char>(it, size)) {
    if (negative) *ptr++ = static_cast<Char>('-');
    format_decimal<Char>(ptr, abs_value, num_digits);
    return out;
  }
  if (negative) *it++ = static_cast<Char>('-');
  it = format_decimal<Char>(it, abs_value, num_digits).end;
  return base_iterator(out, it);
}

// FMT_ENABLE_IF() condition separated to workaround an MSVC bug.
template <
    typename Char, typename OutputIt, typename T,
    bool check =
        std::is_enum<T>::value && !std::is_same<T, Char>::value &&
        mapped_type_constant<T, basic_format_context<OutputIt, Char>>::value !=
            type::custom_type,
    FMT_ENABLE_IF(check)>
FMT_CONSTEXPR auto write(OutputIt out, T value) -> OutputIt {
  return write<Char>(
      out, static_cast<typename std::underlying_type<T>::type>(value));
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(std::is_same<T, bool>::value)>
FMT_CONSTEXPR auto write(OutputIt out, T value,
                         const basic_format_specs<Char>& specs = {},
                         locale_ref = {}) -> OutputIt {
  return specs.type != presentation_type::none &&
                 specs.type != presentation_type::string
             ? write(out, value ? 1 : 0, specs, {})
             : write_bytes(out, value ? "true" : "false", specs);
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, Char value) -> OutputIt {
  auto it = reserve(out, 1);
  *it++ = value;
  return base_iterator(out, it);
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR_CHAR_TRAITS auto write(OutputIt out, const Char* value)
    -> OutputIt {
  if (!value) {
    throw_format_error("string pointer is null");
  } else {
    out = write(out, basic_string_view<Char>(value));
  }
  return out;
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(std::is_same<T, void>::value)>
auto write(OutputIt out, const T* value,
           const basic_format_specs<Char>& specs = {}, locale_ref = {})
    -> OutputIt {
  check_pointer_type_spec(specs.type, error_handler());
  return write_ptr<Char>(out, to_uintptr(value), &specs);
}

// A write overload that handles implicit conversions.
template <typename Char, typename OutputIt, typename T,
          typename Context = basic_format_context<OutputIt, Char>>
FMT_CONSTEXPR auto write(OutputIt out, const T& value) -> enable_if_t<
    std::is_class<T>::value && !is_string<T>::value &&
        !std::is_same<T, Char>::value &&
        !std::is_same<const T&,
                      decltype(arg_mapper<Context>().map(value))>::value,
    OutputIt> {
  return write<Char>(out, arg_mapper<Context>().map(value));
}

template <typename Char, typename OutputIt, typename T,
          typename Context = basic_format_context<OutputIt, Char>>
FMT_CONSTEXPR auto write(OutputIt out, const T& value)
    -> enable_if_t<mapped_type_constant<T, Context>::value == type::custom_type,
                   OutputIt> {
  using formatter_type =
      conditional_t<has_formatter<T, Context>::value,
                    typename Context::template formatter_type<T>,
                    fallback_formatter<T, Char>>;
  auto ctx = Context(out, {}, {});
  return formatter_type().format(value, ctx);
}

// An argument visitor that formats the argument and writes it via the output
// iterator. It's a class and not a generic lambda for compatibility with C++11.
template <typename Char> struct default_arg_formatter {
  using iterator = buffer_appender<Char>;
  using context = buffer_context<Char>;

  iterator out;
  basic_format_args<context> args;
  locale_ref loc;

  template <typename T> auto operator()(T value) -> iterator {
    return write<Char>(out, value);
  }
  auto operator()(typename basic_format_arg<context>::handle h) -> iterator {
    basic_format_parse_context<Char> parse_ctx({});
    context format_ctx(out, args, loc);
    h.format(parse_ctx, format_ctx);
    return format_ctx.out();
  }
};

template <typename Char> struct arg_formatter {
  using iterator = buffer_appender<Char>;
  using context = buffer_context<Char>;

  iterator out;
  const basic_format_specs<Char>& specs;
  locale_ref locale;

  template <typename T>
  FMT_CONSTEXPR FMT_INLINE auto operator()(T value) -> iterator {
    return detail::write(out, value, specs, locale);
  }
  auto operator()(typename basic_format_arg<context>::handle) -> iterator {
    // User-defined types are handled separately because they require access
    // to the parse context.
    return out;
  }
};

template <typename Char> struct custom_formatter {
  basic_format_parse_context<Char>& parse_ctx;
  buffer_context<Char>& ctx;

  void operator()(
      typename basic_format_arg<buffer_context<Char>>::handle h) const {
    h.format(parse_ctx, ctx);
  }
  template <typename T> void operator()(T) const {}
};

template <typename T>
using is_integer =
    bool_constant<is_integral<T>::value && !std::is_same<T, bool>::value &&
                  !std::is_same<T, char>::value &&
                  !std::is_same<T, wchar_t>::value>;

template <typename ErrorHandler> class width_checker {
 public:
  explicit FMT_CONSTEXPR width_checker(ErrorHandler& eh) : handler_(eh) {}

  template <typename T, FMT_ENABLE_IF(is_integer<T>::value)>
  FMT_CONSTEXPR auto operator()(T value) -> unsigned long long {
    if (is_negative(value)) handler_.on_error("negative width");
    return static_cast<unsigned long long>(value);
  }

  template <typename T, FMT_ENABLE_IF(!is_integer<T>::value)>
  FMT_CONSTEXPR auto operator()(T) -> unsigned long long {
    handler_.on_error("width is not integer");
    return 0;
  }

 private:
  ErrorHandler& handler_;
};

template <typename ErrorHandler> class precision_checker {
 public:
  explicit FMT_CONSTEXPR precision_checker(ErrorHandler& eh) : handler_(eh) {}

  template <typename T, FMT_ENABLE_IF(is_integer<T>::value)>
  FMT_CONSTEXPR auto operator()(T value) -> unsigned long long {
    if (is_negative(value)) handler_.on_error("negative precision");
    return static_cast<unsigned long long>(value);
  }

  template <typename T, FMT_ENABLE_IF(!is_integer<T>::value)>
  FMT_CONSTEXPR auto operator()(T) -> unsigned long long {
    handler_.on_error("precision is not integer");
    return 0;
  }

 private:
  ErrorHandler& handler_;
};

template <template <typename> class Handler, typename FormatArg,
          typename ErrorHandler>
FMT_CONSTEXPR auto get_dynamic_spec(FormatArg arg, ErrorHandler eh) -> int {
  unsigned long long value = visit_format_arg(Handler<ErrorHandler>(eh), arg);
  if (value > to_unsigned(max_value<int>())) eh.on_error("number is too big");
  return static_cast<int>(value);
}

template <typename Context, typename ID>
FMT_CONSTEXPR auto get_arg(Context& ctx, ID id) ->
    typename Context::format_arg {
  auto arg = ctx.arg(id);
  if (!arg) ctx.on_error("argument not found");
  return arg;
}

// The standard format specifier handler with checking.
template <typename Char> class specs_handler : public specs_setter<Char> {
 private:
  basic_format_parse_context<Char>& parse_context_;
  buffer_context<Char>& context_;

  // This is only needed for compatibility with gcc 4.4.
  using format_arg = basic_format_arg<buffer_context<Char>>;

  FMT_CONSTEXPR auto get_arg(auto_id) -> format_arg {
    return detail::get_arg(context_, parse_context_.next_arg_id());
  }

  FMT_CONSTEXPR auto get_arg(int arg_id) -> format_arg {
    parse_context_.check_arg_id(arg_id);
    return detail::get_arg(context_, arg_id);
  }

  FMT_CONSTEXPR auto get_arg(basic_string_view<Char> arg_id) -> format_arg {
    parse_context_.check_arg_id(arg_id);
    return detail::get_arg(context_, arg_id);
  }

 public:
  FMT_CONSTEXPR specs_handler(basic_format_specs<Char>& specs,
                              basic_format_parse_context<Char>& parse_ctx,
                              buffer_context<Char>& ctx)
      : specs_setter<Char>(specs), parse_context_(parse_ctx), context_(ctx) {}

  template <typename Id> FMT_CONSTEXPR void on_dynamic_width(Id arg_id) {
    this->specs_.width = get_dynamic_spec<width_checker>(
        get_arg(arg_id), context_.error_handler());
  }

  template <typename Id> FMT_CONSTEXPR void on_dynamic_precision(Id arg_id) {
    this->specs_.precision = get_dynamic_spec<precision_checker>(
        get_arg(arg_id), context_.error_handler());
  }

  void on_error(const char* message) { context_.on_error(message); }
};

template <template <typename> class Handler, typename Context>
FMT_CONSTEXPR void handle_dynamic_spec(int& value,
                                       arg_ref<typename Context::char_type> ref,
                                       Context& ctx) {
  switch (ref.kind) {
  case arg_id_kind::none:
    break;
  case arg_id_kind::index:
    value = detail::get_dynamic_spec<Handler>(ctx.arg(ref.val.index),
                                              ctx.error_handler());
    break;
  case arg_id_kind::name:
    value = detail::get_dynamic_spec<Handler>(ctx.arg(ref.val.name),
                                              ctx.error_handler());
    break;
  }
}

#define FMT_STRING_IMPL(s, base, explicit)                                 \
  [] {                                                                     \
    /* Use the hidden visibility as a workaround for a GCC bug (#1973). */ \
    /* Use a macro-like name to avoid shadowing warnings. */               \
    struct FMT_GCC_VISIBILITY_HIDDEN FMT_COMPILE_STRING : base {           \
      using char_type = fmt::remove_cvref_t<decltype(s[0])>;               \
      FMT_MAYBE_UNUSED FMT_CONSTEXPR explicit                              \
      operator fmt::basic_string_view<char_type>() const {                 \
        return fmt::detail_exported::compile_string_to_view<char_type>(s); \
      }                                                                    \
    };                                                                     \
    return FMT_COMPILE_STRING();                                           \
  }()

/**
  \rst
  Constructs a compile-time format string from a string literal *s*.

  **Example**::

    // A compile-time error because 'd' is an invalid specifier for strings.
    std::string s = fmt::format(FMT_STRING("{:d}"), "foo");
  \endrst
 */
#define FMT_STRING(s) FMT_STRING_IMPL(s, fmt::compile_string, )

#if FMT_USE_USER_DEFINED_LITERALS
template <typename Char> struct udl_formatter {
  basic_string_view<Char> str;

  template <typename... T>
  auto operator()(T&&... args) const -> std::basic_string<Char> {
    return vformat(str, fmt::make_args_checked<T...>(str, args...));
  }
};

#  if FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
template <typename T, typename Char, size_t N,
          fmt::detail_exported::fixed_string<Char, N> Str>
struct statically_named_arg : view {
  static constexpr auto name = Str.data;

  const T& value;
  statically_named_arg(const T& v) : value(v) {}
};

template <typename T, typename Char, size_t N,
          fmt::detail_exported::fixed_string<Char, N> Str>
struct is_named_arg<statically_named_arg<T, Char, N, Str>> : std::true_type {};

template <typename T, typename Char, size_t N,
          fmt::detail_exported::fixed_string<Char, N> Str>
struct is_statically_named_arg<statically_named_arg<T, Char, N, Str>>
    : std::true_type {};

template <typename Char, size_t N,
          fmt::detail_exported::fixed_string<Char, N> Str>
struct udl_arg {
  template <typename T> auto operator=(T&& value) const {
    return statically_named_arg<T, Char, N, Str>(std::forward<T>(value));
  }
};
#  else
template <typename Char> struct udl_arg {
  const Char* str;

  template <typename T> auto operator=(T&& value) const -> named_arg<Char, T> {
    return {str, std::forward<T>(value)};
  }
};
#  endif
#endif  // FMT_USE_USER_DEFINED_LITERALS

template <typename Locale, typename Char>
auto vformat(const Locale& loc, basic_string_view<Char> format_str,
             basic_format_args<buffer_context<type_identity_t<Char>>> args)
    -> std::basic_string<Char> {
  basic_memory_buffer<Char> buffer;
  detail::vformat_to(buffer, format_str, args, detail::locale_ref(loc));
  return {buffer.data(), buffer.size()};
}

using format_func = void (*)(detail::buffer<char>&, int, const char*);

FMT_API void format_error_code(buffer<char>& out, int error_code,
                               string_view message) FMT_NOEXCEPT;

FMT_API void report_error(format_func func, int error_code,
                          const char* message) FMT_NOEXCEPT;
FMT_END_DETAIL_NAMESPACE

FMT_API auto vsystem_error(int error_code, string_view format_str,
                           format_args args) -> std::system_error;

/**
 \rst
 Constructs :class:`std::system_error` with a message formatted with
 ``fmt::format(fmt, args...)``.
  *error_code* is a system error code as given by ``errno``.

 **Example**::

   // This throws std::system_error with the description
   //   cannot open file 'madeup': No such file or directory
   // or similar (system message may vary).
   const char* filename = "madeup";
   std::FILE* file = std::fopen(filename, "r");
   if (!file)
     throw fmt::system_error(errno, "cannot open file '{}'", filename);
 \endrst
*/
template <typename... T>
auto system_error(int error_code, format_string<T...> fmt, T&&... args)
    -> std::system_error {
  return vsystem_error(error_code, fmt, fmt::make_format_args(args...));
}

/**
  \rst
  Formats an error message for an error returned by an operating system or a
  language runtime, for example a file opening error, and writes it to *out*.
  The format is the same as the one used by ``std::system_error(ec, message)``
  where ``ec`` is ``std::error_code(error_code, std::generic_category()})``.
  It is implementation-defined but normally looks like:

  .. parsed-literal::
     *<message>*: *<system-message>*

  where *<message>* is the passed message and *<system-message>* is the system
  message corresponding to the error code.
  *error_code* is a system error code as given by ``errno``.
  \endrst
 */
FMT_API void format_system_error(detail::buffer<char>& out, int error_code,
                                 const char* message) FMT_NOEXCEPT;

// Reports a system error without throwing an exception.
// Can be used to report errors from destructors.
FMT_API void report_system_error(int error_code,
                                 const char* message) FMT_NOEXCEPT;

/** Fast integer formatter. */
class format_int {
 private:
  // Buffer should be large enough to hold all digits (digits10 + 1),
  // a sign and a null character.
  enum { buffer_size = std::numeric_limits<unsigned long long>::digits10 + 3 };
  mutable char buffer_[buffer_size];
  char* str_;

  template <typename UInt> auto format_unsigned(UInt value) -> char* {
    auto n = static_cast<detail::uint32_or_64_or_128_t<UInt>>(value);
    return detail::format_decimal(buffer_, n, buffer_size - 1).begin;
  }

  template <typename Int> auto format_signed(Int value) -> char* {
    auto abs_value = static_cast<detail::uint32_or_64_or_128_t<Int>>(value);
    bool negative = value < 0;
    if (negative) abs_value = 0 - abs_value;
    auto begin = format_unsigned(abs_value);
    if (negative) *--begin = '-';
    return begin;
  }

 public:
  explicit format_int(int value) : str_(format_signed(value)) {}
  explicit format_int(long value) : str_(format_signed(value)) {}
  explicit format_int(long long value) : str_(format_signed(value)) {}
  explicit format_int(unsigned value) : str_(format_unsigned(value)) {}
  explicit format_int(unsigned long value) : str_(format_unsigned(value)) {}
  explicit format_int(unsigned long long value)
      : str_(format_unsigned(value)) {}

  /** Returns the number of characters written to the output buffer. */
  auto size() const -> size_t {
    return detail::to_unsigned(buffer_ - str_ + buffer_size - 1);
  }

  /**
    Returns a pointer to the output buffer content. No terminating null
    character is appended.
   */
  auto data() const -> const char* { return str_; }

  /**
    Returns a pointer to the output buffer content with terminating null
    character appended.
   */
  auto c_str() const -> const char* {
    buffer_[buffer_size - 1] = '\0';
    return str_;
  }

  /**
    \rst
    Returns the content of the output buffer as an ``std::string``.
    \endrst
   */
  auto str() const -> std::string { return std::string(str_, size()); }
};

template <typename T, typename Char>
template <typename FormatContext>
FMT_CONSTEXPR FMT_INLINE auto
formatter<T, Char,
          enable_if_t<detail::type_constant<T, Char>::value !=
                      detail::type::custom_type>>::format(const T& val,
                                                          FormatContext& ctx)
    const -> decltype(ctx.out()) {
  if (specs_.width_ref.kind != detail::arg_id_kind::none ||
      specs_.precision_ref.kind != detail::arg_id_kind::none) {
    auto specs = specs_;
    detail::handle_dynamic_spec<detail::width_checker>(specs.width,
                                                       specs.width_ref, ctx);
    detail::handle_dynamic_spec<detail::precision_checker>(
        specs.precision, specs.precision_ref, ctx);
    return detail::write<Char>(ctx.out(), val, specs, ctx.locale());
  }
  return detail::write<Char>(ctx.out(), val, specs_, ctx.locale());
}

#define FMT_FORMAT_AS(Type, Base)                                        \
  template <typename Char>                                               \
  struct formatter<Type, Char> : formatter<Base, Char> {                 \
    template <typename FormatContext>                                    \
    auto format(Type const& val, FormatContext& ctx) const               \
        -> decltype(ctx.out()) {                                         \
      return formatter<Base, Char>::format(static_cast<Base>(val), ctx); \
    }                                                                    \
  }

FMT_FORMAT_AS(signed char, int);
FMT_FORMAT_AS(unsigned char, unsigned);
FMT_FORMAT_AS(short, int);
FMT_FORMAT_AS(unsigned short, unsigned);
FMT_FORMAT_AS(long, long long);
FMT_FORMAT_AS(unsigned long, unsigned long long);
FMT_FORMAT_AS(Char*, const Char*);
FMT_FORMAT_AS(std::basic_string<Char>, basic_string_view<Char>);
FMT_FORMAT_AS(std::nullptr_t, const void*);
FMT_FORMAT_AS(detail::byte, unsigned char);
FMT_FORMAT_AS(detail::std_string_view<Char>, basic_string_view<Char>);

template <typename Char>
struct formatter<void*, Char> : formatter<const void*, Char> {
  template <typename FormatContext>
  auto format(void* val, FormatContext& ctx) const -> decltype(ctx.out()) {
    return formatter<const void*, Char>::format(val, ctx);
  }
};

template <typename Char, size_t N>
struct formatter<Char[N], Char> : formatter<basic_string_view<Char>, Char> {
  template <typename FormatContext>
  FMT_CONSTEXPR auto format(const Char* val, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return formatter<basic_string_view<Char>, Char>::format(val, ctx);
  }
};

// A formatter for types known only at run time such as variant alternatives.
//
// Usage:
//   using variant = std::variant<int, std::string>;
//   template <>
//   struct formatter<variant>: dynamic_formatter<> {
//     auto format(const variant& v, format_context& ctx) {
//       return visit([&](const auto& val) {
//           return dynamic_formatter<>::format(val, ctx);
//       }, v);
//     }
//   };
template <typename Char = char> class dynamic_formatter {
 private:
  detail::dynamic_format_specs<Char> specs_;
  const Char* format_str_;

  struct null_handler : detail::error_handler {
    void on_align(align_t) {}
    void on_sign(sign_t) {}
    void on_hash() {}
  };

  template <typename Context> void handle_specs(Context& ctx) {
    detail::handle_dynamic_spec<detail::width_checker>(specs_.width,
                                                       specs_.width_ref, ctx);
    detail::handle_dynamic_spec<detail::precision_checker>(
        specs_.precision, specs_.precision_ref, ctx);
  }

 public:
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    format_str_ = ctx.begin();
    // Checks are deferred to formatting time when the argument type is known.
    detail::dynamic_specs_handler<ParseContext> handler(specs_, ctx);
    return detail::parse_format_specs(ctx.begin(), ctx.end(), handler);
  }

  template <typename T, typename FormatContext>
  auto format(const T& val, FormatContext& ctx) -> decltype(ctx.out()) {
    handle_specs(ctx);
    detail::specs_checker<null_handler> checker(
        null_handler(), detail::mapped_type_constant<T, FormatContext>::value);
    checker.on_align(specs_.align);
    if (specs_.sign != sign::none) checker.on_sign(specs_.sign);
    if (specs_.alt) checker.on_hash();
    if (specs_.precision >= 0) checker.end_precision();
    return detail::write<Char>(ctx.out(), val, specs_, ctx.locale());
  }
};

/**
  \rst
  Converts ``p`` to ``const void*`` for pointer formatting.

  **Example**::

    auto s = fmt::format("{}", fmt::ptr(p));
  \endrst
 */
template <typename T> auto ptr(T p) -> const void* {
  static_assert(std::is_pointer<T>::value, "");
  return detail::bit_cast<const void*>(p);
}
template <typename T> auto ptr(const std::unique_ptr<T>& p) -> const void* {
  return p.get();
}
template <typename T> auto ptr(const std::shared_ptr<T>& p) -> const void* {
  return p.get();
}

class bytes {
 private:
  string_view data_;
  friend struct formatter<bytes>;

 public:
  explicit bytes(string_view data) : data_(data) {}
};

template <> struct formatter<bytes> {
 private:
  detail::dynamic_format_specs<char> specs_;

 public:
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    using handler_type = detail::dynamic_specs_handler<ParseContext>;
    detail::specs_checker<handler_type> handler(handler_type(specs_, ctx),
                                                detail::type::string_type);
    auto it = parse_format_specs(ctx.begin(), ctx.end(), handler);
    detail::check_string_type_spec(specs_.type, ctx.error_handler());
    return it;
  }

  template <typename FormatContext>
  auto format(bytes b, FormatContext& ctx) -> decltype(ctx.out()) {
    detail::handle_dynamic_spec<detail::width_checker>(specs_.width,
                                                       specs_.width_ref, ctx);
    detail::handle_dynamic_spec<detail::precision_checker>(
        specs_.precision, specs_.precision_ref, ctx);
    return detail::write_bytes(ctx.out(), b.data_, specs_);
  }
};

// group_digits_view is not derived from view because it copies the argument.
template <typename T> struct group_digits_view { T value; };

/**
  \rst
  Returns a view that formats an integer value using ',' as a locale-independent
  thousands separator.

  **Example**::

    fmt::print("{}", fmt::group_digits(12345));
    // Output: "12,345"
  \endrst
 */
template <typename T> auto group_digits(T value) -> group_digits_view<T> {
  return {value};
}

template <typename T> struct formatter<group_digits_view<T>> : formatter<T> {
 private:
  detail::dynamic_format_specs<char> specs_;

 public:
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    using handler_type = detail::dynamic_specs_handler<ParseContext>;
    detail::specs_checker<handler_type> handler(handler_type(specs_, ctx),
                                                detail::type::int_type);
    auto it = parse_format_specs(ctx.begin(), ctx.end(), handler);
    detail::check_string_type_spec(specs_.type, ctx.error_handler());
    return it;
  }

  template <typename FormatContext>
  auto format(group_digits_view<T> t, FormatContext& ctx)
      -> decltype(ctx.out()) {
    detail::handle_dynamic_spec<detail::width_checker>(specs_.width,
                                                       specs_.width_ref, ctx);
    detail::handle_dynamic_spec<detail::precision_checker>(
        specs_.precision, specs_.precision_ref, ctx);
    return detail::write_int_localized(
        ctx.out(), static_cast<detail::uint64_or_128_t<T>>(t.value), 0, specs_,
        detail::digit_grouping<char>({"\3", ','}));
  }
};

template <typename It, typename Sentinel, typename Char = char>
struct join_view : detail::view {
  It begin;
  Sentinel end;
  basic_string_view<Char> sep;

  join_view(It b, Sentinel e, basic_string_view<Char> s)
      : begin(b), end(e), sep(s) {}
};

template <typename It, typename Sentinel, typename Char>
using arg_join FMT_DEPRECATED_ALIAS = join_view<It, Sentinel, Char>;

template <typename It, typename Sentinel, typename Char>
struct formatter<join_view<It, Sentinel, Char>, Char> {
 private:
  using value_type =
#ifdef __cpp_lib_ranges
      std::iter_value_t<It>;
#else
      typename std::iterator_traits<It>::value_type;
#endif
  using context = buffer_context<Char>;
  using mapper = detail::arg_mapper<context>;

  template <typename T, FMT_ENABLE_IF(has_formatter<T, context>::value)>
  static auto map(const T& value) -> const T& {
    return value;
  }
  template <typename T, FMT_ENABLE_IF(!has_formatter<T, context>::value)>
  static auto map(const T& value) -> decltype(mapper().map(value)) {
    return mapper().map(value);
  }

  using formatter_type =
      conditional_t<is_formattable<value_type, Char>::value,
                    formatter<remove_cvref_t<decltype(map(
                                  std::declval<const value_type&>()))>,
                              Char>,
                    detail::fallback_formatter<value_type, Char>>;

  formatter_type value_formatter_;

 public:
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext& ctx) -> decltype(ctx.begin()) {
    return value_formatter_.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const join_view<It, Sentinel, Char>& value, FormatContext& ctx)
      -> decltype(ctx.out()) {
    auto it = value.begin;
    auto out = ctx.out();
    if (it != value.end) {
      out = value_formatter_.format(map(*it), ctx);
      ++it;
      while (it != value.end) {
        out = detail::copy_str<Char>(value.sep.begin(), value.sep.end(), out);
        ctx.advance_to(out);
        out = value_formatter_.format(map(*it), ctx);
        ++it;
      }
    }
    return out;
  }
};

/**
  Returns a view that formats the iterator range `[begin, end)` with elements
  separated by `sep`.
 */
template <typename It, typename Sentinel>
auto join(It begin, Sentinel end, string_view sep) -> join_view<It, Sentinel> {
  return {begin, end, sep};
}

/**
  \rst
  Returns a view that formats `range` with elements separated by `sep`.

  **Example**::

    std::vector<int> v = {1, 2, 3};
    fmt::print("{}", fmt::join(v, ", "));
    // Output: "1, 2, 3"

  ``fmt::join`` applies passed format specifiers to the range elements::

    fmt::print("{:02}", fmt::join(v, ", "));
    // Output: "01, 02, 03"
  \endrst
 */
template <typename Range>
auto join(Range&& range, string_view sep)
    -> join_view<detail::iterator_t<Range>, detail::sentinel_t<Range>> {
  return join(std::begin(range), std::end(range), sep);
}

/**
  \rst
  Converts *value* to ``std::string`` using the default format for type *T*.

  **Example**::

    #include <fmt/format.h>

    std::string answer = fmt::to_string(42);
  \endrst
 */
template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value)>
inline auto to_string(const T& value) -> std::string {
  auto result = std::string();
  detail::write<char>(std::back_inserter(result), value);
  return result;
}

template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
FMT_NODISCARD inline auto to_string(T value) -> std::string {
  // The buffer should be large enough to store the number including the sign
  // or "false" for bool.
  constexpr int max_size = detail::digits10<T>() + 2;
  char buffer[max_size > 5 ? static_cast<unsigned>(max_size) : 5];
  char* begin = buffer;
  return std::string(begin, detail::write<char>(begin, value));
}

template <typename Char, size_t SIZE>
FMT_NODISCARD auto to_string(const basic_memory_buffer<Char, SIZE>& buf)
    -> std::basic_string<Char> {
  auto size = buf.size();
  detail::assume(size < std::basic_string<Char>().max_size());
  return std::basic_string<Char>(buf.data(), size);
}

FMT_BEGIN_DETAIL_NAMESPACE

template <typename Char>
void vformat_to(
    buffer<Char>& buf, basic_string_view<Char> fmt,
    basic_format_args<FMT_BUFFER_CONTEXT(type_identity_t<Char>)> args,
    locale_ref loc) {
  // workaround for msvc bug regarding name-lookup in module
  // link names into function scope
  using detail::arg_formatter;
  using detail::buffer_appender;
  using detail::custom_formatter;
  using detail::default_arg_formatter;
  using detail::get_arg;
  using detail::locale_ref;
  using detail::parse_format_specs;
  using detail::specs_checker;
  using detail::specs_handler;
  using detail::to_unsigned;
  using detail::type;
  using detail::write;
  auto out = buffer_appender<Char>(buf);
  if (fmt.size() == 2 && equal2(fmt.data(), "{}")) {
    auto arg = args.get(0);
    if (!arg) error_handler().on_error("argument not found");
    visit_format_arg(default_arg_formatter<Char>{out, args, loc}, arg);
    return;
  }

  struct format_handler : error_handler {
    basic_format_parse_context<Char> parse_context;
    buffer_context<Char> context;

    format_handler(buffer_appender<Char> out, basic_string_view<Char> str,
                   basic_format_args<buffer_context<Char>> args, locale_ref loc)
        : parse_context(str), context(out, args, loc) {}

    void on_text(const Char* begin, const Char* end) {
      auto text = basic_string_view<Char>(begin, to_unsigned(end - begin));
      context.advance_to(write<Char>(context.out(), text));
    }

    FMT_CONSTEXPR auto on_arg_id() -> int {
      return parse_context.next_arg_id();
    }
    FMT_CONSTEXPR auto on_arg_id(int id) -> int {
      return parse_context.check_arg_id(id), id;
    }
    FMT_CONSTEXPR auto on_arg_id(basic_string_view<Char> id) -> int {
      int arg_id = context.arg_id(id);
      if (arg_id < 0) on_error("argument not found");
      return arg_id;
    }

    FMT_INLINE void on_replacement_field(int id, const Char*) {
      auto arg = get_arg(context, id);
      context.advance_to(visit_format_arg(
          default_arg_formatter<Char>{context.out(), context.args(),
                                      context.locale()},
          arg));
    }

    auto on_format_specs(int id, const Char* begin, const Char* end)
        -> const Char* {
      auto arg = get_arg(context, id);
      if (arg.type() == type::custom_type) {
        parse_context.advance_to(parse_context.begin() +
                                 (begin - &*parse_context.begin()));
        visit_format_arg(custom_formatter<Char>{parse_context, context}, arg);
        return parse_context.begin();
      }
      auto specs = basic_format_specs<Char>();
      specs_checker<specs_handler<Char>> handler(
          specs_handler<Char>(specs, parse_context, context), arg.type());
      begin = parse_format_specs(begin, end, handler);
      if (begin == end || *begin != '}')
        on_error("missing '}' in format string");
      auto f = arg_formatter<Char>{context.out(), specs, context.locale()};
      context.advance_to(visit_format_arg(f, arg));
      return begin;
    }
  };
  detail::parse_format_string<false>(fmt, format_handler(out, fmt, args, loc));
}

#ifndef FMT_HEADER_ONLY
extern template FMT_API auto thousands_sep_impl<char>(locale_ref)
    -> thousands_sep_result<char>;
extern template FMT_API auto thousands_sep_impl<wchar_t>(locale_ref)
    -> thousands_sep_result<wchar_t>;
extern template FMT_API auto decimal_point_impl(locale_ref) -> char;
extern template FMT_API auto decimal_point_impl(locale_ref) -> wchar_t;
extern template auto format_float<double>(double value, int precision,
                                          float_specs specs, buffer<char>& buf)
    -> int;
extern template auto format_float<long double>(long double value, int precision,
                                               float_specs specs,
                                               buffer<char>& buf) -> int;
void snprintf_float(float, int, float_specs, buffer<char>&) = delete;
extern template auto snprintf_float<double>(double value, int precision,
                                            float_specs specs,
                                            buffer<char>& buf) -> int;
extern template auto snprintf_float<long double>(long double value,
                                                 int precision,
                                                 float_specs specs,
                                                 buffer<char>& buf) -> int;
#endif  // FMT_HEADER_ONLY

FMT_END_DETAIL_NAMESPACE

#if FMT_USE_USER_DEFINED_LITERALS
inline namespace literals {
/**
  \rst
  User-defined literal equivalent of :func:`fmt::arg`.

  **Example**::

    using namespace fmt::literals;
    fmt::print("Elapsed time: {s:.2f} seconds", "s"_a=1.23);
  \endrst
 */
#  if FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
template <detail_exported::fixed_string Str>
constexpr auto operator""_a()
    -> detail::udl_arg<remove_cvref_t<decltype(Str.data[0])>,
                       sizeof(Str.data) / sizeof(decltype(Str.data[0])), Str> {
  return {};
}
#  else
constexpr auto operator"" _a(const char* s, size_t) -> detail::udl_arg<char> {
  return {s};
}
#  endif

// DEPRECATED!
// User-defined literal equivalent of fmt::format.
FMT_DEPRECATED constexpr auto operator"" _format(const char* s, size_t n)
    -> detail::udl_formatter<char> {
  return {{s, n}};
}
}  // namespace literals
#endif  // FMT_USE_USER_DEFINED_LITERALS

template <typename Locale, FMT_ENABLE_IF(detail::is_locale<Locale>::value)>
inline auto vformat(const Locale& loc, string_view fmt, format_args args)
    -> std::string {
  return detail::vformat(loc, fmt, args);
}

template <typename Locale, typename... T,
          FMT_ENABLE_IF(detail::is_locale<Locale>::value)>
inline auto format(const Locale& loc, format_string<T...> fmt, T&&... args)
    -> std::string {
  return vformat(loc, string_view(fmt), fmt::make_format_args(args...));
}

template <typename... T, size_t SIZE, typename Allocator>
FMT_DEPRECATED auto format_to(basic_memory_buffer<char, SIZE, Allocator>& buf,
                              format_string<T...> fmt, T&&... args)
    -> appender {
  detail::vformat_to(buf, string_view(fmt), fmt::make_format_args(args...));
  return appender(buf);
}

template <typename OutputIt, typename Locale,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value&&
                            detail::is_locale<Locale>::value)>
auto vformat_to(OutputIt out, const Locale& loc, string_view fmt,
                format_args args) -> OutputIt {
  using detail::get_buffer;
  auto&& buf = get_buffer<char>(out);
  detail::vformat_to(buf, fmt, args, detail::locale_ref(loc));
  return detail::get_iterator(buf);
}

template <typename OutputIt, typename Locale, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value&&
                            detail::is_locale<Locale>::value)>
FMT_INLINE auto format_to(OutputIt out, const Locale& loc,
                          format_string<T...> fmt, T&&... args) -> OutputIt {
  return vformat_to(out, loc, fmt, fmt::make_format_args(args...));
}

FMT_MODULE_EXPORT_END
FMT_END_NAMESPACE

#ifdef FMT_DEPRECATED_INCLUDE_XCHAR
// #  include "xchar.h"
// Formatting library for C++ - optional wchar_t and exotic character support
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_XCHAR_H_
#define FMT_XCHAR_H_

#include <cwchar>
#include <tuple>

// #include "format.h"


FMT_BEGIN_NAMESPACE
namespace detail {
template <typename T>
using is_exotic_char = bool_constant<!std::is_same<T, char>::value>;
}

FMT_MODULE_EXPORT_BEGIN

using wstring_view = basic_string_view<wchar_t>;
using wformat_parse_context = basic_format_parse_context<wchar_t>;
using wformat_context = buffer_context<wchar_t>;
using wformat_args = basic_format_args<wformat_context>;
using wmemory_buffer = basic_memory_buffer<wchar_t>;

#if FMT_GCC_VERSION && FMT_GCC_VERSION < 409
// Workaround broken conversion on older gcc.
template <typename... Args> using wformat_string = wstring_view;
#else
template <typename... Args>
using wformat_string = basic_format_string<wchar_t, type_identity_t<Args>...>;
#endif

template <> struct is_char<wchar_t> : std::true_type {};
template <> struct is_char<detail::char8_type> : std::true_type {};
template <> struct is_char<char16_t> : std::true_type {};
template <> struct is_char<char32_t> : std::true_type {};

template <typename... Args>
constexpr format_arg_store<wformat_context, Args...> make_wformat_args(
    const Args&... args) {
  return {args...};
}

inline namespace literals {
constexpr auto operator"" _format(const wchar_t* s, size_t n)
    -> detail::udl_formatter<wchar_t> {
  return {{s, n}};
}

#if FMT_USE_USER_DEFINED_LITERALS && !FMT_USE_NONTYPE_TEMPLATE_PARAMETERS
constexpr detail::udl_arg<wchar_t> operator"" _a(const wchar_t* s, size_t) {
  return {s};
}
#endif
}  // namespace literals

template <typename It, typename Sentinel>
auto join(It begin, Sentinel end, wstring_view sep)
    -> join_view<It, Sentinel, wchar_t> {
  return {begin, end, sep};
}

template <typename Range>
auto join(Range&& range, wstring_view sep)
    -> join_view<detail::iterator_t<Range>, detail::sentinel_t<Range>,
                 wchar_t> {
  return join(std::begin(range), std::end(range), sep);
}

template <typename T>
auto join(std::initializer_list<T> list, wstring_view sep)
    -> join_view<const T*, const T*, wchar_t> {
  return join(std::begin(list), std::end(list), sep);
}

template <typename Char, FMT_ENABLE_IF(!std::is_same<Char, char>::value)>
auto vformat(basic_string_view<Char> format_str,
             basic_format_args<buffer_context<type_identity_t<Char>>> args)
    -> std::basic_string<Char> {
  basic_memory_buffer<Char> buffer;
  detail::vformat_to(buffer, format_str, args);
  return to_string(buffer);
}

// Pass char_t as a default template parameter instead of using
// std::basic_string<char_t<S>> to reduce the symbol size.
template <typename S, typename... Args, typename Char = char_t<S>,
          FMT_ENABLE_IF(!std::is_same<Char, char>::value)>
auto format(const S& format_str, Args&&... args) -> std::basic_string<Char> {
  const auto& vargs = fmt::make_args_checked<Args...>(format_str, args...);
  return vformat(to_string_view(format_str), vargs);
}

template <typename Locale, typename S, typename Char = char_t<S>,
          FMT_ENABLE_IF(detail::is_locale<Locale>::value&&
                            detail::is_exotic_char<Char>::value)>
inline auto vformat(
    const Locale& loc, const S& format_str,
    basic_format_args<buffer_context<type_identity_t<Char>>> args)
    -> std::basic_string<Char> {
  return detail::vformat(loc, to_string_view(format_str), args);
}

template <typename Locale, typename S, typename... Args,
          typename Char = char_t<S>,
          FMT_ENABLE_IF(detail::is_locale<Locale>::value&&
                            detail::is_exotic_char<Char>::value)>
inline auto format(const Locale& loc, const S& format_str, Args&&... args)
    -> std::basic_string<Char> {
  return detail::vformat(loc, to_string_view(format_str),
                         fmt::make_args_checked<Args...>(format_str, args...));
}

template <typename OutputIt, typename S, typename Char = char_t<S>,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, Char>::value&&
                            detail::is_exotic_char<Char>::value)>
auto vformat_to(OutputIt out, const S& format_str,
                basic_format_args<buffer_context<type_identity_t<Char>>> args)
    -> OutputIt {
  auto&& buf = detail::get_buffer<Char>(out);
  detail::vformat_to(buf, to_string_view(format_str), args);
  return detail::get_iterator(buf);
}

template <typename OutputIt, typename S, typename... Args,
          typename Char = char_t<S>,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, Char>::value&&
                            detail::is_exotic_char<Char>::value)>
inline auto format_to(OutputIt out, const S& fmt, Args&&... args) -> OutputIt {
  const auto& vargs = fmt::make_args_checked<Args...>(fmt, args...);
  return vformat_to(out, to_string_view(fmt), vargs);
}

template <typename S, typename... Args, typename Char, size_t SIZE,
          typename Allocator, FMT_ENABLE_IF(detail::is_string<S>::value)>
FMT_DEPRECATED auto format_to(basic_memory_buffer<Char, SIZE, Allocator>& buf,
                              const S& format_str, Args&&... args) ->
    typename buffer_context<Char>::iterator {
  const auto& vargs = fmt::make_args_checked<Args...>(format_str, args...);
  detail::vformat_to(buf, to_string_view(format_str), vargs, {});
  return detail::buffer_appender<Char>(buf);
}

template <typename Locale, typename S, typename OutputIt, typename... Args,
          typename Char = char_t<S>,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, Char>::value&&
                            detail::is_locale<Locale>::value&&
                                detail::is_exotic_char<Char>::value)>
inline auto vformat_to(
    OutputIt out, const Locale& loc, const S& format_str,
    basic_format_args<buffer_context<type_identity_t<Char>>> args) -> OutputIt {
  auto&& buf = detail::get_buffer<Char>(out);
  vformat_to(buf, to_string_view(format_str), args, detail::locale_ref(loc));
  return detail::get_iterator(buf);
}

template <
    typename OutputIt, typename Locale, typename S, typename... Args,
    typename Char = char_t<S>,
    bool enable = detail::is_output_iterator<OutputIt, Char>::value&&
        detail::is_locale<Locale>::value&& detail::is_exotic_char<Char>::value>
inline auto format_to(OutputIt out, const Locale& loc, const S& format_str,
                      Args&&... args) ->
    typename std::enable_if<enable, OutputIt>::type {
  const auto& vargs = fmt::make_args_checked<Args...>(format_str, args...);
  return vformat_to(out, loc, to_string_view(format_str), vargs);
}

template <typename OutputIt, typename Char, typename... Args,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, Char>::value&&
                            detail::is_exotic_char<Char>::value)>
inline auto vformat_to_n(
    OutputIt out, size_t n, basic_string_view<Char> format_str,
    basic_format_args<buffer_context<type_identity_t<Char>>> args)
    -> format_to_n_result<OutputIt> {
  detail::iterator_buffer<OutputIt, Char, detail::fixed_buffer_traits> buf(out,
                                                                           n);
  detail::vformat_to(buf, format_str, args);
  return {buf.out(), buf.count()};
}

template <typename OutputIt, typename S, typename... Args,
          typename Char = char_t<S>,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, Char>::value&&
                            detail::is_exotic_char<Char>::value)>
inline auto format_to_n(OutputIt out, size_t n, const S& fmt,
                        const Args&... args) -> format_to_n_result<OutputIt> {
  const auto& vargs = fmt::make_args_checked<Args...>(fmt, args...);
  return vformat_to_n(out, n, to_string_view(fmt), vargs);
}

template <typename S, typename... Args, typename Char = char_t<S>,
          FMT_ENABLE_IF(detail::is_exotic_char<Char>::value)>
inline auto formatted_size(const S& fmt, Args&&... args) -> size_t {
  detail::counting_buffer<Char> buf;
  const auto& vargs = fmt::make_args_checked<Args...>(fmt, args...);
  detail::vformat_to(buf, to_string_view(fmt), vargs);
  return buf.count();
}

inline void vprint(std::FILE* f, wstring_view fmt, wformat_args args) {
  wmemory_buffer buffer;
  detail::vformat_to(buffer, fmt, args);
  buffer.push_back(L'\0');
  if (std::fputws(buffer.data(), f) == -1)
    FMT_THROW(system_error(errno, FMT_STRING("cannot write to file")));
}

inline void vprint(wstring_view fmt, wformat_args args) {
  vprint(stdout, fmt, args);
}

template <typename... T>
void print(std::FILE* f, wformat_string<T...> fmt, T&&... args) {
  return vprint(f, wstring_view(fmt), fmt::make_wformat_args(args...));
}

template <typename... T> void print(wformat_string<T...> fmt, T&&... args) {
  return vprint(wstring_view(fmt), fmt::make_wformat_args(args...));
}

/**
  Converts *value* to ``std::wstring`` using the default format for type *T*.
 */
template <typename T> inline auto to_wstring(const T& value) -> std::wstring {
  return format(FMT_STRING(L"{}"), value);
}
FMT_MODULE_EXPORT_END
FMT_END_NAMESPACE

#endif  // FMT_XCHAR_H_

#endif

#ifdef FMT_HEADER_ONLY
#  define FMT_FUNC inline
// #  include "format-inl.h"
// Formatting library for C++ - implementation
//
// Copyright (c) 2012 - 2016, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_FORMAT_INL_H_
#define FMT_FORMAT_INL_H_

#include <algorithm>
#include <cctype>
#include <cerrno>  // errno
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstring>  // std::memmove
#include <cwchar>
#include <exception>

#ifndef FMT_STATIC_THOUSANDS_SEPARATOR
#  include <locale>
#endif

#ifdef _WIN32
#  include <io.h>  // _isatty
#endif

// #include "format.h"


FMT_BEGIN_NAMESPACE
namespace detail {

FMT_FUNC void assert_fail(const char* file, int line, const char* message) {
  // Use unchecked std::fprintf to avoid triggering another assertion when
  // writing to stderr fails
  std::fprintf(stderr, "%s:%d: assertion failed: %s", file, line, message);
  // Chosen instead of std::abort to satisfy Clang in CUDA mode during device
  // code pass.
  std::terminate();
}

FMT_FUNC void throw_format_error(const char* message) {
  FMT_THROW(format_error(message));
}

#ifndef _MSC_VER
#  define FMT_SNPRINTF snprintf
#else  // _MSC_VER
inline int fmt_snprintf(char* buffer, size_t size, const char* format, ...) {
  va_list args;
  va_start(args, format);
  int result = vsnprintf_s(buffer, size, _TRUNCATE, format, args);
  va_end(args);
  return result;
}
#  define FMT_SNPRINTF fmt_snprintf
#endif  // _MSC_VER

FMT_FUNC void format_error_code(detail::buffer<char>& out, int error_code,
                                string_view message) FMT_NOEXCEPT {
  // Report error code making sure that the output fits into
  // inline_buffer_size to avoid dynamic memory allocation and potential
  // bad_alloc.
  out.try_resize(0);
  static const char SEP[] = ": ";
  static const char ERROR_STR[] = "error ";
  // Subtract 2 to account for terminating null characters in SEP and ERROR_STR.
  size_t error_code_size = sizeof(SEP) + sizeof(ERROR_STR) - 2;
  auto abs_value = static_cast<uint32_or_64_or_128_t<int>>(error_code);
  if (detail::is_negative(error_code)) {
    abs_value = 0 - abs_value;
    ++error_code_size;
  }
  error_code_size += detail::to_unsigned(detail::count_digits(abs_value));
  auto it = buffer_appender<char>(out);
  if (message.size() <= inline_buffer_size - error_code_size)
    format_to(it, FMT_STRING("{}{}"), message, SEP);
  format_to(it, FMT_STRING("{}{}"), ERROR_STR, error_code);
  FMT_ASSERT(out.size() <= inline_buffer_size, "");
}

FMT_FUNC void report_error(format_func func, int error_code,
                           const char* message) FMT_NOEXCEPT {
  memory_buffer full_message;
  func(full_message, error_code, message);
  // Don't use fwrite_fully because the latter may throw.
  if (std::fwrite(full_message.data(), full_message.size(), 1, stderr) > 0)
    std::fputc('\n', stderr);
}

// A wrapper around fwrite that throws on error.
inline void fwrite_fully(const void* ptr, size_t size, size_t count,
                         FILE* stream) {
  size_t written = std::fwrite(ptr, size, count, stream);
  if (written < count) FMT_THROW(system_error(errno, "cannot write to file"));
}

#ifndef FMT_STATIC_THOUSANDS_SEPARATOR
template <typename Locale>
locale_ref::locale_ref(const Locale& loc) : locale_(&loc) {
  static_assert(std::is_same<Locale, std::locale>::value, "");
}

template <typename Locale> Locale locale_ref::get() const {
  static_assert(std::is_same<Locale, std::locale>::value, "");
  return locale_ ? *static_cast<const std::locale*>(locale_) : std::locale();
}

template <typename Char>
FMT_FUNC auto thousands_sep_impl(locale_ref loc) -> thousands_sep_result<Char> {
  auto& facet = std::use_facet<std::numpunct<Char>>(loc.get<std::locale>());
  auto grouping = facet.grouping();
  auto thousands_sep = grouping.empty() ? Char() : facet.thousands_sep();
  return {std::move(grouping), thousands_sep};
}
template <typename Char> FMT_FUNC Char decimal_point_impl(locale_ref loc) {
  return std::use_facet<std::numpunct<Char>>(loc.get<std::locale>())
      .decimal_point();
}
#else
template <typename Char>
FMT_FUNC auto thousands_sep_impl(locale_ref) -> thousands_sep_result<Char> {
  return {"\03", FMT_STATIC_THOUSANDS_SEPARATOR};
}
template <typename Char> FMT_FUNC Char decimal_point_impl(locale_ref) {
  return '.';
}
#endif
}  // namespace detail

#if !FMT_MSC_VER
FMT_API FMT_FUNC format_error::~format_error() FMT_NOEXCEPT = default;
#endif

FMT_FUNC std::system_error vsystem_error(int error_code, string_view format_str,
                                         format_args args) {
  auto ec = std::error_code(error_code, std::generic_category());
  return std::system_error(ec, vformat(format_str, args));
}

namespace detail {

template <> FMT_FUNC int count_digits<4>(detail::fallback_uintptr n) {
  // fallback_uintptr is always stored in little endian.
  int i = static_cast<int>(sizeof(void*)) - 1;
  while (i > 0 && n.value[i] == 0) --i;
  auto char_digits = std::numeric_limits<unsigned char>::digits / 4;
  return i >= 0 ? i * char_digits + count_digits<4, unsigned>(n.value[i]) : 1;
}

// log10(2) = 0x0.4d104d427de7fbcc...
static constexpr uint64_t log10_2_significand = 0x4d104d427de7fbcc;

template <typename T = void> struct basic_impl_data {
  // Normalized 64-bit significands of pow(10, k), for k = -348, -340, ..., 340.
  // These are generated by support/compute-powers.py.
  static constexpr uint64_t pow10_significands[87] = {
      0xfa8fd5a0081c0288, 0xbaaee17fa23ebf76, 0x8b16fb203055ac76,
      0xcf42894a5dce35ea, 0x9a6bb0aa55653b2d, 0xe61acf033d1a45df,
      0xab70fe17c79ac6ca, 0xff77b1fcbebcdc4f, 0xbe5691ef416bd60c,
      0x8dd01fad907ffc3c, 0xd3515c2831559a83, 0x9d71ac8fada6c9b5,
      0xea9c227723ee8bcb, 0xaecc49914078536d, 0x823c12795db6ce57,
      0xc21094364dfb5637, 0x9096ea6f3848984f, 0xd77485cb25823ac7,
      0xa086cfcd97bf97f4, 0xef340a98172aace5, 0xb23867fb2a35b28e,
      0x84c8d4dfd2c63f3b, 0xc5dd44271ad3cdba, 0x936b9fcebb25c996,
      0xdbac6c247d62a584, 0xa3ab66580d5fdaf6, 0xf3e2f893dec3f126,
      0xb5b5ada8aaff80b8, 0x87625f056c7c4a8b, 0xc9bcff6034c13053,
      0x964e858c91ba2655, 0xdff9772470297ebd, 0xa6dfbd9fb8e5b88f,
      0xf8a95fcf88747d94, 0xb94470938fa89bcf, 0x8a08f0f8bf0f156b,
      0xcdb02555653131b6, 0x993fe2c6d07b7fac, 0xe45c10c42a2b3b06,
      0xaa242499697392d3, 0xfd87b5f28300ca0e, 0xbce5086492111aeb,
      0x8cbccc096f5088cc, 0xd1b71758e219652c, 0x9c40000000000000,
      0xe8d4a51000000000, 0xad78ebc5ac620000, 0x813f3978f8940984,
      0xc097ce7bc90715b3, 0x8f7e32ce7bea5c70, 0xd5d238a4abe98068,
      0x9f4f2726179a2245, 0xed63a231d4c4fb27, 0xb0de65388cc8ada8,
      0x83c7088e1aab65db, 0xc45d1df942711d9a, 0x924d692ca61be758,
      0xda01ee641a708dea, 0xa26da3999aef774a, 0xf209787bb47d6b85,
      0xb454e4a179dd1877, 0x865b86925b9bc5c2, 0xc83553c5c8965d3d,
      0x952ab45cfa97a0b3, 0xde469fbd99a05fe3, 0xa59bc234db398c25,
      0xf6c69a72a3989f5c, 0xb7dcbf5354e9bece, 0x88fcf317f22241e2,
      0xcc20ce9bd35c78a5, 0x98165af37b2153df, 0xe2a0b5dc971f303a,
      0xa8d9d1535ce3b396, 0xfb9b7cd9a4a7443c, 0xbb764c4ca7a44410,
      0x8bab8eefb6409c1a, 0xd01fef10a657842c, 0x9b10a4e5e9913129,
      0xe7109bfba19c0c9d, 0xac2820d9623bf429, 0x80444b5e7aa7cf85,
      0xbf21e44003acdd2d, 0x8e679c2f5e44ff8f, 0xd433179d9c8cb841,
      0x9e19db92b4e31ba9, 0xeb96bf6ebadf77d9, 0xaf87023b9bf0ee6b,
  };

#if FMT_GCC_VERSION && FMT_GCC_VERSION < 409
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wnarrowing"
#endif
  // Binary exponents of pow(10, k), for k = -348, -340, ..., 340, corresponding
  // to significands above.
  static constexpr int16_t pow10_exponents[87] = {
      -1220, -1193, -1166, -1140, -1113, -1087, -1060, -1034, -1007, -980, -954,
      -927,  -901,  -874,  -847,  -821,  -794,  -768,  -741,  -715,  -688, -661,
      -635,  -608,  -582,  -555,  -529,  -502,  -475,  -449,  -422,  -396, -369,
      -343,  -316,  -289,  -263,  -236,  -210,  -183,  -157,  -130,  -103, -77,
      -50,   -24,   3,     30,    56,    83,    109,   136,   162,   189,  216,
      242,   269,   295,   322,   348,   375,   402,   428,   455,   481,  508,
      534,   561,   588,   614,   641,   667,   694,   720,   747,   774,  800,
      827,   853,   880,   907,   933,   960,   986,   1013,  1039,  1066};
#if FMT_GCC_VERSION && FMT_GCC_VERSION < 409
#  pragma GCC diagnostic pop
#endif

  static constexpr uint64_t power_of_10_64[20] = {
      1, FMT_POWERS_OF_10(1ULL), FMT_POWERS_OF_10(1000000000ULL),
      10000000000000000000ULL};
};

// This is a struct rather than an alias to avoid shadowing warnings in gcc.
struct impl_data : basic_impl_data<> {};

#if __cplusplus < 201703L
template <typename T>
constexpr uint64_t basic_impl_data<T>::pow10_significands[];
template <typename T> constexpr int16_t basic_impl_data<T>::pow10_exponents[];
template <typename T> constexpr uint64_t basic_impl_data<T>::power_of_10_64[];
#endif

template <typename T> struct bits {
  static FMT_CONSTEXPR_DECL const int value =
      static_cast<int>(sizeof(T) * std::numeric_limits<unsigned char>::digits);
};

// Returns the number of significand bits in Float excluding the implicit bit.
template <typename Float> constexpr int num_significand_bits() {
  // Subtract 1 to account for an implicit most significant bit in the
  // normalized form.
  return std::numeric_limits<Float>::digits - 1;
}

// A floating-point number f * pow(2, e).
struct fp {
  uint64_t f;
  int e;

  static constexpr const int num_significand_bits = bits<decltype(f)>::value;

  constexpr fp() : f(0), e(0) {}
  constexpr fp(uint64_t f_val, int e_val) : f(f_val), e(e_val) {}

  // Constructs fp from an IEEE754 floating-point number. It is a template to
  // prevent compile errors on systems where n is not IEEE754.
  template <typename Float> explicit FMT_CONSTEXPR fp(Float n) { assign(n); }

  template <typename Float>
  using is_supported = bool_constant<sizeof(Float) == sizeof(uint64_t) ||
                                     sizeof(Float) == sizeof(uint32_t)>;

  // Assigns d to this and return true iff predecessor is closer than successor.
  template <typename Float, FMT_ENABLE_IF(is_supported<Float>::value)>
  FMT_CONSTEXPR bool assign(Float n) {
    // Assume float is in the format [sign][exponent][significand].
    const int num_float_significand_bits =
        detail::num_significand_bits<Float>();
    const uint64_t implicit_bit = 1ULL << num_float_significand_bits;
    const uint64_t significand_mask = implicit_bit - 1;
    constexpr bool is_double = sizeof(Float) == sizeof(uint64_t);
    auto u = bit_cast<conditional_t<is_double, uint64_t, uint32_t>>(n);
    f = u & significand_mask;
    const uint64_t exponent_mask = (~0ULL >> 1) & ~significand_mask;
    int biased_e =
        static_cast<int>((u & exponent_mask) >> num_float_significand_bits);
    // The predecessor is closer if n is a normalized power of 2 (f == 0) other
    // than the smallest normalized number (biased_e > 1).
    bool is_predecessor_closer = f == 0 && biased_e > 1;
    if (biased_e != 0)
      f += implicit_bit;
    else
      biased_e = 1;  // Subnormals use biased exponent 1 (min exponent).
    const int exponent_bias = std::numeric_limits<Float>::max_exponent - 1;
    e = biased_e - exponent_bias - num_float_significand_bits;
    return is_predecessor_closer;
  }

  template <typename Float, FMT_ENABLE_IF(!is_supported<Float>::value)>
  bool assign(Float) {
    FMT_ASSERT(false, "");
    return false;
  }
};

// Normalizes the value converted from double and multiplied by (1 << SHIFT).
template <int SHIFT = 0> FMT_CONSTEXPR fp normalize(fp value) {
  // Handle subnormals.
  const uint64_t implicit_bit = 1ULL << num_significand_bits<double>();
  const auto shifted_implicit_bit = implicit_bit << SHIFT;
  while ((value.f & shifted_implicit_bit) == 0) {
    value.f <<= 1;
    --value.e;
  }
  // Subtract 1 to account for hidden bit.
  const auto offset =
      fp::num_significand_bits - num_significand_bits<double>() - SHIFT - 1;
  value.f <<= offset;
  value.e -= offset;
  return value;
}

inline bool operator==(fp x, fp y) { return x.f == y.f && x.e == y.e; }

// Computes lhs * rhs / pow(2, 64) rounded to nearest with half-up tie breaking.
FMT_CONSTEXPR inline uint64_t multiply(uint64_t lhs, uint64_t rhs) {
#if FMT_USE_INT128
  auto product = static_cast<__uint128_t>(lhs) * rhs;
  auto f = static_cast<uint64_t>(product >> 64);
  return (static_cast<uint64_t>(product) & (1ULL << 63)) != 0 ? f + 1 : f;
#else
  // Multiply 32-bit parts of significands.
  uint64_t mask = (1ULL << 32) - 1;
  uint64_t a = lhs >> 32, b = lhs & mask;
  uint64_t c = rhs >> 32, d = rhs & mask;
  uint64_t ac = a * c, bc = b * c, ad = a * d, bd = b * d;
  // Compute mid 64-bit of result and round.
  uint64_t mid = (bd >> 32) + (ad & mask) + (bc & mask) + (1U << 31);
  return ac + (ad >> 32) + (bc >> 32) + (mid >> 32);
#endif
}

FMT_CONSTEXPR inline fp operator*(fp x, fp y) {
  return {multiply(x.f, y.f), x.e + y.e + 64};
}

// Returns a cached power of 10 `c_k = c_k.f * pow(2, c_k.e)` such that its
// (binary) exponent satisfies `min_exponent <= c_k.e <= min_exponent + 28`.
FMT_CONSTEXPR inline fp get_cached_power(int min_exponent,
                                         int& pow10_exponent) {
  const int shift = 32;
  const auto significand = static_cast<int64_t>(log10_2_significand);
  int index = static_cast<int>(
      ((min_exponent + fp::num_significand_bits - 1) * (significand >> shift) +
       ((int64_t(1) << shift) - 1))  // ceil
      >> 32                          // arithmetic shift
  );
  // Decimal exponent of the first (smallest) cached power of 10.
  const int first_dec_exp = -348;
  // Difference between 2 consecutive decimal exponents in cached powers of 10.
  const int dec_exp_step = 8;
  index = (index - first_dec_exp - 1) / dec_exp_step + 1;
  pow10_exponent = first_dec_exp + index * dec_exp_step;
  return {impl_data::pow10_significands[index],
          impl_data::pow10_exponents[index]};
}

// A simple accumulator to hold the sums of terms in bigint::square if uint128_t
// is not available.
struct accumulator {
  uint64_t lower;
  uint64_t upper;

  constexpr accumulator() : lower(0), upper(0) {}
  constexpr explicit operator uint32_t() const {
    return static_cast<uint32_t>(lower);
  }

  FMT_CONSTEXPR void operator+=(uint64_t n) {
    lower += n;
    if (lower < n) ++upper;
  }
  FMT_CONSTEXPR void operator>>=(int shift) {
    FMT_ASSERT(shift == 32, "");
    (void)shift;
    lower = (upper << 32) | (lower >> 32);
    upper >>= 32;
  }
};

class bigint {
 private:
  // A bigint is stored as an array of bigits (big digits), with bigit at index
  // 0 being the least significant one.
  using bigit = uint32_t;
  using double_bigit = uint64_t;
  enum { bigits_capacity = 32 };
  basic_memory_buffer<bigit, bigits_capacity> bigits_;
  int exp_;

  FMT_CONSTEXPR20 bigit operator[](int index) const {
    return bigits_[to_unsigned(index)];
  }
  FMT_CONSTEXPR20 bigit& operator[](int index) {
    return bigits_[to_unsigned(index)];
  }

  static FMT_CONSTEXPR_DECL const int bigit_bits = bits<bigit>::value;

  friend struct formatter<bigint>;

  FMT_CONSTEXPR20 void subtract_bigits(int index, bigit other, bigit& borrow) {
    auto result = static_cast<double_bigit>((*this)[index]) - other - borrow;
    (*this)[index] = static_cast<bigit>(result);
    borrow = static_cast<bigit>(result >> (bigit_bits * 2 - 1));
  }

  FMT_CONSTEXPR20 void remove_leading_zeros() {
    int num_bigits = static_cast<int>(bigits_.size()) - 1;
    while (num_bigits > 0 && (*this)[num_bigits] == 0) --num_bigits;
    bigits_.resize(to_unsigned(num_bigits + 1));
  }

  // Computes *this -= other assuming aligned bigints and *this >= other.
  FMT_CONSTEXPR20 void subtract_aligned(const bigint& other) {
    FMT_ASSERT(other.exp_ >= exp_, "unaligned bigints");
    FMT_ASSERT(compare(*this, other) >= 0, "");
    bigit borrow = 0;
    int i = other.exp_ - exp_;
    for (size_t j = 0, n = other.bigits_.size(); j != n; ++i, ++j)
      subtract_bigits(i, other.bigits_[j], borrow);
    while (borrow > 0) subtract_bigits(i, 0, borrow);
    remove_leading_zeros();
  }

  FMT_CONSTEXPR20 void multiply(uint32_t value) {
    const double_bigit wide_value = value;
    bigit carry = 0;
    for (size_t i = 0, n = bigits_.size(); i < n; ++i) {
      double_bigit result = bigits_[i] * wide_value + carry;
      bigits_[i] = static_cast<bigit>(result);
      carry = static_cast<bigit>(result >> bigit_bits);
    }
    if (carry != 0) bigits_.push_back(carry);
  }

  FMT_CONSTEXPR20 void multiply(uint64_t value) {
    const bigit mask = ~bigit(0);
    const double_bigit lower = value & mask;
    const double_bigit upper = value >> bigit_bits;
    double_bigit carry = 0;
    for (size_t i = 0, n = bigits_.size(); i < n; ++i) {
      double_bigit result = bigits_[i] * lower + (carry & mask);
      carry =
          bigits_[i] * upper + (result >> bigit_bits) + (carry >> bigit_bits);
      bigits_[i] = static_cast<bigit>(result);
    }
    while (carry != 0) {
      bigits_.push_back(carry & mask);
      carry >>= bigit_bits;
    }
  }

 public:
  FMT_CONSTEXPR20 bigint() : exp_(0) {}
  explicit bigint(uint64_t n) { assign(n); }
  FMT_CONSTEXPR20 ~bigint() {
    FMT_ASSERT(bigits_.capacity() <= bigits_capacity, "");
  }

  bigint(const bigint&) = delete;
  void operator=(const bigint&) = delete;

  FMT_CONSTEXPR20 void assign(const bigint& other) {
    auto size = other.bigits_.size();
    bigits_.resize(size);
    auto data = other.bigits_.data();
    std::copy(data, data + size, make_checked(bigits_.data(), size));
    exp_ = other.exp_;
  }

  FMT_CONSTEXPR20 void assign(uint64_t n) {
    size_t num_bigits = 0;
    do {
      bigits_[num_bigits++] = n & ~bigit(0);
      n >>= bigit_bits;
    } while (n != 0);
    bigits_.resize(num_bigits);
    exp_ = 0;
  }

  FMT_CONSTEXPR20 int num_bigits() const {
    return static_cast<int>(bigits_.size()) + exp_;
  }

  FMT_NOINLINE FMT_CONSTEXPR20 bigint& operator<<=(int shift) {
    FMT_ASSERT(shift >= 0, "");
    exp_ += shift / bigit_bits;
    shift %= bigit_bits;
    if (shift == 0) return *this;
    bigit carry = 0;
    for (size_t i = 0, n = bigits_.size(); i < n; ++i) {
      bigit c = bigits_[i] >> (bigit_bits - shift);
      bigits_[i] = (bigits_[i] << shift) + carry;
      carry = c;
    }
    if (carry != 0) bigits_.push_back(carry);
    return *this;
  }

  template <typename Int> FMT_CONSTEXPR20 bigint& operator*=(Int value) {
    FMT_ASSERT(value > 0, "");
    multiply(uint32_or_64_or_128_t<Int>(value));
    return *this;
  }

  friend FMT_CONSTEXPR20 int compare(const bigint& lhs, const bigint& rhs) {
    int num_lhs_bigits = lhs.num_bigits(), num_rhs_bigits = rhs.num_bigits();
    if (num_lhs_bigits != num_rhs_bigits)
      return num_lhs_bigits > num_rhs_bigits ? 1 : -1;
    int i = static_cast<int>(lhs.bigits_.size()) - 1;
    int j = static_cast<int>(rhs.bigits_.size()) - 1;
    int end = i - j;
    if (end < 0) end = 0;
    for (; i >= end; --i, --j) {
      bigit lhs_bigit = lhs[i], rhs_bigit = rhs[j];
      if (lhs_bigit != rhs_bigit) return lhs_bigit > rhs_bigit ? 1 : -1;
    }
    if (i != j) return i > j ? 1 : -1;
    return 0;
  }

  // Returns compare(lhs1 + lhs2, rhs).
  friend FMT_CONSTEXPR20 int add_compare(const bigint& lhs1, const bigint& lhs2,
                                         const bigint& rhs) {
    int max_lhs_bigits = (std::max)(lhs1.num_bigits(), lhs2.num_bigits());
    int num_rhs_bigits = rhs.num_bigits();
    if (max_lhs_bigits + 1 < num_rhs_bigits) return -1;
    if (max_lhs_bigits > num_rhs_bigits) return 1;
    auto get_bigit = [](const bigint& n, int i) -> bigit {
      return i >= n.exp_ && i < n.num_bigits() ? n[i - n.exp_] : 0;
    };
    double_bigit borrow = 0;
    int min_exp = (std::min)((std::min)(lhs1.exp_, lhs2.exp_), rhs.exp_);
    for (int i = num_rhs_bigits - 1; i >= min_exp; --i) {
      double_bigit sum =
          static_cast<double_bigit>(get_bigit(lhs1, i)) + get_bigit(lhs2, i);
      bigit rhs_bigit = get_bigit(rhs, i);
      if (sum > rhs_bigit + borrow) return 1;
      borrow = rhs_bigit + borrow - sum;
      if (borrow > 1) return -1;
      borrow <<= bigit_bits;
    }
    return borrow != 0 ? -1 : 0;
  }

  // Assigns pow(10, exp) to this bigint.
  FMT_CONSTEXPR20 void assign_pow10(int exp) {
    FMT_ASSERT(exp >= 0, "");
    if (exp == 0) return assign(1);
    // Find the top bit.
    int bitmask = 1;
    while (exp >= bitmask) bitmask <<= 1;
    bitmask >>= 1;
    // pow(10, exp) = pow(5, exp) * pow(2, exp). First compute pow(5, exp) by
    // repeated squaring and multiplication.
    assign(5);
    bitmask >>= 1;
    while (bitmask != 0) {
      square();
      if ((exp & bitmask) != 0) *this *= 5;
      bitmask >>= 1;
    }
    *this <<= exp;  // Multiply by pow(2, exp) by shifting.
  }

  FMT_CONSTEXPR20 void square() {
    int num_bigits = static_cast<int>(bigits_.size());
    int num_result_bigits = 2 * num_bigits;
    basic_memory_buffer<bigit, bigits_capacity> n(std::move(bigits_));
    bigits_.resize(to_unsigned(num_result_bigits));
    using accumulator_t = conditional_t<FMT_USE_INT128, uint128_t, accumulator>;
    auto sum = accumulator_t();
    for (int bigit_index = 0; bigit_index < num_bigits; ++bigit_index) {
      // Compute bigit at position bigit_index of the result by adding
      // cross-product terms n[i] * n[j] such that i + j == bigit_index.
      for (int i = 0, j = bigit_index; j >= 0; ++i, --j) {
        // Most terms are multiplied twice which can be optimized in the future.
        sum += static_cast<double_bigit>(n[i]) * n[j];
      }
      (*this)[bigit_index] = static_cast<bigit>(sum);
      sum >>= bits<bigit>::value;  // Compute the carry.
    }
    // Do the same for the top half.
    for (int bigit_index = num_bigits; bigit_index < num_result_bigits;
         ++bigit_index) {
      for (int j = num_bigits - 1, i = bigit_index - j; i < num_bigits;)
        sum += static_cast<double_bigit>(n[i++]) * n[j--];
      (*this)[bigit_index] = static_cast<bigit>(sum);
      sum >>= bits<bigit>::value;
    }
    remove_leading_zeros();
    exp_ *= 2;
  }

  // If this bigint has a bigger exponent than other, adds trailing zero to make
  // exponents equal. This simplifies some operations such as subtraction.
  FMT_CONSTEXPR20 void align(const bigint& other) {
    int exp_difference = exp_ - other.exp_;
    if (exp_difference <= 0) return;
    int num_bigits = static_cast<int>(bigits_.size());
    bigits_.resize(to_unsigned(num_bigits + exp_difference));
    for (int i = num_bigits - 1, j = i + exp_difference; i >= 0; --i, --j)
      bigits_[j] = bigits_[i];
    std::uninitialized_fill_n(bigits_.data(), exp_difference, 0);
    exp_ -= exp_difference;
  }

  // Divides this bignum by divisor, assigning the remainder to this and
  // returning the quotient.
  FMT_CONSTEXPR20 int divmod_assign(const bigint& divisor) {
    FMT_ASSERT(this != &divisor, "");
    if (compare(*this, divisor) < 0) return 0;
    FMT_ASSERT(divisor.bigits_[divisor.bigits_.size() - 1u] != 0, "");
    align(divisor);
    int quotient = 0;
    do {
      subtract_aligned(divisor);
      ++quotient;
    } while (compare(*this, divisor) >= 0);
    return quotient;
  }
};

enum class round_direction { unknown, up, down };

// Given the divisor (normally a power of 10), the remainder = v % divisor for
// some number v and the error, returns whether v should be rounded up, down, or
// whether the rounding direction can't be determined due to error.
// error should be less than divisor / 2.
FMT_CONSTEXPR inline round_direction get_round_direction(uint64_t divisor,
                                                         uint64_t remainder,
                                                         uint64_t error) {
  FMT_ASSERT(remainder < divisor, "");  // divisor - remainder won't overflow.
  FMT_ASSERT(error < divisor, "");      // divisor - error won't overflow.
  FMT_ASSERT(error < divisor - error, "");  // error * 2 won't overflow.
  // Round down if (remainder + error) * 2 <= divisor.
  if (remainder <= divisor - remainder && error * 2 <= divisor - remainder * 2)
    return round_direction::down;
  // Round up if (remainder - error) * 2 >= divisor.
  if (remainder >= error &&
      remainder - error >= divisor - (remainder - error)) {
    return round_direction::up;
  }
  return round_direction::unknown;
}

namespace digits {
enum result {
  more,  // Generate more digits.
  done,  // Done generating digits.
  error  // Digit generation cancelled due to an error.
};
}

struct gen_digits_handler {
  char* buf;
  int size;
  int precision;
  int exp10;
  bool fixed;

  FMT_CONSTEXPR digits::result on_digit(char digit, uint64_t divisor,
                                        uint64_t remainder, uint64_t error,
                                        bool integral) {
    FMT_ASSERT(remainder < divisor, "");
    buf[size++] = digit;
    if (!integral && error >= remainder) return digits::error;
    if (size < precision) return digits::more;
    if (!integral) {
      // Check if error * 2 < divisor with overflow prevention.
      // The check is not needed for the integral part because error = 1
      // and divisor > (1 << 32) there.
      if (error >= divisor || error >= divisor - error) return digits::error;
    } else {
      FMT_ASSERT(error == 1 && divisor > 2, "");
    }
    auto dir = get_round_direction(divisor, remainder, error);
    if (dir != round_direction::up)
      return dir == round_direction::down ? digits::done : digits::error;
    ++buf[size - 1];
    for (int i = size - 1; i > 0 && buf[i] > '9'; --i) {
      buf[i] = '0';
      ++buf[i - 1];
    }
    if (buf[0] > '9') {
      buf[0] = '1';
      if (fixed)
        buf[size++] = '0';
      else
        ++exp10;
    }
    return digits::done;
  }
};

// Generates output using the Grisu digit-gen algorithm.
// error: the size of the region (lower, upper) outside of which numbers
// definitely do not round to value (Delta in Grisu3).
FMT_INLINE FMT_CONSTEXPR20 digits::result grisu_gen_digits(
    fp value, uint64_t error, int& exp, gen_digits_handler& handler) {
  const fp one(1ULL << -value.e, value.e);
  // The integral part of scaled value (p1 in Grisu) = value / one. It cannot be
  // zero because it contains a product of two 64-bit numbers with MSB set (due
  // to normalization) - 1, shifted right by at most 60 bits.
  auto integral = static_cast<uint32_t>(value.f >> -one.e);
  FMT_ASSERT(integral != 0, "");
  FMT_ASSERT(integral == value.f >> -one.e, "");
  // The fractional part of scaled value (p2 in Grisu) c = value % one.
  uint64_t fractional = value.f & (one.f - 1);
  exp = count_digits(integral);  // kappa in Grisu.
  // Non-fixed formats require at least one digit and no precision adjustment.
  if (handler.fixed) {
    // Adjust fixed precision by exponent because it is relative to decimal
    // point.
    int precision_offset = exp + handler.exp10;
    if (precision_offset > 0 &&
        handler.precision > max_value<int>() - precision_offset) {
      FMT_THROW(format_error("number is too big"));
    }
    handler.precision += precision_offset;
    // Check if precision is satisfied just by leading zeros, e.g.
    // format("{:.2f}", 0.001) gives "0.00" without generating any digits.
    if (handler.precision <= 0) {
      if (handler.precision < 0) return digits::done;
      // Divide by 10 to prevent overflow.
      uint64_t divisor = impl_data::power_of_10_64[exp - 1] << -one.e;
      auto dir = get_round_direction(divisor, value.f / 10, error * 10);
      if (dir == round_direction::unknown) return digits::error;
      handler.buf[handler.size++] = dir == round_direction::up ? '1' : '0';
      return digits::done;
    }
  }
  // Generate digits for the integral part. This can produce up to 10 digits.
  do {
    uint32_t digit = 0;
    auto divmod_integral = [&](uint32_t divisor) {
      digit = integral / divisor;
      integral %= divisor;
    };
    // This optimization by Milo Yip reduces the number of integer divisions by
    // one per iteration.
    switch (exp) {
    case 10:
      divmod_integral(1000000000);
      break;
    case 9:
      divmod_integral(100000000);
      break;
    case 8:
      divmod_integral(10000000);
      break;
    case 7:
      divmod_integral(1000000);
      break;
    case 6:
      divmod_integral(100000);
      break;
    case 5:
      divmod_integral(10000);
      break;
    case 4:
      divmod_integral(1000);
      break;
    case 3:
      divmod_integral(100);
      break;
    case 2:
      divmod_integral(10);
      break;
    case 1:
      digit = integral;
      integral = 0;
      break;
    default:
      FMT_ASSERT(false, "invalid number of digits");
    }
    --exp;
    auto remainder = (static_cast<uint64_t>(integral) << -one.e) + fractional;
    auto result = handler.on_digit(static_cast<char>('0' + digit),
                                   impl_data::power_of_10_64[exp] << -one.e,
                                   remainder, error, true);
    if (result != digits::more) return result;
  } while (exp > 0);
  // Generate digits for the fractional part.
  for (;;) {
    fractional *= 10;
    error *= 10;
    char digit = static_cast<char>('0' + (fractional >> -one.e));
    fractional &= one.f - 1;
    --exp;
    auto result = handler.on_digit(digit, one.f, fractional, error, false);
    if (result != digits::more) return result;
  }
}

// A 128-bit integer type used internally,
struct uint128_wrapper {
  uint128_wrapper() = default;

#if FMT_USE_INT128
  uint128_t internal_;

  constexpr uint128_wrapper(uint64_t high, uint64_t low) FMT_NOEXCEPT
      : internal_{static_cast<uint128_t>(low) |
                  (static_cast<uint128_t>(high) << 64)} {}

  constexpr uint128_wrapper(uint128_t u) : internal_{u} {}

  constexpr uint64_t high() const FMT_NOEXCEPT {
    return uint64_t(internal_ >> 64);
  }
  constexpr uint64_t low() const FMT_NOEXCEPT { return uint64_t(internal_); }

  uint128_wrapper& operator+=(uint64_t n) FMT_NOEXCEPT {
    internal_ += n;
    return *this;
  }
#else
  uint64_t high_;
  uint64_t low_;

  constexpr uint128_wrapper(uint64_t high, uint64_t low) FMT_NOEXCEPT
      : high_{high},
        low_{low} {}

  constexpr uint64_t high() const FMT_NOEXCEPT { return high_; }
  constexpr uint64_t low() const FMT_NOEXCEPT { return low_; }

  uint128_wrapper& operator+=(uint64_t n) FMT_NOEXCEPT {
#  if defined(_MSC_VER) && defined(_M_X64)
    unsigned char carry = _addcarry_u64(0, low_, n, &low_);
    _addcarry_u64(carry, high_, 0, &high_);
    return *this;
#  else
    uint64_t sum = low_ + n;
    high_ += (sum < low_ ? 1 : 0);
    low_ = sum;
    return *this;
#  endif
  }
#endif
};

// Implementation of Dragonbox algorithm: https://github.com/jk-jeon/dragonbox.
namespace dragonbox {
// Computes 128-bit result of multiplication of two 64-bit unsigned integers.
inline uint128_wrapper umul128(uint64_t x, uint64_t y) FMT_NOEXCEPT {
#if FMT_USE_INT128
  return static_cast<uint128_t>(x) * static_cast<uint128_t>(y);
#elif defined(_MSC_VER) && defined(_M_X64)
  uint128_wrapper result;
  result.low_ = _umul128(x, y, &result.high_);
  return result;
#else
  const uint64_t mask = (uint64_t(1) << 32) - uint64_t(1);

  uint64_t a = x >> 32;
  uint64_t b = x & mask;
  uint64_t c = y >> 32;
  uint64_t d = y & mask;

  uint64_t ac = a * c;
  uint64_t bc = b * c;
  uint64_t ad = a * d;
  uint64_t bd = b * d;

  uint64_t intermediate = (bd >> 32) + (ad & mask) + (bc & mask);

  return {ac + (intermediate >> 32) + (ad >> 32) + (bc >> 32),
          (intermediate << 32) + (bd & mask)};
#endif
}

// Computes upper 64 bits of multiplication of two 64-bit unsigned integers.
inline uint64_t umul128_upper64(uint64_t x, uint64_t y) FMT_NOEXCEPT {
#if FMT_USE_INT128
  auto p = static_cast<uint128_t>(x) * static_cast<uint128_t>(y);
  return static_cast<uint64_t>(p >> 64);
#elif defined(_MSC_VER) && defined(_M_X64)
  return __umulh(x, y);
#else
  return umul128(x, y).high();
#endif
}

// Computes upper 64 bits of multiplication of a 64-bit unsigned integer and a
// 128-bit unsigned integer.
inline uint64_t umul192_upper64(uint64_t x, uint128_wrapper y) FMT_NOEXCEPT {
  uint128_wrapper g0 = umul128(x, y.high());
  g0 += umul128_upper64(x, y.low());
  return g0.high();
}

// Computes upper 32 bits of multiplication of a 32-bit unsigned integer and a
// 64-bit unsigned integer.
inline uint32_t umul96_upper32(uint32_t x, uint64_t y) FMT_NOEXCEPT {
  return static_cast<uint32_t>(umul128_upper64(x, y));
}

// Computes middle 64 bits of multiplication of a 64-bit unsigned integer and a
// 128-bit unsigned integer.
inline uint64_t umul192_middle64(uint64_t x, uint128_wrapper y) FMT_NOEXCEPT {
  uint64_t g01 = x * y.high();
  uint64_t g10 = umul128_upper64(x, y.low());
  return g01 + g10;
}

// Computes lower 64 bits of multiplication of a 32-bit unsigned integer and a
// 64-bit unsigned integer.
inline uint64_t umul96_lower64(uint32_t x, uint64_t y) FMT_NOEXCEPT {
  return x * y;
}

// Computes floor(log10(pow(2, e))) for e in [-1700, 1700] using the method from
// https://fmt.dev/papers/Grisu-Exact.pdf#page=5, section 3.4.
inline int floor_log10_pow2(int e) FMT_NOEXCEPT {
  FMT_ASSERT(e <= 1700 && e >= -1700, "too large exponent");
  const int shift = 22;
  return (e * static_cast<int>(log10_2_significand >> (64 - shift))) >> shift;
}

// Various fast log computations.
inline int floor_log2_pow10(int e) FMT_NOEXCEPT {
  FMT_ASSERT(e <= 1233 && e >= -1233, "too large exponent");
  const uint64_t log2_10_integer_part = 3;
  const uint64_t log2_10_fractional_digits = 0x5269e12f346e2bf9;
  const int shift_amount = 19;
  return (e * static_cast<int>(
                  (log2_10_integer_part << shift_amount) |
                  (log2_10_fractional_digits >> (64 - shift_amount)))) >>
         shift_amount;
}
inline int floor_log10_pow2_minus_log10_4_over_3(int e) FMT_NOEXCEPT {
  FMT_ASSERT(e <= 1700 && e >= -1700, "too large exponent");
  const uint64_t log10_4_over_3_fractional_digits = 0x1ffbfc2bbc780375;
  const int shift_amount = 22;
  return (e * static_cast<int>(log10_2_significand >> (64 - shift_amount)) -
          static_cast<int>(log10_4_over_3_fractional_digits >>
                           (64 - shift_amount))) >>
         shift_amount;
}

// Returns true iff x is divisible by pow(2, exp).
inline bool divisible_by_power_of_2(uint32_t x, int exp) FMT_NOEXCEPT {
  FMT_ASSERT(exp >= 1, "");
  FMT_ASSERT(x != 0, "");
#ifdef FMT_BUILTIN_CTZ
  return FMT_BUILTIN_CTZ(x) >= exp;
#else
  return exp < num_bits<uint32_t>() && x == ((x >> exp) << exp);
#endif
}
inline bool divisible_by_power_of_2(uint64_t x, int exp) FMT_NOEXCEPT {
  FMT_ASSERT(exp >= 1, "");
  FMT_ASSERT(x != 0, "");
#ifdef FMT_BUILTIN_CTZLL
  return FMT_BUILTIN_CTZLL(x) >= exp;
#else
  return exp < num_bits<uint64_t>() && x == ((x >> exp) << exp);
#endif
}

// Table entry type for divisibility test.
template <typename T> struct divtest_table_entry {
  T mod_inv;
  T max_quotient;
};

// Returns true iff x is divisible by pow(5, exp).
inline bool divisible_by_power_of_5(uint32_t x, int exp) FMT_NOEXCEPT {
  FMT_ASSERT(exp <= 10, "too large exponent");
  static constexpr const divtest_table_entry<uint32_t> divtest_table[] = {
      {0x00000001, 0xffffffff}, {0xcccccccd, 0x33333333},
      {0xc28f5c29, 0x0a3d70a3}, {0x26e978d5, 0x020c49ba},
      {0x3afb7e91, 0x0068db8b}, {0x0bcbe61d, 0x0014f8b5},
      {0x68c26139, 0x000431bd}, {0xae8d46a5, 0x0000d6bf},
      {0x22e90e21, 0x00002af3}, {0x3a2e9c6d, 0x00000897},
      {0x3ed61f49, 0x000001b7}};
  return x * divtest_table[exp].mod_inv <= divtest_table[exp].max_quotient;
}
inline bool divisible_by_power_of_5(uint64_t x, int exp) FMT_NOEXCEPT {
  FMT_ASSERT(exp <= 23, "too large exponent");
  static constexpr const divtest_table_entry<uint64_t> divtest_table[] = {
      {0x0000000000000001, 0xffffffffffffffff},
      {0xcccccccccccccccd, 0x3333333333333333},
      {0x8f5c28f5c28f5c29, 0x0a3d70a3d70a3d70},
      {0x1cac083126e978d5, 0x020c49ba5e353f7c},
      {0xd288ce703afb7e91, 0x0068db8bac710cb2},
      {0x5d4e8fb00bcbe61d, 0x0014f8b588e368f0},
      {0x790fb65668c26139, 0x000431bde82d7b63},
      {0xe5032477ae8d46a5, 0x0000d6bf94d5e57a},
      {0xc767074b22e90e21, 0x00002af31dc46118},
      {0x8e47ce423a2e9c6d, 0x0000089705f4136b},
      {0x4fa7f60d3ed61f49, 0x000001b7cdfd9d7b},
      {0x0fee64690c913975, 0x00000057f5ff85e5},
      {0x3662e0e1cf503eb1, 0x000000119799812d},
      {0xa47a2cf9f6433fbd, 0x0000000384b84d09},
      {0x54186f653140a659, 0x00000000b424dc35},
      {0x7738164770402145, 0x0000000024075f3d},
      {0xe4a4d1417cd9a041, 0x000000000734aca5},
      {0xc75429d9e5c5200d, 0x000000000170ef54},
      {0xc1773b91fac10669, 0x000000000049c977},
      {0x26b172506559ce15, 0x00000000000ec1e4},
      {0xd489e3a9addec2d1, 0x000000000002f394},
      {0x90e860bb892c8d5d, 0x000000000000971d},
      {0x502e79bf1b6f4f79, 0x0000000000001e39},
      {0xdcd618596be30fe5, 0x000000000000060b}};
  return x * divtest_table[exp].mod_inv <= divtest_table[exp].max_quotient;
}

// Replaces n by floor(n / pow(5, N)) returning true if and only if n is
// divisible by pow(5, N).
// Precondition: n <= 2 * pow(5, N + 1).
template <int N>
bool check_divisibility_and_divide_by_pow5(uint32_t& n) FMT_NOEXCEPT {
  static constexpr struct {
    uint32_t magic_number;
    int bits_for_comparison;
    uint32_t threshold;
    int shift_amount;
  } infos[] = {{0xcccd, 16, 0x3333, 18}, {0xa429, 8, 0x0a, 20}};
  constexpr auto info = infos[N - 1];
  n *= info.magic_number;
  const uint32_t comparison_mask = (1u << info.bits_for_comparison) - 1;
  bool result = (n & comparison_mask) <= info.threshold;
  n >>= info.shift_amount;
  return result;
}

// Computes floor(n / pow(10, N)) for small n and N.
// Precondition: n <= pow(10, N + 1).
template <int N> uint32_t small_division_by_pow10(uint32_t n) FMT_NOEXCEPT {
  static constexpr struct {
    uint32_t magic_number;
    int shift_amount;
    uint32_t divisor_times_10;
  } infos[] = {{0xcccd, 19, 100}, {0xa3d8, 22, 1000}};
  constexpr auto info = infos[N - 1];
  FMT_ASSERT(n <= info.divisor_times_10, "n is too large");
  return n * info.magic_number >> info.shift_amount;
}

// Computes floor(n / 10^(kappa + 1)) (float)
inline uint32_t divide_by_10_to_kappa_plus_1(uint32_t n) FMT_NOEXCEPT {
  return n / float_info<float>::big_divisor;
}
// Computes floor(n / 10^(kappa + 1)) (double)
inline uint64_t divide_by_10_to_kappa_plus_1(uint64_t n) FMT_NOEXCEPT {
  return umul128_upper64(n, 0x83126e978d4fdf3c) >> 9;
}

// Various subroutines using pow10 cache
template <class T> struct cache_accessor;

template <> struct cache_accessor<float> {
  using carrier_uint = float_info<float>::carrier_uint;
  using cache_entry_type = uint64_t;

  static uint64_t get_cached_power(int k) FMT_NOEXCEPT {
    FMT_ASSERT(k >= float_info<float>::min_k && k <= float_info<float>::max_k,
               "k is out of range");
    static constexpr const uint64_t pow10_significands[] = {
        0x81ceb32c4b43fcf5, 0xa2425ff75e14fc32, 0xcad2f7f5359a3b3f,
        0xfd87b5f28300ca0e, 0x9e74d1b791e07e49, 0xc612062576589ddb,
        0xf79687aed3eec552, 0x9abe14cd44753b53, 0xc16d9a0095928a28,
        0xf1c90080baf72cb2, 0x971da05074da7bef, 0xbce5086492111aeb,
        0xec1e4a7db69561a6, 0x9392ee8e921d5d08, 0xb877aa3236a4b44a,
        0xe69594bec44de15c, 0x901d7cf73ab0acda, 0xb424dc35095cd810,
        0xe12e13424bb40e14, 0x8cbccc096f5088cc, 0xafebff0bcb24aaff,
        0xdbe6fecebdedd5bf, 0x89705f4136b4a598, 0xabcc77118461cefd,
        0xd6bf94d5e57a42bd, 0x8637bd05af6c69b6, 0xa7c5ac471b478424,
        0xd1b71758e219652c, 0x83126e978d4fdf3c, 0xa3d70a3d70a3d70b,
        0xcccccccccccccccd, 0x8000000000000000, 0xa000000000000000,
        0xc800000000000000, 0xfa00000000000000, 0x9c40000000000000,
        0xc350000000000000, 0xf424000000000000, 0x9896800000000000,
        0xbebc200000000000, 0xee6b280000000000, 0x9502f90000000000,
        0xba43b74000000000, 0xe8d4a51000000000, 0x9184e72a00000000,
        0xb5e620f480000000, 0xe35fa931a0000000, 0x8e1bc9bf04000000,
        0xb1a2bc2ec5000000, 0xde0b6b3a76400000, 0x8ac7230489e80000,
        0xad78ebc5ac620000, 0xd8d726b7177a8000, 0x878678326eac9000,
        0xa968163f0a57b400, 0xd3c21bcecceda100, 0x84595161401484a0,
        0xa56fa5b99019a5c8, 0xcecb8f27f4200f3a, 0x813f3978f8940984,
        0xa18f07d736b90be5, 0xc9f2c9cd04674ede, 0xfc6f7c4045812296,
        0x9dc5ada82b70b59d, 0xc5371912364ce305, 0xf684df56c3e01bc6,
        0x9a130b963a6c115c, 0xc097ce7bc90715b3, 0xf0bdc21abb48db20,
        0x96769950b50d88f4, 0xbc143fa4e250eb31, 0xeb194f8e1ae525fd,
        0x92efd1b8d0cf37be, 0xb7abc627050305ad, 0xe596b7b0c643c719,
        0x8f7e32ce7bea5c6f, 0xb35dbf821ae4f38b, 0xe0352f62a19e306e};
    return pow10_significands[k - float_info<float>::min_k];
  }

  static carrier_uint compute_mul(carrier_uint u,
                                  const cache_entry_type& cache) FMT_NOEXCEPT {
    return umul96_upper32(u, cache);
  }

  static uint32_t compute_delta(const cache_entry_type& cache,
                                int beta_minus_1) FMT_NOEXCEPT {
    return static_cast<uint32_t>(cache >> (64 - 1 - beta_minus_1));
  }

  static bool compute_mul_parity(carrier_uint two_f,
                                 const cache_entry_type& cache,
                                 int beta_minus_1) FMT_NOEXCEPT {
    FMT_ASSERT(beta_minus_1 >= 1, "");
    FMT_ASSERT(beta_minus_1 < 64, "");

    return ((umul96_lower64(two_f, cache) >> (64 - beta_minus_1)) & 1) != 0;
  }

  static carrier_uint compute_left_endpoint_for_shorter_interval_case(
      const cache_entry_type& cache, int beta_minus_1) FMT_NOEXCEPT {
    return static_cast<carrier_uint>(
        (cache - (cache >> (float_info<float>::significand_bits + 2))) >>
        (64 - float_info<float>::significand_bits - 1 - beta_minus_1));
  }

  static carrier_uint compute_right_endpoint_for_shorter_interval_case(
      const cache_entry_type& cache, int beta_minus_1) FMT_NOEXCEPT {
    return static_cast<carrier_uint>(
        (cache + (cache >> (float_info<float>::significand_bits + 1))) >>
        (64 - float_info<float>::significand_bits - 1 - beta_minus_1));
  }

  static carrier_uint compute_round_up_for_shorter_interval_case(
      const cache_entry_type& cache, int beta_minus_1) FMT_NOEXCEPT {
    return (static_cast<carrier_uint>(
                cache >>
                (64 - float_info<float>::significand_bits - 2 - beta_minus_1)) +
            1) /
           2;
  }
};

template <> struct cache_accessor<double> {
  using carrier_uint = float_info<double>::carrier_uint;
  using cache_entry_type = uint128_wrapper;

  static uint128_wrapper get_cached_power(int k) FMT_NOEXCEPT {
    FMT_ASSERT(k >= float_info<double>::min_k && k <= float_info<double>::max_k,
               "k is out of range");

    static constexpr const uint128_wrapper pow10_significands[] = {
#if FMT_USE_FULL_CACHE_DRAGONBOX
      {0xff77b1fcbebcdc4f, 0x25e8e89c13bb0f7b},
      {0x9faacf3df73609b1, 0x77b191618c54e9ad},
      {0xc795830d75038c1d, 0xd59df5b9ef6a2418},
      {0xf97ae3d0d2446f25, 0x4b0573286b44ad1e},
      {0x9becce62836ac577, 0x4ee367f9430aec33},
      {0xc2e801fb244576d5, 0x229c41f793cda740},
      {0xf3a20279ed56d48a, 0x6b43527578c11110},
      {0x9845418c345644d6, 0x830a13896b78aaaa},
      {0xbe5691ef416bd60c, 0x23cc986bc656d554},
      {0xedec366b11c6cb8f, 0x2cbfbe86b7ec8aa9},
      {0x94b3a202eb1c3f39, 0x7bf7d71432f3d6aa},
      {0xb9e08a83a5e34f07, 0xdaf5ccd93fb0cc54},
      {0xe858ad248f5c22c9, 0xd1b3400f8f9cff69},
      {0x91376c36d99995be, 0x23100809b9c21fa2},
      {0xb58547448ffffb2d, 0xabd40a0c2832a78b},
      {0xe2e69915b3fff9f9, 0x16c90c8f323f516d},
      {0x8dd01fad907ffc3b, 0xae3da7d97f6792e4},
      {0xb1442798f49ffb4a, 0x99cd11cfdf41779d},
      {0xdd95317f31c7fa1d, 0x40405643d711d584},
      {0x8a7d3eef7f1cfc52, 0x482835ea666b2573},
      {0xad1c8eab5ee43b66, 0xda3243650005eed0},
      {0xd863b256369d4a40, 0x90bed43e40076a83},
      {0x873e4f75e2224e68, 0x5a7744a6e804a292},
      {0xa90de3535aaae202, 0x711515d0a205cb37},
      {0xd3515c2831559a83, 0x0d5a5b44ca873e04},
      {0x8412d9991ed58091, 0xe858790afe9486c3},
      {0xa5178fff668ae0b6, 0x626e974dbe39a873},
      {0xce5d73ff402d98e3, 0xfb0a3d212dc81290},
      {0x80fa687f881c7f8e, 0x7ce66634bc9d0b9a},
      {0xa139029f6a239f72, 0x1c1fffc1ebc44e81},
      {0xc987434744ac874e, 0xa327ffb266b56221},
      {0xfbe9141915d7a922, 0x4bf1ff9f0062baa9},
      {0x9d71ac8fada6c9b5, 0x6f773fc3603db4aa},
      {0xc4ce17b399107c22, 0xcb550fb4384d21d4},
      {0xf6019da07f549b2b, 0x7e2a53a146606a49},
      {0x99c102844f94e0fb, 0x2eda7444cbfc426e},
      {0xc0314325637a1939, 0xfa911155fefb5309},
      {0xf03d93eebc589f88, 0x793555ab7eba27cb},
      {0x96267c7535b763b5, 0x4bc1558b2f3458df},
      {0xbbb01b9283253ca2, 0x9eb1aaedfb016f17},
      {0xea9c227723ee8bcb, 0x465e15a979c1cadd},
      {0x92a1958a7675175f, 0x0bfacd89ec191eca},
      {0xb749faed14125d36, 0xcef980ec671f667c},
      {0xe51c79a85916f484, 0x82b7e12780e7401b},
      {0x8f31cc0937ae58d2, 0xd1b2ecb8b0908811},
      {0xb2fe3f0b8599ef07, 0x861fa7e6dcb4aa16},
      {0xdfbdcece67006ac9, 0x67a791e093e1d49b},
      {0x8bd6a141006042bd, 0xe0c8bb2c5c6d24e1},
      {0xaecc49914078536d, 0x58fae9f773886e19},
      {0xda7f5bf590966848, 0xaf39a475506a899f},
      {0x888f99797a5e012d, 0x6d8406c952429604},
      {0xaab37fd7d8f58178, 0xc8e5087ba6d33b84},
      {0xd5605fcdcf32e1d6, 0xfb1e4a9a90880a65},
      {0x855c3be0a17fcd26, 0x5cf2eea09a550680},
      {0xa6b34ad8c9dfc06f, 0xf42faa48c0ea481f},
      {0xd0601d8efc57b08b, 0xf13b94daf124da27},
      {0x823c12795db6ce57, 0x76c53d08d6b70859},
      {0xa2cb1717b52481ed, 0x54768c4b0c64ca6f},
      {0xcb7ddcdda26da268, 0xa9942f5dcf7dfd0a},
      {0xfe5d54150b090b02, 0xd3f93b35435d7c4d},
      {0x9efa548d26e5a6e1, 0xc47bc5014a1a6db0},
      {0xc6b8e9b0709f109a, 0x359ab6419ca1091c},
      {0xf867241c8cc6d4c0, 0xc30163d203c94b63},
      {0x9b407691d7fc44f8, 0x79e0de63425dcf1e},
      {0xc21094364dfb5636, 0x985915fc12f542e5},
      {0xf294b943e17a2bc4, 0x3e6f5b7b17b2939e},
      {0x979cf3ca6cec5b5a, 0xa705992ceecf9c43},
      {0xbd8430bd08277231, 0x50c6ff782a838354},
      {0xece53cec4a314ebd, 0xa4f8bf5635246429},
      {0x940f4613ae5ed136, 0x871b7795e136be9a},
      {0xb913179899f68584, 0x28e2557b59846e40},
      {0xe757dd7ec07426e5, 0x331aeada2fe589d0},
      {0x9096ea6f3848984f, 0x3ff0d2c85def7622},
      {0xb4bca50b065abe63, 0x0fed077a756b53aa},
      {0xe1ebce4dc7f16dfb, 0xd3e8495912c62895},
      {0x8d3360f09cf6e4bd, 0x64712dd7abbbd95d},
      {0xb080392cc4349dec, 0xbd8d794d96aacfb4},
      {0xdca04777f541c567, 0xecf0d7a0fc5583a1},
      {0x89e42caaf9491b60, 0xf41686c49db57245},
      {0xac5d37d5b79b6239, 0x311c2875c522ced6},
      {0xd77485cb25823ac7, 0x7d633293366b828c},
      {0x86a8d39ef77164bc, 0xae5dff9c02033198},
      {0xa8530886b54dbdeb, 0xd9f57f830283fdfd},
      {0xd267caa862a12d66, 0xd072df63c324fd7c},
      {0x8380dea93da4bc60, 0x4247cb9e59f71e6e},
      {0xa46116538d0deb78, 0x52d9be85f074e609},
      {0xcd795be870516656, 0x67902e276c921f8c},
      {0x806bd9714632dff6, 0x00ba1cd8a3db53b7},
      {0xa086cfcd97bf97f3, 0x80e8a40eccd228a5},
      {0xc8a883c0fdaf7df0, 0x6122cd128006b2ce},
      {0xfad2a4b13d1b5d6c, 0x796b805720085f82},
      {0x9cc3a6eec6311a63, 0xcbe3303674053bb1},
      {0xc3f490aa77bd60fc, 0xbedbfc4411068a9d},
      {0xf4f1b4d515acb93b, 0xee92fb5515482d45},
      {0x991711052d8bf3c5, 0x751bdd152d4d1c4b},
      {0xbf5cd54678eef0b6, 0xd262d45a78a0635e},
      {0xef340a98172aace4, 0x86fb897116c87c35},
      {0x9580869f0e7aac0e, 0xd45d35e6ae3d4da1},
      {0xbae0a846d2195712, 0x8974836059cca10a},
      {0xe998d258869facd7, 0x2bd1a438703fc94c},
      {0x91ff83775423cc06, 0x7b6306a34627ddd0},
      {0xb67f6455292cbf08, 0x1a3bc84c17b1d543},
      {0xe41f3d6a7377eeca, 0x20caba5f1d9e4a94},
      {0x8e938662882af53e, 0x547eb47b7282ee9d},
      {0xb23867fb2a35b28d, 0xe99e619a4f23aa44},
      {0xdec681f9f4c31f31, 0x6405fa00e2ec94d5},
      {0x8b3c113c38f9f37e, 0xde83bc408dd3dd05},
      {0xae0b158b4738705e, 0x9624ab50b148d446},
      {0xd98ddaee19068c76, 0x3badd624dd9b0958},
      {0x87f8a8d4cfa417c9, 0xe54ca5d70a80e5d7},
      {0xa9f6d30a038d1dbc, 0x5e9fcf4ccd211f4d},
      {0xd47487cc8470652b, 0x7647c32000696720},
      {0x84c8d4dfd2c63f3b, 0x29ecd9f40041e074},
      {0xa5fb0a17c777cf09, 0xf468107100525891},
      {0xcf79cc9db955c2cc, 0x7182148d4066eeb5},
      {0x81ac1fe293d599bf, 0xc6f14cd848405531},
      {0xa21727db38cb002f, 0xb8ada00e5a506a7d},
      {0xca9cf1d206fdc03b, 0xa6d90811f0e4851d},
      {0xfd442e4688bd304a, 0x908f4a166d1da664},
      {0x9e4a9cec15763e2e, 0x9a598e4e043287ff},
      {0xc5dd44271ad3cdba, 0x40eff1e1853f29fe},
      {0xf7549530e188c128, 0xd12bee59e68ef47d},
      {0x9a94dd3e8cf578b9, 0x82bb74f8301958cf},
      {0xc13a148e3032d6e7, 0xe36a52363c1faf02},
      {0xf18899b1bc3f8ca1, 0xdc44e6c3cb279ac2},
      {0x96f5600f15a7b7e5, 0x29ab103a5ef8c0ba},
      {0xbcb2b812db11a5de, 0x7415d448f6b6f0e8},
      {0xebdf661791d60f56, 0x111b495b3464ad22},
      {0x936b9fcebb25c995, 0xcab10dd900beec35},
      {0xb84687c269ef3bfb, 0x3d5d514f40eea743},
      {0xe65829b3046b0afa, 0x0cb4a5a3112a5113},
      {0x8ff71a0fe2c2e6dc, 0x47f0e785eaba72ac},
      {0xb3f4e093db73a093, 0x59ed216765690f57},
      {0xe0f218b8d25088b8, 0x306869c13ec3532d},
      {0x8c974f7383725573, 0x1e414218c73a13fc},
      {0xafbd2350644eeacf, 0xe5d1929ef90898fb},
      {0xdbac6c247d62a583, 0xdf45f746b74abf3a},
      {0x894bc396ce5da772, 0x6b8bba8c328eb784},
      {0xab9eb47c81f5114f, 0x066ea92f3f326565},
      {0xd686619ba27255a2, 0xc80a537b0efefebe},
      {0x8613fd0145877585, 0xbd06742ce95f5f37},
      {0xa798fc4196e952e7, 0x2c48113823b73705},
      {0xd17f3b51fca3a7a0, 0xf75a15862ca504c6},
      {0x82ef85133de648c4, 0x9a984d73dbe722fc},
      {0xa3ab66580d5fdaf5, 0xc13e60d0d2e0ebbb},
      {0xcc963fee10b7d1b3, 0x318df905079926a9},
      {0xffbbcfe994e5c61f, 0xfdf17746497f7053},
      {0x9fd561f1fd0f9bd3, 0xfeb6ea8bedefa634},
      {0xc7caba6e7c5382c8, 0xfe64a52ee96b8fc1},
      {0xf9bd690a1b68637b, 0x3dfdce7aa3c673b1},
      {0x9c1661a651213e2d, 0x06bea10ca65c084f},
      {0xc31bfa0fe5698db8, 0x486e494fcff30a63},
      {0xf3e2f893dec3f126, 0x5a89dba3c3efccfb},
      {0x986ddb5c6b3a76b7, 0xf89629465a75e01d},
      {0xbe89523386091465, 0xf6bbb397f1135824},
      {0xee2ba6c0678b597f, 0x746aa07ded582e2d},
      {0x94db483840b717ef, 0xa8c2a44eb4571cdd},
      {0xba121a4650e4ddeb, 0x92f34d62616ce414},
      {0xe896a0d7e51e1566, 0x77b020baf9c81d18},
      {0x915e2486ef32cd60, 0x0ace1474dc1d122f},
      {0xb5b5ada8aaff80b8, 0x0d819992132456bb},
      {0xe3231912d5bf60e6, 0x10e1fff697ed6c6a},
      {0x8df5efabc5979c8f, 0xca8d3ffa1ef463c2},
      {0xb1736b96b6fd83b3, 0xbd308ff8a6b17cb3},
      {0xddd0467c64bce4a0, 0xac7cb3f6d05ddbdf},
      {0x8aa22c0dbef60ee4, 0x6bcdf07a423aa96c},
      {0xad4ab7112eb3929d, 0x86c16c98d2c953c7},
      {0xd89d64d57a607744, 0xe871c7bf077ba8b8},
      {0x87625f056c7c4a8b, 0x11471cd764ad4973},
      {0xa93af6c6c79b5d2d, 0xd598e40d3dd89bd0},
      {0xd389b47879823479, 0x4aff1d108d4ec2c4},
      {0x843610cb4bf160cb, 0xcedf722a585139bb},
      {0xa54394fe1eedb8fe, 0xc2974eb4ee658829},
      {0xce947a3da6a9273e, 0x733d226229feea33},
      {0x811ccc668829b887, 0x0806357d5a3f5260},
      {0xa163ff802a3426a8, 0xca07c2dcb0cf26f8},
      {0xc9bcff6034c13052, 0xfc89b393dd02f0b6},
      {0xfc2c3f3841f17c67, 0xbbac2078d443ace3},
      {0x9d9ba7832936edc0, 0xd54b944b84aa4c0e},
      {0xc5029163f384a931, 0x0a9e795e65d4df12},
      {0xf64335bcf065d37d, 0x4d4617b5ff4a16d6},
      {0x99ea0196163fa42e, 0x504bced1bf8e4e46},
      {0xc06481fb9bcf8d39, 0xe45ec2862f71e1d7},
      {0xf07da27a82c37088, 0x5d767327bb4e5a4d},
      {0x964e858c91ba2655, 0x3a6a07f8d510f870},
      {0xbbe226efb628afea, 0x890489f70a55368c},
      {0xeadab0aba3b2dbe5, 0x2b45ac74ccea842f},
      {0x92c8ae6b464fc96f, 0x3b0b8bc90012929e},
      {0xb77ada0617e3bbcb, 0x09ce6ebb40173745},
      {0xe55990879ddcaabd, 0xcc420a6a101d0516},
      {0x8f57fa54c2a9eab6, 0x9fa946824a12232e},
      {0xb32df8e9f3546564, 0x47939822dc96abfa},
      {0xdff9772470297ebd, 0x59787e2b93bc56f8},
      {0x8bfbea76c619ef36, 0x57eb4edb3c55b65b},
      {0xaefae51477a06b03, 0xede622920b6b23f2},
      {0xdab99e59958885c4, 0xe95fab368e45ecee},
      {0x88b402f7fd75539b, 0x11dbcb0218ebb415},
      {0xaae103b5fcd2a881, 0xd652bdc29f26a11a},
      {0xd59944a37c0752a2, 0x4be76d3346f04960},
      {0x857fcae62d8493a5, 0x6f70a4400c562ddc},
      {0xa6dfbd9fb8e5b88e, 0xcb4ccd500f6bb953},
      {0xd097ad07a71f26b2, 0x7e2000a41346a7a8},
      {0x825ecc24c873782f, 0x8ed400668c0c28c9},
      {0xa2f67f2dfa90563b, 0x728900802f0f32fb},
      {0xcbb41ef979346bca, 0x4f2b40a03ad2ffba},
      {0xfea126b7d78186bc, 0xe2f610c84987bfa9},
      {0x9f24b832e6b0f436, 0x0dd9ca7d2df4d7ca},
      {0xc6ede63fa05d3143, 0x91503d1c79720dbc},
      {0xf8a95fcf88747d94, 0x75a44c6397ce912b},
      {0x9b69dbe1b548ce7c, 0xc986afbe3ee11abb},
      {0xc24452da229b021b, 0xfbe85badce996169},
      {0xf2d56790ab41c2a2, 0xfae27299423fb9c4},
      {0x97c560ba6b0919a5, 0xdccd879fc967d41b},
      {0xbdb6b8e905cb600f, 0x5400e987bbc1c921},
      {0xed246723473e3813, 0x290123e9aab23b69},
      {0x9436c0760c86e30b, 0xf9a0b6720aaf6522},
      {0xb94470938fa89bce, 0xf808e40e8d5b3e6a},
      {0xe7958cb87392c2c2, 0xb60b1d1230b20e05},
      {0x90bd77f3483bb9b9, 0xb1c6f22b5e6f48c3},
      {0xb4ecd5f01a4aa828, 0x1e38aeb6360b1af4},
      {0xe2280b6c20dd5232, 0x25c6da63c38de1b1},
      {0x8d590723948a535f, 0x579c487e5a38ad0f},
      {0xb0af48ec79ace837, 0x2d835a9df0c6d852},
      {0xdcdb1b2798182244, 0xf8e431456cf88e66},
      {0x8a08f0f8bf0f156b, 0x1b8e9ecb641b5900},
      {0xac8b2d36eed2dac5, 0xe272467e3d222f40},
      {0xd7adf884aa879177, 0x5b0ed81dcc6abb10},
      {0x86ccbb52ea94baea, 0x98e947129fc2b4ea},
      {0xa87fea27a539e9a5, 0x3f2398d747b36225},
      {0xd29fe4b18e88640e, 0x8eec7f0d19a03aae},
      {0x83a3eeeef9153e89, 0x1953cf68300424ad},
      {0xa48ceaaab75a8e2b, 0x5fa8c3423c052dd8},
      {0xcdb02555653131b6, 0x3792f412cb06794e},
      {0x808e17555f3ebf11, 0xe2bbd88bbee40bd1},
      {0xa0b19d2ab70e6ed6, 0x5b6aceaeae9d0ec5},
      {0xc8de047564d20a8b, 0xf245825a5a445276},
      {0xfb158592be068d2e, 0xeed6e2f0f0d56713},
      {0x9ced737bb6c4183d, 0x55464dd69685606c},
      {0xc428d05aa4751e4c, 0xaa97e14c3c26b887},
      {0xf53304714d9265df, 0xd53dd99f4b3066a9},
      {0x993fe2c6d07b7fab, 0xe546a8038efe402a},
      {0xbf8fdb78849a5f96, 0xde98520472bdd034},
      {0xef73d256a5c0f77c, 0x963e66858f6d4441},
      {0x95a8637627989aad, 0xdde7001379a44aa9},
      {0xbb127c53b17ec159, 0x5560c018580d5d53},
      {0xe9d71b689dde71af, 0xaab8f01e6e10b4a7},
      {0x9226712162ab070d, 0xcab3961304ca70e9},
      {0xb6b00d69bb55c8d1, 0x3d607b97c5fd0d23},
      {0xe45c10c42a2b3b05, 0x8cb89a7db77c506b},
      {0x8eb98a7a9a5b04e3, 0x77f3608e92adb243},
      {0xb267ed1940f1c61c, 0x55f038b237591ed4},
      {0xdf01e85f912e37a3, 0x6b6c46dec52f6689},
      {0x8b61313bbabce2c6, 0x2323ac4b3b3da016},
      {0xae397d8aa96c1b77, 0xabec975e0a0d081b},
      {0xd9c7dced53c72255, 0x96e7bd358c904a22},
      {0x881cea14545c7575, 0x7e50d64177da2e55},
      {0xaa242499697392d2, 0xdde50bd1d5d0b9ea},
      {0xd4ad2dbfc3d07787, 0x955e4ec64b44e865},
      {0x84ec3c97da624ab4, 0xbd5af13bef0b113f},
      {0xa6274bbdd0fadd61, 0xecb1ad8aeacdd58f},
      {0xcfb11ead453994ba, 0x67de18eda5814af3},
      {0x81ceb32c4b43fcf4, 0x80eacf948770ced8},
      {0xa2425ff75e14fc31, 0xa1258379a94d028e},
      {0xcad2f7f5359a3b3e, 0x096ee45813a04331},
      {0xfd87b5f28300ca0d, 0x8bca9d6e188853fd},
      {0x9e74d1b791e07e48, 0x775ea264cf55347e},
      {0xc612062576589dda, 0x95364afe032a819e},
      {0xf79687aed3eec551, 0x3a83ddbd83f52205},
      {0x9abe14cd44753b52, 0xc4926a9672793543},
      {0xc16d9a0095928a27, 0x75b7053c0f178294},
      {0xf1c90080baf72cb1, 0x5324c68b12dd6339},
      {0x971da05074da7bee, 0xd3f6fc16ebca5e04},
      {0xbce5086492111aea, 0x88f4bb1ca6bcf585},
      {0xec1e4a7db69561a5, 0x2b31e9e3d06c32e6},
      {0x9392ee8e921d5d07, 0x3aff322e62439fd0},
      {0xb877aa3236a4b449, 0x09befeb9fad487c3},
      {0xe69594bec44de15b, 0x4c2ebe687989a9b4},
      {0x901d7cf73ab0acd9, 0x0f9d37014bf60a11},
      {0xb424dc35095cd80f, 0x538484c19ef38c95},
      {0xe12e13424bb40e13, 0x2865a5f206b06fba},
      {0x8cbccc096f5088cb, 0xf93f87b7442e45d4},
      {0xafebff0bcb24aafe, 0xf78f69a51539d749},
      {0xdbe6fecebdedd5be, 0xb573440e5a884d1c},
      {0x89705f4136b4a597, 0x31680a88f8953031},
      {0xabcc77118461cefc, 0xfdc20d2b36ba7c3e},
      {0xd6bf94d5e57a42bc, 0x3d32907604691b4d},
      {0x8637bd05af6c69b5, 0xa63f9a49c2c1b110},
      {0xa7c5ac471b478423, 0x0fcf80dc33721d54},
      {0xd1b71758e219652b, 0xd3c36113404ea4a9},
      {0x83126e978d4fdf3b, 0x645a1cac083126ea},
      {0xa3d70a3d70a3d70a, 0x3d70a3d70a3d70a4},
      {0xcccccccccccccccc, 0xcccccccccccccccd},
      {0x8000000000000000, 0x0000000000000000},
      {0xa000000000000000, 0x0000000000000000},
      {0xc800000000000000, 0x0000000000000000},
      {0xfa00000000000000, 0x0000000000000000},
      {0x9c40000000000000, 0x0000000000000000},
      {0xc350000000000000, 0x0000000000000000},
      {0xf424000000000000, 0x0000000000000000},
      {0x9896800000000000, 0x0000000000000000},
      {0xbebc200000000000, 0x0000000000000000},
      {0xee6b280000000000, 0x0000000000000000},
      {0x9502f90000000000, 0x0000000000000000},
      {0xba43b74000000000, 0x0000000000000000},
      {0xe8d4a51000000000, 0x0000000000000000},
      {0x9184e72a00000000, 0x0000000000000000},
      {0xb5e620f480000000, 0x0000000000000000},
      {0xe35fa931a0000000, 0x0000000000000000},
      {0x8e1bc9bf04000000, 0x0000000000000000},
      {0xb1a2bc2ec5000000, 0x0000000000000000},
      {0xde0b6b3a76400000, 0x0000000000000000},
      {0x8ac7230489e80000, 0x0000000000000000},
      {0xad78ebc5ac620000, 0x0000000000000000},
      {0xd8d726b7177a8000, 0x0000000000000000},
      {0x878678326eac9000, 0x0000000000000000},
      {0xa968163f0a57b400, 0x0000000000000000},
      {0xd3c21bcecceda100, 0x0000000000000000},
      {0x84595161401484a0, 0x0000000000000000},
      {0xa56fa5b99019a5c8, 0x0000000000000000},
      {0xcecb8f27f4200f3a, 0x0000000000000000},
      {0x813f3978f8940984, 0x4000000000000000},
      {0xa18f07d736b90be5, 0x5000000000000000},
      {0xc9f2c9cd04674ede, 0xa400000000000000},
      {0xfc6f7c4045812296, 0x4d00000000000000},
      {0x9dc5ada82b70b59d, 0xf020000000000000},
      {0xc5371912364ce305, 0x6c28000000000000},
      {0xf684df56c3e01bc6, 0xc732000000000000},
      {0x9a130b963a6c115c, 0x3c7f400000000000},
      {0xc097ce7bc90715b3, 0x4b9f100000000000},
      {0xf0bdc21abb48db20, 0x1e86d40000000000},
      {0x96769950b50d88f4, 0x1314448000000000},
      {0xbc143fa4e250eb31, 0x17d955a000000000},
      {0xeb194f8e1ae525fd, 0x5dcfab0800000000},
      {0x92efd1b8d0cf37be, 0x5aa1cae500000000},
      {0xb7abc627050305ad, 0xf14a3d9e40000000},
      {0xe596b7b0c643c719, 0x6d9ccd05d0000000},
      {0x8f7e32ce7bea5c6f, 0xe4820023a2000000},
      {0xb35dbf821ae4f38b, 0xdda2802c8a800000},
      {0xe0352f62a19e306e, 0xd50b2037ad200000},
      {0x8c213d9da502de45, 0x4526f422cc340000},
      {0xaf298d050e4395d6, 0x9670b12b7f410000},
      {0xdaf3f04651d47b4c, 0x3c0cdd765f114000},
      {0x88d8762bf324cd0f, 0xa5880a69fb6ac800},
      {0xab0e93b6efee0053, 0x8eea0d047a457a00},
      {0xd5d238a4abe98068, 0x72a4904598d6d880},
      {0x85a36366eb71f041, 0x47a6da2b7f864750},
      {0xa70c3c40a64e6c51, 0x999090b65f67d924},
      {0xd0cf4b50cfe20765, 0xfff4b4e3f741cf6d},
      {0x82818f1281ed449f, 0xbff8f10e7a8921a4},
      {0xa321f2d7226895c7, 0xaff72d52192b6a0d},
      {0xcbea6f8ceb02bb39, 0x9bf4f8a69f764490},
      {0xfee50b7025c36a08, 0x02f236d04753d5b4},
      {0x9f4f2726179a2245, 0x01d762422c946590},
      {0xc722f0ef9d80aad6, 0x424d3ad2b7b97ef5},
      {0xf8ebad2b84e0d58b, 0xd2e0898765a7deb2},
      {0x9b934c3b330c8577, 0x63cc55f49f88eb2f},
      {0xc2781f49ffcfa6d5, 0x3cbf6b71c76b25fb},
      {0xf316271c7fc3908a, 0x8bef464e3945ef7a},
      {0x97edd871cfda3a56, 0x97758bf0e3cbb5ac},
      {0xbde94e8e43d0c8ec, 0x3d52eeed1cbea317},
      {0xed63a231d4c4fb27, 0x4ca7aaa863ee4bdd},
      {0x945e455f24fb1cf8, 0x8fe8caa93e74ef6a},
      {0xb975d6b6ee39e436, 0xb3e2fd538e122b44},
      {0xe7d34c64a9c85d44, 0x60dbbca87196b616},
      {0x90e40fbeea1d3a4a, 0xbc8955e946fe31cd},
      {0xb51d13aea4a488dd, 0x6babab6398bdbe41},
      {0xe264589a4dcdab14, 0xc696963c7eed2dd1},
      {0x8d7eb76070a08aec, 0xfc1e1de5cf543ca2},
      {0xb0de65388cc8ada8, 0x3b25a55f43294bcb},
      {0xdd15fe86affad912, 0x49ef0eb713f39ebe},
      {0x8a2dbf142dfcc7ab, 0x6e3569326c784337},
      {0xacb92ed9397bf996, 0x49c2c37f07965404},
      {0xd7e77a8f87daf7fb, 0xdc33745ec97be906},
      {0x86f0ac99b4e8dafd, 0x69a028bb3ded71a3},
      {0xa8acd7c0222311bc, 0xc40832ea0d68ce0c},
      {0xd2d80db02aabd62b, 0xf50a3fa490c30190},
      {0x83c7088e1aab65db, 0x792667c6da79e0fa},
      {0xa4b8cab1a1563f52, 0x577001b891185938},
      {0xcde6fd5e09abcf26, 0xed4c0226b55e6f86},
      {0x80b05e5ac60b6178, 0x544f8158315b05b4},
      {0xa0dc75f1778e39d6, 0x696361ae3db1c721},
      {0xc913936dd571c84c, 0x03bc3a19cd1e38e9},
      {0xfb5878494ace3a5f, 0x04ab48a04065c723},
      {0x9d174b2dcec0e47b, 0x62eb0d64283f9c76},
      {0xc45d1df942711d9a, 0x3ba5d0bd324f8394},
      {0xf5746577930d6500, 0xca8f44ec7ee36479},
      {0x9968bf6abbe85f20, 0x7e998b13cf4e1ecb},
      {0xbfc2ef456ae276e8, 0x9e3fedd8c321a67e},
      {0xefb3ab16c59b14a2, 0xc5cfe94ef3ea101e},
      {0x95d04aee3b80ece5, 0xbba1f1d158724a12},
      {0xbb445da9ca61281f, 0x2a8a6e45ae8edc97},
      {0xea1575143cf97226, 0xf52d09d71a3293bd},
      {0x924d692ca61be758, 0x593c2626705f9c56},
      {0xb6e0c377cfa2e12e, 0x6f8b2fb00c77836c},
      {0xe498f455c38b997a, 0x0b6dfb9c0f956447},
      {0x8edf98b59a373fec, 0x4724bd4189bd5eac},
      {0xb2977ee300c50fe7, 0x58edec91ec2cb657},
      {0xdf3d5e9bc0f653e1, 0x2f2967b66737e3ed},
      {0x8b865b215899f46c, 0xbd79e0d20082ee74},
      {0xae67f1e9aec07187, 0xecd8590680a3aa11},
      {0xda01ee641a708de9, 0xe80e6f4820cc9495},
      {0x884134fe908658b2, 0x3109058d147fdcdd},
      {0xaa51823e34a7eede, 0xbd4b46f0599fd415},
      {0xd4e5e2cdc1d1ea96, 0x6c9e18ac7007c91a},
      {0x850fadc09923329e, 0x03e2cf6bc604ddb0},
      {0xa6539930bf6bff45, 0x84db8346b786151c},
      {0xcfe87f7cef46ff16, 0xe612641865679a63},
      {0x81f14fae158c5f6e, 0x4fcb7e8f3f60c07e},
      {0xa26da3999aef7749, 0xe3be5e330f38f09d},
      {0xcb090c8001ab551c, 0x5cadf5bfd3072cc5},
      {0xfdcb4fa002162a63, 0x73d9732fc7c8f7f6},
      {0x9e9f11c4014dda7e, 0x2867e7fddcdd9afa},
      {0xc646d63501a1511d, 0xb281e1fd541501b8},
      {0xf7d88bc24209a565, 0x1f225a7ca91a4226},
      {0x9ae757596946075f, 0x3375788de9b06958},
      {0xc1a12d2fc3978937, 0x0052d6b1641c83ae},
      {0xf209787bb47d6b84, 0xc0678c5dbd23a49a},
      {0x9745eb4d50ce6332, 0xf840b7ba963646e0},
      {0xbd176620a501fbff, 0xb650e5a93bc3d898},
      {0xec5d3fa8ce427aff, 0xa3e51f138ab4cebe},
      {0x93ba47c980e98cdf, 0xc66f336c36b10137},
      {0xb8a8d9bbe123f017, 0xb80b0047445d4184},
      {0xe6d3102ad96cec1d, 0xa60dc059157491e5},
      {0x9043ea1ac7e41392, 0x87c89837ad68db2f},
      {0xb454e4a179dd1877, 0x29babe4598c311fb},
      {0xe16a1dc9d8545e94, 0xf4296dd6fef3d67a},
      {0x8ce2529e2734bb1d, 0x1899e4a65f58660c},
      {0xb01ae745b101e9e4, 0x5ec05dcff72e7f8f},
      {0xdc21a1171d42645d, 0x76707543f4fa1f73},
      {0x899504ae72497eba, 0x6a06494a791c53a8},
      {0xabfa45da0edbde69, 0x0487db9d17636892},
      {0xd6f8d7509292d603, 0x45a9d2845d3c42b6},
      {0x865b86925b9bc5c2, 0x0b8a2392ba45a9b2},
      {0xa7f26836f282b732, 0x8e6cac7768d7141e},
      {0xd1ef0244af2364ff, 0x3207d795430cd926},
      {0x8335616aed761f1f, 0x7f44e6bd49e807b8},
      {0xa402b9c5a8d3a6e7, 0x5f16206c9c6209a6},
      {0xcd036837130890a1, 0x36dba887c37a8c0f},
      {0x802221226be55a64, 0xc2494954da2c9789},
      {0xa02aa96b06deb0fd, 0xf2db9baa10b7bd6c},
      {0xc83553c5c8965d3d, 0x6f92829494e5acc7},
      {0xfa42a8b73abbf48c, 0xcb772339ba1f17f9},
      {0x9c69a97284b578d7, 0xff2a760414536efb},
      {0xc38413cf25e2d70d, 0xfef5138519684aba},
      {0xf46518c2ef5b8cd1, 0x7eb258665fc25d69},
      {0x98bf2f79d5993802, 0xef2f773ffbd97a61},
      {0xbeeefb584aff8603, 0xaafb550ffacfd8fa},
      {0xeeaaba2e5dbf6784, 0x95ba2a53f983cf38},
      {0x952ab45cfa97a0b2, 0xdd945a747bf26183},
      {0xba756174393d88df, 0x94f971119aeef9e4},
      {0xe912b9d1478ceb17, 0x7a37cd5601aab85d},
      {0x91abb422ccb812ee, 0xac62e055c10ab33a},
      {0xb616a12b7fe617aa, 0x577b986b314d6009},
      {0xe39c49765fdf9d94, 0xed5a7e85fda0b80b},
      {0x8e41ade9fbebc27d, 0x14588f13be847307},
      {0xb1d219647ae6b31c, 0x596eb2d8ae258fc8},
      {0xde469fbd99a05fe3, 0x6fca5f8ed9aef3bb},
      {0x8aec23d680043bee, 0x25de7bb9480d5854},
      {0xada72ccc20054ae9, 0xaf561aa79a10ae6a},
      {0xd910f7ff28069da4, 0x1b2ba1518094da04},
      {0x87aa9aff79042286, 0x90fb44d2f05d0842},
      {0xa99541bf57452b28, 0x353a1607ac744a53},
      {0xd3fa922f2d1675f2, 0x42889b8997915ce8},
      {0x847c9b5d7c2e09b7, 0x69956135febada11},
      {0xa59bc234db398c25, 0x43fab9837e699095},
      {0xcf02b2c21207ef2e, 0x94f967e45e03f4bb},
      {0x8161afb94b44f57d, 0x1d1be0eebac278f5},
      {0xa1ba1ba79e1632dc, 0x6462d92a69731732},
      {0xca28a291859bbf93, 0x7d7b8f7503cfdcfe},
      {0xfcb2cb35e702af78, 0x5cda735244c3d43e},
      {0x9defbf01b061adab, 0x3a0888136afa64a7},
      {0xc56baec21c7a1916, 0x088aaa1845b8fdd0},
      {0xf6c69a72a3989f5b, 0x8aad549e57273d45},
      {0x9a3c2087a63f6399, 0x36ac54e2f678864b},
      {0xc0cb28a98fcf3c7f, 0x84576a1bb416a7dd},
      {0xf0fdf2d3f3c30b9f, 0x656d44a2a11c51d5},
      {0x969eb7c47859e743, 0x9f644ae5a4b1b325},
      {0xbc4665b596706114, 0x873d5d9f0dde1fee},
      {0xeb57ff22fc0c7959, 0xa90cb506d155a7ea},
      {0x9316ff75dd87cbd8, 0x09a7f12442d588f2},
      {0xb7dcbf5354e9bece, 0x0c11ed6d538aeb2f},
      {0xe5d3ef282a242e81, 0x8f1668c8a86da5fa},
      {0x8fa475791a569d10, 0xf96e017d694487bc},
      {0xb38d92d760ec4455, 0x37c981dcc395a9ac},
      {0xe070f78d3927556a, 0x85bbe253f47b1417},
      {0x8c469ab843b89562, 0x93956d7478ccec8e},
      {0xaf58416654a6babb, 0x387ac8d1970027b2},
      {0xdb2e51bfe9d0696a, 0x06997b05fcc0319e},
      {0x88fcf317f22241e2, 0x441fece3bdf81f03},
      {0xab3c2fddeeaad25a, 0xd527e81cad7626c3},
      {0xd60b3bd56a5586f1, 0x8a71e223d8d3b074},
      {0x85c7056562757456, 0xf6872d5667844e49},
      {0xa738c6bebb12d16c, 0xb428f8ac016561db},
      {0xd106f86e69d785c7, 0xe13336d701beba52},
      {0x82a45b450226b39c, 0xecc0024661173473},
      {0xa34d721642b06084, 0x27f002d7f95d0190},
      {0xcc20ce9bd35c78a5, 0x31ec038df7b441f4},
      {0xff290242c83396ce, 0x7e67047175a15271},
      {0x9f79a169bd203e41, 0x0f0062c6e984d386},
      {0xc75809c42c684dd1, 0x52c07b78a3e60868},
      {0xf92e0c3537826145, 0xa7709a56ccdf8a82},
      {0x9bbcc7a142b17ccb, 0x88a66076400bb691},
      {0xc2abf989935ddbfe, 0x6acff893d00ea435},
      {0xf356f7ebf83552fe, 0x0583f6b8c4124d43},
      {0x98165af37b2153de, 0xc3727a337a8b704a},
      {0xbe1bf1b059e9a8d6, 0x744f18c0592e4c5c},
      {0xeda2ee1c7064130c, 0x1162def06f79df73},
      {0x9485d4d1c63e8be7, 0x8addcb5645ac2ba8},
      {0xb9a74a0637ce2ee1, 0x6d953e2bd7173692},
      {0xe8111c87c5c1ba99, 0xc8fa8db6ccdd0437},
      {0x910ab1d4db9914a0, 0x1d9c9892400a22a2},
      {0xb54d5e4a127f59c8, 0x2503beb6d00cab4b},
      {0xe2a0b5dc971f303a, 0x2e44ae64840fd61d},
      {0x8da471a9de737e24, 0x5ceaecfed289e5d2},
      {0xb10d8e1456105dad, 0x7425a83e872c5f47},
      {0xdd50f1996b947518, 0xd12f124e28f77719},
      {0x8a5296ffe33cc92f, 0x82bd6b70d99aaa6f},
      {0xace73cbfdc0bfb7b, 0x636cc64d1001550b},
      {0xd8210befd30efa5a, 0x3c47f7e05401aa4e},
      {0x8714a775e3e95c78, 0x65acfaec34810a71},
      {0xa8d9d1535ce3b396, 0x7f1839a741a14d0d},
      {0xd31045a8341ca07c, 0x1ede48111209a050},
      {0x83ea2b892091e44d, 0x934aed0aab460432},
      {0xa4e4b66b68b65d60, 0xf81da84d5617853f},
      {0xce1de40642e3f4b9, 0x36251260ab9d668e},
      {0x80d2ae83e9ce78f3, 0xc1d72b7c6b426019},
      {0xa1075a24e4421730, 0xb24cf65b8612f81f},
      {0xc94930ae1d529cfc, 0xdee033f26797b627},
      {0xfb9b7cd9a4a7443c, 0x169840ef017da3b1},
      {0x9d412e0806e88aa5, 0x8e1f289560ee864e},
      {0xc491798a08a2ad4e, 0xf1a6f2bab92a27e2},
      {0xf5b5d7ec8acb58a2, 0xae10af696774b1db},
      {0x9991a6f3d6bf1765, 0xacca6da1e0a8ef29},
      {0xbff610b0cc6edd3f, 0x17fd090a58d32af3},
      {0xeff394dcff8a948e, 0xddfc4b4cef07f5b0},
      {0x95f83d0a1fb69cd9, 0x4abdaf101564f98e},
      {0xbb764c4ca7a4440f, 0x9d6d1ad41abe37f1},
      {0xea53df5fd18d5513, 0x84c86189216dc5ed},
      {0x92746b9be2f8552c, 0x32fd3cf5b4e49bb4},
      {0xb7118682dbb66a77, 0x3fbc8c33221dc2a1},
      {0xe4d5e82392a40515, 0x0fabaf3feaa5334a},
      {0x8f05b1163ba6832d, 0x29cb4d87f2a7400e},
      {0xb2c71d5bca9023f8, 0x743e20e9ef511012},
      {0xdf78e4b2bd342cf6, 0x914da9246b255416},
      {0x8bab8eefb6409c1a, 0x1ad089b6c2f7548e},
      {0xae9672aba3d0c320, 0xa184ac2473b529b1},
      {0xda3c0f568cc4f3e8, 0xc9e5d72d90a2741e},
      {0x8865899617fb1871, 0x7e2fa67c7a658892},
      {0xaa7eebfb9df9de8d, 0xddbb901b98feeab7},
      {0xd51ea6fa85785631, 0x552a74227f3ea565},
      {0x8533285c936b35de, 0xd53a88958f87275f},
      {0xa67ff273b8460356, 0x8a892abaf368f137},
      {0xd01fef10a657842c, 0x2d2b7569b0432d85},
      {0x8213f56a67f6b29b, 0x9c3b29620e29fc73},
      {0xa298f2c501f45f42, 0x8349f3ba91b47b8f},
      {0xcb3f2f7642717713, 0x241c70a936219a73},
      {0xfe0efb53d30dd4d7, 0xed238cd383aa0110},
      {0x9ec95d1463e8a506, 0xf4363804324a40aa},
      {0xc67bb4597ce2ce48, 0xb143c6053edcd0d5},
      {0xf81aa16fdc1b81da, 0xdd94b7868e94050a},
      {0x9b10a4e5e9913128, 0xca7cf2b4191c8326},
      {0xc1d4ce1f63f57d72, 0xfd1c2f611f63a3f0},
      {0xf24a01a73cf2dccf, 0xbc633b39673c8cec},
      {0x976e41088617ca01, 0xd5be0503e085d813},
      {0xbd49d14aa79dbc82, 0x4b2d8644d8a74e18},
      {0xec9c459d51852ba2, 0xddf8e7d60ed1219e},
      {0x93e1ab8252f33b45, 0xcabb90e5c942b503},
      {0xb8da1662e7b00a17, 0x3d6a751f3b936243},
      {0xe7109bfba19c0c9d, 0x0cc512670a783ad4},
      {0x906a617d450187e2, 0x27fb2b80668b24c5},
      {0xb484f9dc9641e9da, 0xb1f9f660802dedf6},
      {0xe1a63853bbd26451, 0x5e7873f8a0396973},
      {0x8d07e33455637eb2, 0xdb0b487b6423e1e8},
      {0xb049dc016abc5e5f, 0x91ce1a9a3d2cda62},
      {0xdc5c5301c56b75f7, 0x7641a140cc7810fb},
      {0x89b9b3e11b6329ba, 0xa9e904c87fcb0a9d},
      {0xac2820d9623bf429, 0x546345fa9fbdcd44},
      {0xd732290fbacaf133, 0xa97c177947ad4095},
      {0x867f59a9d4bed6c0, 0x49ed8eabcccc485d},
      {0xa81f301449ee8c70, 0x5c68f256bfff5a74},
      {0xd226fc195c6a2f8c, 0x73832eec6fff3111},
      {0x83585d8fd9c25db7, 0xc831fd53c5ff7eab},
      {0xa42e74f3d032f525, 0xba3e7ca8b77f5e55},
      {0xcd3a1230c43fb26f, 0x28ce1bd2e55f35eb},
      {0x80444b5e7aa7cf85, 0x7980d163cf5b81b3},
      {0xa0555e361951c366, 0xd7e105bcc332621f},
      {0xc86ab5c39fa63440, 0x8dd9472bf3fefaa7},
      {0xfa856334878fc150, 0xb14f98f6f0feb951},
      {0x9c935e00d4b9d8d2, 0x6ed1bf9a569f33d3},
      {0xc3b8358109e84f07, 0x0a862f80ec4700c8},
      {0xf4a642e14c6262c8, 0xcd27bb612758c0fa},
      {0x98e7e9cccfbd7dbd, 0x8038d51cb897789c},
      {0xbf21e44003acdd2c, 0xe0470a63e6bd56c3},
      {0xeeea5d5004981478, 0x1858ccfce06cac74},
      {0x95527a5202df0ccb, 0x0f37801e0c43ebc8},
      {0xbaa718e68396cffd, 0xd30560258f54e6ba},
      {0xe950df20247c83fd, 0x47c6b82ef32a2069},
      {0x91d28b7416cdd27e, 0x4cdc331d57fa5441},
      {0xb6472e511c81471d, 0xe0133fe4adf8e952},
      {0xe3d8f9e563a198e5, 0x58180fddd97723a6},
      {0x8e679c2f5e44ff8f, 0x570f09eaa7ea7648},
      {0xb201833b35d63f73, 0x2cd2cc6551e513da},
      {0xde81e40a034bcf4f, 0xf8077f7ea65e58d1},
      {0x8b112e86420f6191, 0xfb04afaf27faf782},
      {0xadd57a27d29339f6, 0x79c5db9af1f9b563},
      {0xd94ad8b1c7380874, 0x18375281ae7822bc},
      {0x87cec76f1c830548, 0x8f2293910d0b15b5},
      {0xa9c2794ae3a3c69a, 0xb2eb3875504ddb22},
      {0xd433179d9c8cb841, 0x5fa60692a46151eb},
      {0x849feec281d7f328, 0xdbc7c41ba6bcd333},
      {0xa5c7ea73224deff3, 0x12b9b522906c0800},
      {0xcf39e50feae16bef, 0xd768226b34870a00},
      {0x81842f29f2cce375, 0xe6a1158300d46640},
      {0xa1e53af46f801c53, 0x60495ae3c1097fd0},
      {0xca5e89b18b602368, 0x385bb19cb14bdfc4},
      {0xfcf62c1dee382c42, 0x46729e03dd9ed7b5},
      {0x9e19db92b4e31ba9, 0x6c07a2c26a8346d1},
      {0xc5a05277621be293, 0xc7098b7305241885},
      { 0xf70867153aa2db38,
        0xb8cbee4fc66d1ea7 }
#else
      {0xff77b1fcbebcdc4f, 0x25e8e89c13bb0f7b},
      {0xce5d73ff402d98e3, 0xfb0a3d212dc81290},
      {0xa6b34ad8c9dfc06f, 0xf42faa48c0ea481f},
      {0x86a8d39ef77164bc, 0xae5dff9c02033198},
      {0xd98ddaee19068c76, 0x3badd624dd9b0958},
      {0xafbd2350644eeacf, 0xe5d1929ef90898fb},
      {0x8df5efabc5979c8f, 0xca8d3ffa1ef463c2},
      {0xe55990879ddcaabd, 0xcc420a6a101d0516},
      {0xb94470938fa89bce, 0xf808e40e8d5b3e6a},
      {0x95a8637627989aad, 0xdde7001379a44aa9},
      {0xf1c90080baf72cb1, 0x5324c68b12dd6339},
      {0xc350000000000000, 0x0000000000000000},
      {0x9dc5ada82b70b59d, 0xf020000000000000},
      {0xfee50b7025c36a08, 0x02f236d04753d5b4},
      {0xcde6fd5e09abcf26, 0xed4c0226b55e6f86},
      {0xa6539930bf6bff45, 0x84db8346b786151c},
      {0x865b86925b9bc5c2, 0x0b8a2392ba45a9b2},
      {0xd910f7ff28069da4, 0x1b2ba1518094da04},
      {0xaf58416654a6babb, 0x387ac8d1970027b2},
      {0x8da471a9de737e24, 0x5ceaecfed289e5d2},
      {0xe4d5e82392a40515, 0x0fabaf3feaa5334a},
      {0xb8da1662e7b00a17, 0x3d6a751f3b936243},
      { 0x95527a5202df0ccb,
        0x0f37801e0c43ebc8 }
#endif
    };

#if FMT_USE_FULL_CACHE_DRAGONBOX
    return pow10_significands[k - float_info<double>::min_k];
#else
    static constexpr const uint64_t powers_of_5_64[] = {
        0x0000000000000001, 0x0000000000000005, 0x0000000000000019,
        0x000000000000007d, 0x0000000000000271, 0x0000000000000c35,
        0x0000000000003d09, 0x000000000001312d, 0x000000000005f5e1,
        0x00000000001dcd65, 0x00000000009502f9, 0x0000000002e90edd,
        0x000000000e8d4a51, 0x0000000048c27395, 0x000000016bcc41e9,
        0x000000071afd498d, 0x0000002386f26fc1, 0x000000b1a2bc2ec5,
        0x000003782dace9d9, 0x00001158e460913d, 0x000056bc75e2d631,
        0x0001b1ae4d6e2ef5, 0x000878678326eac9, 0x002a5a058fc295ed,
        0x00d3c21bcecceda1, 0x0422ca8b0a00a425, 0x14adf4b7320334b9};

    static constexpr const uint32_t pow10_recovery_errors[] = {
        0x50001400, 0x54044100, 0x54014555, 0x55954415, 0x54115555, 0x00000001,
        0x50000000, 0x00104000, 0x54010004, 0x05004001, 0x55555544, 0x41545555,
        0x54040551, 0x15445545, 0x51555514, 0x10000015, 0x00101100, 0x01100015,
        0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x04450514, 0x45414110,
        0x55555145, 0x50544050, 0x15040155, 0x11054140, 0x50111514, 0x11451454,
        0x00400541, 0x00000000, 0x55555450, 0x10056551, 0x10054011, 0x55551014,
        0x69514555, 0x05151109, 0x00155555};

    static const int compression_ratio = 27;

    // Compute base index.
    int cache_index = (k - float_info<double>::min_k) / compression_ratio;
    int kb = cache_index * compression_ratio + float_info<double>::min_k;
    int offset = k - kb;

    // Get base cache.
    uint128_wrapper base_cache = pow10_significands[cache_index];
    if (offset == 0) return base_cache;

    // Compute the required amount of bit-shift.
    int alpha = floor_log2_pow10(kb + offset) - floor_log2_pow10(kb) - offset;
    FMT_ASSERT(alpha > 0 && alpha < 64, "shifting error detected");

    // Try to recover the real cache.
    uint64_t pow5 = powers_of_5_64[offset];
    uint128_wrapper recovered_cache = umul128(base_cache.high(), pow5);
    uint128_wrapper middle_low =
        umul128(base_cache.low() - (kb < 0 ? 1u : 0u), pow5);

    recovered_cache += middle_low.high();

    uint64_t high_to_middle = recovered_cache.high() << (64 - alpha);
    uint64_t middle_to_low = recovered_cache.low() << (64 - alpha);

    recovered_cache =
        uint128_wrapper{(recovered_cache.low() >> alpha) | high_to_middle,
                        ((middle_low.low() >> alpha) | middle_to_low)};

    if (kb < 0) recovered_cache += 1;

    // Get error.
    int error_idx = (k - float_info<double>::min_k) / 16;
    uint32_t error = (pow10_recovery_errors[error_idx] >>
                      ((k - float_info<double>::min_k) % 16) * 2) &
                     0x3;

    // Add the error back.
    FMT_ASSERT(recovered_cache.low() + error >= recovered_cache.low(), "");
    return {recovered_cache.high(), recovered_cache.low() + error};
#endif
  }

  static carrier_uint compute_mul(carrier_uint u,
                                  const cache_entry_type& cache) FMT_NOEXCEPT {
    return umul192_upper64(u, cache);
  }

  static uint32_t compute_delta(cache_entry_type const& cache,
                                int beta_minus_1) FMT_NOEXCEPT {
    return static_cast<uint32_t>(cache.high() >> (64 - 1 - beta_minus_1));
  }

  static bool compute_mul_parity(carrier_uint two_f,
                                 const cache_entry_type& cache,
                                 int beta_minus_1) FMT_NOEXCEPT {
    FMT_ASSERT(beta_minus_1 >= 1, "");
    FMT_ASSERT(beta_minus_1 < 64, "");

    return ((umul192_middle64(two_f, cache) >> (64 - beta_minus_1)) & 1) != 0;
  }

  static carrier_uint compute_left_endpoint_for_shorter_interval_case(
      const cache_entry_type& cache, int beta_minus_1) FMT_NOEXCEPT {
    return (cache.high() -
            (cache.high() >> (float_info<double>::significand_bits + 2))) >>
           (64 - float_info<double>::significand_bits - 1 - beta_minus_1);
  }

  static carrier_uint compute_right_endpoint_for_shorter_interval_case(
      const cache_entry_type& cache, int beta_minus_1) FMT_NOEXCEPT {
    return (cache.high() +
            (cache.high() >> (float_info<double>::significand_bits + 1))) >>
           (64 - float_info<double>::significand_bits - 1 - beta_minus_1);
  }

  static carrier_uint compute_round_up_for_shorter_interval_case(
      const cache_entry_type& cache, int beta_minus_1) FMT_NOEXCEPT {
    return ((cache.high() >>
             (64 - float_info<double>::significand_bits - 2 - beta_minus_1)) +
            1) /
           2;
  }
};

// Various integer checks
template <class T>
bool is_left_endpoint_integer_shorter_interval(int exponent) FMT_NOEXCEPT {
  return exponent >=
             float_info<
                 T>::case_shorter_interval_left_endpoint_lower_threshold &&
         exponent <=
             float_info<T>::case_shorter_interval_left_endpoint_upper_threshold;
}
template <class T>
bool is_endpoint_integer(typename float_info<T>::carrier_uint two_f,
                         int exponent, int minus_k) FMT_NOEXCEPT {
  if (exponent < float_info<T>::case_fc_pm_half_lower_threshold) return false;
  // For k >= 0.
  if (exponent <= float_info<T>::case_fc_pm_half_upper_threshold) return true;
  // For k < 0.
  if (exponent > float_info<T>::divisibility_check_by_5_threshold) return false;
  return divisible_by_power_of_5(two_f, minus_k);
}

template <class T>
bool is_center_integer(typename float_info<T>::carrier_uint two_f, int exponent,
                       int minus_k) FMT_NOEXCEPT {
  // Exponent for 5 is negative.
  if (exponent > float_info<T>::divisibility_check_by_5_threshold) return false;
  if (exponent > float_info<T>::case_fc_upper_threshold)
    return divisible_by_power_of_5(two_f, minus_k);
  // Both exponents are nonnegative.
  if (exponent >= float_info<T>::case_fc_lower_threshold) return true;
  // Exponent for 2 is negative.
  return divisible_by_power_of_2(two_f, minus_k - exponent + 1);
}

// Remove trailing zeros from n and return the number of zeros removed (float)
FMT_INLINE int remove_trailing_zeros(uint32_t& n) FMT_NOEXCEPT {
#ifdef FMT_BUILTIN_CTZ
  int t = FMT_BUILTIN_CTZ(n);
#else
  int t = ctz(n);
#endif
  if (t > float_info<float>::max_trailing_zeros)
    t = float_info<float>::max_trailing_zeros;

  const uint32_t mod_inv1 = 0xcccccccd;
  const uint32_t max_quotient1 = 0x33333333;
  const uint32_t mod_inv2 = 0xc28f5c29;
  const uint32_t max_quotient2 = 0x0a3d70a3;

  int s = 0;
  for (; s < t - 1; s += 2) {
    if (n * mod_inv2 > max_quotient2) break;
    n *= mod_inv2;
  }
  if (s < t && n * mod_inv1 <= max_quotient1) {
    n *= mod_inv1;
    ++s;
  }
  n >>= s;
  return s;
}

// Removes trailing zeros and returns the number of zeros removed (double)
FMT_INLINE int remove_trailing_zeros(uint64_t& n) FMT_NOEXCEPT {
#ifdef FMT_BUILTIN_CTZLL
  int t = FMT_BUILTIN_CTZLL(n);
#else
  int t = ctzll(n);
#endif
  if (t > float_info<double>::max_trailing_zeros)
    t = float_info<double>::max_trailing_zeros;
  // Divide by 10^8 and reduce to 32-bits
  // Since ret_value.significand <= (2^64 - 1) / 1000 < 10^17,
  // both of the quotient and the r should fit in 32-bits

  const uint32_t mod_inv1 = 0xcccccccd;
  const uint32_t max_quotient1 = 0x33333333;
  const uint64_t mod_inv8 = 0xc767074b22e90e21;
  const uint64_t max_quotient8 = 0x00002af31dc46118;

  // If the number is divisible by 1'0000'0000, work with the quotient
  if (t >= 8) {
    auto quotient_candidate = n * mod_inv8;

    if (quotient_candidate <= max_quotient8) {
      auto quotient = static_cast<uint32_t>(quotient_candidate >> 8);

      int s = 8;
      for (; s < t; ++s) {
        if (quotient * mod_inv1 > max_quotient1) break;
        quotient *= mod_inv1;
      }
      quotient >>= (s - 8);
      n = quotient;
      return s;
    }
  }

  // Otherwise, work with the remainder
  auto quotient = static_cast<uint32_t>(n / 100000000);
  auto remainder = static_cast<uint32_t>(n - 100000000 * quotient);

  if (t == 0 || remainder * mod_inv1 > max_quotient1) {
    return 0;
  }
  remainder *= mod_inv1;

  if (t == 1 || remainder * mod_inv1 > max_quotient1) {
    n = (remainder >> 1) + quotient * 10000000ull;
    return 1;
  }
  remainder *= mod_inv1;

  if (t == 2 || remainder * mod_inv1 > max_quotient1) {
    n = (remainder >> 2) + quotient * 1000000ull;
    return 2;
  }
  remainder *= mod_inv1;

  if (t == 3 || remainder * mod_inv1 > max_quotient1) {
    n = (remainder >> 3) + quotient * 100000ull;
    return 3;
  }
  remainder *= mod_inv1;

  if (t == 4 || remainder * mod_inv1 > max_quotient1) {
    n = (remainder >> 4) + quotient * 10000ull;
    return 4;
  }
  remainder *= mod_inv1;

  if (t == 5 || remainder * mod_inv1 > max_quotient1) {
    n = (remainder >> 5) + quotient * 1000ull;
    return 5;
  }
  remainder *= mod_inv1;

  if (t == 6 || remainder * mod_inv1 > max_quotient1) {
    n = (remainder >> 6) + quotient * 100ull;
    return 6;
  }
  remainder *= mod_inv1;

  n = (remainder >> 7) + quotient * 10ull;
  return 7;
}

// The main algorithm for shorter interval case
template <class T>
FMT_INLINE decimal_fp<T> shorter_interval_case(int exponent) FMT_NOEXCEPT {
  decimal_fp<T> ret_value;
  // Compute k and beta
  const int minus_k = floor_log10_pow2_minus_log10_4_over_3(exponent);
  const int beta_minus_1 = exponent + floor_log2_pow10(-minus_k);

  // Compute xi and zi
  using cache_entry_type = typename cache_accessor<T>::cache_entry_type;
  const cache_entry_type cache = cache_accessor<T>::get_cached_power(-minus_k);

  auto xi = cache_accessor<T>::compute_left_endpoint_for_shorter_interval_case(
      cache, beta_minus_1);
  auto zi = cache_accessor<T>::compute_right_endpoint_for_shorter_interval_case(
      cache, beta_minus_1);

  // If the left endpoint is not an integer, increase it
  if (!is_left_endpoint_integer_shorter_interval<T>(exponent)) ++xi;

  // Try bigger divisor
  ret_value.significand = zi / 10;

  // If succeed, remove trailing zeros if necessary and return
  if (ret_value.significand * 10 >= xi) {
    ret_value.exponent = minus_k + 1;
    ret_value.exponent += remove_trailing_zeros(ret_value.significand);
    return ret_value;
  }

  // Otherwise, compute the round-up of y
  ret_value.significand =
      cache_accessor<T>::compute_round_up_for_shorter_interval_case(
          cache, beta_minus_1);
  ret_value.exponent = minus_k;

  // When tie occurs, choose one of them according to the rule
  if (exponent >= float_info<T>::shorter_interval_tie_lower_threshold &&
      exponent <= float_info<T>::shorter_interval_tie_upper_threshold) {
    ret_value.significand = ret_value.significand % 2 == 0
                                ? ret_value.significand
                                : ret_value.significand - 1;
  } else if (ret_value.significand < xi) {
    ++ret_value.significand;
  }
  return ret_value;
}

template <typename T> decimal_fp<T> to_decimal(T x) FMT_NOEXCEPT {
  // Step 1: integer promotion & Schubfach multiplier calculation.

  using carrier_uint = typename float_info<T>::carrier_uint;
  using cache_entry_type = typename cache_accessor<T>::cache_entry_type;
  auto br = bit_cast<carrier_uint>(x);

  // Extract significand bits and exponent bits.
  const carrier_uint significand_mask =
      (static_cast<carrier_uint>(1) << float_info<T>::significand_bits) - 1;
  carrier_uint significand = (br & significand_mask);
  int exponent = static_cast<int>((br & exponent_mask<T>()) >>
                                  float_info<T>::significand_bits);

  if (exponent != 0) {  // Check if normal.
    exponent += float_info<T>::exponent_bias - float_info<T>::significand_bits;

    // Shorter interval case; proceed like Schubfach.
    if (significand == 0) return shorter_interval_case<T>(exponent);

    significand |=
        (static_cast<carrier_uint>(1) << float_info<T>::significand_bits);
  } else {
    // Subnormal case; the interval is always regular.
    if (significand == 0) return {0, 0};
    exponent = float_info<T>::min_exponent - float_info<T>::significand_bits;
  }

  const bool include_left_endpoint = (significand % 2 == 0);
  const bool include_right_endpoint = include_left_endpoint;

  // Compute k and beta.
  const int minus_k = floor_log10_pow2(exponent) - float_info<T>::kappa;
  const cache_entry_type cache = cache_accessor<T>::get_cached_power(-minus_k);
  const int beta_minus_1 = exponent + floor_log2_pow10(-minus_k);

  // Compute zi and deltai
  // 10^kappa <= deltai < 10^(kappa + 1)
  const uint32_t deltai = cache_accessor<T>::compute_delta(cache, beta_minus_1);
  const carrier_uint two_fc = significand << 1;
  const carrier_uint two_fr = two_fc | 1;
  const carrier_uint zi =
      cache_accessor<T>::compute_mul(two_fr << beta_minus_1, cache);

  // Step 2: Try larger divisor; remove trailing zeros if necessary

  // Using an upper bound on zi, we might be able to optimize the division
  // better than the compiler; we are computing zi / big_divisor here
  decimal_fp<T> ret_value;
  ret_value.significand = divide_by_10_to_kappa_plus_1(zi);
  uint32_t r = static_cast<uint32_t>(zi - float_info<T>::big_divisor *
                                              ret_value.significand);

  if (r > deltai) {
    goto small_divisor_case_label;
  } else if (r < deltai) {
    // Exclude the right endpoint if necessary
    if (r == 0 && !include_right_endpoint &&
        is_endpoint_integer<T>(two_fr, exponent, minus_k)) {
      --ret_value.significand;
      r = float_info<T>::big_divisor;
      goto small_divisor_case_label;
    }
  } else {
    // r == deltai; compare fractional parts
    // Check conditions in the order different from the paper
    // to take advantage of short-circuiting
    const carrier_uint two_fl = two_fc - 1;
    if ((!include_left_endpoint ||
         !is_endpoint_integer<T>(two_fl, exponent, minus_k)) &&
        !cache_accessor<T>::compute_mul_parity(two_fl, cache, beta_minus_1)) {
      goto small_divisor_case_label;
    }
  }
  ret_value.exponent = minus_k + float_info<T>::kappa + 1;

  // We may need to remove trailing zeros
  ret_value.exponent += remove_trailing_zeros(ret_value.significand);
  return ret_value;

  // Step 3: Find the significand with the smaller divisor

small_divisor_case_label:
  ret_value.significand *= 10;
  ret_value.exponent = minus_k + float_info<T>::kappa;

  const uint32_t mask = (1u << float_info<T>::kappa) - 1;
  auto dist = r - (deltai / 2) + (float_info<T>::small_divisor / 2);

  // Is dist divisible by 2^kappa?
  if ((dist & mask) == 0) {
    const bool approx_y_parity =
        ((dist ^ (float_info<T>::small_divisor / 2)) & 1) != 0;
    dist >>= float_info<T>::kappa;

    // Is dist divisible by 5^kappa?
    if (check_divisibility_and_divide_by_pow5<float_info<T>::kappa>(dist)) {
      ret_value.significand += dist;

      // Check z^(f) >= epsilon^(f)
      // We have either yi == zi - epsiloni or yi == (zi - epsiloni) - 1,
      // where yi == zi - epsiloni if and only if z^(f) >= epsilon^(f)
      // Since there are only 2 possibilities, we only need to care about the
      // parity. Also, zi and r should have the same parity since the divisor
      // is an even number
      if (cache_accessor<T>::compute_mul_parity(two_fc, cache, beta_minus_1) !=
          approx_y_parity) {
        --ret_value.significand;
      } else {
        // If z^(f) >= epsilon^(f), we might have a tie
        // when z^(f) == epsilon^(f), or equivalently, when y is an integer
        if (is_center_integer<T>(two_fc, exponent, minus_k)) {
          ret_value.significand = ret_value.significand % 2 == 0
                                      ? ret_value.significand
                                      : ret_value.significand - 1;
        }
      }
    }
    // Is dist not divisible by 5^kappa?
    else {
      ret_value.significand += dist;
    }
  }
  // Is dist not divisible by 2^kappa?
  else {
    // Since we know dist is small, we might be able to optimize the division
    // better than the compiler; we are computing dist / small_divisor here
    ret_value.significand +=
        small_division_by_pow10<float_info<T>::kappa>(dist);
  }
  return ret_value;
}
}  // namespace dragonbox

// Formats a floating-point number using a variation of the Fixed-Precision
// Positive Floating-Point Printout ((FPP)^2) algorithm by Steele & White:
// https://fmt.dev/papers/p372-steele.pdf.
FMT_CONSTEXPR20 inline void format_dragon(fp value, bool is_predecessor_closer,
                                          int num_digits, buffer<char>& buf,
                                          int& exp10) {
  bigint numerator;    // 2 * R in (FPP)^2.
  bigint denominator;  // 2 * S in (FPP)^2.
  // lower and upper are differences between value and corresponding boundaries.
  bigint lower;             // (M^- in (FPP)^2).
  bigint upper_store;       // upper's value if different from lower.
  bigint* upper = nullptr;  // (M^+ in (FPP)^2).
  // Shift numerator and denominator by an extra bit or two (if lower boundary
  // is closer) to make lower and upper integers. This eliminates multiplication
  // by 2 during later computations.
  int shift = is_predecessor_closer ? 2 : 1;
  uint64_t significand = value.f << shift;
  if (value.e >= 0) {
    numerator.assign(significand);
    numerator <<= value.e;
    lower.assign(1);
    lower <<= value.e;
    if (shift != 1) {
      upper_store.assign(1);
      upper_store <<= value.e + 1;
      upper = &upper_store;
    }
    denominator.assign_pow10(exp10);
    denominator <<= shift;
  } else if (exp10 < 0) {
    numerator.assign_pow10(-exp10);
    lower.assign(numerator);
    if (shift != 1) {
      upper_store.assign(numerator);
      upper_store <<= 1;
      upper = &upper_store;
    }
    numerator *= significand;
    denominator.assign(1);
    denominator <<= shift - value.e;
  } else {
    numerator.assign(significand);
    denominator.assign_pow10(exp10);
    denominator <<= shift - value.e;
    lower.assign(1);
    if (shift != 1) {
      upper_store.assign(1ULL << 1);
      upper = &upper_store;
    }
  }
  // Invariant: value == (numerator / denominator) * pow(10, exp10).
  if (num_digits < 0) {
    // Generate the shortest representation.
    if (!upper) upper = &lower;
    bool even = (value.f & 1) == 0;
    num_digits = 0;
    char* data = buf.data();
    for (;;) {
      int digit = numerator.divmod_assign(denominator);
      bool low = compare(numerator, lower) - even < 0;  // numerator <[=] lower.
      // numerator + upper >[=] pow10:
      bool high = add_compare(numerator, *upper, denominator) + even > 0;
      data[num_digits++] = static_cast<char>('0' + digit);
      if (low || high) {
        if (!low) {
          ++data[num_digits - 1];
        } else if (high) {
          int result = add_compare(numerator, numerator, denominator);
          // Round half to even.
          if (result > 0 || (result == 0 && (digit % 2) != 0))
            ++data[num_digits - 1];
        }
        buf.try_resize(to_unsigned(num_digits));
        exp10 -= num_digits - 1;
        return;
      }
      numerator *= 10;
      lower *= 10;
      if (upper != &lower) *upper *= 10;
    }
  }
  // Generate the given number of digits.
  exp10 -= num_digits - 1;
  if (num_digits == 0) {
    denominator *= 10;
    auto digit = add_compare(numerator, numerator, denominator) > 0 ? '1' : '0';
    buf.push_back(digit);
    return;
  }
  buf.try_resize(to_unsigned(num_digits));
  for (int i = 0; i < num_digits - 1; ++i) {
    int digit = numerator.divmod_assign(denominator);
    buf[i] = static_cast<char>('0' + digit);
    numerator *= 10;
  }
  int digit = numerator.divmod_assign(denominator);
  auto result = add_compare(numerator, numerator, denominator);
  if (result > 0 || (result == 0 && (digit % 2) != 0)) {
    if (digit == 9) {
      const auto overflow = '0' + 10;
      buf[num_digits - 1] = overflow;
      // Propagate the carry.
      for (int i = num_digits - 1; i > 0 && buf[i] == overflow; --i) {
        buf[i] = '0';
        ++buf[i - 1];
      }
      if (buf[0] == overflow) {
        buf[0] = '1';
        ++exp10;
      }
      return;
    }
    ++digit;
  }
  buf[num_digits - 1] = static_cast<char>('0' + digit);
}

template <typename Float>
FMT_HEADER_ONLY_CONSTEXPR20 int format_float(Float value, int precision,
                                             float_specs specs,
                                             buffer<char>& buf) {
  // float is passed as double to reduce the number of instantiations.
  static_assert(!std::is_same<Float, float>::value, "");
  FMT_ASSERT(value >= 0, "value is negative");

  const bool fixed = specs.format == float_format::fixed;
  if (value <= 0) {  // <= instead of == to silence a warning.
    if (precision <= 0 || !fixed) {
      buf.push_back('0');
      return 0;
    }
    buf.try_resize(to_unsigned(precision));
    fill_n(buf.data(), precision, '0');
    return -precision;
  }

  if (specs.fallback) return snprintf_float(value, precision, specs, buf);

  if (!is_constant_evaluated() && precision < 0) {
    // Use Dragonbox for the shortest format.
    if (specs.binary32) {
      auto dec = dragonbox::to_decimal(static_cast<float>(value));
      write<char>(buffer_appender<char>(buf), dec.significand);
      return dec.exponent;
    }
    auto dec = dragonbox::to_decimal(static_cast<double>(value));
    write<char>(buffer_appender<char>(buf), dec.significand);
    return dec.exponent;
  }

  int exp = 0;
  bool use_dragon = true;
  if (is_fast_float<Float>()) {
    // Use Grisu + Dragon4 for the given precision:
    // https://www.cs.tufts.edu/~nr/cs257/archive/florian-loitsch/printf.pdf.
    const int min_exp = -60;  // alpha in Grisu.
    int cached_exp10 = 0;     // K in Grisu.
    fp normalized = normalize(fp(value));
    const auto cached_pow = get_cached_power(
        min_exp - (normalized.e + fp::num_significand_bits), cached_exp10);
    normalized = normalized * cached_pow;
    gen_digits_handler handler{buf.data(), 0, precision, -cached_exp10, fixed};
    if (grisu_gen_digits(normalized, 1, exp, handler) != digits::error &&
        !is_constant_evaluated()) {
      exp += handler.exp10;
      buf.try_resize(to_unsigned(handler.size));
      use_dragon = false;
    } else {
      exp += handler.size - cached_exp10 - 1;
      precision = handler.precision;
    }
  }
  if (use_dragon) {
    auto f = fp();
    bool is_predecessor_closer =
        specs.binary32 ? f.assign(static_cast<float>(value)) : f.assign(value);
    // Limit precision to the maximum possible number of significant digits in
    // an IEEE754 double because we don't need to generate zeros.
    const int max_double_digits = 767;
    if (precision > max_double_digits) precision = max_double_digits;
    format_dragon(f, is_predecessor_closer, precision, buf, exp);
  }
  if (!fixed && !specs.showpoint) {
    // Remove trailing zeros.
    auto num_digits = buf.size();
    while (num_digits > 0 && buf[num_digits - 1] == '0') {
      --num_digits;
      ++exp;
    }
    buf.try_resize(num_digits);
  }
  return exp;
}

template <typename T>
int snprintf_float(T value, int precision, float_specs specs,
                   buffer<char>& buf) {
  // Buffer capacity must be non-zero, otherwise MSVC's vsnprintf_s will fail.
  FMT_ASSERT(buf.capacity() > buf.size(), "empty buffer");
  static_assert(!std::is_same<T, float>::value, "");

  // Subtract 1 to account for the difference in precision since we use %e for
  // both general and exponent format.
  if (specs.format == float_format::general ||
      specs.format == float_format::exp)
    precision = (precision >= 0 ? precision : 6) - 1;

  // Build the format string.
  enum { max_format_size = 7 };  // The longest format is "%#.*Le".
  char format[max_format_size];
  char* format_ptr = format;
  *format_ptr++ = '%';
  if (specs.showpoint && specs.format == float_format::hex) *format_ptr++ = '#';
  if (precision >= 0) {
    *format_ptr++ = '.';
    *format_ptr++ = '*';
  }
  if (std::is_same<T, long double>()) *format_ptr++ = 'L';
  *format_ptr++ = specs.format != float_format::hex
                      ? (specs.format == float_format::fixed ? 'f' : 'e')
                      : (specs.upper ? 'A' : 'a');
  *format_ptr = '\0';

  // Format using snprintf.
  auto offset = buf.size();
  for (;;) {
    auto begin = buf.data() + offset;
    auto capacity = buf.capacity() - offset;
#ifdef FMT_FUZZ
    if (precision > 100000)
      throw std::runtime_error(
          "fuzz mode - avoid large allocation inside snprintf");
#endif
    // Suppress the warning about a nonliteral format string.
    // Cannot use auto because of a bug in MinGW (#1532).
    int (*snprintf_ptr)(char*, size_t, const char*, ...) = FMT_SNPRINTF;
    int result = precision >= 0
                     ? snprintf_ptr(begin, capacity, format, precision, value)
                     : snprintf_ptr(begin, capacity, format, value);
    if (result < 0) {
      // The buffer will grow exponentially.
      buf.try_reserve(buf.capacity() + 1);
      continue;
    }
    auto size = to_unsigned(result);
    // Size equal to capacity means that the last character was truncated.
    if (size >= capacity) {
      buf.try_reserve(size + offset + 1);  // Add 1 for the terminating '\0'.
      continue;
    }
    auto is_digit = [](char c) { return c >= '0' && c <= '9'; };
    if (specs.format == float_format::fixed) {
      if (precision == 0) {
        buf.try_resize(size);
        return 0;
      }
      // Find and remove the decimal point.
      auto end = begin + size, p = end;
      do {
        --p;
      } while (is_digit(*p));
      int fraction_size = static_cast<int>(end - p - 1);
      std::memmove(p, p + 1, to_unsigned(fraction_size));
      buf.try_resize(size - 1);
      return -fraction_size;
    }
    if (specs.format == float_format::hex) {
      buf.try_resize(size + offset);
      return 0;
    }
    // Find and parse the exponent.
    auto end = begin + size, exp_pos = end;
    do {
      --exp_pos;
    } while (*exp_pos != 'e');
    char sign = exp_pos[1];
    FMT_ASSERT(sign == '+' || sign == '-', "");
    int exp = 0;
    auto p = exp_pos + 2;  // Skip 'e' and sign.
    do {
      FMT_ASSERT(is_digit(*p), "");
      exp = exp * 10 + (*p++ - '0');
    } while (p != end);
    if (sign == '-') exp = -exp;
    int fraction_size = 0;
    if (exp_pos != begin + 1) {
      // Remove trailing zeros.
      auto fraction_end = exp_pos - 1;
      while (*fraction_end == '0') --fraction_end;
      // Move the fractional part left to get rid of the decimal point.
      fraction_size = static_cast<int>(fraction_end - begin - 1);
      std::memmove(begin + 1, begin + 2, to_unsigned(fraction_size));
    }
    buf.try_resize(to_unsigned(fraction_size) + offset + 1);
    return exp - fraction_size;
  }
}
}  // namespace detail

template <> struct formatter<detail::bigint> {
  FMT_CONSTEXPR format_parse_context::iterator parse(
      format_parse_context& ctx) {
    return ctx.begin();
  }

  format_context::iterator format(const detail::bigint& n,
                                  format_context& ctx) {
    auto out = ctx.out();
    bool first = true;
    for (auto i = n.bigits_.size(); i > 0; --i) {
      auto value = n.bigits_[i - 1u];
      if (first) {
        out = format_to(out, FMT_STRING("{:x}"), value);
        first = false;
        continue;
      }
      out = format_to(out, FMT_STRING("{:08x}"), value);
    }
    if (n.exp_ > 0)
      out = format_to(out, FMT_STRING("p{}"),
                      n.exp_ * detail::bigint::bigit_bits);
    return out;
  }
};

FMT_FUNC detail::utf8_to_utf16::utf8_to_utf16(string_view s) {
  for_each_codepoint(s, [this](uint32_t cp, string_view) {
    if (cp == invalid_code_point) FMT_THROW(std::runtime_error("invalid utf8"));
    if (cp <= 0xFFFF) {
      buffer_.push_back(static_cast<wchar_t>(cp));
    } else {
      cp -= 0x10000;
      buffer_.push_back(static_cast<wchar_t>(0xD800 + (cp >> 10)));
      buffer_.push_back(static_cast<wchar_t>(0xDC00 + (cp & 0x3FF)));
    }
    return true;
  });
  buffer_.push_back(0);
}

FMT_FUNC void format_system_error(detail::buffer<char>& out, int error_code,
                                  const char* message) FMT_NOEXCEPT {
  FMT_TRY {
    auto ec = std::error_code(error_code, std::generic_category());
    write(std::back_inserter(out), std::system_error(ec, message).what());
    return;
  }
  FMT_CATCH(...) {}
  format_error_code(out, error_code, message);
}

FMT_FUNC void report_system_error(int error_code,
                                  const char* message) FMT_NOEXCEPT {
  report_error(format_system_error, error_code, message);
}

// DEPRECATED!
// This function is defined here and not inline for ABI compatiblity.
FMT_FUNC void detail::error_handler::on_error(const char* message) {
  throw_format_error(message);
}

FMT_FUNC std::string vformat(string_view fmt, format_args args) {
  // Don't optimize the "{}" case to keep the binary size small and because it
  // can be better optimized in fmt::format anyway.
  auto buffer = memory_buffer();
  detail::vformat_to(buffer, fmt, args);
  return to_string(buffer);
}

#ifdef _WIN32
namespace detail {
using dword = conditional_t<sizeof(long) == 4, unsigned long, unsigned>;
extern "C" __declspec(dllimport) int __stdcall WriteConsoleW(  //
    void*, const void*, dword, dword*, void*);
}  // namespace detail
#endif

namespace detail {
FMT_FUNC void print(std::FILE* f, string_view text) {
#ifdef _WIN32
  auto fd = _fileno(f);
  if (_isatty(fd)) {
    detail::utf8_to_utf16 u16(string_view(text.data(), text.size()));
    auto written = detail::dword();
    if (detail::WriteConsoleW(reinterpret_cast<void*>(_get_osfhandle(fd)),
                              u16.c_str(), static_cast<uint32_t>(u16.size()),
                              &written, nullptr)) {
      return;
    }
    // Fallback to fwrite on failure. It can happen if the output has been
    // redirected to NUL.
  }
#endif
  detail::fwrite_fully(text.data(), 1, text.size(), f);
}
}  // namespace detail

FMT_FUNC void vprint(std::FILE* f, string_view format_str, format_args args) {
  memory_buffer buffer;
  detail::vformat_to(buffer, format_str, args);
  detail::print(f, {buffer.data(), buffer.size()});
}

#ifdef _WIN32
// Print assuming legacy (non-Unicode) encoding.
FMT_FUNC void detail::vprint_mojibake(std::FILE* f, string_view format_str,
                                      format_args args) {
  memory_buffer buffer;
  detail::vformat_to(buffer, format_str,
                     basic_format_args<buffer_context<char>>(args));
  fwrite_fully(buffer.data(), 1, buffer.size(), f);
}
#endif

FMT_FUNC void vprint(string_view format_str, format_args args) {
  vprint(stdout, format_str, args);
}

FMT_END_NAMESPACE

#endif  // FMT_FORMAT_INL_H_

#else
#  define FMT_FUNC
#endif

#endif  // FMT_FORMAT_H_

#endif
#endif  // FMT_CORE_H_

// #    include <spdlog/fmt/bundled/format.h>

#else // SPDLOG_FMT_EXTERNAL is defined - use external fmtlib
#    include <fmt/core.h>
#    include <fmt/format.h>
#endif


#if !defined(SPDLOG_USE_STD_FORMAT) && FMT_VERSION >= 80000 // backward compatibility with fmt versions older than 8
#    define SPDLOG_FMT_RUNTIME(format_string) fmt::runtime(format_string)
#    define SPDLOG_FMT_STRING(format_string) FMT_STRING(format_string)
#    if defined(SPDLOG_WCHAR_FILENAMES) || defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT)
// #        include <spdlog/fmt/xchar.h>
//
// Copyright(c) 2016 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//


//
// include bundled or external copy of fmtlib's xchar support
//

#if !defined(SPDLOG_USE_STD_FORMAT)
#    if !defined(SPDLOG_FMT_EXTERNAL)
#        ifdef SPDLOG_HEADER_ONLY
#            ifndef FMT_HEADER_ONLY
#                define FMT_HEADER_ONLY
#            endif
#        endif
// #        include <spdlog/fmt/bundled/xchar.h>

#    else
#        include <fmt/xchar.h>
#    endif
#endif

#    endif
#else
#    define SPDLOG_FMT_RUNTIME(format_string) format_string
#    define SPDLOG_FMT_STRING(format_string) format_string
#endif

// visual studio up to 2013 does not support noexcept nor constexpr
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#    define SPDLOG_NOEXCEPT _NOEXCEPT
#    define SPDLOG_CONSTEXPR
#    define SPDLOG_CONSTEXPR_FUNC
#else
#    define SPDLOG_NOEXCEPT noexcept
#    define SPDLOG_CONSTEXPR constexpr
#    if __cplusplus >= 201402L
#        define SPDLOG_CONSTEXPR_FUNC constexpr
#    else
#        define SPDLOG_CONSTEXPR_FUNC
#    endif
#endif

#if defined(__GNUC__) || defined(__clang__)
#    define SPDLOG_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#    define SPDLOG_DEPRECATED __declspec(deprecated)
#else
#    define SPDLOG_DEPRECATED
#endif

// disable thread local on msvc 2013
#ifndef SPDLOG_NO_TLS
#    if (defined(_MSC_VER) && (_MSC_VER < 1900)) || defined(__cplusplus_winrt)
#        define SPDLOG_NO_TLS 1
#    endif
#endif

#ifndef SPDLOG_FUNCTION
#    define SPDLOG_FUNCTION static_cast<const char *>(__FUNCTION__)
#endif

#ifdef SPDLOG_NO_EXCEPTIONS
#    define SPDLOG_TRY
#    define SPDLOG_THROW(ex)                                                                                                               \
        do                                                                                                                                 \
        {                                                                                                                                  \
            printf("spdlog fatal error: %s\n", ex.what());                                                                                 \
            std::abort();                                                                                                                  \
        } while (0)
#    define SPDLOG_CATCH_STD
#else
#    define SPDLOG_TRY try
#    define SPDLOG_THROW(ex) throw(ex)
#    define SPDLOG_CATCH_STD                                                                                                               \
        catch (const std::exception &) {}
#endif

namespace spdlog {

class formatter;

namespace sinks {
class sink;
}

#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
using filename_t = std::wstring;
// allow macro expansion to occur in SPDLOG_FILENAME_T
#    define SPDLOG_FILENAME_T_INNER(s) L##s
#    define SPDLOG_FILENAME_T(s) SPDLOG_FILENAME_T_INNER(s)
#else
using filename_t = std::string;
#    define SPDLOG_FILENAME_T(s) s
#endif

using log_clock = std::chrono::system_clock;
using sink_ptr = std::shared_ptr<sinks::sink>;
using sinks_init_list = std::initializer_list<sink_ptr>;
using err_handler = std::function<void(const std::string &err_msg)>;
#ifdef SPDLOG_USE_STD_FORMAT
namespace fmt_lib = std;

using string_view_t = std::string_view;
using memory_buf_t = std::string;

template<typename... Args>
using format_string_t = std::string_view;

template<class T, class Char = char>
struct is_convertible_to_basic_format_string : std::integral_constant<bool, std::is_convertible<T, std::basic_string_view<Char>>::value>
{};

#    if defined(SPDLOG_WCHAR_FILENAMES) || defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT)
using wstring_view_t = std::wstring_view;
using wmemory_buf_t = std::wstring;

template<typename... Args>
using wformat_string_t = std::wstring_view;
#    endif
#    define SPDLOG_BUF_TO_STRING(x) x
#else // use fmt lib instead of std::format
namespace fmt_lib = fmt;

using string_view_t = fmt::basic_string_view<char>;
using memory_buf_t = fmt::basic_memory_buffer<char, 250>;

template<typename... Args>
using format_string_t = fmt::format_string<Args...>;

template<class T>
using remove_cvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

// clang doesn't like SFINAE disabled constructor in std::is_convertible<> so have to repeat the condition from basic_format_string here,
// in addition, fmt::basic_runtime<Char> is only convertible to basic_format_string<Char> but not basic_string_view<Char>
template<class T, class Char = char>
struct is_convertible_to_basic_format_string
    : std::integral_constant<bool,
          std::is_convertible<T, fmt::basic_string_view<Char>>::value || std::is_same<remove_cvref_t<T>, fmt::basic_runtime<Char>>::value>
{};

#    if defined(SPDLOG_WCHAR_FILENAMES) || defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT)
using wstring_view_t = fmt::basic_string_view<wchar_t>;
using wmemory_buf_t = fmt::basic_memory_buffer<wchar_t, 250>;

template<typename... Args>
using wformat_string_t = fmt::wformat_string<Args...>;
#    endif
#    define SPDLOG_BUF_TO_STRING(x) fmt::to_string(x)
#endif

#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT
#    ifndef _WIN32
#        error SPDLOG_WCHAR_TO_UTF8_SUPPORT only supported on windows
#    endif // _WIN32
#endif     // SPDLOG_WCHAR_TO_UTF8_SUPPORT

template<class T>
struct is_convertible_to_any_format_string : std::integral_constant<bool, is_convertible_to_basic_format_string<T, char>::value ||
                                                                              is_convertible_to_basic_format_string<T, wchar_t>::value>
{};

#if defined(SPDLOG_NO_ATOMIC_LEVELS)
using level_t = details::null_atomic_int;
#else
using level_t = std::atomic<int>;
#endif

#define SPDLOG_LEVEL_TRACE 0
#define SPDLOG_LEVEL_DEBUG 1
#define SPDLOG_LEVEL_INFO 2
#define SPDLOG_LEVEL_WARN 3
#define SPDLOG_LEVEL_ERROR 4
#define SPDLOG_LEVEL_CRITICAL 5
#define SPDLOG_LEVEL_OFF 6

#if !defined(SPDLOG_ACTIVE_LEVEL)
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

// Log level enum
namespace level {
enum level_enum : int
{
    trace = SPDLOG_LEVEL_TRACE,
    debug = SPDLOG_LEVEL_DEBUG,
    info = SPDLOG_LEVEL_INFO,
    warn = SPDLOG_LEVEL_WARN,
    err = SPDLOG_LEVEL_ERROR,
    critical = SPDLOG_LEVEL_CRITICAL,
    off = SPDLOG_LEVEL_OFF,
    n_levels
};

#define SPDLOG_LEVEL_NAME_TRACE spdlog::string_view_t("trace", 5)
#define SPDLOG_LEVEL_NAME_DEBUG spdlog::string_view_t("debug", 5)
#define SPDLOG_LEVEL_NAME_INFO spdlog::string_view_t("info", 4)
#define SPDLOG_LEVEL_NAME_WARNING spdlog::string_view_t("warning", 7)
#define SPDLOG_LEVEL_NAME_ERROR spdlog::string_view_t("error", 5)
#define SPDLOG_LEVEL_NAME_CRITICAL spdlog::string_view_t("critical", 8)
#define SPDLOG_LEVEL_NAME_OFF spdlog::string_view_t("off", 3)

#if !defined(SPDLOG_LEVEL_NAMES)
#    define SPDLOG_LEVEL_NAMES                                                                                                             \
        {                                                                                                                                  \
            SPDLOG_LEVEL_NAME_TRACE, SPDLOG_LEVEL_NAME_DEBUG, SPDLOG_LEVEL_NAME_INFO, SPDLOG_LEVEL_NAME_WARNING, SPDLOG_LEVEL_NAME_ERROR,  \
                SPDLOG_LEVEL_NAME_CRITICAL, SPDLOG_LEVEL_NAME_OFF                                                                          \
        }
#endif

#if !defined(SPDLOG_SHORT_LEVEL_NAMES)

#    define SPDLOG_SHORT_LEVEL_NAMES                                                                                                       \
        {                                                                                                                                  \
            "T", "D", "I", "W", "E", "C", "O"                                                                                              \
        }
#endif

SPDLOG_API const string_view_t &to_string_view(spdlog::level::level_enum l) SPDLOG_NOEXCEPT;
SPDLOG_API const char *to_short_c_str(spdlog::level::level_enum l) SPDLOG_NOEXCEPT;
SPDLOG_API spdlog::level::level_enum from_str(const std::string &name) SPDLOG_NOEXCEPT;

} // namespace level

//
// Color mode used by sinks with color support.
//
enum class color_mode
{
    always,
    automatic,
    never
};

//
// Pattern time - specific time getting to use for pattern_formatter.
// local time by default
//
enum class pattern_time_type
{
    local, // log localtime
    utc    // log utc
};

//
// Log exception
//
class SPDLOG_API spdlog_ex : public std::exception
{
public:
    explicit spdlog_ex(std::string msg);
    spdlog_ex(const std::string &msg, int last_errno);
    const char *what() const SPDLOG_NOEXCEPT override;

private:
    std::string msg_;
};

[[noreturn]] SPDLOG_API void throw_spdlog_ex(const std::string &msg, int last_errno);
[[noreturn]] SPDLOG_API void throw_spdlog_ex(std::string msg);

struct source_loc
{
    SPDLOG_CONSTEXPR source_loc() = default;
    SPDLOG_CONSTEXPR source_loc(const char *filename_in, int line_in, const char *funcname_in)
        : filename{filename_in}
        , line{line_in}
        , funcname{funcname_in}
    {}

    SPDLOG_CONSTEXPR bool empty() const SPDLOG_NOEXCEPT
    {
        return line == 0;
    }
    const char *filename{nullptr};
    int line{0};
    const char *funcname{nullptr};
};

struct file_event_handlers
{
    file_event_handlers()
        : before_open(nullptr)
        , after_open(nullptr)
        , before_close(nullptr)
        , after_close(nullptr)
    {}

    std::function<void(const filename_t &filename)> before_open;
    std::function<void(const filename_t &filename, std::FILE *file_stream)> after_open;
    std::function<void(const filename_t &filename, std::FILE *file_stream)> before_close;
    std::function<void(const filename_t &filename)> after_close;
};

namespace details {

// make_unique support for pre c++14

#if __cplusplus >= 201402L // C++14 and beyond
using std::enable_if_t;
using std::make_unique;
#else
template<bool B, class T = void>
using enable_if_t = typename std::enable_if<B, T>::type;

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... args)
{
    static_assert(!std::is_array<T>::value, "arrays not supported");
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

// to avoid useless casts (see https://github.com/nlohmann/json/issues/2893#issuecomment-889152324)
template<typename T, typename U, enable_if_t<!std::is_same<T, U>::value, int> = 0>
constexpr T conditional_static_cast(U value)
{
    return static_cast<T>(value);
}

template<typename T, typename U, enable_if_t<std::is_same<T, U>::value, int> = 0>
constexpr T conditional_static_cast(U value)
{
    return value;
}

} // namespace details
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "common-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/common.h>

#endif

#include <algorithm>
#include <iterator>

namespace spdlog {
namespace level {

#if __cplusplus >= 201703L
constexpr
#endif
    static string_view_t level_string_views[] SPDLOG_LEVEL_NAMES;

static const char *short_level_names[] SPDLOG_SHORT_LEVEL_NAMES;

SPDLOG_INLINE const string_view_t &to_string_view(spdlog::level::level_enum l) SPDLOG_NOEXCEPT
{
    return level_string_views[l];
}

SPDLOG_INLINE const char *to_short_c_str(spdlog::level::level_enum l) SPDLOG_NOEXCEPT
{
    return short_level_names[l];
}

SPDLOG_INLINE spdlog::level::level_enum from_str(const std::string &name) SPDLOG_NOEXCEPT
{
    auto it = std::find(std::begin(level_string_views), std::end(level_string_views), name);
    if (it != std::end(level_string_views))
        return static_cast<level::level_enum>(it - std::begin(level_string_views));

    // check also for "warn" and "err" before giving up..
    if (name == "warn")
    {
        return level::warn;
    }
    if (name == "err")
    {
        return level::err;
    }
    return level::off;
}
} // namespace level

SPDLOG_INLINE spdlog_ex::spdlog_ex(std::string msg)
    : msg_(std::move(msg))
{}

SPDLOG_INLINE spdlog_ex::spdlog_ex(const std::string &msg, int last_errno)
{
#ifdef SPDLOG_USE_STD_FORMAT
    msg_ = std::system_error(std::error_code(last_errno, std::generic_category()), msg).what();
#else
    memory_buf_t outbuf;
    fmt::format_system_error(outbuf, last_errno, msg.c_str());
    msg_ = fmt::to_string(outbuf);
#endif
}

SPDLOG_INLINE const char *spdlog_ex::what() const SPDLOG_NOEXCEPT
{
    return msg_.c_str();
}

SPDLOG_INLINE void throw_spdlog_ex(const std::string &msg, int last_errno)
{
    SPDLOG_THROW(spdlog_ex(msg, last_errno));
}

SPDLOG_INLINE void throw_spdlog_ex(std::string msg)
{
    SPDLOG_THROW(spdlog_ex(std::move(msg)));
}

} // namespace spdlog

#endif

// #include <spdlog/details/registry.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// Loggers registry of unique name->logger pointer
// An attempt to create a logger with an already existing name will result with spdlog_ex exception.
// If user requests a non existing logger, nullptr will be returned
// This class is thread safe

// #include <spdlog/common.h>

// #include <spdlog/details/periodic_worker.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// periodic worker thread - periodically executes the given callback function.
//
// RAII over the owned thread:
//    creates the thread on construction.
//    stops and joins the thread on destruction (if the thread is executing a callback, wait for it to finish first).

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
namespace spdlog {
namespace details {

class SPDLOG_API periodic_worker
{
public:
    template<typename Rep, typename Period>
    periodic_worker(const std::function<void()> &callback_fun, std::chrono::duration<Rep, Period> interval) {
        active_ = (interval > std::chrono::duration<Rep, Period>::zero());
        if (!active_)
        {
            return;
        }

        worker_thread_ = std::thread([this, callback_fun, interval]() {
            for (;;)
            {
                std::unique_lock<std::mutex> lock(this->mutex_);
                if (this->cv_.wait_for(lock, interval, [this] { return !this->active_; }))
                {
                    return; // active_ == false, so exit this thread
                }
                callback_fun();
            }
        });
    }
    periodic_worker(const periodic_worker &) = delete;
    periodic_worker &operator=(const periodic_worker &) = delete;
    // stop the worker thread and join it
    ~periodic_worker();

private:
    bool active_;
    std::thread worker_thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
} // namespace details
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "periodic_worker-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/details/periodic_worker.h>

#endif

namespace spdlog {
namespace details {

// stop the worker thread and join it
SPDLOG_INLINE periodic_worker::~periodic_worker()
{
    if (worker_thread_.joinable())
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            active_ = false;
        }
        cv_.notify_one();
        worker_thread_.join();
    }
}

} // namespace details
} // namespace spdlog

#endif


#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

namespace spdlog {
class logger;

namespace details {
class thread_pool;

class SPDLOG_API registry
{
public:
    using log_levels = std::unordered_map<std::string, level::level_enum>;
    registry(const registry &) = delete;
    registry &operator=(const registry &) = delete;

    void register_logger(std::shared_ptr<logger> new_logger);
    void initialize_logger(std::shared_ptr<logger> new_logger);
    std::shared_ptr<logger> get(const std::string &logger_name);
    std::shared_ptr<logger> default_logger();

    // Return raw ptr to the default logger.
    // To be used directly by the spdlog default api (e.g. spdlog::info)
    // This make the default API faster, but cannot be used concurrently with set_default_logger().
    // e.g do not call set_default_logger() from one thread while calling spdlog::info() from another.
    logger *get_default_raw();

    // set default logger.
    // default logger is stored in default_logger_ (for faster retrieval) and in the loggers_ map.
    void set_default_logger(std::shared_ptr<logger> new_default_logger);

    void set_tp(std::shared_ptr<thread_pool> tp);

    std::shared_ptr<thread_pool> get_tp();

    // Set global formatter. Each sink in each logger will get a clone of this object
    void set_formatter(std::unique_ptr<formatter> formatter);

    void enable_backtrace(size_t n_messages);

    void disable_backtrace();

    void set_level(level::level_enum log_level);

    void flush_on(level::level_enum log_level);

    template<typename Rep, typename Period>
    void flush_every(std::chrono::duration<Rep, Period> interval)
    {
        std::lock_guard<std::mutex> lock(flusher_mutex_);
        auto clbk = [this]() { this->flush_all(); };
        periodic_flusher_ = details::make_unique<periodic_worker>(clbk, interval);
    }

    void set_error_handler(err_handler handler);

    void apply_all(const std::function<void(const std::shared_ptr<logger>)> &fun);

    void flush_all();

    void drop(const std::string &logger_name);

    void drop_all();

    // clean all resources and threads started by the registry
    void shutdown();

    std::recursive_mutex &tp_mutex();

    void set_automatic_registration(bool automatic_registration);

    // set levels for all existing/future loggers. global_level can be null if should not set.
    void set_levels(log_levels levels, level::level_enum *global_level);

    static registry &instance();

private:
    registry();
    ~registry();

    void throw_if_exists_(const std::string &logger_name);
    void register_logger_(std::shared_ptr<logger> new_logger);
    bool set_level_from_cfg_(logger *logger);
    std::mutex logger_map_mutex_, flusher_mutex_;
    std::recursive_mutex tp_mutex_;
    std::unordered_map<std::string, std::shared_ptr<logger>> loggers_;
    log_levels log_levels_;
    std::unique_ptr<formatter> formatter_;
    spdlog::level::level_enum global_log_level_ = level::info;
    level::level_enum flush_level_ = level::off;
    err_handler err_handler_;
    std::shared_ptr<thread_pool> tp_;
    std::unique_ptr<periodic_worker> periodic_flusher_;
    std::shared_ptr<logger> default_logger_;
    bool automatic_registration_ = true;
    size_t backtrace_n_messages_ = 0;
};

} // namespace details
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "registry-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/details/registry.h>

#endif

// #include <spdlog/common.h>

// #include <spdlog/details/periodic_worker.h>

// #include <spdlog/logger.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// Thread safe logger (except for set_error_handler())
// Has name, log level, vector of std::shared sink pointers and formatter
// Upon each log write the logger:
// 1. Checks if its log level is enough to log the message and if yes:
// 2. Call the underlying sinks to do the job.
// 3. Each sink use its own private copy of a formatter to format the message
// and send to its destination.
//
// The use of private formatter per sink provides the opportunity to cache some
// formatted data, and support for different format per sink.

// #include <spdlog/common.h>

// #include <spdlog/details/log_msg.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/common.h>

#include <string>

namespace spdlog {
namespace details {
struct SPDLOG_API log_msg
{
    log_msg() = default;
    log_msg(log_clock::time_point log_time, source_loc loc, string_view_t logger_name, level::level_enum lvl, string_view_t msg);
    log_msg(source_loc loc, string_view_t logger_name, level::level_enum lvl, string_view_t msg);
    log_msg(string_view_t logger_name, level::level_enum lvl, string_view_t msg);
    log_msg(const log_msg &other) = default;
    log_msg &operator=(const log_msg &other) = default;

    string_view_t logger_name;
    level::level_enum level{level::off};
    log_clock::time_point time;
    size_t thread_id{0};

    // wrapping the formatted text with color (updated by pattern_formatter).
    mutable size_t color_range_start{0};
    mutable size_t color_range_end{0};

    source_loc source;
    string_view_t payload;
};
} // namespace details
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "log_msg-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/details/log_msg.h>

#endif

// #include <spdlog/details/os.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/common.h>

#include <ctime> // std::time_t

namespace spdlog {
namespace details {
namespace os {

SPDLOG_API spdlog::log_clock::time_point now() SPDLOG_NOEXCEPT;

SPDLOG_API std::tm localtime(const std::time_t &time_tt) SPDLOG_NOEXCEPT;

SPDLOG_API std::tm localtime() SPDLOG_NOEXCEPT;

SPDLOG_API std::tm gmtime(const std::time_t &time_tt) SPDLOG_NOEXCEPT;

SPDLOG_API std::tm gmtime() SPDLOG_NOEXCEPT;

// eol definition
#if !defined(SPDLOG_EOL)
#    ifdef _WIN32
#        define SPDLOG_EOL "\r\n"
#    else
#        define SPDLOG_EOL "\n"
#    endif
#endif

SPDLOG_CONSTEXPR static const char *default_eol = SPDLOG_EOL;

// folder separator
#if !defined(SPDLOG_FOLDER_SEPS)
#    ifdef _WIN32
#        define SPDLOG_FOLDER_SEPS "\\/"
#    else
#        define SPDLOG_FOLDER_SEPS "/"
#    endif
#endif

SPDLOG_CONSTEXPR static const char folder_seps[] = SPDLOG_FOLDER_SEPS;
SPDLOG_CONSTEXPR static const filename_t::value_type folder_seps_filename[] = SPDLOG_FILENAME_T(SPDLOG_FOLDER_SEPS);

// fopen_s on non windows for writing
SPDLOG_API bool fopen_s(FILE **fp, const filename_t &filename, const filename_t &mode);

// Remove filename. return 0 on success
SPDLOG_API int remove(const filename_t &filename) SPDLOG_NOEXCEPT;

// Remove file if exists. return 0 on success
// Note: Non atomic (might return failure to delete if concurrently deleted by other process/thread)
SPDLOG_API int remove_if_exists(const filename_t &filename) SPDLOG_NOEXCEPT;

SPDLOG_API int rename(const filename_t &filename1, const filename_t &filename2) SPDLOG_NOEXCEPT;

// Return if file exists.
SPDLOG_API bool path_exists(const filename_t &filename) SPDLOG_NOEXCEPT;

// Return file size according to open FILE* object
SPDLOG_API size_t filesize(FILE *f);

// Return utc offset in minutes or throw spdlog_ex on failure
SPDLOG_API int utc_minutes_offset(const std::tm &tm = details::os::localtime());

// Return current thread id as size_t
// It exists because the std::this_thread::get_id() is much slower(especially
// under VS 2013)
SPDLOG_API size_t _thread_id() SPDLOG_NOEXCEPT;

// Return current thread id as size_t (from thread local storage)
SPDLOG_API size_t thread_id() SPDLOG_NOEXCEPT;

// This is avoid msvc issue in sleep_for that happens if the clock changes.
// See https://github.com/gabime/spdlog/issues/609
SPDLOG_API void sleep_for_millis(unsigned int milliseconds) SPDLOG_NOEXCEPT;

SPDLOG_API std::string filename_to_str(const filename_t &filename);

SPDLOG_API int pid() SPDLOG_NOEXCEPT;

// Determine if the terminal supports colors
// Source: https://github.com/agauniyal/rang/
SPDLOG_API bool is_color_terminal() SPDLOG_NOEXCEPT;

// Determine if the terminal attached
// Source: https://github.com/agauniyal/rang/
SPDLOG_API bool in_terminal(FILE *file) SPDLOG_NOEXCEPT;

#if (defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT) || defined(SPDLOG_WCHAR_FILENAMES)) && defined(_WIN32)
SPDLOG_API void wstr_to_utf8buf(wstring_view_t wstr, memory_buf_t &target);

SPDLOG_API void utf8_to_wstrbuf(string_view_t str, wmemory_buf_t &target);
#endif

// Return directory name from given path or empty string
// "abc/file" => "abc"
// "abc/" => "abc"
// "abc" => ""
// "abc///" => "abc//"
SPDLOG_API filename_t dir_name(const filename_t &path);

// Create a dir from the given path.
// Return true if succeeded or if this dir already exists.
SPDLOG_API bool create_dir(const filename_t &path);

// non thread safe, cross platform getenv/getenv_s
// return empty string if field not found
SPDLOG_API std::string getenv(const char *field);

} // namespace os
} // namespace details
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "os-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/details/os.h>

#endif

// #include <spdlog/common.h>


#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <thread>
#include <array>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32

#    include <io.h>      // _get_osfhandle and _isatty support
#    include <process.h> //  _get_pid support
// #    include <spdlog/details/windows_include.h>


#ifndef NOMINMAX
#    define NOMINMAX // prevent windows redefining min/max
#endif

#ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>


#    ifdef __MINGW32__
#        include <share.h>
#    endif

#    if defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT) || defined(SPDLOG_WCHAR_FILENAMES)
#        include <limits>
#    endif

#    include <direct.h> // for _mkdir/_wmkdir

#else // unix

#    include <fcntl.h>
#    include <unistd.h>

#    ifdef __linux__
#        include <sys/syscall.h> //Use gettid() syscall under linux to get thread id

#    elif defined(_AIX)
#        include <pthread.h> // for pthread_getthrds_np

#    elif defined(__DragonFly__) || defined(__FreeBSD__)
#        include <pthread_np.h> // for pthread_getthreadid_np

#    elif defined(__NetBSD__)
#        include <lwp.h> // for _lwp_self

#    elif defined(__sun)
#        include <thread.h> // for thr_self
#    endif

#endif // unix

#ifndef __has_feature          // Clang - feature checking macros.
#    define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif

namespace spdlog {
namespace details {
namespace os {

SPDLOG_INLINE spdlog::log_clock::time_point now() SPDLOG_NOEXCEPT
{

#if defined __linux__ && defined SPDLOG_CLOCK_COARSE
    timespec ts;
    ::clock_gettime(CLOCK_REALTIME_COARSE, &ts);
    return std::chrono::time_point<log_clock, typename log_clock::duration>(
        std::chrono::duration_cast<typename log_clock::duration>(std::chrono::seconds(ts.tv_sec) + std::chrono::nanoseconds(ts.tv_nsec)));

#else
    return log_clock::now();
#endif
}
SPDLOG_INLINE std::tm localtime(const std::time_t &time_tt) SPDLOG_NOEXCEPT
{

#ifdef _WIN32
    std::tm tm;
    ::localtime_s(&tm, &time_tt);
#else
    std::tm tm;
    ::localtime_r(&time_tt, &tm);
#endif
    return tm;
}

SPDLOG_INLINE std::tm localtime() SPDLOG_NOEXCEPT
{
    std::time_t now_t = ::time(nullptr);
    return localtime(now_t);
}

SPDLOG_INLINE std::tm gmtime(const std::time_t &time_tt) SPDLOG_NOEXCEPT
{

#ifdef _WIN32
    std::tm tm;
    ::gmtime_s(&tm, &time_tt);
#else
    std::tm tm;
    ::gmtime_r(&time_tt, &tm);
#endif
    return tm;
}

SPDLOG_INLINE std::tm gmtime() SPDLOG_NOEXCEPT
{
    std::time_t now_t = ::time(nullptr);
    return gmtime(now_t);
}

// fopen_s on non windows for writing
SPDLOG_INLINE bool fopen_s(FILE **fp, const filename_t &filename, const filename_t &mode)
{
#ifdef _WIN32
#    ifdef SPDLOG_WCHAR_FILENAMES
    *fp = ::_wfsopen((filename.c_str()), mode.c_str(), _SH_DENYNO);
#    else
    *fp = ::_fsopen((filename.c_str()), mode.c_str(), _SH_DENYNO);
#    endif
#    if defined(SPDLOG_PREVENT_CHILD_FD)
    if (*fp != nullptr)
    {
        auto file_handle = reinterpret_cast<HANDLE>(_get_osfhandle(::_fileno(*fp)));
        if (!::SetHandleInformation(file_handle, HANDLE_FLAG_INHERIT, 0))
        {
            ::fclose(*fp);
            *fp = nullptr;
        }
    }
#    endif
#else // unix
#    if defined(SPDLOG_PREVENT_CHILD_FD)
    const int mode_flag = mode == SPDLOG_FILENAME_T("ab") ? O_APPEND : O_TRUNC;
    const int fd = ::open((filename.c_str()), O_CREAT | O_WRONLY | O_CLOEXEC | mode_flag, mode_t(0644));
    if (fd == -1)
    {
        return true;
    }
    *fp = ::fdopen(fd, mode.c_str());
    if (*fp == nullptr)
    {
        ::close(fd);
    }
#    else
    *fp = ::fopen((filename.c_str()), mode.c_str());
#    endif
#endif

    return *fp == nullptr;
}

SPDLOG_INLINE int remove(const filename_t &filename) SPDLOG_NOEXCEPT
{
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
    return ::_wremove(filename.c_str());
#else
    return std::remove(filename.c_str());
#endif
}

SPDLOG_INLINE int remove_if_exists(const filename_t &filename) SPDLOG_NOEXCEPT
{
    return path_exists(filename) ? remove(filename) : 0;
}

SPDLOG_INLINE int rename(const filename_t &filename1, const filename_t &filename2) SPDLOG_NOEXCEPT
{
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
    return ::_wrename(filename1.c_str(), filename2.c_str());
#else
    return std::rename(filename1.c_str(), filename2.c_str());
#endif
}

// Return true if path exists (file or directory)
SPDLOG_INLINE bool path_exists(const filename_t &filename) SPDLOG_NOEXCEPT
{
#ifdef _WIN32
#    ifdef SPDLOG_WCHAR_FILENAMES
    auto attribs = ::GetFileAttributesW(filename.c_str());
#    else
    auto attribs = ::GetFileAttributesA(filename.c_str());
#    endif
    return attribs != INVALID_FILE_ATTRIBUTES;
#else // common linux/unix all have the stat system call
    struct stat buffer;
    return (::stat(filename.c_str(), &buffer) == 0);
#endif
}

#ifdef _MSC_VER
// avoid warning about unreachable statement at the end of filesize()
#    pragma warning(push)
#    pragma warning(disable : 4702)
#endif

// Return file size according to open FILE* object
SPDLOG_INLINE size_t filesize(FILE *f)
{
    if (f == nullptr)
    {
        throw_spdlog_ex("Failed getting file size. fd is null");
    }
#if defined(_WIN32) && !defined(__CYGWIN__)
    int fd = ::_fileno(f);
#    if defined(_WIN64) // 64 bits
    __int64 ret = ::_filelengthi64(fd);
    if (ret >= 0)
    {
        return static_cast<size_t>(ret);
    }

#    else // windows 32 bits
    long ret = ::_filelength(fd);
    if (ret >= 0)
    {
        return static_cast<size_t>(ret);
    }
#    endif

#else // unix
// OpenBSD and AIX doesn't compile with :: before the fileno(..)
#    if defined(__OpenBSD__) || defined(_AIX)
    int fd = fileno(f);
#    else
    int fd = ::fileno(f);
#    endif
// 64 bits(but not in osx or cygwin, where fstat64 is deprecated)
#    if (defined(__linux__) || defined(__sun) || defined(_AIX)) && (defined(__LP64__) || defined(_LP64))
    struct stat64 st;
    if (::fstat64(fd, &st) == 0)
    {
        return static_cast<size_t>(st.st_size);
    }
#    else // other unix or linux 32 bits or cygwin
    struct stat st;
    if (::fstat(fd, &st) == 0)
    {
        return static_cast<size_t>(st.st_size);
    }
#    endif
#endif
    throw_spdlog_ex("Failed getting file size from fd", errno);
    return 0; // will not be reached.
}

#ifdef _MSC_VER
#    pragma warning(pop)
#endif

// Return utc offset in minutes or throw spdlog_ex on failure
SPDLOG_INLINE int utc_minutes_offset(const std::tm &tm)
{

#ifdef _WIN32
#    if _WIN32_WINNT < _WIN32_WINNT_WS08
    TIME_ZONE_INFORMATION tzinfo;
    auto rv = ::GetTimeZoneInformation(&tzinfo);
#    else
    DYNAMIC_TIME_ZONE_INFORMATION tzinfo;
    auto rv = ::GetDynamicTimeZoneInformation(&tzinfo);
#    endif
    if (rv == TIME_ZONE_ID_INVALID)
        throw_spdlog_ex("Failed getting timezone info. ", errno);

    int offset = -tzinfo.Bias;
    if (tm.tm_isdst)
    {
        offset -= tzinfo.DaylightBias;
    }
    else
    {
        offset -= tzinfo.StandardBias;
    }
    return offset;
#else

#    if defined(sun) || defined(__sun) || defined(_AIX) || (!defined(_BSD_SOURCE) && !defined(_GNU_SOURCE))
    // 'tm_gmtoff' field is BSD extension and it's missing on SunOS/Solaris
    struct helper
    {
        static long int calculate_gmt_offset(const std::tm &localtm = details::os::localtime(), const std::tm &gmtm = details::os::gmtime())
        {
            int local_year = localtm.tm_year + (1900 - 1);
            int gmt_year = gmtm.tm_year + (1900 - 1);

            long int days = (
                // difference in day of year
                localtm.tm_yday -
                gmtm.tm_yday

                // + intervening leap days
                + ((local_year >> 2) - (gmt_year >> 2)) - (local_year / 100 - gmt_year / 100) +
                ((local_year / 100 >> 2) - (gmt_year / 100 >> 2))

                // + difference in years * 365 */
                + static_cast<long int>(local_year - gmt_year) * 365);

            long int hours = (24 * days) + (localtm.tm_hour - gmtm.tm_hour);
            long int mins = (60 * hours) + (localtm.tm_min - gmtm.tm_min);
            long int secs = (60 * mins) + (localtm.tm_sec - gmtm.tm_sec);

            return secs;
        }
    };

    auto offset_seconds = helper::calculate_gmt_offset(tm);
#    else
    auto offset_seconds = tm.tm_gmtoff;
#    endif

    return static_cast<int>(offset_seconds / 60);
#endif
}

// Return current thread id as size_t
// It exists because the std::this_thread::get_id() is much slower(especially
// under VS 2013)
SPDLOG_INLINE size_t _thread_id() SPDLOG_NOEXCEPT
{
#ifdef _WIN32
    return static_cast<size_t>(::GetCurrentThreadId());
#elif defined(__linux__)
#    if defined(__ANDROID__) && defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
#        define SYS_gettid __NR_gettid
#    endif
    return static_cast<size_t>(::syscall(SYS_gettid));
#elif defined(_AIX)
    struct __pthrdsinfo buf;
    int reg_size = 0;
    pthread_t pt = pthread_self();
    int retval = pthread_getthrds_np(&pt, PTHRDSINFO_QUERY_TID, &buf, sizeof(buf), NULL, &reg_size);
    int tid = (!retval) ? buf.__pi_tid : 0;
    return static_cast<size_t>(tid);
#elif defined(__DragonFly__) || defined(__FreeBSD__)
    return static_cast<size_t>(::pthread_getthreadid_np());
#elif defined(__NetBSD__)
    return static_cast<size_t>(::_lwp_self());
#elif defined(__OpenBSD__)
    return static_cast<size_t>(::getthrid());
#elif defined(__sun)
    return static_cast<size_t>(::thr_self());
#elif __APPLE__
    uint64_t tid;
    pthread_threadid_np(nullptr, &tid);
    return static_cast<size_t>(tid);
#else // Default to standard C++11 (other Unix)
    return static_cast<size_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
}

// Return current thread id as size_t (from thread local storage)
SPDLOG_INLINE size_t thread_id() SPDLOG_NOEXCEPT
{
#if defined(SPDLOG_NO_TLS)
    return _thread_id();
#else // cache thread id in tls
    static thread_local const size_t tid = _thread_id();
    return tid;
#endif
}

// This is avoid msvc issue in sleep_for that happens if the clock changes.
// See https://github.com/gabime/spdlog/issues/609
SPDLOG_INLINE void sleep_for_millis(unsigned int milliseconds) SPDLOG_NOEXCEPT
{
#if defined(_WIN32)
    ::Sleep(milliseconds);
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
#endif
}

// wchar support for windows file names (SPDLOG_WCHAR_FILENAMES must be defined)
#if defined(_WIN32) && defined(SPDLOG_WCHAR_FILENAMES)
SPDLOG_INLINE std::string filename_to_str(const filename_t &filename)
{
    memory_buf_t buf;
    wstr_to_utf8buf(filename, buf);
    return SPDLOG_BUF_TO_STRING(buf);
}
#else
SPDLOG_INLINE std::string filename_to_str(const filename_t &filename)
{
    return filename;
}
#endif

SPDLOG_INLINE int pid() SPDLOG_NOEXCEPT
{

#ifdef _WIN32
    return conditional_static_cast<int>(::GetCurrentProcessId());
#else
    return conditional_static_cast<int>(::getpid());
#endif
}

// Determine if the terminal supports colors
// Based on: https://github.com/agauniyal/rang/
SPDLOG_INLINE bool is_color_terminal() SPDLOG_NOEXCEPT
{
#ifdef _WIN32
    return true;
#else

    static const bool result = []() {
        const char *env_colorterm_p = std::getenv("COLORTERM");
        if (env_colorterm_p != nullptr)
        {
            return true;
        }

        static constexpr std::array<const char *, 16> terms = {{"ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm", "linux",
            "msys", "putty", "rxvt", "screen", "vt100", "xterm", "alacritty", "vt102"}};

        const char *env_term_p = std::getenv("TERM");
        if (env_term_p == nullptr)
        {
            return false;
        }

        return std::any_of(terms.begin(), terms.end(), [&](const char *term) { return std::strstr(env_term_p, term) != nullptr; });
    }();

    return result;
#endif
}

// Determine if the terminal attached
// Source: https://github.com/agauniyal/rang/
SPDLOG_INLINE bool in_terminal(FILE *file) SPDLOG_NOEXCEPT
{

#ifdef _WIN32
    return ::_isatty(_fileno(file)) != 0;
#else
    return ::isatty(fileno(file)) != 0;
#endif
}

#if (defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT) || defined(SPDLOG_WCHAR_FILENAMES)) && defined(_WIN32)
SPDLOG_INLINE void wstr_to_utf8buf(wstring_view_t wstr, memory_buf_t &target)
{
    if (wstr.size() > static_cast<size_t>((std::numeric_limits<int>::max)()) / 2 - 1)
    {
        throw_spdlog_ex("UTF-16 string is too big to be converted to UTF-8");
    }

    int wstr_size = static_cast<int>(wstr.size());
    if (wstr_size == 0)
    {
        target.resize(0);
        return;
    }

    int result_size = static_cast<int>(target.capacity());
    if ((wstr_size + 1) * 2 > result_size)
    {
        result_size = ::WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr_size, NULL, 0, NULL, NULL);
    }

    if (result_size > 0)
    {
        target.resize(result_size);
        result_size = ::WideCharToMultiByte(CP_UTF8, 0, wstr.data(), wstr_size, target.data(), result_size, NULL, NULL);

        if (result_size > 0)
        {
            target.resize(result_size);
            return;
        }
    }

    throw_spdlog_ex(fmt_lib::format("WideCharToMultiByte failed. Last error: {}", ::GetLastError()));
}

SPDLOG_INLINE void utf8_to_wstrbuf(string_view_t str, wmemory_buf_t &target)
{
    if (str.size() > static_cast<size_t>((std::numeric_limits<int>::max)()) - 1)
    {
        throw_spdlog_ex("UTF-8 string is too big to be converted to UTF-16");
    }

    int str_size = static_cast<int>(str.size());
    if (str_size == 0)
    {
        target.resize(0);
        return;
    }

    int result_size = static_cast<int>(target.capacity());
    if (str_size + 1 > result_size)
    {
        result_size = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), str_size, NULL, 0);
    }

    if (result_size > 0)
    {
        target.resize(result_size);
        result_size = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), str_size, target.data(), result_size);

        if (result_size > 0)
        {
            target.resize(result_size);
            return;
        }
    }

    throw_spdlog_ex(fmt_lib::format("MultiByteToWideChar failed. Last error: {}", ::GetLastError()));
}
#endif // (defined(SPDLOG_WCHAR_TO_UTF8_SUPPORT) || defined(SPDLOG_WCHAR_FILENAMES)) && defined(_WIN32)

// return true on success
static SPDLOG_INLINE bool mkdir_(const filename_t &path)
{
#ifdef _WIN32
#    ifdef SPDLOG_WCHAR_FILENAMES
    return ::_wmkdir(path.c_str()) == 0;
#    else
    return ::_mkdir(path.c_str()) == 0;
#    endif
#else
    return ::mkdir(path.c_str(), mode_t(0755)) == 0;
#endif
}

// create the given directory - and all directories leading to it
// return true on success or if the directory already exists
SPDLOG_INLINE bool create_dir(const filename_t &path)
{
    if (path_exists(path))
    {
        return true;
    }

    if (path.empty())
    {
        return false;
    }

    size_t search_offset = 0;
    do
    {
        auto token_pos = path.find_first_of(folder_seps_filename, search_offset);
        // treat the entire path as a folder if no folder separator not found
        if (token_pos == filename_t::npos)
        {
            token_pos = path.size();
        }

        auto subdir = path.substr(0, token_pos);

        if (!subdir.empty() && !path_exists(subdir) && !mkdir_(subdir))
        {
            return false; // return error if failed creating dir
        }
        search_offset = token_pos + 1;
    } while (search_offset < path.size());

    return true;
}

// Return directory name from given path or empty string
// "abc/file" => "abc"
// "abc/" => "abc"
// "abc" => ""
// "abc///" => "abc//"
SPDLOG_INLINE filename_t dir_name(const filename_t &path)
{
    auto pos = path.find_last_of(folder_seps_filename);
    return pos != filename_t::npos ? path.substr(0, pos) : filename_t{};
}

std::string SPDLOG_INLINE getenv(const char *field)
{

#if defined(_MSC_VER)
#    if defined(__cplusplus_winrt)
    return std::string{}; // not supported under uwp
#    else
    size_t len = 0;
    char buf[128];
    bool ok = ::getenv_s(&len, buf, sizeof(buf), field) == 0;
    return ok ? buf : std::string{};
#    endif
#else // revert to getenv
    char *buf = ::getenv(field);
    return buf ? buf : std::string{};
#endif
}

} // namespace os
} // namespace details
} // namespace spdlog

#endif


namespace spdlog {
namespace details {

SPDLOG_INLINE log_msg::log_msg(spdlog::log_clock::time_point log_time, spdlog::source_loc loc, string_view_t a_logger_name,
    spdlog::level::level_enum lvl, spdlog::string_view_t msg)
    : logger_name(a_logger_name)
    , level(lvl)
    , time(log_time)
#ifndef SPDLOG_NO_THREAD_ID
    , thread_id(os::thread_id())
#endif
    , source(loc)
    , payload(msg)
{}

SPDLOG_INLINE log_msg::log_msg(
    spdlog::source_loc loc, string_view_t a_logger_name, spdlog::level::level_enum lvl, spdlog::string_view_t msg)
    : log_msg(os::now(), loc, a_logger_name, lvl, msg)
{}

SPDLOG_INLINE log_msg::log_msg(string_view_t a_logger_name, spdlog::level::level_enum lvl, spdlog::string_view_t msg)
    : log_msg(os::now(), source_loc{}, a_logger_name, lvl, msg)
{}

} // namespace details
} // namespace spdlog

#endif

// #include <spdlog/details/backtracer.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/details/log_msg_buffer.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/details/log_msg.h>


namespace spdlog {
namespace details {

// Extend log_msg with internal buffer to store its payload.
// This is needed since log_msg holds string_views that points to stack data.

class SPDLOG_API log_msg_buffer : public log_msg
{
    memory_buf_t buffer;
    void update_string_views();

public:
    log_msg_buffer() = default;
    explicit log_msg_buffer(const log_msg &orig_msg);
    log_msg_buffer(const log_msg_buffer &other);
    log_msg_buffer(log_msg_buffer &&other) SPDLOG_NOEXCEPT;
    log_msg_buffer &operator=(const log_msg_buffer &other);
    log_msg_buffer &operator=(log_msg_buffer &&other) SPDLOG_NOEXCEPT;
};

} // namespace details
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "log_msg_buffer-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/details/log_msg_buffer.h>

#endif

namespace spdlog {
namespace details {

SPDLOG_INLINE log_msg_buffer::log_msg_buffer(const log_msg &orig_msg)
    : log_msg{orig_msg}
{
    buffer.append(logger_name.begin(), logger_name.end());
    buffer.append(payload.begin(), payload.end());
    update_string_views();
}

SPDLOG_INLINE log_msg_buffer::log_msg_buffer(const log_msg_buffer &other)
    : log_msg{other}
{
    buffer.append(logger_name.begin(), logger_name.end());
    buffer.append(payload.begin(), payload.end());
    update_string_views();
}

SPDLOG_INLINE log_msg_buffer::log_msg_buffer(log_msg_buffer &&other) SPDLOG_NOEXCEPT : log_msg{other}, buffer{std::move(other.buffer)}
{
    update_string_views();
}

SPDLOG_INLINE log_msg_buffer &log_msg_buffer::operator=(const log_msg_buffer &other)
{
    log_msg::operator=(other);
    buffer.clear();
    buffer.append(other.buffer.data(), other.buffer.data() + other.buffer.size());
    update_string_views();
    return *this;
}

SPDLOG_INLINE log_msg_buffer &log_msg_buffer::operator=(log_msg_buffer &&other) SPDLOG_NOEXCEPT
{
    log_msg::operator=(other);
    buffer = std::move(other.buffer);
    update_string_views();
    return *this;
}

SPDLOG_INLINE void log_msg_buffer::update_string_views()
{
    logger_name = string_view_t{buffer.data(), logger_name.size()};
    payload = string_view_t{buffer.data() + logger_name.size(), payload.size()};
}

} // namespace details
} // namespace spdlog

#endif

// #include <spdlog/details/circular_q.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

// circular q view of std::vector.


#include <vector>
#include <cassert>

namespace spdlog {
namespace details {
template<typename T>
class circular_q
{
    size_t max_items_ = 0;
    typename std::vector<T>::size_type head_ = 0;
    typename std::vector<T>::size_type tail_ = 0;
    size_t overrun_counter_ = 0;
    std::vector<T> v_;

public:
    using value_type = T;

    // empty ctor - create a disabled queue with no elements allocated at all
    circular_q() = default;

    explicit circular_q(size_t max_items)
        : max_items_(max_items + 1) // one item is reserved as marker for full q
        , v_(max_items_)
    {}

    circular_q(const circular_q &) = default;
    circular_q &operator=(const circular_q &) = default;

    // move cannot be default,
    // since we need to reset head_, tail_, etc to zero in the moved object
    circular_q(circular_q &&other) SPDLOG_NOEXCEPT
    {
        copy_moveable(std::move(other));
    }

    circular_q &operator=(circular_q &&other) SPDLOG_NOEXCEPT
    {
        copy_moveable(std::move(other));
        return *this;
    }

    // push back, overrun (oldest) item if no room left
    void push_back(T &&item)
    {
        if (max_items_ > 0)
        {
            v_[tail_] = std::move(item);
            tail_ = (tail_ + 1) % max_items_;

            if (tail_ == head_) // overrun last item if full
            {
                head_ = (head_ + 1) % max_items_;
                ++overrun_counter_;
            }
        }
    }

    // Return reference to the front item.
    // If there are no elements in the container, the behavior is undefined.
    const T &front() const
    {
        return v_[head_];
    }

    T &front()
    {
        return v_[head_];
    }

    // Return number of elements actually stored
    size_t size() const
    {
        if (tail_ >= head_)
        {
            return tail_ - head_;
        }
        else
        {
            return max_items_ - (head_ - tail_);
        }
    }

    // Return const reference to item by index.
    // If index is out of range 0…size()-1, the behavior is undefined.
    const T &at(size_t i) const
    {
        assert(i < size());
        return v_[(head_ + i) % max_items_];
    }

    // Pop item from front.
    // If there are no elements in the container, the behavior is undefined.
    void pop_front()
    {
        head_ = (head_ + 1) % max_items_;
    }

    bool empty() const
    {
        return tail_ == head_;
    }

    bool full() const
    {
        // head is ahead of the tail by 1
        if (max_items_ > 0)
        {
            return ((tail_ + 1) % max_items_) == head_;
        }
        return false;
    }

    size_t overrun_counter() const
    {
        return overrun_counter_;
    }

    void reset_overrun_counter()
    {
        overrun_counter_ = 0;
    }

private:
    // copy from other&& and reset it to disabled state
    void copy_moveable(circular_q &&other) SPDLOG_NOEXCEPT
    {
        max_items_ = other.max_items_;
        head_ = other.head_;
        tail_ = other.tail_;
        overrun_counter_ = other.overrun_counter_;
        v_ = std::move(other.v_);

        // put &&other in disabled, but valid state
        other.max_items_ = 0;
        other.head_ = other.tail_ = 0;
        other.overrun_counter_ = 0;
    }
};
} // namespace details
} // namespace spdlog


#include <atomic>
#include <mutex>
#include <functional>

// Store log messages in circular buffer.
// Useful for storing debug data in case of error/warning happens.

namespace spdlog {
namespace details {
class SPDLOG_API backtracer
{
    mutable std::mutex mutex_;
    std::atomic<bool> enabled_{false};
    circular_q<log_msg_buffer> messages_;

public:
    backtracer() = default;
    backtracer(const backtracer &other);

    backtracer(backtracer &&other) SPDLOG_NOEXCEPT;
    backtracer &operator=(backtracer other);

    void enable(size_t size);
    void disable();
    bool enabled() const;
    void push_back(const log_msg &msg);

    // pop all items in the q and apply the given fun on each of them.
    void foreach_pop(std::function<void(const details::log_msg &)> fun);
};

} // namespace details
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "backtracer-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/details/backtracer.h>

#endif
namespace spdlog {
namespace details {
SPDLOG_INLINE backtracer::backtracer(const backtracer &other)
{
    std::lock_guard<std::mutex> lock(other.mutex_);
    enabled_ = other.enabled();
    messages_ = other.messages_;
}

SPDLOG_INLINE backtracer::backtracer(backtracer &&other) SPDLOG_NOEXCEPT
{
    std::lock_guard<std::mutex> lock(other.mutex_);
    enabled_ = other.enabled();
    messages_ = std::move(other.messages_);
}

SPDLOG_INLINE backtracer &backtracer::operator=(backtracer other)
{
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = other.enabled();
    messages_ = std::move(other.messages_);
    return *this;
}

SPDLOG_INLINE void backtracer::enable(size_t size)
{
    std::lock_guard<std::mutex> lock{mutex_};
    enabled_.store(true, std::memory_order_relaxed);
    messages_ = circular_q<log_msg_buffer>{size};
}

SPDLOG_INLINE void backtracer::disable()
{
    std::lock_guard<std::mutex> lock{mutex_};
    enabled_.store(false, std::memory_order_relaxed);
}

SPDLOG_INLINE bool backtracer::enabled() const
{
    return enabled_.load(std::memory_order_relaxed);
}

SPDLOG_INLINE void backtracer::push_back(const log_msg &msg)
{
    std::lock_guard<std::mutex> lock{mutex_};
    messages_.push_back(log_msg_buffer{msg});
}

// pop all items in the q and apply the given fun on each of them.
SPDLOG_INLINE void backtracer::foreach_pop(std::function<void(const details::log_msg &)> fun)
{
    std::lock_guard<std::mutex> lock{mutex_};
    while (!messages_.empty())
    {
        auto &front_msg = messages_.front();
        fun(front_msg);
        messages_.pop_front();
    }
}
} // namespace details
} // namespace spdlog

#endif


#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT
#    ifndef _WIN32
#        error SPDLOG_WCHAR_TO_UTF8_SUPPORT only supported on windows
#    endif
// #    include <spdlog/details/os.h>

#endif

#include <vector>

#ifndef SPDLOG_NO_EXCEPTIONS
#    define SPDLOG_LOGGER_CATCH(location)                                                                                                  \
        catch (const std::exception &ex)                                                                                                   \
        {                                                                                                                                  \
            if (location.filename)                                                                                                         \
            {                                                                                                                              \
                err_handler_(fmt_lib::format(SPDLOG_FMT_STRING("{} [{}({})]"), ex.what(), location.filename, location.line));              \
            }                                                                                                                              \
            else                                                                                                                           \
            {                                                                                                                              \
                err_handler_(ex.what());                                                                                                   \
            }                                                                                                                              \
        }                                                                                                                                  \
        catch (...)                                                                                                                        \
        {                                                                                                                                  \
            err_handler_("Rethrowing unknown exception in logger");                                                                        \
            throw;                                                                                                                         \
        }
#else
#    define SPDLOG_LOGGER_CATCH(location)
#endif

namespace spdlog {

class SPDLOG_API logger
{
public:
    // Empty logger
    explicit logger(std::string name)
        : name_(std::move(name))
        , sinks_()
    {}

    // Logger with range on sinks
    template<typename It>
    logger(std::string name, It begin, It end)
        : name_(std::move(name))
        , sinks_(begin, end)
    {}

    // Logger with single sink
    logger(std::string name, sink_ptr single_sink)
        : logger(std::move(name), {std::move(single_sink)})
    {}

    // Logger with sinks init list
    logger(std::string name, sinks_init_list sinks)
        : logger(std::move(name), sinks.begin(), sinks.end())
    {}

    virtual ~logger() = default;

    logger(const logger &other);
    logger(logger &&other) SPDLOG_NOEXCEPT;
    logger &operator=(logger other) SPDLOG_NOEXCEPT;
    void swap(spdlog::logger &other) SPDLOG_NOEXCEPT;

    template<typename... Args>
    void log(source_loc loc, level::level_enum lvl, format_string_t<Args...> fmt, Args &&... args)
    {
        log_(loc, lvl, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void log(level::level_enum lvl, format_string_t<Args...> fmt, Args &&... args)
    {
        log(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
    }

    template<typename T>
    void log(level::level_enum lvl, const T &msg)
    {
        log(source_loc{}, lvl, msg);
    }

    // T cannot be statically converted to format string (including string_view/wstring_view)
    template<class T, typename std::enable_if<!is_convertible_to_any_format_string<const T &>::value, int>::type = 0>
    void log(source_loc loc, level::level_enum lvl, const T &msg)
    {
        log(loc, lvl, "{}", msg);
    }

    void log(log_clock::time_point log_time, source_loc loc, level::level_enum lvl, string_view_t msg)
    {
        bool log_enabled = should_log(lvl);
        bool traceback_enabled = tracer_.enabled();
        if (!log_enabled && !traceback_enabled)
        {
            return;
        }

        details::log_msg log_msg(log_time, loc, name_, lvl, msg);
        log_it_(log_msg, log_enabled, traceback_enabled);
    }

    void log(source_loc loc, level::level_enum lvl, string_view_t msg)
    {
        bool log_enabled = should_log(lvl);
        bool traceback_enabled = tracer_.enabled();
        if (!log_enabled && !traceback_enabled)
        {
            return;
        }

        details::log_msg log_msg(loc, name_, lvl, msg);
        log_it_(log_msg, log_enabled, traceback_enabled);
    }

    void log(level::level_enum lvl, string_view_t msg)
    {
        log(source_loc{}, lvl, msg);
    }

    template<typename... Args>
    void trace(format_string_t<Args...> fmt, Args &&... args)
    {
        log(level::trace, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(format_string_t<Args...> fmt, Args &&... args)
    {
        log(level::debug, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(format_string_t<Args...> fmt, Args &&... args)
    {
        log(level::info, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(format_string_t<Args...> fmt, Args &&... args)
    {
        log(level::warn, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(format_string_t<Args...> fmt, Args &&... args)
    {
        log(level::err, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(format_string_t<Args...> fmt, Args &&... args)
    {
        log(level::critical, fmt, std::forward<Args>(args)...);
    }

#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT
    template<typename... Args>
    void log(source_loc loc, level::level_enum lvl, wformat_string_t<Args...> fmt, Args &&... args)
    {
        log_(loc, lvl, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void log(level::level_enum lvl, wformat_string_t<Args...> fmt, Args &&... args)
    {
        log(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
    }

    void log(log_clock::time_point log_time, source_loc loc, level::level_enum lvl, wstring_view_t msg)
    {
        bool log_enabled = should_log(lvl);
        bool traceback_enabled = tracer_.enabled();
        if (!log_enabled && !traceback_enabled)
        {
            return;
        }

        memory_buf_t buf;
        details::os::wstr_to_utf8buf(wstring_view_t(msg.data(), msg.size()), buf);
        details::log_msg log_msg(log_time, loc, name_, lvl, string_view_t(buf.data(), buf.size()));
        log_it_(log_msg, log_enabled, traceback_enabled);
    }

    void log(source_loc loc, level::level_enum lvl, wstring_view_t msg)
    {
        bool log_enabled = should_log(lvl);
        bool traceback_enabled = tracer_.enabled();
        if (!log_enabled && !traceback_enabled)
        {
            return;
        }

        memory_buf_t buf;
        details::os::wstr_to_utf8buf(wstring_view_t(msg.data(), msg.size()), buf);
        details::log_msg log_msg(loc, name_, lvl, string_view_t(buf.data(), buf.size()));
        log_it_(log_msg, log_enabled, traceback_enabled);
    }

    void log(level::level_enum lvl, wstring_view_t msg)
    {
        log(source_loc{}, lvl, msg);
    }

    template<typename... Args>
    void trace(wformat_string_t<Args...> fmt, Args &&... args)
    {
        log(level::trace, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(wformat_string_t<Args...> fmt, Args &&... args)
    {
        log(level::debug, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(wformat_string_t<Args...> fmt, Args &&... args)
    {
        log(level::info, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(wformat_string_t<Args...> fmt, Args &&... args)
    {
        log(level::warn, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(wformat_string_t<Args...> fmt, Args &&... args)
    {
        log(level::err, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(wformat_string_t<Args...> fmt, Args &&... args)
    {
        log(level::critical, fmt, std::forward<Args>(args)...);
    }
#endif

    template<typename T>
    void trace(const T &msg)
    {
        log(level::trace, msg);
    }

    template<typename T>
    void debug(const T &msg)
    {
        log(level::debug, msg);
    }

    template<typename T>
    void info(const T &msg)
    {
        log(level::info, msg);
    }

    template<typename T>
    void warn(const T &msg)
    {
        log(level::warn, msg);
    }

    template<typename T>
    void error(const T &msg)
    {
        log(level::err, msg);
    }

    template<typename T>
    void critical(const T &msg)
    {
        log(level::critical, msg);
    }

    // return true logging is enabled for the given level.
    bool should_log(level::level_enum msg_level) const
    {
        return msg_level >= level_.load(std::memory_order_relaxed);
    }

    // return true if backtrace logging is enabled.
    bool should_backtrace() const
    {
        return tracer_.enabled();
    }

    void set_level(level::level_enum log_level);

    level::level_enum level() const;

    const std::string &name() const;

    // set formatting for the sinks in this logger.
    // each sink will get a separate instance of the formatter object.
    void set_formatter(std::unique_ptr<formatter> f);

    // set formatting for the sinks in this logger.
    // equivalent to
    //     set_formatter(make_unique<pattern_formatter>(pattern, time_type))
    // Note: each sink will get a new instance of a formatter object, replacing the old one.
    void set_pattern(std::string pattern, pattern_time_type time_type = pattern_time_type::local);

    // backtrace support.
    // efficiently store all debug/trace messages in a circular buffer until needed for debugging.
    void enable_backtrace(size_t n_messages);
    void disable_backtrace();
    void dump_backtrace();

    // flush functions
    void flush();
    void flush_on(level::level_enum log_level);
    level::level_enum flush_level() const;

    // sinks
    const std::vector<sink_ptr> &sinks() const;

    std::vector<sink_ptr> &sinks();

    // error handler
    void set_error_handler(err_handler);

    // create new logger with same sinks and configuration.
    virtual std::shared_ptr<logger> clone(std::string logger_name);

protected:
    std::string name_;
    std::vector<sink_ptr> sinks_;
    spdlog::level_t level_{level::info};
    spdlog::level_t flush_level_{level::off};
    err_handler custom_err_handler_{nullptr};
    details::backtracer tracer_;

    // common implementation for after templated public api has been resolved
    template<typename... Args>
    void log_(source_loc loc, level::level_enum lvl, string_view_t fmt, Args &&... args)
    {
        bool log_enabled = should_log(lvl);
        bool traceback_enabled = tracer_.enabled();
        if (!log_enabled && !traceback_enabled)
        {
            return;
        }
        SPDLOG_TRY
        {
            memory_buf_t buf;
#ifdef SPDLOG_USE_STD_FORMAT
            fmt_lib::vformat_to(std::back_inserter(buf), fmt, fmt_lib::make_format_args(std::forward<Args>(args)...));
#else
            // seems that fmt::detail::vformat_to(buf, ...) is ~20ns faster than fmt::vformat_to(std::back_inserter(buf),..)
            fmt::detail::vformat_to(buf, fmt, fmt::make_format_args(std::forward<Args>(args)...));
#endif

            details::log_msg log_msg(loc, name_, lvl, string_view_t(buf.data(), buf.size()));
            log_it_(log_msg, log_enabled, traceback_enabled);
        }
        SPDLOG_LOGGER_CATCH(loc)
    }

#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT
    template<typename... Args>
    void log_(source_loc loc, level::level_enum lvl, wstring_view_t fmt, Args &&... args)
    {
        bool log_enabled = should_log(lvl);
        bool traceback_enabled = tracer_.enabled();
        if (!log_enabled && !traceback_enabled)
        {
            return;
        }
        SPDLOG_TRY
        {
            // format to wmemory_buffer and convert to utf8
            wmemory_buf_t wbuf;
#    ifdef SPDLOG_USE_STD_FORMAT
            fmt_lib::vformat_to(
                std::back_inserter(wbuf), fmt, fmt_lib::make_format_args<fmt_lib::wformat_context>(std::forward<Args>(args)...));
#    else
            fmt::detail::vformat_to(wbuf, fmt, fmt::make_format_args<fmt::wformat_context>(std::forward<Args>(args)...));
#    endif

            memory_buf_t buf;
            details::os::wstr_to_utf8buf(wstring_view_t(wbuf.data(), wbuf.size()), buf);
            details::log_msg log_msg(loc, name_, lvl, string_view_t(buf.data(), buf.size()));
            log_it_(log_msg, log_enabled, traceback_enabled);
        }
        SPDLOG_LOGGER_CATCH(loc)
    }

    // T can be statically converted to wstring_view, and no formatting needed.
    template<class T, typename std::enable_if<std::is_convertible<const T &, spdlog::wstring_view_t>::value, int>::type = 0>
    void log_(source_loc loc, level::level_enum lvl, const T &msg)
    {
        bool log_enabled = should_log(lvl);
        bool traceback_enabled = tracer_.enabled();
        if (!log_enabled && !traceback_enabled)
        {
            return;
        }
        SPDLOG_TRY
        {
            memory_buf_t buf;
            details::os::wstr_to_utf8buf(msg, buf);
            details::log_msg log_msg(loc, name_, lvl, string_view_t(buf.data(), buf.size()));
            log_it_(log_msg, log_enabled, traceback_enabled);
        }
        SPDLOG_LOGGER_CATCH(loc)
    }

#endif // SPDLOG_WCHAR_TO_UTF8_SUPPORT

    // log the given message (if the given log level is high enough),
    // and save backtrace (if backtrace is enabled).
    void log_it_(const details::log_msg &log_msg, bool log_enabled, bool traceback_enabled);
    virtual void sink_it_(const details::log_msg &msg);
    virtual void flush_();
    void dump_backtrace_();
    bool should_flush_(const details::log_msg &msg);

    // handle errors during logging.
    // default handler prints the error to stderr at max rate of 1 message/sec.
    void err_handler_(const std::string &msg);
};

void swap(logger &a, logger &b);

} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "logger-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/logger.h>

#endif

// #include <spdlog/sinks/sink.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/details/log_msg.h>

// #include <spdlog/formatter.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/fmt/fmt.h>

// #include <spdlog/details/log_msg.h>


namespace spdlog {

class formatter
{
public:
    virtual ~formatter() = default;
    virtual void format(const details::log_msg &msg, memory_buf_t &dest) = 0;
    virtual std::unique_ptr<formatter> clone() const = 0;
};
} // namespace spdlog


namespace spdlog {

namespace sinks {
class SPDLOG_API sink
{
public:
    virtual ~sink() = default;
    virtual void log(const details::log_msg &msg) = 0;
    virtual void flush() = 0;
    virtual void set_pattern(const std::string &pattern) = 0;
    virtual void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) = 0;

    void set_level(level::level_enum log_level);
    level::level_enum level() const;
    bool should_log(level::level_enum msg_level) const;

protected:
    // sink log level - default is all
    level_t level_{level::trace};
};

} // namespace sinks
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "sink-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/sinks/sink.h>

#endif

// #include <spdlog/common.h>


SPDLOG_INLINE bool spdlog::sinks::sink::should_log(spdlog::level::level_enum msg_level) const
{
    return msg_level >= level_.load(std::memory_order_relaxed);
}

SPDLOG_INLINE void spdlog::sinks::sink::set_level(level::level_enum log_level)
{
    level_.store(log_level, std::memory_order_relaxed);
}

SPDLOG_INLINE spdlog::level::level_enum spdlog::sinks::sink::level() const
{
    return static_cast<spdlog::level::level_enum>(level_.load(std::memory_order_relaxed));
}

#endif

// #include <spdlog/details/backtracer.h>

// #include <spdlog/pattern_formatter.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/common.h>

// #include <spdlog/details/log_msg.h>

// #include <spdlog/details/os.h>

// #include <spdlog/formatter.h>


#include <chrono>
#include <ctime>
#include <memory>

#include <string>
#include <vector>
#include <unordered_map>

namespace spdlog {
namespace details {

// padding information.
struct padding_info
{
    enum class pad_side
    {
        left,
        right,
        center
    };

    padding_info() = default;
    padding_info(size_t width, padding_info::pad_side side, bool truncate)
        : width_(width)
        , side_(side)
        , truncate_(truncate)
        , enabled_(true)
    {}

    bool enabled() const
    {
        return enabled_;
    }
    size_t width_ = 0;
    pad_side side_ = pad_side::left;
    bool truncate_ = false;
    bool enabled_ = false;
};

class SPDLOG_API flag_formatter
{
public:
    explicit flag_formatter(padding_info padinfo)
        : padinfo_(padinfo)
    {}
    flag_formatter() = default;
    virtual ~flag_formatter() = default;
    virtual void format(const details::log_msg &msg, const std::tm &tm_time, memory_buf_t &dest) = 0;

protected:
    padding_info padinfo_;
};

} // namespace details

class SPDLOG_API custom_flag_formatter : public details::flag_formatter
{
public:
    virtual std::unique_ptr<custom_flag_formatter> clone() const = 0;

    void set_padding_info(const details::padding_info &padding)
    {
        flag_formatter::padinfo_ = padding;
    }
};

class SPDLOG_API pattern_formatter final : public formatter
{
public:
    using custom_flags = std::unordered_map<char, std::unique_ptr<custom_flag_formatter>>;

    explicit pattern_formatter(std::string pattern, pattern_time_type time_type = pattern_time_type::local,
        std::string eol = spdlog::details::os::default_eol, custom_flags custom_user_flags = custom_flags());

    // use default pattern is not given
    explicit pattern_formatter(pattern_time_type time_type = pattern_time_type::local, std::string eol = spdlog::details::os::default_eol);

    pattern_formatter(const pattern_formatter &other) = delete;
    pattern_formatter &operator=(const pattern_formatter &other) = delete;

    std::unique_ptr<formatter> clone() const override;
    void format(const details::log_msg &msg, memory_buf_t &dest) override;

    template<typename T, typename... Args>
    pattern_formatter &add_flag(char flag, Args &&... args)
    {
        custom_handlers_[flag] = details::make_unique<T>(std::forward<Args>(args)...);
        return *this;
    }
    void set_pattern(std::string pattern);
    void need_localtime(bool need = true);

private:
    std::string pattern_;
    std::string eol_;
    pattern_time_type pattern_time_type_;
    bool need_localtime_;
    std::tm cached_tm_;
    std::chrono::seconds last_log_secs_;
    std::vector<std::unique_ptr<details::flag_formatter>> formatters_;
    custom_flags custom_handlers_;

    std::tm get_time_(const details::log_msg &msg);
    template<typename Padder>
    void handle_flag_(char flag, details::padding_info padding);

    // Extract given pad spec (e.g. %8X)
    // Advance the given it pass the end of the padding spec found (if any)
    // Return padding.
    static details::padding_info handle_padspec_(std::string::const_iterator &it, std::string::const_iterator end);

    void compile_pattern_(const std::string &pattern);
};
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "pattern_formatter-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/pattern_formatter.h>

#endif

// #include <spdlog/details/fmt_helper.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)


#include <chrono>
#include <type_traits>
#include <iterator>
// #include <spdlog/fmt/fmt.h>

// #include <spdlog/common.h>


#ifdef SPDLOG_USE_STD_FORMAT
#    include <charconv>
#    include <limits>
#endif

// Some fmt helpers to efficiently format and pad ints and strings
namespace spdlog {
namespace details {
namespace fmt_helper {

inline spdlog::string_view_t to_string_view(const memory_buf_t &buf) SPDLOG_NOEXCEPT
{
    return spdlog::string_view_t{buf.data(), buf.size()};
}

inline void append_string_view(spdlog::string_view_t view, memory_buf_t &dest)
{
    auto *buf_ptr = view.data();
    dest.append(buf_ptr, buf_ptr + view.size());
}

#ifdef SPDLOG_USE_STD_FORMAT
template<typename T>
inline void append_int(T n, memory_buf_t &dest)
{
    // Buffer should be large enough to hold all digits (digits10 + 1) and a sign
    SPDLOG_CONSTEXPR const auto BUF_SIZE = std::numeric_limits<T>::digits10 + 2;
    char buf[BUF_SIZE];

    auto [ptr, ec] = std::to_chars(buf, buf + BUF_SIZE, n, 10);
    if (ec == std::errc())
    {
        dest.append(buf, ptr);
    }
    else
    {
        throw_spdlog_ex("Failed to format int", static_cast<int>(ec));
    }
}
#else
template<typename T>
inline void append_int(T n, memory_buf_t &dest)
{
    fmt::format_int i(n);
    dest.append(i.data(), i.data() + i.size());
}
#endif

template<typename T>
SPDLOG_CONSTEXPR_FUNC unsigned int count_digits_fallback(T n)
{
    // taken from fmt: https://github.com/fmtlib/fmt/blob/8.0.1/include/fmt/format.h#L899-L912
    unsigned int count = 1;
    for (;;)
    {
        // Integer division is slow so do it for a group of four digits instead
        // of for every digit. The idea comes from the talk by Alexandrescu
        // "Three Optimization Tips for C++". See speed-test for a comparison.
        if (n < 10)
            return count;
        if (n < 100)
            return count + 1;
        if (n < 1000)
            return count + 2;
        if (n < 10000)
            return count + 3;
        n /= 10000u;
        count += 4;
    }
}

template<typename T>
inline unsigned int count_digits(T n)
{
    using count_type = typename std::conditional<(sizeof(T) > sizeof(uint32_t)), uint64_t, uint32_t>::type;
#ifdef SPDLOG_USE_STD_FORMAT
    return count_digits_fallback(static_cast<count_type>(n));
#else
    return static_cast<unsigned int>(fmt::
// fmt 7.0.0 renamed the internal namespace to detail.
// See: https://github.com/fmtlib/fmt/issues/1538
#    if FMT_VERSION < 70000
            internal
#    else
            detail
#    endif
        ::count_digits(static_cast<count_type>(n)));
#endif
}

inline void pad2(int n, memory_buf_t &dest)
{
    if (n >= 0 && n < 100) // 0-99
    {
        dest.push_back(static_cast<char>('0' + n / 10));
        dest.push_back(static_cast<char>('0' + n % 10));
    }
    else // unlikely, but just in case, let fmt deal with it
    {
        fmt_lib::format_to(std::back_inserter(dest), SPDLOG_FMT_STRING("{:02}"), n);
    }
}

template<typename T>
inline void pad_uint(T n, unsigned int width, memory_buf_t &dest)
{
    static_assert(std::is_unsigned<T>::value, "pad_uint must get unsigned T");
    for (auto digits = count_digits(n); digits < width; digits++)
    {
        dest.push_back('0');
    }
    append_int(n, dest);
}

template<typename T>
inline void pad3(T n, memory_buf_t &dest)
{
    static_assert(std::is_unsigned<T>::value, "pad3 must get unsigned T");
    if (n < 1000)
    {
        dest.push_back(static_cast<char>(n / 100 + '0'));
        n = n % 100;
        dest.push_back(static_cast<char>((n / 10) + '0'));
        dest.push_back(static_cast<char>((n % 10) + '0'));
    }
    else
    {
        append_int(n, dest);
    }
}

template<typename T>
inline void pad6(T n, memory_buf_t &dest)
{
    pad_uint(n, 6, dest);
}

template<typename T>
inline void pad9(T n, memory_buf_t &dest)
{
    pad_uint(n, 9, dest);
}

// return fraction of a second of the given time_point.
// e.g.
// fraction<std::milliseconds>(tp) -> will return the millis part of the second
template<typename ToDuration>
inline ToDuration time_fraction(log_clock::time_point tp)
{
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    auto duration = tp.time_since_epoch();
    auto secs = duration_cast<seconds>(duration);
    return duration_cast<ToDuration>(duration) - duration_cast<ToDuration>(secs);
}

} // namespace fmt_helper
} // namespace details
} // namespace spdlog

// #include <spdlog/details/log_msg.h>

// #include <spdlog/details/os.h>

// #include <spdlog/fmt/fmt.h>

// #include <spdlog/formatter.h>


#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <cctype>
#include <cstring>
#include <iterator>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace spdlog {
namespace details {

///////////////////////////////////////////////////////////////////////
// name & level pattern appender
///////////////////////////////////////////////////////////////////////

class scoped_padder
{
public:
    scoped_padder(size_t wrapped_size, const padding_info &padinfo, memory_buf_t &dest)
        : padinfo_(padinfo)
        , dest_(dest)
    {
        remaining_pad_ = static_cast<long>(padinfo.width_) - static_cast<long>(wrapped_size);
        if (remaining_pad_ <= 0)
        {
            return;
        }

        if (padinfo_.side_ == padding_info::pad_side::left)
        {
            pad_it(remaining_pad_);
            remaining_pad_ = 0;
        }
        else if (padinfo_.side_ == padding_info::pad_side::center)
        {
            auto half_pad = remaining_pad_ / 2;
            auto reminder = remaining_pad_ & 1;
            pad_it(half_pad);
            remaining_pad_ = half_pad + reminder; // for the right side
        }
    }

    template<typename T>
    static unsigned int count_digits(T n)
    {
        return fmt_helper::count_digits(n);
    }

    ~scoped_padder()
    {
        if (remaining_pad_ >= 0)
        {
            pad_it(remaining_pad_);
        }
        else if (padinfo_.truncate_)
        {
            long new_size = static_cast<long>(dest_.size()) + remaining_pad_;
            dest_.resize(static_cast<size_t>(new_size));
        }
    }

private:
    void pad_it(long count)
    {
        fmt_helper::append_string_view(string_view_t(spaces_.data(), static_cast<size_t>(count)), dest_);
    }

    const padding_info &padinfo_;
    memory_buf_t &dest_;
    long remaining_pad_;
    string_view_t spaces_{"                                                                ", 64};
};

struct null_scoped_padder
{
    null_scoped_padder(size_t /*wrapped_size*/, const padding_info & /*padinfo*/, memory_buf_t & /*dest*/) {}

    template<typename T>
    static unsigned int count_digits(T /* number */)
    {
        return 0;
    }
};

template<typename ScopedPadder>
class name_formatter final : public flag_formatter
{
public:
    explicit name_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        ScopedPadder p(msg.logger_name.size(), padinfo_, dest);
        fmt_helper::append_string_view(msg.logger_name, dest);
    }
};

// log level appender
template<typename ScopedPadder>
class level_formatter final : public flag_formatter
{
public:
    explicit level_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        const string_view_t &level_name = level::to_string_view(msg.level);
        ScopedPadder p(level_name.size(), padinfo_, dest);
        fmt_helper::append_string_view(level_name, dest);
    }
};

// short log level appender
template<typename ScopedPadder>
class short_level_formatter final : public flag_formatter
{
public:
    explicit short_level_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        string_view_t level_name{level::to_short_c_str(msg.level)};
        ScopedPadder p(level_name.size(), padinfo_, dest);
        fmt_helper::append_string_view(level_name, dest);
    }
};

///////////////////////////////////////////////////////////////////////
// Date time pattern appenders
///////////////////////////////////////////////////////////////////////

static const char *ampm(const tm &t)
{
    return t.tm_hour >= 12 ? "PM" : "AM";
}

static int to12h(const tm &t)
{
    return t.tm_hour > 12 ? t.tm_hour - 12 : t.tm_hour;
}

// Abbreviated weekday name
static std::array<const char *, 7> days{{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}};

template<typename ScopedPadder>
class a_formatter final : public flag_formatter
{
public:
    explicit a_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        string_view_t field_value{days[static_cast<size_t>(tm_time.tm_wday)]};
        ScopedPadder p(field_value.size(), padinfo_, dest);
        fmt_helper::append_string_view(field_value, dest);
    }
};

// Full weekday name
static std::array<const char *, 7> full_days{{"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}};

template<typename ScopedPadder>
class A_formatter : public flag_formatter
{
public:
    explicit A_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        string_view_t field_value{full_days[static_cast<size_t>(tm_time.tm_wday)]};
        ScopedPadder p(field_value.size(), padinfo_, dest);
        fmt_helper::append_string_view(field_value, dest);
    }
};

// Abbreviated month
static const std::array<const char *, 12> months{{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"}};

template<typename ScopedPadder>
class b_formatter final : public flag_formatter
{
public:
    explicit b_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        string_view_t field_value{months[static_cast<size_t>(tm_time.tm_mon)]};
        ScopedPadder p(field_value.size(), padinfo_, dest);
        fmt_helper::append_string_view(field_value, dest);
    }
};

// Full month name
static const std::array<const char *, 12> full_months{
    {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"}};

template<typename ScopedPadder>
class B_formatter final : public flag_formatter
{
public:
    explicit B_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        string_view_t field_value{full_months[static_cast<size_t>(tm_time.tm_mon)]};
        ScopedPadder p(field_value.size(), padinfo_, dest);
        fmt_helper::append_string_view(field_value, dest);
    }
};

// Date and time representation (Thu Aug 23 15:35:46 2014)
template<typename ScopedPadder>
class c_formatter final : public flag_formatter
{
public:
    explicit c_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 24;
        ScopedPadder p(field_size, padinfo_, dest);

        fmt_helper::append_string_view(days[static_cast<size_t>(tm_time.tm_wday)], dest);
        dest.push_back(' ');
        fmt_helper::append_string_view(months[static_cast<size_t>(tm_time.tm_mon)], dest);
        dest.push_back(' ');
        fmt_helper::append_int(tm_time.tm_mday, dest);
        dest.push_back(' ');
        // time

        fmt_helper::pad2(tm_time.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_sec, dest);
        dest.push_back(' ');
        fmt_helper::append_int(tm_time.tm_year + 1900, dest);
    }
};

// year - 2 digit
template<typename ScopedPadder>
class C_formatter final : public flag_formatter
{
public:
    explicit C_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 2;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad2(tm_time.tm_year % 100, dest);
    }
};

// Short MM/DD/YY date, equivalent to %m/%d/%y 08/23/01
template<typename ScopedPadder>
class D_formatter final : public flag_formatter
{
public:
    explicit D_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 10;
        ScopedPadder p(field_size, padinfo_, dest);

        fmt_helper::pad2(tm_time.tm_mon + 1, dest);
        dest.push_back('/');
        fmt_helper::pad2(tm_time.tm_mday, dest);
        dest.push_back('/');
        fmt_helper::pad2(tm_time.tm_year % 100, dest);
    }
};

// year - 4 digit
template<typename ScopedPadder>
class Y_formatter final : public flag_formatter
{
public:
    explicit Y_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 4;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::append_int(tm_time.tm_year + 1900, dest);
    }
};

// month 1-12
template<typename ScopedPadder>
class m_formatter final : public flag_formatter
{
public:
    explicit m_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 2;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad2(tm_time.tm_mon + 1, dest);
    }
};

// day of month 1-31
template<typename ScopedPadder>
class d_formatter final : public flag_formatter
{
public:
    explicit d_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 2;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad2(tm_time.tm_mday, dest);
    }
};

// hours in 24 format 0-23
template<typename ScopedPadder>
class H_formatter final : public flag_formatter
{
public:
    explicit H_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 2;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad2(tm_time.tm_hour, dest);
    }
};

// hours in 12 format 1-12
template<typename ScopedPadder>
class I_formatter final : public flag_formatter
{
public:
    explicit I_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 2;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad2(to12h(tm_time), dest);
    }
};

// minutes 0-59
template<typename ScopedPadder>
class M_formatter final : public flag_formatter
{
public:
    explicit M_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 2;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad2(tm_time.tm_min, dest);
    }
};

// seconds 0-59
template<typename ScopedPadder>
class S_formatter final : public flag_formatter
{
public:
    explicit S_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 2;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad2(tm_time.tm_sec, dest);
    }
};

// milliseconds
template<typename ScopedPadder>
class e_formatter final : public flag_formatter
{
public:
    explicit e_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        auto millis = fmt_helper::time_fraction<std::chrono::milliseconds>(msg.time);
        const size_t field_size = 3;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
    }
};

// microseconds
template<typename ScopedPadder>
class f_formatter final : public flag_formatter
{
public:
    explicit f_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        auto micros = fmt_helper::time_fraction<std::chrono::microseconds>(msg.time);

        const size_t field_size = 6;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad6(static_cast<size_t>(micros.count()), dest);
    }
};

// nanoseconds
template<typename ScopedPadder>
class F_formatter final : public flag_formatter
{
public:
    explicit F_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        auto ns = fmt_helper::time_fraction<std::chrono::nanoseconds>(msg.time);
        const size_t field_size = 9;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::pad9(static_cast<size_t>(ns.count()), dest);
    }
};

// seconds since epoch
template<typename ScopedPadder>
class E_formatter final : public flag_formatter
{
public:
    explicit E_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        const size_t field_size = 10;
        ScopedPadder p(field_size, padinfo_, dest);
        auto duration = msg.time.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        fmt_helper::append_int(seconds, dest);
    }
};

// AM/PM
template<typename ScopedPadder>
class p_formatter final : public flag_formatter
{
public:
    explicit p_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 2;
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::append_string_view(ampm(tm_time), dest);
    }
};

// 12 hour clock 02:55:02 pm
template<typename ScopedPadder>
class r_formatter final : public flag_formatter
{
public:
    explicit r_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 11;
        ScopedPadder p(field_size, padinfo_, dest);

        fmt_helper::pad2(to12h(tm_time), dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_sec, dest);
        dest.push_back(' ');
        fmt_helper::append_string_view(ampm(tm_time), dest);
    }
};

// 24-hour HH:MM time, equivalent to %H:%M
template<typename ScopedPadder>
class R_formatter final : public flag_formatter
{
public:
    explicit R_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 5;
        ScopedPadder p(field_size, padinfo_, dest);

        fmt_helper::pad2(tm_time.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_min, dest);
    }
};

// ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S
template<typename ScopedPadder>
class T_formatter final : public flag_formatter
{
public:
    explicit T_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 8;
        ScopedPadder p(field_size, padinfo_, dest);

        fmt_helper::pad2(tm_time.tm_hour, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_min, dest);
        dest.push_back(':');
        fmt_helper::pad2(tm_time.tm_sec, dest);
    }
};

// ISO 8601 offset from UTC in timezone (+-HH:MM)
template<typename ScopedPadder>
class z_formatter final : public flag_formatter
{
public:
    explicit z_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    z_formatter() = default;
    z_formatter(const z_formatter &) = delete;
    z_formatter &operator=(const z_formatter &) = delete;

    void format(const details::log_msg &msg, const std::tm &tm_time, memory_buf_t &dest) override
    {
        const size_t field_size = 6;
        ScopedPadder p(field_size, padinfo_, dest);

        auto total_minutes = get_cached_offset(msg, tm_time);
        bool is_negative = total_minutes < 0;
        if (is_negative)
        {
            total_minutes = -total_minutes;
            dest.push_back('-');
        }
        else
        {
            dest.push_back('+');
        }

        fmt_helper::pad2(total_minutes / 60, dest); // hours
        dest.push_back(':');
        fmt_helper::pad2(total_minutes % 60, dest); // minutes
    }

private:
    log_clock::time_point last_update_{std::chrono::seconds(0)};
    int offset_minutes_{0};

    int get_cached_offset(const log_msg &msg, const std::tm &tm_time)
    {
        // refresh every 10 seconds
        if (msg.time - last_update_ >= std::chrono::seconds(10))
        {
            offset_minutes_ = os::utc_minutes_offset(tm_time);
            last_update_ = msg.time;
        }
        return offset_minutes_;
    }
};

// Thread id
template<typename ScopedPadder>
class t_formatter final : public flag_formatter
{
public:
    explicit t_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        const auto field_size = ScopedPadder::count_digits(msg.thread_id);
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::append_int(msg.thread_id, dest);
    }
};

// Current pid
template<typename ScopedPadder>
class pid_formatter final : public flag_formatter
{
public:
    explicit pid_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &, const std::tm &, memory_buf_t &dest) override
    {
        const auto pid = static_cast<uint32_t>(details::os::pid());
        auto field_size = ScopedPadder::count_digits(pid);
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::append_int(pid, dest);
    }
};

template<typename ScopedPadder>
class v_formatter final : public flag_formatter
{
public:
    explicit v_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        ScopedPadder p(msg.payload.size(), padinfo_, dest);
        fmt_helper::append_string_view(msg.payload, dest);
    }
};

class ch_formatter final : public flag_formatter
{
public:
    explicit ch_formatter(char ch)
        : ch_(ch)
    {}

    void format(const details::log_msg &, const std::tm &, memory_buf_t &dest) override
    {
        dest.push_back(ch_);
    }

private:
    char ch_;
};

// aggregate user chars to display as is
class aggregate_formatter final : public flag_formatter
{
public:
    aggregate_formatter() = default;

    void add_ch(char ch)
    {
        str_ += ch;
    }
    void format(const details::log_msg &, const std::tm &, memory_buf_t &dest) override
    {
        fmt_helper::append_string_view(str_, dest);
    }

private:
    std::string str_;
};

// mark the color range. expect it to be in the form of "%^colored text%$"
class color_start_formatter final : public flag_formatter
{
public:
    explicit color_start_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        msg.color_range_start = dest.size();
    }
};

class color_stop_formatter final : public flag_formatter
{
public:
    explicit color_stop_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        msg.color_range_end = dest.size();
    }
};

// print source location
template<typename ScopedPadder>
class source_location_formatter final : public flag_formatter
{
public:
    explicit source_location_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        if (msg.source.empty())
        {
            ScopedPadder p(0, padinfo_, dest);
            return;
        }

        size_t text_size;
        if (padinfo_.enabled())
        {
            // calc text size for padding based on "filename:line"
            text_size = std::char_traits<char>::length(msg.source.filename) + ScopedPadder::count_digits(msg.source.line) + 1;
        }
        else
        {
            text_size = 0;
        }

        ScopedPadder p(text_size, padinfo_, dest);
        fmt_helper::append_string_view(msg.source.filename, dest);
        dest.push_back(':');
        fmt_helper::append_int(msg.source.line, dest);
    }
};

// print source filename
template<typename ScopedPadder>
class source_filename_formatter final : public flag_formatter
{
public:
    explicit source_filename_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        if (msg.source.empty())
        {
            ScopedPadder p(0, padinfo_, dest);
            return;
        }
        size_t text_size = padinfo_.enabled() ? std::char_traits<char>::length(msg.source.filename) : 0;
        ScopedPadder p(text_size, padinfo_, dest);
        fmt_helper::append_string_view(msg.source.filename, dest);
    }
};

template<typename ScopedPadder>
class short_filename_formatter final : public flag_formatter
{
public:
    explicit short_filename_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4127) // consider using 'if constexpr' instead
#endif                              // _MSC_VER
    static const char *basename(const char *filename)
    {
        // if the size is 2 (1 character + null terminator) we can use the more efficient strrchr
        // the branch will be elided by optimizations
        if (sizeof(os::folder_seps) == 2)
        {
            const char *rv = std::strrchr(filename, os::folder_seps[0]);
            return rv != nullptr ? rv + 1 : filename;
        }
        else
        {
            const std::reverse_iterator<const char *> begin(filename + std::strlen(filename));
            const std::reverse_iterator<const char *> end(filename);

            const auto it = std::find_first_of(begin, end, std::begin(os::folder_seps), std::end(os::folder_seps) - 1);
            return it != end ? it.base() : filename;
        }
    }
#ifdef _MSC_VER
#    pragma warning(pop)
#endif // _MSC_VER

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        if (msg.source.empty())
        {
            ScopedPadder p(0, padinfo_, dest);
            return;
        }
        auto filename = basename(msg.source.filename);
        size_t text_size = padinfo_.enabled() ? std::char_traits<char>::length(filename) : 0;
        ScopedPadder p(text_size, padinfo_, dest);
        fmt_helper::append_string_view(filename, dest);
    }
};

template<typename ScopedPadder>
class source_linenum_formatter final : public flag_formatter
{
public:
    explicit source_linenum_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        if (msg.source.empty())
        {
            ScopedPadder p(0, padinfo_, dest);
            return;
        }

        auto field_size = ScopedPadder::count_digits(msg.source.line);
        ScopedPadder p(field_size, padinfo_, dest);
        fmt_helper::append_int(msg.source.line, dest);
    }
};

// print source funcname
template<typename ScopedPadder>
class source_funcname_formatter final : public flag_formatter
{
public:
    explicit source_funcname_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        if (msg.source.empty())
        {
            ScopedPadder p(0, padinfo_, dest);
            return;
        }
        size_t text_size = padinfo_.enabled() ? std::char_traits<char>::length(msg.source.funcname) : 0;
        ScopedPadder p(text_size, padinfo_, dest);
        fmt_helper::append_string_view(msg.source.funcname, dest);
    }
};

// print elapsed time since last message
template<typename ScopedPadder, typename Units>
class elapsed_formatter final : public flag_formatter
{
public:
    using DurationUnits = Units;

    explicit elapsed_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
        , last_message_time_(log_clock::now())
    {}

    void format(const details::log_msg &msg, const std::tm &, memory_buf_t &dest) override
    {
        auto delta = (std::max)(msg.time - last_message_time_, log_clock::duration::zero());
        auto delta_units = std::chrono::duration_cast<DurationUnits>(delta);
        last_message_time_ = msg.time;
        auto delta_count = static_cast<size_t>(delta_units.count());
        auto n_digits = static_cast<size_t>(ScopedPadder::count_digits(delta_count));
        ScopedPadder p(n_digits, padinfo_, dest);
        fmt_helper::append_int(delta_count, dest);
    }

private:
    log_clock::time_point last_message_time_;
};

// Full info formatter
// pattern: [%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%s:%#] %v
class full_formatter final : public flag_formatter
{
public:
    explicit full_formatter(padding_info padinfo)
        : flag_formatter(padinfo)
    {}

    void format(const details::log_msg &msg, const std::tm &tm_time, memory_buf_t &dest) override
    {
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;
        using std::chrono::seconds;

        // cache the date/time part for the next second.
        auto duration = msg.time.time_since_epoch();
        auto secs = duration_cast<seconds>(duration);

        if (cache_timestamp_ != secs || cached_datetime_.size() == 0)
        {
            cached_datetime_.clear();
            cached_datetime_.push_back('[');
            fmt_helper::append_int(tm_time.tm_year + 1900, cached_datetime_);
            cached_datetime_.push_back('-');

            fmt_helper::pad2(tm_time.tm_mon + 1, cached_datetime_);
            cached_datetime_.push_back('-');

            fmt_helper::pad2(tm_time.tm_mday, cached_datetime_);
            cached_datetime_.push_back(' ');

            fmt_helper::pad2(tm_time.tm_hour, cached_datetime_);
            cached_datetime_.push_back(':');

            fmt_helper::pad2(tm_time.tm_min, cached_datetime_);
            cached_datetime_.push_back(':');

            fmt_helper::pad2(tm_time.tm_sec, cached_datetime_);
            cached_datetime_.push_back('.');

            cache_timestamp_ = secs;
        }
        dest.append(cached_datetime_.begin(), cached_datetime_.end());

        auto millis = fmt_helper::time_fraction<milliseconds>(msg.time);
        fmt_helper::pad3(static_cast<uint32_t>(millis.count()), dest);
        dest.push_back(']');
        dest.push_back(' ');

        // append logger name if exists
        if (msg.logger_name.size() > 0)
        {
            dest.push_back('[');
            fmt_helper::append_string_view(msg.logger_name, dest);
            dest.push_back(']');
            dest.push_back(' ');
        }

        dest.push_back('[');
        // wrap the level name with color
        msg.color_range_start = dest.size();
        // fmt_helper::append_string_view(level::to_c_str(msg.level), dest);
        fmt_helper::append_string_view(level::to_string_view(msg.level), dest);
        msg.color_range_end = dest.size();
        dest.push_back(']');
        dest.push_back(' ');

        // add source location if present
        if (!msg.source.empty())
        {
            dest.push_back('[');
            const char *filename = details::short_filename_formatter<details::null_scoped_padder>::basename(msg.source.filename);
            fmt_helper::append_string_view(filename, dest);
            dest.push_back(':');
            fmt_helper::append_int(msg.source.line, dest);
            dest.push_back(']');
            dest.push_back(' ');
        }
        // fmt_helper::append_string_view(msg.msg(), dest);
        fmt_helper::append_string_view(msg.payload, dest);
    }

private:
    std::chrono::seconds cache_timestamp_{0};
    memory_buf_t cached_datetime_;
};

} // namespace details

SPDLOG_INLINE pattern_formatter::pattern_formatter(
    std::string pattern, pattern_time_type time_type, std::string eol, custom_flags custom_user_flags)
    : pattern_(std::move(pattern))
    , eol_(std::move(eol))
    , pattern_time_type_(time_type)
    , need_localtime_(false)
    , last_log_secs_(0)
    , custom_handlers_(std::move(custom_user_flags))
{
    std::memset(&cached_tm_, 0, sizeof(cached_tm_));
    compile_pattern_(pattern_);
}

// use by default full formatter for if pattern is not given
SPDLOG_INLINE pattern_formatter::pattern_formatter(pattern_time_type time_type, std::string eol)
    : pattern_("%+")
    , eol_(std::move(eol))
    , pattern_time_type_(time_type)
    , need_localtime_(true)
    , last_log_secs_(0)
{
    std::memset(&cached_tm_, 0, sizeof(cached_tm_));
    formatters_.push_back(details::make_unique<details::full_formatter>(details::padding_info{}));
}

SPDLOG_INLINE std::unique_ptr<formatter> pattern_formatter::clone() const
{
    custom_flags cloned_custom_formatters;
    for (auto &it : custom_handlers_)
    {
        cloned_custom_formatters[it.first] = it.second->clone();
    }
    auto cloned = details::make_unique<pattern_formatter>(pattern_, pattern_time_type_, eol_, std::move(cloned_custom_formatters));
    cloned->need_localtime(need_localtime_);
#if defined(__GNUC__) && __GNUC__ < 5
    return std::move(cloned);
#else
    return cloned;
#endif
}

SPDLOG_INLINE void pattern_formatter::format(const details::log_msg &msg, memory_buf_t &dest)
{
    if (need_localtime_)
    {
        const auto secs = std::chrono::duration_cast<std::chrono::seconds>(msg.time.time_since_epoch());
        if (secs != last_log_secs_)
        {
            cached_tm_ = get_time_(msg);
            last_log_secs_ = secs;
        }
    }

    for (auto &f : formatters_)
    {
        f->format(msg, cached_tm_, dest);
    }
    // write eol
    details::fmt_helper::append_string_view(eol_, dest);
}

SPDLOG_INLINE void pattern_formatter::set_pattern(std::string pattern)
{
    pattern_ = std::move(pattern);
    need_localtime_ = false;
    compile_pattern_(pattern_);
}

SPDLOG_INLINE void pattern_formatter::need_localtime(bool need)
{
    need_localtime_ = need;
}

SPDLOG_INLINE std::tm pattern_formatter::get_time_(const details::log_msg &msg)
{
    if (pattern_time_type_ == pattern_time_type::local)
    {
        return details::os::localtime(log_clock::to_time_t(msg.time));
    }
    return details::os::gmtime(log_clock::to_time_t(msg.time));
}

template<typename Padder>
SPDLOG_INLINE void pattern_formatter::handle_flag_(char flag, details::padding_info padding)
{
    // process custom flags
    auto it = custom_handlers_.find(flag);
    if (it != custom_handlers_.end())
    {
        auto custom_handler = it->second->clone();
        custom_handler->set_padding_info(padding);
        formatters_.push_back(std::move(custom_handler));
        return;
    }

    // process built-in flags
    switch (flag)
    {
    case ('+'): // default formatter
        formatters_.push_back(details::make_unique<details::full_formatter>(padding));
        need_localtime_ = true;
        break;

    case 'n': // logger name
        formatters_.push_back(details::make_unique<details::name_formatter<Padder>>(padding));
        break;

    case 'l': // level
        formatters_.push_back(details::make_unique<details::level_formatter<Padder>>(padding));
        break;

    case 'L': // short level
        formatters_.push_back(details::make_unique<details::short_level_formatter<Padder>>(padding));
        break;

    case ('t'): // thread id
        formatters_.push_back(details::make_unique<details::t_formatter<Padder>>(padding));
        break;

    case ('v'): // the message text
        formatters_.push_back(details::make_unique<details::v_formatter<Padder>>(padding));
        break;

    case ('a'): // weekday
        formatters_.push_back(details::make_unique<details::a_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('A'): // short weekday
        formatters_.push_back(details::make_unique<details::A_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('b'):
    case ('h'): // month
        formatters_.push_back(details::make_unique<details::b_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('B'): // short month
        formatters_.push_back(details::make_unique<details::B_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('c'): // datetime
        formatters_.push_back(details::make_unique<details::c_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('C'): // year 2 digits
        formatters_.push_back(details::make_unique<details::C_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('Y'): // year 4 digits
        formatters_.push_back(details::make_unique<details::Y_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('D'):
    case ('x'): // datetime MM/DD/YY
        formatters_.push_back(details::make_unique<details::D_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('m'): // month 1-12
        formatters_.push_back(details::make_unique<details::m_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('d'): // day of month 1-31
        formatters_.push_back(details::make_unique<details::d_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('H'): // hours 24
        formatters_.push_back(details::make_unique<details::H_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('I'): // hours 12
        formatters_.push_back(details::make_unique<details::I_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('M'): // minutes
        formatters_.push_back(details::make_unique<details::M_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('S'): // seconds
        formatters_.push_back(details::make_unique<details::S_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('e'): // milliseconds
        formatters_.push_back(details::make_unique<details::e_formatter<Padder>>(padding));
        break;

    case ('f'): // microseconds
        formatters_.push_back(details::make_unique<details::f_formatter<Padder>>(padding));
        break;

    case ('F'): // nanoseconds
        formatters_.push_back(details::make_unique<details::F_formatter<Padder>>(padding));
        break;

    case ('E'): // seconds since epoch
        formatters_.push_back(details::make_unique<details::E_formatter<Padder>>(padding));
        break;

    case ('p'): // am/pm
        formatters_.push_back(details::make_unique<details::p_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('r'): // 12 hour clock 02:55:02 pm
        formatters_.push_back(details::make_unique<details::r_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('R'): // 24-hour HH:MM time
        formatters_.push_back(details::make_unique<details::R_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('T'):
    case ('X'): // ISO 8601 time format (HH:MM:SS)
        formatters_.push_back(details::make_unique<details::T_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('z'): // timezone
        formatters_.push_back(details::make_unique<details::z_formatter<Padder>>(padding));
        need_localtime_ = true;
        break;

    case ('P'): // pid
        formatters_.push_back(details::make_unique<details::pid_formatter<Padder>>(padding));
        break;

    case ('^'): // color range start
        formatters_.push_back(details::make_unique<details::color_start_formatter>(padding));
        break;

    case ('$'): // color range end
        formatters_.push_back(details::make_unique<details::color_stop_formatter>(padding));
        break;

    case ('@'): // source location (filename:filenumber)
        formatters_.push_back(details::make_unique<details::source_location_formatter<Padder>>(padding));
        break;

    case ('s'): // short source filename - without directory name
        formatters_.push_back(details::make_unique<details::short_filename_formatter<Padder>>(padding));
        break;

    case ('g'): // full source filename
        formatters_.push_back(details::make_unique<details::source_filename_formatter<Padder>>(padding));
        break;

    case ('#'): // source line number
        formatters_.push_back(details::make_unique<details::source_linenum_formatter<Padder>>(padding));
        break;

    case ('!'): // source funcname
        formatters_.push_back(details::make_unique<details::source_funcname_formatter<Padder>>(padding));
        break;

    case ('%'): // % char
        formatters_.push_back(details::make_unique<details::ch_formatter>('%'));
        break;

    case ('u'): // elapsed time since last log message in nanos
        formatters_.push_back(details::make_unique<details::elapsed_formatter<Padder, std::chrono::nanoseconds>>(padding));
        break;

    case ('i'): // elapsed time since last log message in micros
        formatters_.push_back(details::make_unique<details::elapsed_formatter<Padder, std::chrono::microseconds>>(padding));
        break;

    case ('o'): // elapsed time since last log message in millis
        formatters_.push_back(details::make_unique<details::elapsed_formatter<Padder, std::chrono::milliseconds>>(padding));
        break;

    case ('O'): // elapsed time since last log message in seconds
        formatters_.push_back(details::make_unique<details::elapsed_formatter<Padder, std::chrono::seconds>>(padding));
        break;

    default: // Unknown flag appears as is
        auto unknown_flag = details::make_unique<details::aggregate_formatter>();

        if (!padding.truncate_)
        {
            unknown_flag->add_ch('%');
            unknown_flag->add_ch(flag);
            formatters_.push_back((std::move(unknown_flag)));
        }
        // fix issue #1617 (prev char was '!' and should have been treated as funcname flag instead of truncating flag)
        // spdlog::set_pattern("[%10!] %v") => "[      main] some message"
        // spdlog::set_pattern("[%3!!] %v") => "[mai] some message"
        else
        {
            padding.truncate_ = false;
            formatters_.push_back(details::make_unique<details::source_funcname_formatter<Padder>>(padding));
            unknown_flag->add_ch(flag);
            formatters_.push_back((std::move(unknown_flag)));
        }

        break;
    }
}

// Extract given pad spec (e.g. %8X, %=8X, %-8!X, %8!X, %=8!X, %-8!X, %+8!X)
// Advance the given it pass the end of the padding spec found (if any)
// Return padding.
SPDLOG_INLINE details::padding_info pattern_formatter::handle_padspec_(std::string::const_iterator &it, std::string::const_iterator end)
{
    using details::padding_info;
    using details::scoped_padder;
    const size_t max_width = 64;
    if (it == end)
    {
        return padding_info{};
    }

    padding_info::pad_side side;
    switch (*it)
    {
    case '-':
        side = padding_info::pad_side::right;
        ++it;
        break;
    case '=':
        side = padding_info::pad_side::center;
        ++it;
        break;
    default:
        side = details::padding_info::pad_side::left;
        break;
    }

    if (it == end || !std::isdigit(static_cast<unsigned char>(*it)))
    {
        return padding_info{}; // no padding if no digit found here
    }

    auto width = static_cast<size_t>(*it) - '0';
    for (++it; it != end && std::isdigit(static_cast<unsigned char>(*it)); ++it)
    {
        auto digit = static_cast<size_t>(*it) - '0';
        width = width * 10 + digit;
    }

    // search for the optional truncate marker '!'
    bool truncate;
    if (it != end && *it == '!')
    {
        truncate = true;
        ++it;
    }
    else
    {
        truncate = false;
    }
    return details::padding_info{std::min<size_t>(width, max_width), side, truncate};
}

SPDLOG_INLINE void pattern_formatter::compile_pattern_(const std::string &pattern)
{
    auto end = pattern.end();
    std::unique_ptr<details::aggregate_formatter> user_chars;
    formatters_.clear();
    for (auto it = pattern.begin(); it != end; ++it)
    {
        if (*it == '%')
        {
            if (user_chars) // append user chars found so far
            {
                formatters_.push_back(std::move(user_chars));
            }

            auto padding = handle_padspec_(++it, end);

            if (it != end)
            {
                if (padding.enabled())
                {
                    handle_flag_<details::scoped_padder>(*it, padding);
                }
                else
                {
                    handle_flag_<details::null_scoped_padder>(*it, padding);
                }
            }
            else
            {
                break;
            }
        }
        else // chars not following the % sign should be displayed as is
        {
            if (!user_chars)
            {
                user_chars = details::make_unique<details::aggregate_formatter>();
            }
            user_chars->add_ch(*it);
        }
    }
    if (user_chars) // append raw chars found so far
    {
        formatters_.push_back(std::move(user_chars));
    }
}
} // namespace spdlog

#endif


#include <cstdio>

namespace spdlog {

// public methods
SPDLOG_INLINE logger::logger(const logger &other)
    : name_(other.name_)
    , sinks_(other.sinks_)
    , level_(other.level_.load(std::memory_order_relaxed))
    , flush_level_(other.flush_level_.load(std::memory_order_relaxed))
    , custom_err_handler_(other.custom_err_handler_)
    , tracer_(other.tracer_)
{}

SPDLOG_INLINE logger::logger(logger &&other) SPDLOG_NOEXCEPT : name_(std::move(other.name_)),
                                                               sinks_(std::move(other.sinks_)),
                                                               level_(other.level_.load(std::memory_order_relaxed)),
                                                               flush_level_(other.flush_level_.load(std::memory_order_relaxed)),
                                                               custom_err_handler_(std::move(other.custom_err_handler_)),
                                                               tracer_(std::move(other.tracer_))

{}

SPDLOG_INLINE logger &logger::operator=(logger other) SPDLOG_NOEXCEPT
{
    this->swap(other);
    return *this;
}

SPDLOG_INLINE void logger::swap(spdlog::logger &other) SPDLOG_NOEXCEPT
{
    name_.swap(other.name_);
    sinks_.swap(other.sinks_);

    // swap level_
    auto other_level = other.level_.load();
    auto my_level = level_.exchange(other_level);
    other.level_.store(my_level);

    // swap flush level_
    other_level = other.flush_level_.load();
    my_level = flush_level_.exchange(other_level);
    other.flush_level_.store(my_level);

    custom_err_handler_.swap(other.custom_err_handler_);
    std::swap(tracer_, other.tracer_);
}

SPDLOG_INLINE void swap(logger &a, logger &b)
{
    a.swap(b);
}

SPDLOG_INLINE void logger::set_level(level::level_enum log_level)
{
    level_.store(log_level);
}

SPDLOG_INLINE level::level_enum logger::level() const
{
    return static_cast<level::level_enum>(level_.load(std::memory_order_relaxed));
}

SPDLOG_INLINE const std::string &logger::name() const
{
    return name_;
}

// set formatting for the sinks in this logger.
// each sink will get a separate instance of the formatter object.
SPDLOG_INLINE void logger::set_formatter(std::unique_ptr<formatter> f)
{
    for (auto it = sinks_.begin(); it != sinks_.end(); ++it)
    {
        if (std::next(it) == sinks_.end())
        {
            // last element - we can be move it.
            (*it)->set_formatter(std::move(f));
            break; // to prevent clang-tidy warning
        }
        else
        {
            (*it)->set_formatter(f->clone());
        }
    }
}

SPDLOG_INLINE void logger::set_pattern(std::string pattern, pattern_time_type time_type)
{
    auto new_formatter = details::make_unique<pattern_formatter>(std::move(pattern), time_type);
    set_formatter(std::move(new_formatter));
}

// create new backtrace sink and move to it all our child sinks
SPDLOG_INLINE void logger::enable_backtrace(size_t n_messages)
{
    tracer_.enable(n_messages);
}

// restore orig sinks and level and delete the backtrace sink
SPDLOG_INLINE void logger::disable_backtrace()
{
    tracer_.disable();
}

SPDLOG_INLINE void logger::dump_backtrace()
{
    dump_backtrace_();
}

// flush functions
SPDLOG_INLINE void logger::flush()
{
    flush_();
}

SPDLOG_INLINE void logger::flush_on(level::level_enum log_level)
{
    flush_level_.store(log_level);
}

SPDLOG_INLINE level::level_enum logger::flush_level() const
{
    return static_cast<level::level_enum>(flush_level_.load(std::memory_order_relaxed));
}

// sinks
SPDLOG_INLINE const std::vector<sink_ptr> &logger::sinks() const
{
    return sinks_;
}

SPDLOG_INLINE std::vector<sink_ptr> &logger::sinks()
{
    return sinks_;
}

// error handler
SPDLOG_INLINE void logger::set_error_handler(err_handler handler)
{
    custom_err_handler_ = std::move(handler);
}

// create new logger with same sinks and configuration.
SPDLOG_INLINE std::shared_ptr<logger> logger::clone(std::string logger_name)
{
    auto cloned = std::make_shared<logger>(*this);
    cloned->name_ = std::move(logger_name);
    return cloned;
}

// protected methods
SPDLOG_INLINE void logger::log_it_(const spdlog::details::log_msg &log_msg, bool log_enabled, bool traceback_enabled)
{
    if (log_enabled)
    {
        sink_it_(log_msg);
    }
    if (traceback_enabled)
    {
        tracer_.push_back(log_msg);
    }
}

SPDLOG_INLINE void logger::sink_it_(const details::log_msg &msg)
{
    for (auto &sink : sinks_)
    {
        if (sink->should_log(msg.level))
        {
            SPDLOG_TRY
            {
                sink->log(msg);
            }
            SPDLOG_LOGGER_CATCH(msg.source)
        }
    }

    if (should_flush_(msg))
    {
        flush_();
    }
}

SPDLOG_INLINE void logger::flush_()
{
    for (auto &sink : sinks_)
    {
        SPDLOG_TRY
        {
            sink->flush();
        }
        SPDLOG_LOGGER_CATCH(source_loc())
    }
}

SPDLOG_INLINE void logger::dump_backtrace_()
{
    using details::log_msg;
    if (tracer_.enabled())
    {
        sink_it_(log_msg{name(), level::info, "****************** Backtrace Start ******************"});
        tracer_.foreach_pop([this](const log_msg &msg) { this->sink_it_(msg); });
        sink_it_(log_msg{name(), level::info, "****************** Backtrace End ********************"});
    }
}

SPDLOG_INLINE bool logger::should_flush_(const details::log_msg &msg)
{
    auto flush_level = flush_level_.load(std::memory_order_relaxed);
    return (msg.level >= flush_level) && (msg.level != level::off);
}

SPDLOG_INLINE void logger::err_handler_(const std::string &msg)
{
    if (custom_err_handler_)
    {
        custom_err_handler_(msg);
    }
    else
    {
        using std::chrono::system_clock;
        static std::mutex mutex;
        static std::chrono::system_clock::time_point last_report_time;
        static size_t err_counter = 0;
        std::lock_guard<std::mutex> lk{mutex};
        auto now = system_clock::now();
        err_counter++;
        if (now - last_report_time < std::chrono::seconds(1))
        {
            return;
        }
        last_report_time = now;
        auto tm_time = details::os::localtime(system_clock::to_time_t(now));
        char date_buf[64];
        std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", &tm_time);
#if defined(USING_R) && defined(R_R_H) // if in R environment
        REprintf("[*** LOG ERROR #%04zu ***] [%s] [%s] {%s}\n", err_counter, date_buf, name().c_str(), msg.c_str());
#else
        std::fprintf(stderr, "[*** LOG ERROR #%04zu ***] [%s] [%s] {%s}\n", err_counter, date_buf, name().c_str(), msg.c_str());
#endif
    }
}
} // namespace spdlog

#endif

// #include <spdlog/pattern_formatter.h>


#ifndef SPDLOG_DISABLE_DEFAULT_LOGGER
// support for the default stdout color logger
#    ifdef _WIN32
// #        include <spdlog/sinks/wincolor_sink.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/common.h>

// #include <spdlog/details/console_globals.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/details/null_mutex.h>

#include <mutex>

namespace spdlog {
namespace details {

struct console_mutex
{
    using mutex_t = std::mutex;
    static mutex_t &mutex()
    {
        static mutex_t s_mutex;
        return s_mutex;
    }
};

struct console_nullmutex
{
    using mutex_t = null_mutex;
    static mutex_t &mutex()
    {
        static mutex_t s_mutex;
        return s_mutex;
    }
};
} // namespace details
} // namespace spdlog

// #include <spdlog/details/null_mutex.h>

// #include <spdlog/sinks/sink.h>


#include <memory>
#include <mutex>
#include <string>
#include <array>
#include <cstdint>

namespace spdlog {
namespace sinks {
/*
 * Windows color console sink. Uses WriteConsoleA to write to the console with
 * colors
 */
template<typename ConsoleMutex>
class wincolor_sink : public sink
{
public:
    wincolor_sink(void *out_handle, color_mode mode);
    ~wincolor_sink() override;

    wincolor_sink(const wincolor_sink &other) = delete;
    wincolor_sink &operator=(const wincolor_sink &other) = delete;

    // change the color for the given level
    void set_color(level::level_enum level, std::uint16_t color);
    void log(const details::log_msg &msg) final override;
    void flush() final override;
    void set_pattern(const std::string &pattern) override final;
    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override final;
    void set_color_mode(color_mode mode);

protected:
    using mutex_t = typename ConsoleMutex::mutex_t;
    void *out_handle_;
    mutex_t &mutex_;
    bool should_do_colors_;
    std::unique_ptr<spdlog::formatter> formatter_;
    std::array<std::uint16_t, level::n_levels> colors_;

    // set foreground color and return the orig console attributes (for resetting later)
    std::uint16_t set_foreground_color_(std::uint16_t attribs);

    // print a range of formatted message to console
    void print_range_(const memory_buf_t &formatted, size_t start, size_t end);

    // in case we are redirected to file (not in console mode)
    void write_to_file_(const memory_buf_t &formatted);

    void set_color_mode_impl(color_mode mode);
};

template<typename ConsoleMutex>
class wincolor_stdout_sink : public wincolor_sink<ConsoleMutex>
{
public:
    explicit wincolor_stdout_sink(color_mode mode = color_mode::automatic);
};

template<typename ConsoleMutex>
class wincolor_stderr_sink : public wincolor_sink<ConsoleMutex>
{
public:
    explicit wincolor_stderr_sink(color_mode mode = color_mode::automatic);
};

using wincolor_stdout_sink_mt = wincolor_stdout_sink<details::console_mutex>;
using wincolor_stdout_sink_st = wincolor_stdout_sink<details::console_nullmutex>;

using wincolor_stderr_sink_mt = wincolor_stderr_sink<details::console_mutex>;
using wincolor_stderr_sink_st = wincolor_stderr_sink<details::console_nullmutex>;
} // namespace sinks
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "wincolor_sink-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/sinks/wincolor_sink.h>

#endif

// #include <spdlog/details/windows_include.h>

#include <wincon.h>

// #include <spdlog/common.h>

// #include <spdlog/pattern_formatter.h>


namespace spdlog {
namespace sinks {
template<typename ConsoleMutex>
SPDLOG_INLINE wincolor_sink<ConsoleMutex>::wincolor_sink(void *out_handle, color_mode mode)
    : out_handle_(out_handle)
    , mutex_(ConsoleMutex::mutex())
    , formatter_(details::make_unique<spdlog::pattern_formatter>())
{

    set_color_mode_impl(mode);
    // set level colors
    colors_[level::trace] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;     // white
    colors_[level::debug] = FOREGROUND_GREEN | FOREGROUND_BLUE;                      // cyan
    colors_[level::info] = FOREGROUND_GREEN;                                         // green
    colors_[level::warn] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; // intense yellow
    colors_[level::err] = FOREGROUND_RED | FOREGROUND_INTENSITY;                     // intense red
    colors_[level::critical] =
        BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; // intense white on red background
    colors_[level::off] = 0;
}

template<typename ConsoleMutex>
SPDLOG_INLINE wincolor_sink<ConsoleMutex>::~wincolor_sink()
{
    this->flush();
}

// change the color for the given level
template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::set_color(level::level_enum level, std::uint16_t color)
{
    std::lock_guard<mutex_t> lock(mutex_);
    colors_[static_cast<size_t>(level)] = color;
}

template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::log(const details::log_msg &msg)
{
    if (out_handle_ == nullptr || out_handle_ == INVALID_HANDLE_VALUE)
    {
        return;
    }

    std::lock_guard<mutex_t> lock(mutex_);
    msg.color_range_start = 0;
    msg.color_range_end = 0;
    memory_buf_t formatted;
    formatter_->format(msg, formatted);
    if (should_do_colors_ && msg.color_range_end > msg.color_range_start)
    {
        // before color range
        print_range_(formatted, 0, msg.color_range_start);
        // in color range
        auto orig_attribs = static_cast<WORD>(set_foreground_color_(colors_[static_cast<size_t>(msg.level)]));
        print_range_(formatted, msg.color_range_start, msg.color_range_end);
        // reset to orig colors
        ::SetConsoleTextAttribute(static_cast<HANDLE>(out_handle_), orig_attribs);
        print_range_(formatted, msg.color_range_end, formatted.size());
    }
    else // print without colors if color range is invalid (or color is disabled)
    {
        write_to_file_(formatted);
    }
}

template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::flush()
{
    // windows console always flushed?
}

template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::set_pattern(const std::string &pattern)
{
    std::lock_guard<mutex_t> lock(mutex_);
    formatter_ = std::unique_ptr<spdlog::formatter>(new pattern_formatter(pattern));
}

template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter)
{
    std::lock_guard<mutex_t> lock(mutex_);
    formatter_ = std::move(sink_formatter);
}

template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::set_color_mode(color_mode mode)
{
    std::lock_guard<mutex_t> lock(mutex_);
    set_color_mode_impl(mode);
}

template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::set_color_mode_impl(color_mode mode)
{
    if (mode == color_mode::automatic)
    {
        // should do colors only if out_handle_  points to actual console.
        DWORD console_mode;
        bool in_console = ::GetConsoleMode(static_cast<HANDLE>(out_handle_), &console_mode) != 0;
        should_do_colors_ = in_console;
    }
    else
    {
        should_do_colors_ = mode == color_mode::always ? true : false;
    }
}

// set foreground color and return the orig console attributes (for resetting later)
template<typename ConsoleMutex>
std::uint16_t SPDLOG_INLINE wincolor_sink<ConsoleMutex>::set_foreground_color_(std::uint16_t attribs)
{
    CONSOLE_SCREEN_BUFFER_INFO orig_buffer_info;
    if (!::GetConsoleScreenBufferInfo(static_cast<HANDLE>(out_handle_), &orig_buffer_info))
    {
        // just return white if failed getting console info
        return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }

    // change only the foreground bits (lowest 4 bits)
    auto new_attribs = static_cast<WORD>(attribs) | (orig_buffer_info.wAttributes & 0xfff0);
    auto ignored = ::SetConsoleTextAttribute(static_cast<HANDLE>(out_handle_), static_cast<WORD>(new_attribs));
    (void)(ignored);
    return static_cast<std::uint16_t>(orig_buffer_info.wAttributes); // return orig attribs
}

// print a range of formatted message to console
template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::print_range_(const memory_buf_t &formatted, size_t start, size_t end)
{
    if (end > start)
    {
        auto size = static_cast<DWORD>(end - start);
        auto ignored = ::WriteConsoleA(static_cast<HANDLE>(out_handle_), formatted.data() + start, size, nullptr, nullptr);
        (void)(ignored);
    }
}

template<typename ConsoleMutex>
void SPDLOG_INLINE wincolor_sink<ConsoleMutex>::write_to_file_(const memory_buf_t &formatted)
{
    auto size = static_cast<DWORD>(formatted.size());
    DWORD bytes_written = 0;
    auto ignored = ::WriteFile(static_cast<HANDLE>(out_handle_), formatted.data(), size, &bytes_written, nullptr);
    (void)(ignored);
}

// wincolor_stdout_sink
template<typename ConsoleMutex>
SPDLOG_INLINE wincolor_stdout_sink<ConsoleMutex>::wincolor_stdout_sink(color_mode mode)
    : wincolor_sink<ConsoleMutex>(::GetStdHandle(STD_OUTPUT_HANDLE), mode)
{}

// wincolor_stderr_sink
template<typename ConsoleMutex>
SPDLOG_INLINE wincolor_stderr_sink<ConsoleMutex>::wincolor_stderr_sink(color_mode mode)
    : wincolor_sink<ConsoleMutex>(::GetStdHandle(STD_ERROR_HANDLE), mode)
{}
} // namespace sinks
} // namespace spdlog

#endif

#    else
// #        include <spdlog/sinks/ansicolor_sink.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include <spdlog/details/console_globals.h>

// #include <spdlog/details/null_mutex.h>

// #include <spdlog/sinks/sink.h>

#include <memory>
#include <mutex>
#include <string>
#include <array>

namespace spdlog {
namespace sinks {

/**
 * This sink prefixes the output with an ANSI escape sequence color code
 * depending on the severity
 * of the message.
 * If no color terminal detected, omit the escape codes.
 */

template<typename ConsoleMutex>
class ansicolor_sink : public sink
{
public:
    using mutex_t = typename ConsoleMutex::mutex_t;
    ansicolor_sink(FILE *target_file, color_mode mode);
    ~ansicolor_sink() override = default;

    ansicolor_sink(const ansicolor_sink &other) = delete;
    ansicolor_sink(ansicolor_sink &&other) = delete;

    ansicolor_sink &operator=(const ansicolor_sink &other) = delete;
    ansicolor_sink &operator=(ansicolor_sink &&other) = delete;

    void set_color(level::level_enum color_level, string_view_t color);
    void set_color_mode(color_mode mode);
    bool should_color();

    void log(const details::log_msg &msg) override;
    void flush() override;
    void set_pattern(const std::string &pattern) final;
    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;

    // Formatting codes
    const string_view_t reset = "\033[m";
    const string_view_t bold = "\033[1m";
    const string_view_t dark = "\033[2m";
    const string_view_t underline = "\033[4m";
    const string_view_t blink = "\033[5m";
    const string_view_t reverse = "\033[7m";
    const string_view_t concealed = "\033[8m";
    const string_view_t clear_line = "\033[K";

    // Foreground colors
    const string_view_t black = "\033[30m";
    const string_view_t red = "\033[31m";
    const string_view_t green = "\033[32m";
    const string_view_t yellow = "\033[33m";
    const string_view_t blue = "\033[34m";
    const string_view_t magenta = "\033[35m";
    const string_view_t cyan = "\033[36m";
    const string_view_t white = "\033[37m";

    /// Background colors
    const string_view_t on_black = "\033[40m";
    const string_view_t on_red = "\033[41m";
    const string_view_t on_green = "\033[42m";
    const string_view_t on_yellow = "\033[43m";
    const string_view_t on_blue = "\033[44m";
    const string_view_t on_magenta = "\033[45m";
    const string_view_t on_cyan = "\033[46m";
    const string_view_t on_white = "\033[47m";

    /// Bold colors
    const string_view_t yellow_bold = "\033[33m\033[1m";
    const string_view_t red_bold = "\033[31m\033[1m";
    const string_view_t bold_on_red = "\033[1m\033[41m";

private:
    FILE *target_file_;
    mutex_t &mutex_;
    bool should_do_colors_;
    std::unique_ptr<spdlog::formatter> formatter_;
    std::array<std::string, level::n_levels> colors_;
    void print_ccode_(const string_view_t &color_code);
    void print_range_(const memory_buf_t &formatted, size_t start, size_t end);
    static std::string to_string_(const string_view_t &sv);
};

template<typename ConsoleMutex>
class ansicolor_stdout_sink : public ansicolor_sink<ConsoleMutex>
{
public:
    explicit ansicolor_stdout_sink(color_mode mode = color_mode::automatic);
};

template<typename ConsoleMutex>
class ansicolor_stderr_sink : public ansicolor_sink<ConsoleMutex>
{
public:
    explicit ansicolor_stderr_sink(color_mode mode = color_mode::automatic);
};

using ansicolor_stdout_sink_mt = ansicolor_stdout_sink<details::console_mutex>;
using ansicolor_stdout_sink_st = ansicolor_stdout_sink<details::console_nullmutex>;

using ansicolor_stderr_sink_mt = ansicolor_stderr_sink<details::console_mutex>;
using ansicolor_stderr_sink_st = ansicolor_stderr_sink<details::console_nullmutex>;

} // namespace sinks
} // namespace spdlog

#ifdef SPDLOG_HEADER_ONLY
// #    include "ansicolor_sink-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/sinks/ansicolor_sink.h>

#endif

// #include <spdlog/pattern_formatter.h>

// #include <spdlog/details/os.h>


namespace spdlog {
namespace sinks {

template<typename ConsoleMutex>
SPDLOG_INLINE ansicolor_sink<ConsoleMutex>::ansicolor_sink(FILE *target_file, color_mode mode)
    : target_file_(target_file)
    , mutex_(ConsoleMutex::mutex())
    , formatter_(details::make_unique<spdlog::pattern_formatter>())

{
    set_color_mode(mode);
    colors_[level::trace] = to_string_(white);
    colors_[level::debug] = to_string_(cyan);
    colors_[level::info] = to_string_(green);
    colors_[level::warn] = to_string_(yellow_bold);
    colors_[level::err] = to_string_(red_bold);
    colors_[level::critical] = to_string_(bold_on_red);
    colors_[level::off] = to_string_(reset);
}

template<typename ConsoleMutex>
SPDLOG_INLINE void ansicolor_sink<ConsoleMutex>::set_color(level::level_enum color_level, string_view_t color)
{
    std::lock_guard<mutex_t> lock(mutex_);
    colors_[static_cast<size_t>(color_level)] = to_string_(color);
}

template<typename ConsoleMutex>
SPDLOG_INLINE void ansicolor_sink<ConsoleMutex>::log(const details::log_msg &msg)
{
    // Wrap the originally formatted message in color codes.
    // If color is not supported in the terminal, log as is instead.
    std::lock_guard<mutex_t> lock(mutex_);
    msg.color_range_start = 0;
    msg.color_range_end = 0;
    memory_buf_t formatted;
    formatter_->format(msg, formatted);
    if (should_do_colors_ && msg.color_range_end > msg.color_range_start)
    {
        // before color range
        print_range_(formatted, 0, msg.color_range_start);
        // in color range
        print_ccode_(colors_[static_cast<size_t>(msg.level)]);
        print_range_(formatted, msg.color_range_start, msg.color_range_end);
        print_ccode_(reset);
        // after color range
        print_range_(formatted, msg.color_range_end, formatted.size());
    }
    else // no color
    {
        print_range_(formatted, 0, formatted.size());
    }
    fflush(target_file_);
}

template<typename ConsoleMutex>
SPDLOG_INLINE void ansicolor_sink<ConsoleMutex>::flush()
{
    std::lock_guard<mutex_t> lock(mutex_);
    fflush(target_file_);
}

template<typename ConsoleMutex>
SPDLOG_INLINE void ansicolor_sink<ConsoleMutex>::set_pattern(const std::string &pattern)
{
    std::lock_guard<mutex_t> lock(mutex_);
    formatter_ = std::unique_ptr<spdlog::formatter>(new pattern_formatter(pattern));
}

template<typename ConsoleMutex>
SPDLOG_INLINE void ansicolor_sink<ConsoleMutex>::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter)
{
    std::lock_guard<mutex_t> lock(mutex_);
    formatter_ = std::move(sink_formatter);
}

template<typename ConsoleMutex>
SPDLOG_INLINE bool ansicolor_sink<ConsoleMutex>::should_color()
{
    return should_do_colors_;
}

template<typename ConsoleMutex>
SPDLOG_INLINE void ansicolor_sink<ConsoleMutex>::set_color_mode(color_mode mode)
{
    switch (mode)
    {
    case color_mode::always:
        should_do_colors_ = true;
        return;
    case color_mode::automatic:
        should_do_colors_ = details::os::in_terminal(target_file_) && details::os::is_color_terminal();
        return;
    case color_mode::never:
        should_do_colors_ = false;
        return;
    default:
        should_do_colors_ = false;
    }
}

template<typename ConsoleMutex>
SPDLOG_INLINE void ansicolor_sink<ConsoleMutex>::print_ccode_(const string_view_t &color_code)
{
    fwrite(color_code.data(), sizeof(char), color_code.size(), target_file_);
}

template<typename ConsoleMutex>
SPDLOG_INLINE void ansicolor_sink<ConsoleMutex>::print_range_(const memory_buf_t &formatted, size_t start, size_t end)
{
    fwrite(formatted.data() + start, sizeof(char), end - start, target_file_);
}

template<typename ConsoleMutex>
SPDLOG_INLINE std::string ansicolor_sink<ConsoleMutex>::to_string_(const string_view_t &sv)
{
    return std::string(sv.data(), sv.size());
}

// ansicolor_stdout_sink
template<typename ConsoleMutex>
SPDLOG_INLINE ansicolor_stdout_sink<ConsoleMutex>::ansicolor_stdout_sink(color_mode mode)
    : ansicolor_sink<ConsoleMutex>(stdout, mode)
{}

// ansicolor_stderr_sink
template<typename ConsoleMutex>
SPDLOG_INLINE ansicolor_stderr_sink<ConsoleMutex>::ansicolor_stderr_sink(color_mode mode)
    : ansicolor_sink<ConsoleMutex>(stderr, mode)
{}

} // namespace sinks
} // namespace spdlog

#endif

#    endif
#endif // SPDLOG_DISABLE_DEFAULT_LOGGER

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace spdlog {
namespace details {

SPDLOG_INLINE registry::registry()
    : formatter_(new pattern_formatter())
{

#ifndef SPDLOG_DISABLE_DEFAULT_LOGGER
    // create default logger (ansicolor_stdout_sink_mt or wincolor_stdout_sink_mt in windows).
#    ifdef _WIN32
    auto color_sink = std::make_shared<sinks::wincolor_stdout_sink_mt>();
#    else
    auto color_sink = std::make_shared<sinks::ansicolor_stdout_sink_mt>();
#    endif

    const char *default_logger_name = "";
    default_logger_ = std::make_shared<spdlog::logger>(default_logger_name, std::move(color_sink));
    loggers_[default_logger_name] = default_logger_;

#endif // SPDLOG_DISABLE_DEFAULT_LOGGER
}

SPDLOG_INLINE registry::~registry() = default;

SPDLOG_INLINE void registry::register_logger(std::shared_ptr<logger> new_logger)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    register_logger_(std::move(new_logger));
}

SPDLOG_INLINE void registry::initialize_logger(std::shared_ptr<logger> new_logger)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    new_logger->set_formatter(formatter_->clone());

    if (err_handler_)
    {
        new_logger->set_error_handler(err_handler_);
    }

    // set new level according to previously configured level or default level
    auto it = log_levels_.find(new_logger->name());
    auto new_level = it != log_levels_.end() ? it->second : global_log_level_;
    new_logger->set_level(new_level);

    new_logger->flush_on(flush_level_);

    if (backtrace_n_messages_ > 0)
    {
        new_logger->enable_backtrace(backtrace_n_messages_);
    }

    if (automatic_registration_)
    {
        register_logger_(std::move(new_logger));
    }
}

SPDLOG_INLINE std::shared_ptr<logger> registry::get(const std::string &logger_name)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    auto found = loggers_.find(logger_name);
    return found == loggers_.end() ? nullptr : found->second;
}

SPDLOG_INLINE std::shared_ptr<logger> registry::default_logger()
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    return default_logger_;
}

// Return raw ptr to the default logger.
// To be used directly by the spdlog default api (e.g. spdlog::info)
// This make the default API faster, but cannot be used concurrently with set_default_logger().
// e.g do not call set_default_logger() from one thread while calling spdlog::info() from another.
SPDLOG_INLINE logger *registry::get_default_raw()
{
    return default_logger_.get();
}

// set default logger.
// default logger is stored in default_logger_ (for faster retrieval) and in the loggers_ map.
SPDLOG_INLINE void registry::set_default_logger(std::shared_ptr<logger> new_default_logger)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    // remove previous default logger from the map
    if (default_logger_ != nullptr)
    {
        loggers_.erase(default_logger_->name());
    }
    if (new_default_logger != nullptr)
    {
        loggers_[new_default_logger->name()] = new_default_logger;
    }
    default_logger_ = std::move(new_default_logger);
}

SPDLOG_INLINE void registry::set_tp(std::shared_ptr<thread_pool> tp)
{
    std::lock_guard<std::recursive_mutex> lock(tp_mutex_);
    tp_ = std::move(tp);
}

SPDLOG_INLINE std::shared_ptr<thread_pool> registry::get_tp()
{
    std::lock_guard<std::recursive_mutex> lock(tp_mutex_);
    return tp_;
}

// Set global formatter. Each sink in each logger will get a clone of this object
SPDLOG_INLINE void registry::set_formatter(std::unique_ptr<formatter> formatter)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    formatter_ = std::move(formatter);
    for (auto &l : loggers_)
    {
        l.second->set_formatter(formatter_->clone());
    }
}

SPDLOG_INLINE void registry::enable_backtrace(size_t n_messages)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    backtrace_n_messages_ = n_messages;

    for (auto &l : loggers_)
    {
        l.second->enable_backtrace(n_messages);
    }
}

SPDLOG_INLINE void registry::disable_backtrace()
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    backtrace_n_messages_ = 0;
    for (auto &l : loggers_)
    {
        l.second->disable_backtrace();
    }
}

SPDLOG_INLINE void registry::set_level(level::level_enum log_level)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    for (auto &l : loggers_)
    {
        l.second->set_level(log_level);
    }
    global_log_level_ = log_level;
}

SPDLOG_INLINE void registry::flush_on(level::level_enum log_level)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    for (auto &l : loggers_)
    {
        l.second->flush_on(log_level);
    }
    flush_level_ = log_level;
}

SPDLOG_INLINE void registry::set_error_handler(err_handler handler)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    for (auto &l : loggers_)
    {
        l.second->set_error_handler(handler);
    }
    err_handler_ = std::move(handler);
}

SPDLOG_INLINE void registry::apply_all(const std::function<void(const std::shared_ptr<logger>)> &fun)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    for (auto &l : loggers_)
    {
        fun(l.second);
    }
}

SPDLOG_INLINE void registry::flush_all()
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    for (auto &l : loggers_)
    {
        l.second->flush();
    }
}

SPDLOG_INLINE void registry::drop(const std::string &logger_name)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    loggers_.erase(logger_name);
    if (default_logger_ && default_logger_->name() == logger_name)
    {
        default_logger_.reset();
    }
}

SPDLOG_INLINE void registry::drop_all()
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    loggers_.clear();
    default_logger_.reset();
}

// clean all resources and threads started by the registry
SPDLOG_INLINE void registry::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(flusher_mutex_);
        periodic_flusher_.reset();
    }

    drop_all();

    {
        std::lock_guard<std::recursive_mutex> lock(tp_mutex_);
        tp_.reset();
    }
}

SPDLOG_INLINE std::recursive_mutex &registry::tp_mutex()
{
    return tp_mutex_;
}

SPDLOG_INLINE void registry::set_automatic_registration(bool automatic_registration)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    automatic_registration_ = automatic_registration;
}

SPDLOG_INLINE void registry::set_levels(log_levels levels, level::level_enum *global_level)
{
    std::lock_guard<std::mutex> lock(logger_map_mutex_);
    log_levels_ = std::move(levels);
    auto global_level_requested = global_level != nullptr;
    global_log_level_ = global_level_requested ? *global_level : global_log_level_;

    for (auto &logger : loggers_)
    {
        auto logger_entry = log_levels_.find(logger.first);
        if (logger_entry != log_levels_.end())
        {
            logger.second->set_level(logger_entry->second);
        }
        else if (global_level_requested)
        {
            logger.second->set_level(*global_level);
        }
    }
}

SPDLOG_INLINE registry &registry::instance()
{
    static registry s_instance;
    return s_instance;
}

SPDLOG_INLINE void registry::throw_if_exists_(const std::string &logger_name)
{
    if (loggers_.find(logger_name) != loggers_.end())
    {
        throw_spdlog_ex("logger with name '" + logger_name + "' already exists");
    }
}

SPDLOG_INLINE void registry::register_logger_(std::shared_ptr<logger> new_logger)
{
    auto logger_name = new_logger->name();
    throw_if_exists_(logger_name);
    loggers_[logger_name] = std::move(new_logger);
}

} // namespace details
} // namespace spdlog

#endif

// #include <spdlog/logger.h>

// #include <spdlog/version.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#define SPDLOG_VER_MAJOR 1
#define SPDLOG_VER_MINOR 10
#define SPDLOG_VER_PATCH 0

#define SPDLOG_VERSION (SPDLOG_VER_MAJOR * 10000 + SPDLOG_VER_MINOR * 100 + SPDLOG_VER_PATCH)

// #include <spdlog/details/synchronous_factory.h>
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



// #include "registry.h"


namespace spdlog {

// Default logger factory-  creates synchronous loggers
class logger;

struct synchronous_factory
{
    template<typename Sink, typename... SinkArgs>
    static std::shared_ptr<spdlog::logger> create(std::string logger_name, SinkArgs &&... args)
    {
        auto sink = std::make_shared<Sink>(std::forward<SinkArgs>(args)...);
        auto new_logger = std::make_shared<spdlog::logger>(std::move(logger_name), std::move(sink));
        details::registry::instance().initialize_logger(new_logger);
        return new_logger;
    }
};
} // namespace spdlog


#include <chrono>
#include <functional>
#include <memory>
#include <string>

namespace spdlog {

using default_factory = synchronous_factory;

// Create and register a logger with a templated sink type
// The logger's level, formatter and flush level will be set according the
// global settings.
//
// Example:
//   spdlog::create<daily_file_sink_st>("logger_name", "dailylog_filename", 11, 59);
template<typename Sink, typename... SinkArgs>
inline std::shared_ptr<spdlog::logger> create(std::string logger_name, SinkArgs &&... sink_args)
{
    return default_factory::create<Sink>(std::move(logger_name), std::forward<SinkArgs>(sink_args)...);
}

// Initialize and register a logger,
// formatter and flush level will be set according the global settings.
//
// Useful for initializing manually created loggers with the global settings.
//
// Example:
//   auto mylogger = std::make_shared<spdlog::logger>("mylogger", ...);
//   spdlog::initialize_logger(mylogger);
SPDLOG_API void initialize_logger(std::shared_ptr<logger> logger);

// Return an existing logger or nullptr if a logger with such name doesn't
// exist.
// example: spdlog::get("my_logger")->info("hello {}", "world");
SPDLOG_API std::shared_ptr<logger> get(const std::string &name);

// Set global formatter. Each sink in each logger will get a clone of this object
SPDLOG_API void set_formatter(std::unique_ptr<spdlog::formatter> formatter);

// Set global format string.
// example: spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
SPDLOG_API void set_pattern(std::string pattern, pattern_time_type time_type = pattern_time_type::local);

// enable global backtrace support
SPDLOG_API void enable_backtrace(size_t n_messages);

// disable global backtrace support
SPDLOG_API void disable_backtrace();

// call dump backtrace on default logger
SPDLOG_API void dump_backtrace();

// Get global logging level
SPDLOG_API level::level_enum get_level();

// Set global logging level
SPDLOG_API void set_level(level::level_enum log_level);

// Determine whether the default logger should log messages with a certain level
SPDLOG_API bool should_log(level::level_enum lvl);

// Set global flush level
SPDLOG_API void flush_on(level::level_enum log_level);

// Start/Restart a periodic flusher thread
// Warning: Use only if all your loggers are thread safe!
template<typename Rep, typename Period>
inline void flush_every(std::chrono::duration<Rep, Period> interval)
{
    details::registry::instance().flush_every(interval);
}

// Set global error handler
SPDLOG_API void set_error_handler(void (*handler)(const std::string &msg));

// Register the given logger with the given name
SPDLOG_API void register_logger(std::shared_ptr<logger> logger);

// Apply a user defined function on all registered loggers
// Example:
// spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) {l->flush();});
SPDLOG_API void apply_all(const std::function<void(std::shared_ptr<logger>)> &fun);

// Drop the reference to the given logger
SPDLOG_API void drop(const std::string &name);

// Drop all references from the registry
SPDLOG_API void drop_all();

// stop any running threads started by spdlog and clean registry loggers
SPDLOG_API void shutdown();

// Automatic registration of loggers when using spdlog::create() or spdlog::create_async
SPDLOG_API void set_automatic_registration(bool automatic_registration);

// API for using default logger (stdout_color_mt),
// e.g: spdlog::info("Message {}", 1);
//
// The default logger object can be accessed using the spdlog::default_logger():
// For example, to add another sink to it:
// spdlog::default_logger()->sinks().push_back(some_sink);
//
// The default logger can replaced using spdlog::set_default_logger(new_logger).
// For example, to replace it with a file logger.
//
// IMPORTANT:
// The default API is thread safe (for _mt loggers), but:
// set_default_logger() *should not* be used concurrently with the default API.
// e.g do not call set_default_logger() from one thread while calling spdlog::info() from another.

SPDLOG_API std::shared_ptr<spdlog::logger> default_logger();

SPDLOG_API spdlog::logger *default_logger_raw();

SPDLOG_API void set_default_logger(std::shared_ptr<spdlog::logger> default_logger);

template<typename... Args>
inline void log(source_loc source, level::level_enum lvl, format_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->log(source, lvl, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void log(level::level_enum lvl, format_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->log(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void trace(format_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->trace(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void debug(format_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(format_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(format_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(format_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void critical(format_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->critical(fmt, std::forward<Args>(args)...);
}

template<typename T>
inline void log(source_loc source, level::level_enum lvl, const T &msg)
{
    default_logger_raw()->log(source, lvl, msg);
}

template<typename T>
inline void log(level::level_enum lvl, const T &msg)
{
    default_logger_raw()->log(lvl, msg);
}

#ifdef SPDLOG_WCHAR_TO_UTF8_SUPPORT
template<typename... Args>
inline void log(source_loc source, level::level_enum lvl, wformat_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->log(source, lvl, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void log(level::level_enum lvl, wformat_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->log(source_loc{}, lvl, fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void trace(wformat_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->trace(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void debug(wformat_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->debug(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(wformat_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->info(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(wformat_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->warn(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(wformat_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->error(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
inline void critical(wformat_string_t<Args...> fmt, Args &&... args)
{
    default_logger_raw()->critical(fmt, std::forward<Args>(args)...);
}
#endif

template<typename T>
inline void trace(const T &msg)
{
    default_logger_raw()->trace(msg);
}

template<typename T>
inline void debug(const T &msg)
{
    default_logger_raw()->debug(msg);
}

template<typename T>
inline void info(const T &msg)
{
    default_logger_raw()->info(msg);
}

template<typename T>
inline void warn(const T &msg)
{
    default_logger_raw()->warn(msg);
}

template<typename T>
inline void error(const T &msg)
{
    default_logger_raw()->error(msg);
}

template<typename T>
inline void critical(const T &msg)
{
    default_logger_raw()->critical(msg);
}

} // namespace spdlog

//
// enable/disable log calls at compile time according to global level.
//
// define SPDLOG_ACTIVE_LEVEL to one of those (before including spdlog.h):
// SPDLOG_LEVEL_TRACE,
// SPDLOG_LEVEL_DEBUG,
// SPDLOG_LEVEL_INFO,
// SPDLOG_LEVEL_WARN,
// SPDLOG_LEVEL_ERROR,
// SPDLOG_LEVEL_CRITICAL,
// SPDLOG_LEVEL_OFF
//

#ifndef SPDLOG_NO_SOURCE_LOC
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)
#else
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{}, level, __VA_ARGS__)
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
#    define SPDLOG_LOGGER_TRACE(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::trace, __VA_ARGS__)
#    define SPDLOG_TRACE(...) SPDLOG_LOGGER_TRACE(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_TRACE(logger, ...) (void)0
#    define SPDLOG_TRACE(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
#    define SPDLOG_LOGGER_DEBUG(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::debug, __VA_ARGS__)
#    define SPDLOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_DEBUG(logger, ...) (void)0
#    define SPDLOG_DEBUG(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
#    define SPDLOG_LOGGER_INFO(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::info, __VA_ARGS__)
#    define SPDLOG_INFO(...) SPDLOG_LOGGER_INFO(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_INFO(logger, ...) (void)0
#    define SPDLOG_INFO(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
#    define SPDLOG_LOGGER_WARN(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::warn, __VA_ARGS__)
#    define SPDLOG_WARN(...) SPDLOG_LOGGER_WARN(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_WARN(logger, ...) (void)0
#    define SPDLOG_WARN(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
#    define SPDLOG_LOGGER_ERROR(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::err, __VA_ARGS__)
#    define SPDLOG_ERROR(...) SPDLOG_LOGGER_ERROR(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_ERROR(logger, ...) (void)0
#    define SPDLOG_ERROR(...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
#    define SPDLOG_LOGGER_CRITICAL(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::critical, __VA_ARGS__)
#    define SPDLOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(spdlog::default_logger_raw(), __VA_ARGS__)
#else
#    define SPDLOG_LOGGER_CRITICAL(logger, ...) (void)0
#    define SPDLOG_CRITICAL(...) (void)0
#endif

#ifdef SPDLOG_HEADER_ONLY
// #    include "spdlog-inl.h"
// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)



#ifndef SPDLOG_HEADER_ONLY
// #    include <spdlog/spdlog.h>

#endif

// #include <spdlog/common.h>

// #include <spdlog/pattern_formatter.h>


namespace spdlog {

SPDLOG_INLINE void initialize_logger(std::shared_ptr<logger> logger)
{
    details::registry::instance().initialize_logger(std::move(logger));
}

SPDLOG_INLINE std::shared_ptr<logger> get(const std::string &name)
{
    return details::registry::instance().get(name);
}

SPDLOG_INLINE void set_formatter(std::unique_ptr<spdlog::formatter> formatter)
{
    details::registry::instance().set_formatter(std::move(formatter));
}

SPDLOG_INLINE void set_pattern(std::string pattern, pattern_time_type time_type)
{
    set_formatter(std::unique_ptr<spdlog::formatter>(new pattern_formatter(std::move(pattern), time_type)));
}

SPDLOG_INLINE void enable_backtrace(size_t n_messages)
{
    details::registry::instance().enable_backtrace(n_messages);
}

SPDLOG_INLINE void disable_backtrace()
{
    details::registry::instance().disable_backtrace();
}

SPDLOG_INLINE void dump_backtrace()
{
    default_logger_raw()->dump_backtrace();
}

SPDLOG_INLINE level::level_enum get_level()
{
    return default_logger_raw()->level();
}

SPDLOG_INLINE bool should_log(level::level_enum log_level)
{
    return default_logger_raw()->should_log(log_level);
}

SPDLOG_INLINE void set_level(level::level_enum log_level)
{
    details::registry::instance().set_level(log_level);
}

SPDLOG_INLINE void flush_on(level::level_enum log_level)
{
    details::registry::instance().flush_on(log_level);
}

SPDLOG_INLINE void set_error_handler(void (*handler)(const std::string &msg))
{
    details::registry::instance().set_error_handler(handler);
}

SPDLOG_INLINE void register_logger(std::shared_ptr<logger> logger)
{
    details::registry::instance().register_logger(std::move(logger));
}

SPDLOG_INLINE void apply_all(const std::function<void(std::shared_ptr<logger>)> &fun)
{
    details::registry::instance().apply_all(fun);
}

SPDLOG_INLINE void drop(const std::string &name)
{
    details::registry::instance().drop(name);
}

SPDLOG_INLINE void drop_all()
{
    details::registry::instance().drop_all();
}

SPDLOG_INLINE void shutdown()
{
    details::registry::instance().shutdown();
}

SPDLOG_INLINE void set_automatic_registration(bool automatic_registration)
{
    details::registry::instance().set_automatic_registration(automatic_registration);
}

SPDLOG_INLINE std::shared_ptr<spdlog::logger> default_logger()
{
    return details::registry::instance().default_logger();
}

SPDLOG_INLINE spdlog::logger *default_logger_raw()
{
    return details::registry::instance().get_default_raw();
}

SPDLOG_INLINE void set_default_logger(std::shared_ptr<spdlog::logger> default_logger)
{
    details::registry::instance().set_default_logger(std::move(default_logger));
}

} // namespace spdlog

#endif

#endif // SPDLOG_H
