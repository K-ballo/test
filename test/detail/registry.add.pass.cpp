// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/run_state.hpp>

#include <source_location>
#include <string_view>
#include <utility>

#include "support.hpp"

namespace {

namespace detail = eggs::test::detail;

void noop_run(detail::run_state&) {}

detail::test_entry make_entry(std::string_view const name)
{
    return {
        .name = name,
        .desc = "desc",
        .run = &noop_run,
        .loc = std::source_location::current(),
    };
}

void add_inserts_into_cases()
{
    auto const* const e = detail::registry::add(make_entry("registry_add_a"));

    EGGS_TEST_DETAIL_CHECK(e != nullptr);
    EGGS_TEST_DETAIL_CHECK(e->name == "registry_add_a");

    auto const it =
        detail::registry::cases().find(std::string_view{"registry_add_a"});
    EGGS_TEST_DETAIL_CHECK(it != detail::registry::cases().end());
    EGGS_TEST_DETAIL_CHECK(&*it == e);
}

void add_duplicate_name_keeps_first_entry()
{
    auto const* const first =
        detail::registry::add(make_entry("registry_add_dup"));

    auto second_entry = make_entry("registry_add_dup");
    second_entry.desc = "second desc";
    auto const* const second = detail::registry::add(std::move(second_entry));

    // insert() into an unordered_set keeps the first element on a
    // duplicate key, so add() silently discards the second registration.
    EGGS_TEST_DETAIL_CHECK(second == first);
    EGGS_TEST_DETAIL_CHECK(second->desc == "desc");
}

void cases_returns_same_container_across_calls()
{
    EGGS_TEST_DETAIL_CHECK(
        &detail::registry::cases() == &detail::registry::cases()
    );
}

} // namespace

int main()
{
    add_inserts_into_cases();
    add_duplicate_name_keeps_first_entry();
    cases_returns_same_container_across_calls();

    return eggs::test_support::report();
}
