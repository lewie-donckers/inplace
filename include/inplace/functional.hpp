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
    requires(S >= sizeof(void*))
class move_only_function;

template <typename R, typename... Args, std::size_t S>
class move_only_function<R(Args...), S> {
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

    move_only_function() noexcept = default;

    move_only_function(std::nullptr_t) noexcept {};

    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, move_only_function> &&
                 std::is_invocable_r_v<R, std::add_lvalue_reference_t<std::decay_t<T>>, Args...> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<T>> &&
                 details::functional::is_storage_large_enough_v<call_type, storage, std::decay_t<T>>)

    move_only_function(T&& callable) noexcept
        : callable_{details::functional::construct<call_type, storage, T>(storage_, std::forward<T>(callable))} {}

    move_only_function(const move_only_function&) = delete;

    move_only_function(move_only_function&& other) noexcept : callable_{move(other, storage_)} {}

    ~move_only_function() { destroy(); }

    move_only_function& operator=(const move_only_function&) = delete;

    move_only_function& operator=(move_only_function&& other) noexcept {
        destroy();
        callable_ = move(other, storage_);
        return *this;
    }

    move_only_function& operator=(std::nullptr_t) noexcept {
        destroy();
        callable_ = nullptr;
        return *this;
    }

    template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, move_only_function> &&
                 std::is_invocable_r_v<R, std::add_lvalue_reference_t<std::decay_t<T>>, Args...> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<T>> &&
                 details::functional::is_storage_large_enough<call_type, storage, std::decay_t<T>>::value)
    move_only_function& operator=(T&& callable) noexcept {
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

    void swap(move_only_function& other) noexcept {
        auto temp = std::move(other);
        other = std::move(*this);
        *this = std::move(temp);
    }

    friend void swap(move_only_function& func1, move_only_function& func2) noexcept { func1.swap(func2); }

    friend bool operator==(const move_only_function& func, [[maybe_unused]] std::nullptr_t null) noexcept {
        return !func;
    }

private:
    storage storage_{};
    i_stored_callable* callable_{nullptr};

    void destroy() noexcept {
        if (callable_) {
            std::destroy_at(callable_);
        }
    }

    static i_stored_callable* move(move_only_function& other, storage& storage) noexcept {
        return other.callable_ ? other.callable_->move(storage) : nullptr;
    }
};

template <typename R, typename... Args>
move_only_function(R (*)(Args...)) -> move_only_function<R(Args...)>;

template <typename T, typename C = details::functional::member_function_call_type_t<decltype(&T::operator())>>
move_only_function(T) -> move_only_function<C>;

}  // namespace inplace