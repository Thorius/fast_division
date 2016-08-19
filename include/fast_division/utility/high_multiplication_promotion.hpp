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
        Integer high_mult_promotion(Integer x, Integer y)
        {
            using p_type = promotion_t<Integer>;
            // Due to lack of constexpr support in VS, it is not as clear as it can be.
            //constexpr const auto word_size = 8*sizeof(Integer);
            return  ((static_cast<p_type>(x) * static_cast<p_type>(y)) >> (8 * sizeof(Integer)));
        }
        
    }
}