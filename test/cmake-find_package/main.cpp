// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(verify, "verifies the library links and runs")
{
    CHECK(1 + 1 == 2);
}

// PROBE: intentionally fails to check whether the internal check_throws<>
// wrapper frame leaks into the diagnostic when this TU is compiled against
// an *installed* copy of the headers (find_package), where the library's
// own library_root() self-lookup was resolved against the original source
// checkout instead. Not meant to be merged.
TEST_CASE(
    probe_require_propagated,
    "PROBE: REQUIRE failing inside CHECK_THROWS propagates out"
)
{
    CHECK_THROWS(REQUIRE(1 + 2 == 4));
}
