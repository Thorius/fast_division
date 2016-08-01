/** 
 *  Fast Division Library
 *  Created by Stefan Ivanov
 */
#pragma once

#include "associated_types.hpp"

namespace fast_division
{
    namespace utility
    {

        template <typename Integer>
        inline constexpr
        Integer high_mult(Integer x, Integer y) {
            using p_type = promotion_t<Integer>;
            // Due to lack of constexpr support in VS, it is not as clear as it can be.
            //constexpr const auto word_size = 8*sizeof(Integer);
            //p_type mult = static_cast<p_type>(x) * static_cast<p_type>(y);
            return  ((static_cast<p_type>(x) * static_cast<p_type>(y)) >> (8 * sizeof(Integer)));
        }

    }
}