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
            static constexpr const auto word_size = 8*sizeof(Integer);
            using p_type = promotion_t<Integer>;
            p_type mult = static_cast<p_type>(x) * static_cast<p_type>(y);
            return  (mult >> word_size);   
        }

    }
}