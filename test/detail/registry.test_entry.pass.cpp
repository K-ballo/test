// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/registry.hpp>

#include <source_location>
#include <string_view>
#include <type_traits>

#include "support.hpp"

// test_entry_hash/test_entry_equal are the Hash and KeyEqual of registry's
// unordered_set<test_entry>. Verify they satisfy the requirements for
// heterogeneous lookup.

namespace {

eggs::test::detail::test_entry make_entry(std::string_view const name)
{
    return {
        .name = name,
        .desc = "desc",
        .run = nullptr,
        .loc = std::source_location::current(),
    };
}

void test_entry_is_transparent()
{
    EGGS_TEST_DETAIL_ASSERT(
        std::is_same_v<
            eggs::test::detail::test_entry_hash::is_transparent, void>
    );
    EGGS_TEST_DETAIL_ASSERT(
        std::is_same_v<
            eggs::test::detail::test_entry_equal::is_transparent, void>
    );
}

void test_entry_hash_agrees_with_name()
{
    eggs::test::detail::test_entry_hash const hash;
    auto const e = make_entry("foo");

    EGGS_TEST_DETAIL_ASSERT(hash(e) == hash(std::string_view{"foo"}));
}

void test_entry_equal_agrees_across_overloads()
{
    eggs::test::detail::test_entry_equal const equal;
    auto const a = make_entry("foo");
    auto const b = make_entry("foo");

    EGGS_TEST_DETAIL_ASSERT(equal(a, b));
    EGGS_TEST_DETAIL_ASSERT(equal(a, std::string_view{"foo"}));
    EGGS_TEST_DETAIL_ASSERT(equal(std::string_view{"foo"}, b));
}

void test_entry_distinguishes_different_names()
{
    eggs::test::detail::test_entry_equal const equal;
    auto const a = make_entry("foo");
    auto const b = make_entry("bar");

    EGGS_TEST_DETAIL_ASSERT(!equal(a, b));
    EGGS_TEST_DETAIL_ASSERT(!equal(std::string_view{"foo"}, b));
    EGGS_TEST_DETAIL_ASSERT(!equal(a, std::string_view{"bar"}));
}

void test_entry_heterogeneous_find()
{
    eggs::test::detail::registry::cases_type cases;
    cases.insert(make_entry("foo"));
    cases.insert(make_entry("bar"));

    auto const it = cases.find(std::string_view{"foo"});
    EGGS_TEST_DETAIL_ASSERT(it != cases.end());
    EGGS_TEST_DETAIL_ASSERT(it->name == "foo");

    EGGS_TEST_DETAIL_ASSERT(
        cases.find(std::string_view{"missing"}) == cases.end()
    );
}

} // namespace

int main()
{
    test_entry_is_transparent();
    test_entry_hash_agrees_with_name();
    test_entry_equal_agrees_across_overloads();
    test_entry_distinguishes_different_names();
    test_entry_heterogeneous_find();

    return 0;
}
