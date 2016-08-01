#include "fast_division_tests.hpp"

namespace fd_t = fast_division::tests;

int main()
{
    auto result = fd_t::division_simd(0, 1000, 1, 101)
          && fd_t::division_by_primes_simd(0, 10000, 0, 100)
          && fd_t::division_random_simd(100, 10000);

    auto unsigned_test = fd_t::random_unsigned_division();

    return !(unsigned_test && result);
}
