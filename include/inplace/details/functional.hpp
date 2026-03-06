// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

#include <inplace/details/macros.hpp>

#include <bit>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <utility>

namespace inplace::details::functional {

template <typename T, std::size_t N>
consteval std::size_t max_align_offset_required() {
    const auto align_storage = alignof(void*);
    const auto align_object = alignof(T);
    static_assert(align_object > 0);

    return std::max(align_storage - 1, align_object) - align_object;
}

template <typename T, std::size_t N>
consteval std::size_t max_size_required() {
    return max_align_offset_required<T, N>() + sizeof(T);
}

template <typename T, std::size_t N>
struct will_fit : std::bool_constant<max_size_required<T, N>() <= N> {};

template <typename T, std::size_t N>
constexpr bool will_fit_v = will_fit<T, N>::value;

enum class operation {
    copy,
    move,
    destroy,
    get_function_ptr,
};

template <typename R, typename... Args>
using function_ptr = R (*)(const std::byte*, Args...);

template <typename R, typename... Args>
using manage_ptr = function_ptr<R, Args...> (*)(operation, const std::byte*, std::byte*);

template <typename T, std::size_t S, typename R, typename... Args>
struct manager {
    template <typename U>
    static T* construct(std::byte* storage, U&& arg) {
        return std::construct_at<T>(get_address(storage), std::forward<U>(arg));
    }

    static R call(const std::byte* storage, Args... args) {
        auto* obj = get_address(const_cast<std::byte*>(storage));
        return std::invoke(*obj, std::forward<Args>(args)...);
    }

    static function_ptr<R, Args...> manage(operation op, const std::byte* arg1, std::byte* arg2) {
        switch (op) {
            case operation::copy: {
                const auto* src = get_address(const_cast<std::byte*>(arg1));
                auto* dst = get_address(arg2);
                std::construct_at<T>(dst, *src);
                return nullptr;
            }
            case operation::move: {
                // TODO use copy if T is not nothrow_move_constructible
                auto* src = get_address(const_cast<std::byte*>(arg1));
                auto* dst = get_address(arg2);
                std::construct_at<T>(dst, std::move(*src));
                return nullptr;
            }
            case operation::destroy: {
                std::destroy_at(get_address(const_cast<std::byte*>(arg1)));
                return nullptr;
            }
            case operation::get_function_ptr: {
                return &call;
            }
        }
        return nullptr;
    }

    static T* get_address(std::byte* storage) {
        const auto align = alignof(T);
        const auto address = std::bit_cast<std::uintptr_t>(storage);
        const auto aligned_address = ((address + align - 1) / align) * align;

        return std::bit_cast<T*>(aligned_address);
    }
};

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