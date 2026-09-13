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

namespace eggs::test_support {

// Invokes fn(out) with out backed by a temporary file, then returns
// everything written to it.
inline std::string capture(void (*fn)(std::FILE*))
{
    std::FILE* const out = std::tmpfile();
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
