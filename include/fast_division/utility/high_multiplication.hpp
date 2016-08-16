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
        Integer low_bits(Integer x)
        {
            return x & ((Integer(1) << (8*sizeof(Integer) / 2)) - Integer(1));
        }

        template <typename Integer>
        inline constexpr
        Integer high_bits(Integer x)
        {
            return x >> (8*sizeof(Integer) / 2);
        }

        template <typename Integer>
        inline constexpr std::enable_if_t<!std::is_signed<Integer>::value, 
            Integer>
            low_bits_mult(Integer x, Integer y)
        {
            return low_bits(x) * low_bits(y);
        }

        template <typename Integer>
        inline constexpr std::enable_if_t<std::is_signed<Integer>::value, 
            std::make_unsigned_t<Integer>>
            low_bits_mult(Integer x, Integer y)
        {
            return low_bits(x) * low_bits(y);
        }

        template <typename Integer>
        inline constexpr
        Integer high_bits_mult(Integer x, Integer y)
        {
            return high_bits(x) * high_bits(y);
        }

        template <typename Integer>
        inline constexpr
        Integer low_high_bits_mult(Integer x, Integer y)
        {
            return low_bits(x) * high_bits(y);
        }

        template <typename Integer>
        inline constexpr
        Integer high_low_bits_mult(Integer x, Integer y)
        {
            return high_bits(x) * low_bits(y);
        }

        template <typename Integer>
        inline constexpr
        Integer low_bits_carry(Integer x, Integer y)
        {
            return ((low_bits_mult(x, y) >> ((8 * sizeof(Integer) / 2))) +
                     low_bits(low_high_bits_mult(x, y)) + low_bits(high_low_bits_mult(x, y)) )
                         >> ((8 * sizeof(Integer) / 2));
        }

        template <typename Integer>
        inline constexpr
        Integer high_mult(Integer x, Integer y)
        {
            return  high_bits_mult(x, y) +
                (low_high_bits_mult(x, y) >> (8*sizeof(Integer) / 2)) +
                (high_low_bits_mult(x, y) >> (8*sizeof(Integer) / 2)) +
                 + low_bits_carry(x,y);
        }

        template <typename Integer>
        inline constexpr
        Integer high_mult_promotion(Integer x, Integer y)
        {
            using p_type = promotion_t<Integer>;
            // Due to lack of constexpr support in VS, it is not as clear as it can be.
            //constexpr const auto word_size = 8*sizeof(Integer);
            return  ((static_cast<p_type>(x) * static_cast<p_type>(y)) >> (8 * sizeof(Integer)));
        }
    }
}