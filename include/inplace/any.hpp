// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

#include <inplace/details/any.hpp>
#include <inplace/details/macros.hpp>

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace inplace {

class bad_any_cast : public std::bad_cast {
public:
    const char* what() const noexcept override { return "bad any_cast"; }
};

template <std::size_t N>
class move_only_any {
public:
    move_only_any() noexcept = default;

    move_only_any(const move_only_any&) = delete;

    move_only_any(move_only_any&& other) noexcept {
        if (other.manage_ != nullptr) {
            other.manage_(details::any::operation::move, other.storage_, storage_);
            manage_ = other.manage_;
        }
    }

    template <typename ValueType>
        requires(!details::any::is_move_only_any_v<std::decay_t<ValueType>> &&
                 !details::any::is_in_place_type_t_v<std::decay_t<ValueType>> &&
                 details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>>)
    move_only_any(ValueType&& value) {
        manager_t<ValueType>::construct(storage_, std::forward<ValueType>(value));
        manage_ = &manager_t<ValueType>::manage;
    }

    template <typename ValueType, typename... Args>
        requires(details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>> &&
                 std::is_constructible_v<std::decay_t<ValueType>, Args...>)
    explicit move_only_any(std::in_place_type_t<ValueType>, Args&&... args) {
        manager_t<ValueType>::construct(storage_, std::forward<Args>(args)...);
        manage_ = &manager_t<ValueType>::manage;
    }

    template <typename ValueType, typename U, typename... Args>
        requires(details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>> &&
                 std::is_constructible_v<std::decay_t<ValueType>, std::initializer_list<U>&, Args...>)
    explicit move_only_any(std::in_place_type_t<ValueType>, std::initializer_list<U> il, Args&&... args) {
        manager_t<ValueType>::construct(storage_, il, std::forward<Args>(args)...);
        manage_ = &manager_t<ValueType>::manage;
    }

    move_only_any& operator=(const move_only_any&) = delete;

    move_only_any& operator=(move_only_any&& other) noexcept {
        if (this != &other) {
            reset();
            if (other.manage_ != nullptr) {
                other.manage_(details::any::operation::move, other.storage_, storage_);
                manage_ = other.manage_;
            }
        }
        return *this;
    }

    template <typename ValueType>
        requires(!details::any::is_move_only_any_v<std::decay_t<ValueType>> &&
                 details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>>)
    move_only_any& operator=(ValueType&& other) {
        reset();
        manager_t<ValueType>::construct(storage_, std::forward<ValueType>(other));
        manage_ = &manager_t<ValueType>::manage;
        return *this;
    }

    ~move_only_any() { reset(); }

    template <typename ValueType, typename... Args>
        requires(details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>> &&
                 std::is_constructible_v<std::decay_t<ValueType>, Args...>)
    std::decay_t<ValueType>& emplace(Args&&... args) {
        reset();
        auto* ptr = manager_t<ValueType>::construct(storage_, std::forward<Args>(args)...);
        manage_ = &manager_t<ValueType>::manage;
        return *ptr;
    }

    template <typename ValueType, typename U, typename... Args>
        requires(details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>> &&
                 std::is_constructible_v<std::decay_t<ValueType>, std::initializer_list<U>&, Args...>)
    std::decay_t<ValueType>& emplace(std::initializer_list<U> il, Args&&... args) {
        reset();
        auto* ptr = manager_t<ValueType>::construct(storage_, il, std::forward<Args>(args)...);
        manage_ = &manager_t<ValueType>::manage;
        return *ptr;
    }

    void reset() noexcept {
        if (manage_ != nullptr) {
            manage_(details::any::operation::destroy, storage_, nullptr);
        }
    }

    void swap(move_only_any& other) noexcept {
        auto temp = std::move(other);
        other = std::move(*this);
        *this = std::move(temp);
    }

    [[nodiscard]] bool has_value() const noexcept { return manage_ != nullptr; }

#ifdef INPLACE_RTTI
    [[nodiscard]] const std::type_info& type() const noexcept {
        if (manage_ != nullptr) {
            return *static_cast<const std::type_info*>(manage_(details::any::operation::get_type, storage_, nullptr));
        }
        return typeid(void);
    }
#endif

private:
    template <typename T>
    using manager_t = details::any::manager<std::decay_t<T>, N>;

    template <typename T, std::size_t M>
    friend const void* details::any::cast(const move_only_any<M>&);

    alignas(void*) std::byte storage_[N];
    details::any::manage_ptr<N> manage_{nullptr};
};

template <std::size_t N>
void swap(move_only_any<N>& lhs, move_only_any<N>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename T, std::size_t N>
T any_cast(const move_only_any<N>& operand) {
    using U = std::remove_cvref_t<T>;
    static_assert(std::is_constructible_v<T, const U&>);
    auto* ptr = any_cast<U>(&operand);
    if (!ptr) {
        INPLACE_THROW_OR_ABORT(bad_any_cast{});
    }
    return static_cast<T>(*ptr);
}

template <typename T, std::size_t N>
T any_cast(move_only_any<N>& operand) {
    using U = std::remove_cvref_t<T>;
    static_assert(std::is_constructible_v<T, U&>);
    auto* ptr = any_cast<U>(&operand);
    if (!ptr) {
        INPLACE_THROW_OR_ABORT(bad_any_cast{});
    }
    return static_cast<T>(*ptr);
}

template <typename T, std::size_t N>
T any_cast(move_only_any<N>&& operand) {
    using U = std::remove_cvref_t<T>;
    static_assert(std::is_constructible_v<T, U>);
    auto* ptr = any_cast<U>(&operand);
    if (!ptr) {
        INPLACE_THROW_OR_ABORT(bad_any_cast{});
    }
    return static_cast<T>(std::move(*ptr));
}

template <typename T, std::size_t N>
const T* any_cast(const move_only_any<N>* operand) noexcept {
    static_assert(!std::is_void_v<T>);
    if ((operand != nullptr) && operand->has_value()) {
        return static_cast<const T*>(details::any::cast<T>(*operand));
    }
    return nullptr;
}

template <typename T, std::size_t N>
T* any_cast(move_only_any<N>* operand) noexcept {
    static_assert(!std::is_void_v<T>);
    if ((operand != nullptr) && operand->has_value()) {
        return static_cast<T*>(const_cast<void*>(details::any::cast<T>(*operand)));
    }
    return nullptr;
}

template <typename T, std::size_t N, typename... Args>
move_only_any<N> make_move_only_any(Args&&... args) {
    return move_only_any<N>{std::in_place_type<T>, std::forward<Args>(args)...};
}

template <typename T, std::size_t N, typename U, typename... Args>
move_only_any<N> make_move_only_any(std::initializer_list<U> il, Args&&... args) {
    return move_only_any<N>{std::in_place_type<T>, il, std::forward<Args>(args)...};
}

template <std::size_t N>
class copyable_any;

template <std::size_t N>
using any = copyable_any<N>;

}  // namespace inplace