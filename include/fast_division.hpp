#pragma once

#include <immintrin.h>
#include <algorithm>
#include <cstdint>
#include <cassert>

/*
/// Definitions for various compiler intrinsics
#if defined(__GNUG__) || defined(__clang__)
# define MTS_CLZ __builtin_clz
# define MTS_CLZLL __builtin_clzll
#elif defined(_MSC_VER)
# define MTS_CLZ __lzcnt
# define MTS_CLZLL __lzcnt64
#endif
/// Compute the base-2 logarithm of an unsigned integer
template <typename T>
T log2i(T value)
{
    //Assert(value >= 0);
#if defined(MTS_CLZ) && defined(MTS_CLZLL)
    if (sizeof(T) <= 4)
        return T(MTS_CLZ((unsigned int)value));
    else
        return T(MTS_CLZLL((unsigned long long) value));
#else
    T r = 0;
    while ((value >> r) != 0)
        r++;
    return r - 1;
#endif
}
*/

/// Efficiently compute the floor of the base-2 logarithm of an unsigned integer
template <typename T> 
T log2i(T value)
{
    assert(value >= 0);
#if defined(__GNUC__) && defined(__x86_64__)
    T result;
    asm("bsr %1, %0" : "=r" (result) : "r" (value));
    return result;
#elif defined(_WIN32)
    unsigned long result;
    if (sizeof(T) <= 4)
        _BitScanReverse(&result, (unsigned long)value);
    else
        _BitScanReverse64(&result, (unsigned __int64)value);
    return T(result);
#else
    T r = 0;
    while ((value >> r) != 0)
        r++;
    return r - 1;
#endif
}


namespace fast_division {

    struct constant_divider {
        constexpr static const auto word_size = sizeof(std::uint32_t) * 8;
        uint32_t divisor;
        uint32_t multiplier;
        uint32_t shift_1;
        uint32_t shift_2;

        constant_divider(std::uint32_t divisor)
            : divisor(divisor)
        {
            switch (divisor) {
            case 1:
                multiplier = 1;
                shift_1 = shift_2 = 0;
                break;
            case 2:
                multiplier = shift_1 = 1;
                shift_2 = 0;
                break;
            default:
                uint32_t l = log2i(divisor - 1) + 1;
                // Check for overflow
                uint32_t l2 = l < 32 ? (1 << l) : 0;
                // Alternatively:  multiplier = ((2 << (N + l)) / d) - (2 << N) + 1;
                multiplier = 1 + uint32_t((uint64_t(l2 - divisor) << 32) / divisor);
                shift_1 = std::min(l, 1u);
                shift_2 = std::max((l - 1), 0u);    // sh2 = l - sh1
                break;
            }
        }

        uint32_t operator()(uint32_t input) const
        {
            std::uint32_t t = uint64_t(multiplier) * input >> word_size;
            std::uint32_t q = (t + ((input - t) >> shift_1)) >> shift_2;
            return q;
        }

        __m128i operator()(__m128i input) const
        {
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
    };

}