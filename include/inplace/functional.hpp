// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

#include <inplace/details/functional.hpp>
#include <inplace/details/macros.hpp>

#include <cstddef>
#include <exception>
#include <type_traits>
#include <utility>

namespace inplace {

class bad_function_call : public std::exception {};

template <typename, std::size_t S = 2 * sizeof(void*)>
class function;

template <typename R, typename... Args, std::size_t S>
class function<R(Args...), S> {
public:
    using result_type = R;
    static constexpr auto storage_size = S;

    function() noexcept = default;

    function(std::nullptr_t) noexcept {};

    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, function> &&
                 std::is_invocable_r_v<R, std::add_lvalue_reference_t<std::decay_t<T>>, Args...> &&
                 std::is_copy_constructible_v<std::decay_t<T>> && std::is_move_constructible_v<std::decay_t<T>> &&
                 details::functional::will_fit_v<std::decay_t<T>, S>)
    function(T&& callable) noexcept {
        manager_t<T>::construct(storage_, std::forward<T>(callable));
        manage_ = &manager_t<T>::manage;
    }

    function(const function& other) {
        if (other.manage_) {
            other.manage_(details::functional::operation::copy, other.storage_, storage_);
            manage_ = other.manage_;
        }
    }

    function(function&& other) noexcept {
        if (other.manage_) {
            other.manage_(details::functional::operation::move, other.storage_, storage_);
            manage_ = other.manage_;
        }
    }

    ~function() { destroy(); }

    function& operator=(const function& other) {
        if (this != &other) {
            destroy();
            if (other.manage_ != nullptr) {
                other.manage_(details::functional::operation::copy, other.storage_, storage_);
                manage_ = other.manage_;
            }
        }
        return *this;
    }

    function& operator=(function&& other) noexcept {
        if (this != &other) {
            destroy();
            if (other.manage_ != nullptr) {
                other.manage_(details::functional::operation::move, other.storage_, storage_);
                manage_ = other.manage_;
            }
        }
        return *this;
    }

    function& operator=(std::nullptr_t) noexcept {
        destroy();
        return *this;
    }

    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, function> &&
                 std::is_invocable_r_v<R, std::add_lvalue_reference_t<std::decay_t<T>>, Args...> &&
                 std::is_copy_constructible_v<std::decay_t<T>> && std::is_move_constructible_v<std::decay_t<T>> &&
                 details::functional::will_fit_v<std::decay_t<T>, S>)
    function& operator=(T&& callable) noexcept {
        destroy();
        manager_t<T>::construct(storage_, std::forward<T>(callable));
        manage_ = &manager_t<T>::manage;
        return *this;
    }

    explicit operator bool() const noexcept { return manage_ != nullptr; }

    R operator()(Args... arguments) const {
        if (manage_ == nullptr) {
            INPLACE_THROW_OR_ABORT(bad_function_call{});
        }
        auto ptr = manage_(details::functional::operation::get_function_ptr, storage_, nullptr);
        return (*ptr)(storage_, std::forward<Args>(arguments)...);
    }

    void swap(function& other) noexcept {
        auto temp = std::move(other);
        other = std::move(*this);
        *this = std::move(temp);
    }

    friend void swap(function& func1, function& func2) noexcept { func1.swap(func2); }

    friend bool operator==(const function& func, [[maybe_unused]] std::nullptr_t null) noexcept { return !func; }

private:
    template <typename T>
    using manager_t = details::functional::manager<std::decay_t<T>, S, R, Args...>;

    alignas(void*) std::byte storage_[S];
    details::functional::manage_ptr<R, Args...> manage_{nullptr};

    void destroy() noexcept {
        if (manage_ != nullptr) {
            manage_(details::functional::operation::destroy, storage_, nullptr);
            manage_ = nullptr;
        }
    }
};

template <typename R, typename... Args>
function(R (*)(Args...)) -> function<R(Args...)>;

template <typename T, typename C = details::functional::member_function_call_type_t<decltype(&T::operator())>>
function(T) -> function<C>;

}  // namespace inplace