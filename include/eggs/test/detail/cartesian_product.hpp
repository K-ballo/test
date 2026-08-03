// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <initializer_list>
#include <ranges>
#include <tuple>
#include <utility>
#include <vector>

namespace eggs::test::detail {

#if defined(__cpp_lib_ranges_cartesian_product)

template <std::ranges::range... Rs>
auto cartesian_product(Rs&&... rs)
{
    return std::views::cartesian_product(std::forward<Rs>(rs)...);
}

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

// A braced-init-list cannot deduce a generic range parameter, so it needs its
// own overload; materialize it into a vector so it satisfies std::ranges::range.
template <typename T>
auto cartesian_product(std::initializer_list<T> il)
{
    return cartesian_product(std::vector<T>(il));
}

template <typename T, typename... Rest>
void cartesian_product(std::initializer_list<T>, Rest&&...)
#if __cpp_deleted_function >= 202403L && __cplusplus > 202302L
    = delete ("only a single braced-init-list is supported");
#else
    = delete;
#endif

} // namespace eggs::test::detail
