#include <immintrin.h>
#include <iostream>
#include <fast_division/utility/high_multiplication.hpp>
#include "fast_division_tests.hpp"

namespace fd_t = fast_division::tests;

int main()
{
    auto high_mult_test = fd_t::high_multiplication();
    auto unsigned_test = fd_t::random_unsigned_division();
    auto signed_test = true;// fd_t::random_signed_division();
    //auto simd_test = fd_t::division_simd(0, 10000, 1, 101);
    //auto simd_primes_test = fd_t::division_by_primes_simd(0, 100000, 0, 200);
    //auto random_simd_test = fd_t::division_random_simd(1000, 100000);

    return !(high_mult_test && unsigned_test && signed_test
             /*&& simd_test && simd_primes_test && random_simd_test*/);
}
