// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test.hpp>

#include <cstddef>
#include <cstdio>
#include <string>

#if !defined(_WIN32)
// Declared ourselves rather than relying on <cstdio>, since glibc/Darwin
// hide it behind feature-test macros that CMAKE_CXX_EXTENSIONS=OFF disables.
extern "C" std::FILE* fmemopen(void* buf, std::size_t size, char const* mode);
#endif

namespace eggs::test_support {

// Invokes fn(out) with out backed by an in-memory stream (a temporary file
// where fmemopen is not available), then returns everything written to it.
inline std::string capture(void (*fn)(std::FILE*))
{
#if !defined(_WIN32)
    char membuf[1 << 16];
    std::FILE* const out = ::fmemopen(membuf, sizeof membuf, "w+");
#else
    std::FILE* const out = std::tmpfile();
#endif
    REQUIRE(out != nullptr);

    fn(out);
    std::rewind(out);

    std::string result;
    char buf[256];
    for (std::size_t n; (n = std::fread(buf, 1, sizeof buf, out)) > 0;) {
        result.append(buf, n);
    }
    std::fclose(out);

    return result;
}

} // namespace eggs::test_support
