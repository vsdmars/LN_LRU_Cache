// A function-like feature checking macro that is a wrapper around
// `__has_attribute`, which is defined by GCC 5+ and Clang and evaluates to a
// nonzero constant integer if the attribute is supported or 0 if not.
//
// It evaluates to zero if `__has_attribute` is not defined by the compiler.
//
// GCC: https://gcc.gnu.org/gcc-5/changes.html
// Clang: https://clang.llvm.org/docs/LanguageExtensions.html
//
// Reference:
// https://chromium.googlesource.com/external/github.com/abseil/abseil-cpp/+/refs/heads/lts_2018_06_20/absl/base/attributes.h

#pragma once

#ifdef __has_attribute
#define ABUSE_HAVE_ATTRIBUTE(x) __has_attribute(x)
#else
#define ABUSE_HAVE_ATTRIBUTE(x) 0
#endif

// ABUSE_LOCKABLE
//
// Documents if a class/type is a lockable type (such as the `Mutex` class).
#if ABUSE_HAVE_ATTRIBUTE(lockable)
#define ABUSE_LOCKABLE __attribute__((lockable))
#else
#define ABUSE_LOCKABLE
#endif
