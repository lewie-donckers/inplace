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

#include <array>
#include <cstddef>
#include <cstdint>
#include <typeinfo>

namespace inplace {

template <std::size_t N>
class move_only_any;

namespace details::any {

template <typename T>
struct is_move_only_any : std::false_type {};

template <std::size_t N>
struct is_move_only_any<move_only_any<N>> : std::true_type {};

template <typename T>
constexpr bool is_move_only_any_v = is_move_only_any<T>::value;

template <typename T>
struct is_in_place_type_t : std::false_type {};

template <typename T>
struct is_in_place_type_t<std::in_place_type_t<T>> : std::true_type {};

template <typename T>
constexpr bool is_in_place_type_t_v = is_in_place_type_t<T>::value;

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
    move,
    destroy,
    get_address,
#ifdef INPLACE_RTTI
    get_type,
#endif
};

template <std::size_t N>
using manage_ptr = const void* (*)(operation, const std::byte*, std::byte*);

template <typename T, std::size_t N>
struct manager {
    template <typename... Args>
    static T* construct(std::byte* storage, Args&&... args) {
        return std::construct_at<T>(get_address(storage), std::forward<Args>(args)...);
    }

    static const void* manage(operation op, const std::byte* storage, std::byte* arg2) {
        switch (op) {
            case operation::move: {
                auto* src = get_address(const_cast<std::byte*>(storage));
                auto* dst = get_address(arg2);
                std::construct_at<T>(dst, std::move(*src));
                return nullptr;
            }
            case operation::destroy: {
                std::destroy_at(get_address(const_cast<std::byte*>(storage)));
                return nullptr;
            }
            case operation::get_address: {
                return get_address(const_cast<std::byte*>(storage));
            }
#ifdef INPLACE_RTTI
            case operation::get_type: {
                return &typeid(T);
            }
#endif
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

template <typename T, std::size_t N>
const void* cast(const move_only_any<N>& operand) {
    using U = std::remove_cv_t<T>;
    if constexpr (!std::is_same_v<std::decay_t<U>, U>) {
        return nullptr;
    } else if ((operand.manage_ == &manager<T, N>::manage)
#ifdef INPLACE_RTTI
               || (&typeid(T) == operand.manage_(operation::get_type, nullptr, nullptr))
#endif
    ) {
        return operand.manage_(operation::get_address, operand.storage_, nullptr);
    }
    return nullptr;
}

}  // namespace details::any
}  // namespace inplace