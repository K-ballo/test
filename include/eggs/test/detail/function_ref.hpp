// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <version>

namespace eggs::test::detail {

#if defined(__cpp_lib_function_ref)

template <typename Sig>
using function_ref = std::function_ref<Sig>;

#else

template <typename Sig>
class function_ref;

template <typename R, typename... Args>
class function_ref<R(Args...)>
{
    void* _obj;
    R (*_invoke)(void*, Args...);

  public:
    template <typename F>
        requires(not std::is_same_v<std::remove_cvref_t<F>, function_ref> &&
                 std::is_invocable_r_v<R, std::remove_reference_t<F>&, Args...>)
    constexpr function_ref(F&& f) noexcept
        : _obj(const_cast<void*>(static_cast<void const*>(std::addressof(f)))),
          _invoke([](void* obj, Args... args) -> R {
              return std::invoke(
                  *static_cast<std::remove_reference_t<F>*>(obj),
                  std::forward<Args>(args)...
              );
          })
    {
    }

    constexpr R operator()(Args... args) const
    {
        return _invoke(_obj, std::forward<Args>(args)...);
    }
};

#endif

} // namespace eggs::test::detail
