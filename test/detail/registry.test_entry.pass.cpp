// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/registry.hpp>

#include <source_location>
#include <string_view>
#include <type_traits>

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

} // namespace

TEST_CASE(
    test_entry_is_transparent,
    "test_entry_hash and test_entry_equal declare is_transparent"
)
{
    CHECK(
        std::is_same_v<
            eggs::test::detail::test_entry_hash::is_transparent, void>
    );
    CHECK(
        std::is_same_v<
            eggs::test::detail::test_entry_equal::is_transparent, void>
    );
}

TEST_CASE(
    test_entry_hash_agrees_with_name,
    "hashing a test_entry equals hashing its name directly"
)
{
    eggs::test::detail::test_entry_hash const hash;
    auto const e = make_entry("foo");

    CHECK(hash(e) == hash(std::string_view{"foo"}));
}

TEST_CASE(
    test_entry_equal_agrees_across_overloads,
    "equal(name, entry) and equal(entry, name) agree with equal(entry, entry)"
)
{
    eggs::test::detail::test_entry_equal const equal;
    auto const a = make_entry("foo");
    auto const b = make_entry("foo");

    REQUIRE(equal(a, b));
    CHECK(equal(a, std::string_view{"foo"}));
    CHECK(equal(std::string_view{"foo"}, b));
}

TEST_CASE(
    test_entry_distinguishes_different_names,
    "entries with different names are unequal, by name, both ways"
)
{
    eggs::test::detail::test_entry_equal const equal;
    auto const a = make_entry("foo");
    auto const b = make_entry("bar");

    REQUIRE(!equal(a, b));
    CHECK(!equal(std::string_view{"foo"}, b));
    CHECK(!equal(a, std::string_view{"bar"}));
}

TEST_CASE(
    test_entry_heterogeneous_find,
    "unordered_set<test_entry> can be looked up by string_view directly"
)
{
    eggs::test::detail::registry::cases_type cases;
    cases.insert(make_entry("foo"));
    cases.insert(make_entry("bar"));

    auto const it = cases.find(std::string_view{"foo"});
    REQUIRE(it != cases.end());
    CHECK(it->name == "foo");

    CHECK(cases.find(std::string_view{"missing"}) == cases.end());
}
