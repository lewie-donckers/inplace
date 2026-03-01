// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

// NOLINTNEXTLINE(misc-include-cleaner) false positive
#include <inplace/details/macros.hpp>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

namespace inplace::details::functional {

template <std::size_t S, std::size_t A>
struct aligned_storage {
    static constexpr auto size = S;
    static constexpr auto align = A;

    // TODO use std::array instead? and can this type then be an alias?
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays)
    alignas(align) std::byte buffer[size];
};

template <typename T>
struct is_aligned_storage : std::false_type {};

template <std::size_t S, std::size_t A>
struct is_aligned_storage<aligned_storage<S, A>> : std::true_type {};

template <typename T>
constexpr bool is_aligned_storage_v = is_aligned_storage<T>::value;

template <typename C, typename S>
    requires(is_aligned_storage_v<S>)
class i_stored_callable;

template <typename R, typename... Args, typename S>
class i_stored_callable<R(Args...), S> {
public:
    virtual ~i_stored_callable() = default;

    // NOLINTNEXTLINE(portability-template-virtual-member-function) only used internally
    [[nodiscard]] virtual R call(Args... arguments) = 0;

    // NOLINTNEXTLINE(portability-template-virtual-member-function) only used internally
    [[nodiscard]] virtual i_stored_callable* move(S& storage) noexcept = 0;
};

template <typename C, typename S, typename T>
    requires(is_aligned_storage_v<S> && std::is_same_v<std::decay_t<T>, T>)
class stored_callable;

template <typename R, typename... Args, typename S, typename T>
class stored_callable<R(Args...), S, T> final : public i_stored_callable<R(Args...), S> {
public:
    explicit stored_callable(T callable) : callable_{std::move(callable)} {}

    [[nodiscard]] R call(Args... arguments) override {
#ifdef INPLACE_COMPILER_GCC
#pragma GCC diagnostic push
// Suppress false-positive warnings in GCC14/15 when using std::invoke with
// member function pointers. Reported as:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=111750
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
        return std::invoke(callable_, std::forward<Args>(arguments)...);
#ifdef INPLACE_COMPILER_GCC
#pragma GCC diagnostic pop
#endif
    }

    [[nodiscard]] i_stored_callable<R(Args...), S>* move(S& storage) noexcept override {
        return construct<R(Args...), S, T>(storage, std::move(callable_));
    }

private:
    T callable_;
};

template <typename C, typename S, typename T>
struct is_storage_large_enough
    : std::bool_constant<S::size >= ((std::max(alignof(stored_callable<C, S, T>), S::align) - S::align) +
                                     sizeof(stored_callable<C, S, T>))> {};

template <typename C, typename S, typename T>
constexpr bool is_storage_large_enough_v = is_storage_large_enough<C, S, T>::value;

template <typename C, typename S, typename T>
auto* get_address(S& storage) {
    static constexpr auto align = alignof(stored_callable<C, S, T>);
    const auto address = std::bit_cast<std::uintptr_t>(&storage.buffer);
    const auto aligned_address = ((address + align - 1) / align) * align;

    return std::bit_cast<stored_callable<C, S, T>*>(aligned_address);
}

template <typename C, typename S, typename T>
auto* construct(S& storage, T&& callable) {
    auto* const address = get_address<C, S, std::decay_t<T>>(storage);

    return std::construct_at<stored_callable<C, S, std::decay_t<T>>>(address, std::forward<T>(callable));
}

template <typename>
struct member_function_call_type;

template <typename R, typename T, typename... Args>
struct member_function_call_type<R (T::*)(Args...)> {
    using type = R(Args...);
};

template <typename R, typename T, typename... Args>
struct member_function_call_type<R (T::*)(Args...) const> {
    using type = R(Args...);
};

template <typename R, typename T, typename... Args>
struct member_function_call_type<R (T::*)(Args...) &> {
    using type = R(Args...);
};

template <typename R, typename T, typename... Args>
struct member_function_call_type<R (T::*)(Args...) const&> {
    using type = R(Args...);
};

template <typename R, typename T, typename... Args>
struct member_function_call_type<R (T::*)(Args...) noexcept> {
    using type = R(Args...);
};

template <typename R, typename T, typename... Args>
struct member_function_call_type<R (T::*)(Args...) const noexcept> {
    using type = R(Args...);
};

template <typename R, typename T, typename... Args>
struct member_function_call_type<R (T::*)(Args...) & noexcept> {
    using type = R(Args...);
};

template <typename R, typename T, typename... Args>
struct member_function_call_type<R (T::*)(Args...) const & noexcept> {
    using type = R(Args...);
};

template <typename T>
using member_function_call_type_t = typename member_function_call_type<T>::type;

}  // namespace inplace::details::functional