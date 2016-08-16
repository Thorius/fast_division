/**
 *  Fast Division Library
 *  Created by Stefan Ivanov
 */
#pragma once

#include <fast_division/utility/associated_types.hpp>

namespace fast_division {
    namespace utility {

        template <typename Integer>
        inline constexpr
        Integer high_bits_mult(Integer x, Integer y)
        {
            return (x >> (sizeof(Integer) / 2)) * (y >> (sizeof(Integer) / 2));
        }

        template <typename Integer>
        inline constexpr
        Integer low_high_bits_mult(Integer x, Integer y)
        {
            return x * (y >> (sizeof(Integer) / 2));
        }

        template <typename Integer>
        inline constexpr
        Integer high_low_bits_mult(Integer x, Integer y)
        {
            return high_bits_mult(y, x);
        }

        template <typename Integer>
        inline constexpr
        Integer high_mult(Integer x, Integer y)
        {
            using p_type = promotion_t<Integer>;
            // Due to lack of constexpr support in VS, it is not as clear as it can be.
            //constexpr const auto word_size = 8*sizeof(Integer);
            return  ((static_cast<p_type>(x) * static_cast<p_type>(y)) >> (8 * sizeof(Integer)));
        }

        template <>
        inline constexpr
        uint64_t high_mult(uint64_t x, uint64_t y)
        {
            return high_bits_mult(x, y) + low_high_bits_mult(x, y) + high_low_bits_mult(x, y);
        }

    }
}