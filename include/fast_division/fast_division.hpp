/** 
 *  Fast Division Library
 *  Created by Stefan Ivanov
 *  
 *  Using ideas from 
 *  Division by Invariant Integers Using Multiplication (1994)
 *  by Torbjörn Granlund, Peter L. Montgomery
 */
#pragma once

#include <immintrin.h>
#include <algorithm>
#include <cstdint>
#include <type_traits>

#include <fast_division/utility/associated_types.hpp>
#include <fast_division/utility/log2i.hpp>
#include <fast_division/utility/high_multiplication.hpp>

namespace fast_division {

    template <typename Integer>
    struct constant_divider {
        constexpr static const auto word_size = sizeof(Integer) * 8;
        using p_type  = utility::promotion_t<Integer>;
        Integer divisor;
        Integer multiplier;
        Integer shift_1;
        Integer shift_2;

        explicit constant_divider(Integer divisor)
            : divisor(divisor) {
            if (divisor == 1) { // A no-op.
                multiplier = shift_1 = shift_2 = 0;
            } 
            else if ((divisor & (divisor - Integer(1))) == 0) { // Power of 2
                multiplier = shift_1 = 0;
                shift_2 = utility::log2i(divisor);
            }
            else { 
                Integer l = utility::log2i(divisor - 1) + 1;
                // The bellow works for most types but not for uint8_t for some strange reason.
                // Check for overflow
                //Integer l2 = l < word_size ? (Integer(1) << l) : 0;
                //multiplier = 1 + Integer((p_type(l2 - divisor) << word_size) / divisor);
                // Alternatively:  multiplier = ((2 << (N + l)) / d) - (2 << N) + 1;
                // or: multiplier = p_type(1 << (word_size + l)) / divisor - p_type(1 << word_size) + 1;
                multiplier = 1 + Integer(((p_type(1) << word_size) * ((p_type(1) << l) - divisor)) / divisor);
                shift_1 = std::min(l, Integer(1));
                shift_2 = l - shift_1;  //max(l - 1, 0)
            }
        }

        Integer operator()(Integer input) const {
            Integer t = utility::high_mult(multiplier, input);
            Integer q = (t + ((input - t) >> shift_1)) >> shift_2;
            return q;
        }

        // TODO Add some sort of SIMD detection.
        template <typename Simd, typename = std::enable_if_t<utility::is_simd<Simd>::value>>
        Simd operator()(Simd input) const 
        {
            static_assert(false, "Division by a simd vector must use a specialization");
            return input;
        }

        /// Equality and comparison operators delegating to the underlying divisor.

        friend
        bool operator== (const constant_divider& x, const constant_divider& y) {
            return x.divisor == y.divisor;
        }
        friend
        bool operator!= (const constant_divider& x, const constant_divider& y) {
            return x.divisor != y.divisor;
        }
        friend
        bool operator<  (const constant_divider& x, const constant_divider& y) {
            return x.divisor < y.divisor;
        }
        friend
        bool operator>  (const constant_divider& x, const constant_divider& y) {
            return x.divisor > y.divisor;
        }
        friend
        bool operator<= (const constant_divider& x, const constant_divider& y) {
            return x.divisor <= y.divisor;
        }
        friend
        bool operator>= (const constant_divider& x, const constant_divider& y) {
            return x.divisor >= y.divisor;
        }

    };

    /// Specializations for various simd types.

    template<> template<>
    inline 
    __m128i constant_divider<uint32_t>::operator()<>(__m128i input) const {
        __m128i m = _mm_set1_epi32(multiplier);
        __m128i s1 = _mm_setr_epi32(shift_1, 0, 0, 0);
        __m128i s2 = _mm_setr_epi32(shift_2, 0, 0, 0);
        // Multiply unsigned integers at positions 0 and 2 in n with the multiplier.
        __m128i batch_1_unshifted = _mm_mul_epu32(input, m);
        // Store the high bits of the results.
        __m128i batch_1 = _mm_srli_epi64(batch_1_unshifted, 32);
        // Shift the input in order to compute the product of the integers at positions 1 and 3.
        __m128i n_shift = _mm_srli_epi64(input, 32);
        __m128i batch_2 = _mm_mul_epu32(n_shift, m);
        // Create a mask to extract the correct bits in the two batches.
        __m128i mask = _mm_set_epi32(-1, 0, -1, 0);
        __m128i mult_result = _mm_blendv_epi8(batch_1, batch_2, mask);
        // Continue the algorithm normally, i.e
        // (((input - mult_result) >> s1) + mult_result) >> s2
        __m128i minus_result = _mm_sub_epi32(input, mult_result);
        __m128i first_shift = _mm_srl_epi32(minus_result, s1);
        __m128i add_result = _mm_add_epi32(first_shift, mult_result);
        __m128i second_shift = _mm_srl_epi32(add_result, s2);
        return second_shift;
    }
    
    template<> template<>
    inline
    __m256i constant_divider<uint32_t>::operator()<>(__m256i input) const {
        __m256i m = _mm256_set1_epi32(multiplier);
        __m128i s1 = _mm_setr_epi32(shift_1, 0, 0, 0);
        __m128i s2 = _mm_setr_epi32(shift_2, 0, 0, 0);
        // Multiply unsigned integers at positions 0 and 2 in n with the multiplier.
        __m256i batch_1_unshifted = _mm256_mul_epu32(input, m);
        // Store the high bits of the results.
        __m256i batch_1 = _mm256_srli_epi64(batch_1_unshifted, 32);
        // Shift the input in order to compute the product of the integers at positions 1 and 3.
        __m256i n_shift = _mm256_srli_epi64(input, 32);
        __m256i batch_2 = _mm256_mul_epu32(n_shift, m);
        // Create a mask to extract the correct bits in the two batches.
        __m256i mask = _mm256_set_epi32(-1, 0, -1, 0, -1, 0, 0, 1);
        __m256i mult_result = _mm256_blendv_epi8(batch_1, batch_2, mask);
        // Continue the algorithm normally, i.e
        // (((input - mult_result) >> s1) + mult_result) >> s2
        __m256i minus_result = _mm256_sub_epi32(input, mult_result);
        __m256i first_shift = _mm256_srl_epi32(minus_result, s1);
        __m256i add_result = _mm256_add_epi32(first_shift, mult_result);
        __m256i second_shift = _mm256_srl_epi32(add_result, s2);
        return second_shift;
    }

    /// Operator overloads so that the fast_divider can be used in a generic context.
    /// Furthermore, this provides a more natural syntax for use.

    template <typename Integer>
    inline
    Integer operator/ (Integer divident, const constant_divider<Integer>& divisor) {
        return divisor(divident);
    }

    template <typename Integer, typename Simd, typename = std::enable_if_t<utility::is_simd<Simd>>>
    inline
    Simd operator/ (Simd divident, const constant_divider<Integer>& divisor) {
        return divisor(divident);
    }

    using constant_divider_uint32 = constant_divider<uint32_t>;
    
}