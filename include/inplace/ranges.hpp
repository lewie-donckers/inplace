// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#pragma once

namespace inplace {

struct from_range_t {
    explicit from_range_t() = default;
};

inline constexpr from_range_t from_range{};

}  // namespace inplace