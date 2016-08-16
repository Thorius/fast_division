/**
*  Fast Division Library
*  Created by Stefan Ivanov
*
*  Using ideas from
*  Division by Invariant Integers Using Multiplication (1994)
*  by Torbjörn Granlund, Peter L. Montgomery
*/
#pragma once

#include <limits>
#include <fast_division/utility/associated_types.hpp>

namespace fast_division {
    
    template <typename Integer, bool Signed>
    struct promotion_policy {
        constexpr static auto word_size = 8*sizeof(Integer);
        using p_type = utility::promotion_t<Integer>;
        
        static
        Integer calculate_multiplier(Integer divisor, Integer log_ceil)
        {
            return Integer(1) + Integer(((p_type(1) << word_size) * ((p_type(1) << log_ceil) - divisor)) / divisor);
        }
        
    };
    
    template <typename Integer>
    struct promotion_policy<Integer, true> {
        constexpr static auto word_size = 8*sizeof(Integer);
        using p_type = utility::promotion_t<Integer>;
        
        static
        Integer calculate_multiplier(Integer abs_divisor, Integer log_ceil)
        {
            return Integer(1) + Integer((p_type(1) << (word_size + log_ceil - 1)) / abs_divisor - (p_type(1) << word_size));
        }
        
    };


    template <typename Integer, bool Signed>
    struct decomposition_policy {
        constexpr static auto word_size = 8*sizeof(Integer);

        static
        Integer calculate_multiplier(Integer divisor, Integer log_ceil)
        {
            constexpr auto max = std::numeric_limits<Integer>::max();
            auto q_1 = max / divisor;
            auto r_1 = max % divisor;
            auto inter = max == log_ceil ? (max - divisor + Integer(1)) : ((Integer(1) << log_ceil) - divisor);
            auto q_2 = inter / divisor;
            auto r_2 = inter % divisor;
            return Integer(1) + q_1 * inter + q_2*(r_1 + Integer(1)) + (r_1 * r_2) / divisor;
        }
    };

    template <typename Integer>
    struct decomposition_policy<Integer, true> {
        constexpr static auto word_size = 8*sizeof(Integer);

        static
        Integer calculate_multiplier(Integer divisor, Integer log_ceil)
        {
            constexpr auto max = std::numeric_limits<Integer>::max();
            auto q_1 = max / divisor;
            auto r_1 = max % divisor;
            auto inter = max == log_ceil ? (max - divisor + Integer(1)) : ((Integer(1) << log_ceil) - divisor);
            auto q_2 = inter / divisor;
            auto r_2 = inter % divisor;
            return Integer(1) + q_1 * inter + q_2*(r_1 + Integer(1)) + (r_1 * r_2) / divisor;
        }
    };

}