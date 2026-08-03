// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <version>

namespace eggs::test::detail {

#if defined(__cpp_lib_move_only_function)

template <typename Sig>
using move_only_function = std::move_only_function<Sig>;

#else

// Minimal move-only type-erased callable, standing in for
// std::move_only_function where the standard library lacks it (e.g. libc++).
// Only the `R(Args...) const` form is provided, matching current usage.
template <typename Sig>
class move_only_function;

template <typename R, typename... Args>
class move_only_function<R(Args...) const>
{
    struct callable_base
    {
        virtual ~callable_base() = default;
        virtual R call(Args... args) const = 0;
    };

    template <typename F>
    struct callable_impl final : callable_base
    {
        F f;

        explicit callable_impl(F f)
            : f(std::move(f))
        {
        }

        R call(Args... args) const override
        {
            return std::invoke(f, std::forward<Args>(args)...);
        }
    };

    std::unique_ptr<callable_base> _callable;

  public:
    move_only_function() noexcept = default;
    move_only_function(move_only_function&&) noexcept = default;
    move_only_function& operator=(move_only_function&&) noexcept = default;

    template <typename F>
        requires(!std::same_as<std::decay_t<F>, move_only_function>)
    move_only_function(F&& f)
        : _callable(
              std::make_unique<callable_impl<std::decay_t<F>>>(
                  std::forward<F>(f)
              )
          )
    {
    }

    explicit operator bool() const noexcept { return _callable != nullptr; }

    R operator()(Args... args) const
    {
        return _callable->call(std::forward<Args>(args)...);
    }
};

#endif

} // namespace eggs::test::detail
