// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// TEMPORARY: deliberately triggers ASan and UBSan reports, to verify that the
// `sanitize` CI workflow actually fails when a sanitizer finds a bug. Not part
// of the permanent test suite; remove once the workflow run is confirmed red.

#include <cstdio>
#include <limits>

int main(int argc, char**)
{
    // ASan: heap-buffer-overflow (read one int past the end of the allocation).
    auto* buf = new int[4];
    int const oob = buf[argc + 4];
    std::printf("oob read: %d\n", oob);
    delete[] buf;

    // UBSan: signed integer overflow.
    int x = std::numeric_limits<int>::max();
    x += argc;
    return x;
}
