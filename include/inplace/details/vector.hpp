// inplace C++ library
//
// Copyright © 2025 Lewie Donckers
//
// Use, modification and distribution is subject to the MIT License. See
// accompanying file LICENSE.txt or copy at https://opensource.org/license/MIT
//
// Project home: https://github.com/lewie-donckers/inplace

#include <iterator>

#pragma once

namespace inplace::details::vector {

template <typename Iter>
constexpr auto distance(Iter first, Iter last) {
    if constexpr (std::forward_iterator<Iter>) {
        return std::distance(first, last);
    } else {
        return 0;
    }
}

}  // namespace inplace::details::vector