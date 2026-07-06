// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace eggs::test::detail {

// Exception thrown by REQUIRE to unwind the current test case.
struct unwind
{};

template <typename T>
inline T require(T value)
{
    return static_cast<bool>(value) ? value : throw unwind{};
}

} // namespace eggs::test::detail
