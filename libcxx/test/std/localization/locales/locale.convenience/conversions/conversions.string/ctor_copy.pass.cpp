//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// These constructors are still unavailable in C++03, but this test depends
// on access control SFINAE and fails without it.
// UNSUPPORTED: c++03

// <locale>

// ADDITIONAL_COMPILE_FLAGS: -D_LIBCPP_DISABLE_DEPRECATION_WARNINGS -D_LIBCPP_ENABLE_CXX26_REMOVED_CODECVT -D_LIBCPP_ENABLE_CXX26_REMOVED_WSTRING_CONVERT

// wstring_convert<Codecvt, Elem, Wide_alloc, Byte_alloc>

// wstring_convert(wstring_convert const&) = delete;
// wstring_convert& operator=(wstring_convert const&) = delete;

// XFAIL: no-wide-characters

#include <cassert>
#include <codecvt>
#include <locale>
#include <type_traits>

#include "test_macros.h"

int main(int, char**)
{
    typedef std::codecvt_utf8<wchar_t> Codecvt;
    typedef std::wstring_convert<Codecvt> Myconv;
    static_assert(!std::is_copy_constructible<Myconv>::value, "");
    static_assert(!std::is_copy_assignable<Myconv>::value, "");

  return 0;
}
