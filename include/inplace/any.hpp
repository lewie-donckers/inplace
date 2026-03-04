#pragma once

#include <inplace/details/any.hpp>
#include <inplace/details/macros.hpp>

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <typeinfo>
#include <utility>

namespace inplace {

// - standard any can have noexcept move ops because it can store the object on the heap
// - we can only offer noexcept move if we require the target to be noexcept move
// - options:
//   - offer noexcept only
//   - offer normal only
//   - offer both
// - is it worth it to also offer no-move-any?
// - todo constexpr support?
// - todo worth it to allow conversions (copyable -> move-only) (small -> large)

template <std::size_t N>
class move_only_any {
public:
    move_only_any() noexcept = default;

    move_only_any(const move_only_any&) = delete;

    move_only_any(move_only_any&& other) noexcept;  // TODO impl

    template <typename ValueType>
        requires(!details::any::is_move_only_any_v<std::decay_t<ValueType>> &&
                 !details::any::is_in_place_type_t_v<std::decay_t<ValueType>> &&
                 details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>>)
    move_only_any(ValueType&& value) {
        details::any::manager<std::decay_t<ValueType>, N>::construct(storage_, std::forward<ValueType>(value));
        manage_ = &details::any::manager<std::decay_t<ValueType>, N>::manage;
    }

    template <typename ValueType, typename... Args>
        requires(details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>> &&
                 std::is_constructible_v<std::decay_t<ValueType>, Args...>)
    explicit move_only_any(std::in_place_type_t<ValueType>, Args&&... args);  // TODO impl

    template <typename ValueType, typename U, typename... Args>
        requires(details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>> &&
                 std::is_constructible_v<std::decay_t<ValueType>, std::initializer_list<U>&, Args...>)
    explicit move_only_any(std::in_place_type_t<ValueType>, std::initializer_list<U> il, Args&&... args);  // TODO impl

    move_only_any& operator=(const move_only_any&) = delete;

    move_only_any& operator=(move_only_any&& other) noexcept;  // TODO impl

    template <typename ValueType>
        requires(!details::any::is_move_only_any_v<std::decay_t<ValueType>> &&
                 details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>>)
    move_only_any& operator=(ValueType&& other) {
        reset();
        details::any::manager<std::decay_t<ValueType>, N>::construct(storage_, std::forward<ValueType>(other));
        manage_ = &details::any::manager<std::decay_t<ValueType>, N>::manage;
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
        auto* ptr = details::any::manager<std::decay_t<ValueType>, N>::construct(storage_, std::forward<Args>(args)...);
        manage_ = &details::any::manager<std::decay_t<ValueType>, N>::manage;
        return *ptr;
    }

    template <typename ValueType, typename U, typename... Args>
        requires(details::any::will_fit_v<std::decay_t<ValueType>, N> &&
                 std::is_nothrow_move_constructible_v<std::decay_t<ValueType>> &&
                 std::is_nothrow_move_assignable_v<std::decay_t<ValueType>> &&
                 std::is_constructible_v<std::decay_t<ValueType>, std::initializer_list<U>&, Args...>)
    std::decay_t<ValueType>& emplace(std::initializer_list<U> il, Args&&... args);  // TODO impl

    void reset() noexcept {
        if (manage_ != nullptr) {
            manage_(details::any::operation::destroy, storage_);
        }
    }

    // TODO swap

    [[nodiscard]] bool has_value() const noexcept { return manage_ != nullptr; }

#ifdef INPLACE_RTTI
    [[nodiscard]] const std::type_info& type() const noexcept {
        if (manage_ != nullptr) {
            return *static_cast<const std::type_info*>(manage_(details::any::operation::get_type, storage_));
        }
        return typeid(void);
    }
#endif

private:
    template <typename T, std::size_t M>
    friend const void* details::any::cast(const move_only_any<M>&);

    alignas(void*) std::byte storage_[N];
    details::any::manage_ptr<N> manage_{nullptr};
};

// TODO non-member swap

template <typename T, std::size_t N>
T any_cast(const move_only_any<N>& operand);  // TODO impl

template <typename T, std::size_t N>
T any_cast(move_only_any<N>& operand);  // TODO impl

template <typename T, std::size_t N>
T any_cast(move_only_any<N>&& operand);  // TODO impl

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

// TODO make_any

// TODO bad_any_cast

template <std::size_t N>
class copyable_any {};

template <std::size_t N>
using any = copyable_any<N>;

}  // namespace inplace