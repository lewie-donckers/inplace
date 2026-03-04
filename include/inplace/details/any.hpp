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

using voidf_ptr = void (*)() noexcept;

class move_only_iface {
public:
    virtual ~move_only_iface() = default;

#ifdef INPLACE_RTTI
    [[nodiscard]] virtual const std::type_info& type() const noexcept = 0;
#endif
    [[nodiscard]] virtual const void* ptr() const noexcept = 0;
    [[nodiscard]] virtual void* ptr() noexcept = 0;

    virtual voidf_ptr check() const noexcept = 0;
};

template <typename T>
class move_only : public move_only_iface {
public:
    template <typename... Args>
    explicit move_only(Args&&... args) : value_{std::forward<Args>(args)...} {}

    ~move_only() override = default;

    move_only(const move_only&) = delete;
    move_only(move_only&&) = delete;

    move_only& operator=(const move_only&) = delete;
    move_only& operator=(move_only&&) = delete;

#ifdef INPLACE_RTTI
    [[nodiscard]] const std::type_info& type() const noexcept override { return typeid(T); }
#endif
    [[nodiscard]] const void* ptr() const noexcept override { return &value_; }
    [[nodiscard]] void* ptr() noexcept override { return &value_; }
    voidf_ptr check() const noexcept override { return scheck; }
    static void scheck() noexcept { ; }

    [[nodiscard]] T& get() { return value_; }

private:
    T value_;
};

template <typename T, std::size_t N>
consteval std::size_t max_align_offset_required() {
    const auto align_storage = alignof(void*);
    const auto align_object = alignof(move_only<T>);
    static_assert(align_object > 0);

    return std::max(align_storage - 1, align_object) - align_object;
}

template <typename T, std::size_t N>
consteval std::size_t max_size_required() {
    return max_align_offset_required<T, N>() + sizeof(move_only<T>);
}

template <typename T, std::size_t N>
struct will_fit : std::bool_constant<max_size_required<T, N>() <= N> {};

template <typename T, std::size_t N>
constexpr bool will_fit_v = will_fit<T, N>::value;

template <typename T, std::size_t N>
const void* cast(const move_only_any<N>& operand) {
    using U = std::remove_cv_t<T>;
    if constexpr (!std::is_same_v<std::decay_t<U>, U>) {
        return nullptr;
    } else if ((operand.impl_->check() == &details::any::move_only<T>::scheck)
#ifdef INPLACE_RTTI
               || (typeid(T) != operand.impl_->type())
#endif
    ) {
        return operand.impl_->ptr();
    }
    return nullptr;
}

template <typename T, std::size_t N>
void* cast(move_only_any<N>& operand) {
    using U = std::remove_cv_t<T>;
    if constexpr (!std::is_same_v<std::decay_t<U>, U>) {
        return nullptr;
    } else if ((operand.impl_->check() == &details::any::move_only<T>::scheck)
#ifdef INPLACE_RTTI
               || (typeid(T) == operand.impl_->type())
#endif
    ) {
        return operand.impl_->ptr();
    }
    return nullptr;
}

template <std::size_t N>
class storage {
public:
    storage() = default;

    template <typename T, typename... Args>
    T* construct(Args&&... args) {
        return std::construct_at<T>(get_address<T>(), std::forward<Args>(args)...);
    }

private:
    alignas(void*) std::array<std::byte, N> data_;

    template <typename T>
    T* get_address() {
        static constexpr auto align = alignof(T);
        const auto address = std::bit_cast<std::uintptr_t>(data_.data());
        const auto aligned_address = ((address + align - 1) / align) * align;

        return std::bit_cast<T*>(aligned_address);
    }
};

}  // namespace details::any
}  // namespace inplace