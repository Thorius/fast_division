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

    template <typename T>
    inline
    void swap_xor_unsafe(T& a, T& b)
    {
        a = a^b; // a = a^b, b = b
        b = a^b; // a = a^b, b = a^b^b = a    
        a = a^b; // b = a  , a = a^b^a = b
    }
    
    /// Calculate  floor((a_1*a_2)/d)  without the need to do a higher precision multiplication.
    // [[Expects: a_1 <= d && a_2 <= d ]] 
    template <typename Integer>
    inline
    Integer multiply_and_divide(Integer a_1, Integer a_2, Integer d)
    {
        using utility::log2i;
        // Check if multiplication can fit into the number of bits of the integers.
        if (log2i(a_1) + log2i(a_2) <= 8 * sizeof(Integer)) {
            return (a_1*a_2)/d;
        }
        Integer accum = Integer(0);
        Integer quotient = Integer(0);
        // Accumulate the larger integer.
        if (a_2 > a_1) {
            swap_xor_unsafe(a_1, a_2);
        }
        while (a_2) {
            accum += a_1;
            if (accum >= d) {
                ++quotient;
                accum -= d;
            }
            --a_2;
        }
        return quotient;
    }
    
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
            Integer q_1 = max / divisor;
            Integer r_1 = max % divisor;
            Integer inter = max == log_ceil ? (max - divisor + Integer(1)) : ((Integer(1) << log_ceil) - divisor);
            Integer decomposition = multiply_and_divide<Integer>(inter, r_1 + Integer(1), divisor);       
            return Integer(1) + q_1 * inter + decomposition;
        }
    };

    template <typename Integer>
    struct decomposition_policy<Integer, true> {
        constexpr static auto word_size = 8*sizeof(Integer);

        static
        Integer calculate_multiplier(Integer divisor, Integer log_ceil)
        {
            // TODO: Implement!
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