// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <ranges>
#include <tuple>
#include <vector>

namespace eggs::test::detail {

#if defined(__cpp_lib_ranges_cartesian_product)

using std::views::cartesian_product;

#else

// Non-lazy fallback: materializes the product into a vector<tuple<...>>.

template <std::ranges::range R>
auto cartesian_product(R const& r)
{
    using V = std::ranges::range_value_t<R>;
    std::vector<std::tuple<V>> result;
    for (auto const& v : r) result.emplace_back(v);
    return result;
}

template <std::ranges::range R, std::ranges::range... Rs>
auto cartesian_product(R const& r, Rs const&... rs)
{
    auto tail = cartesian_product(rs...);
    using H = std::ranges::range_value_t<R>;
    using T = typename decltype(tail)::value_type;
    using E = decltype(std::tuple_cat(
        std::declval<std::tuple<H>>(), std::declval<T>()
    ));
    std::vector<E> result;
    for (auto const& h : r)
        for (auto const& t : tail)
            result.push_back(std::tuple_cat(std::make_tuple(h), t));
    return result;
}

#endif

} // namespace eggs::test::detail
