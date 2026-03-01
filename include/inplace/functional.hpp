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

// TODO documentation

// TODO is this good enough?
// TODO better to use std::bad_function_call?
class bad_function_call : public std::exception {};

template <typename, std::size_t S = 2 * sizeof(void*)>
    requires(S >= sizeof(void*))
class function;

// TODO templated support for copy y/n. throw/abort/function?
template <typename R, typename... Args, std::size_t S>
class function<R(Args...), S> {
private:
    static constexpr auto size_offset = sizeof(void*);
    static constexpr auto actual_storage_size = S + size_offset;

    using call_type = R(Args...);
    using storage = details::functional::aligned_storage<actual_storage_size, sizeof(void*)>;
    using i_stored_callable = details::functional::i_stored_callable<call_type, storage>;

    template <typename T>
    using stored_callable = details::functional::stored_callable<call_type, storage, std::decay_t<T>>;

public:
    using result_type = R;
    static constexpr auto storage_size = S;

    function() noexcept = default;

    // NOLINTNEXTLINE(google-explicit-constructor) implicit conversion allowed
    function(std::nullptr_t) noexcept {};

    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, function> &&
                 std::is_invocable_r_v<R, std::add_lvalue_reference_t<std::decay_t<T>>, Args...> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<T>> &&
                 details::functional::is_storage_large_enough_v<call_type, storage, std::decay_t<T>>)
    // NOLINTNEXTLINE(google-explicit-constructor) implicit conversion allowed
    function(T&& callable) noexcept
        : callable_{details::functional::construct<call_type, storage, T>(storage_, std::forward<T>(callable))} {}

    function(const function&) = delete;

    function(function&& other) noexcept : callable_{move(other, storage_)} {}

    ~function() { destroy(); }

    function& operator=(const function&) = delete;

    function& operator=(function&& other) noexcept {
        destroy();
        callable_ = move(other, storage_);
        return *this;
    }

    function& operator=(std::nullptr_t) noexcept {
        destroy();
        callable_ = nullptr;
        return *this;
    }

    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, function> &&
                 std::is_invocable_r_v<R, std::add_lvalue_reference_t<std::decay_t<T>>, Args...> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<T>> &&
                 details::functional::is_storage_large_enough<call_type, storage, std::decay_t<T>>::value)
    function& operator=(T&& callable) noexcept {
        destroy();
        callable_ = details::functional::construct<call_type, storage, T>(storage_, std::forward<T>(callable));
        return *this;
    }

    explicit operator bool() const noexcept { return callable_ != nullptr; }

    R operator()(Args... arguments) const {
        if (callable_ == nullptr) {
            INPLACE_THROW_OR_ABORT(bad_function_call{});
        }
        return callable_->call(std::forward<Args>(arguments)...);
    }

    void swap(function& other) noexcept {
        // TODO isn't this std::swap?
        auto temp = std::move(other);
        other = std::move(*this);
        *this = std::move(temp);
    }

    friend void swap(function& func1, function& func2) noexcept { func1.swap(func2); }

    friend bool operator==(const function& func, [[maybe_unused]] std::nullptr_t null) noexcept { return !func; }

private:
    storage storage_{};
    i_stored_callable* callable_{nullptr};

    void destroy() noexcept {
        if (callable_) {
            std::destroy_at(callable_);
        }
    }

    static i_stored_callable* move(function& other, storage& storage) noexcept {
        return other.callable_ ? other.callable_->move(storage) : nullptr;
    }
};

template <typename R, typename... Args>
function(R (*)(Args...)) -> function<R(Args...)>;

template <typename T, typename C = details::functional::member_function_call_type_t<decltype(&T::operator())>>
function(T) -> function<C>;

}  // namespace inplace