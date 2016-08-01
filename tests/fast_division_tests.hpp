#pragma once

#include <cstdint>

namespace fast_division {

    namespace tests {

        bool division_simd(uint32_t first_divident, uint32_t last_divident, uint32_t first_divisor, uint32_t last_divisor, uint32_t divisor_step = 1);

        bool division_by_primes_simd(uint32_t first_divident, uint32_t last_divident, uint32_t first_prime_index, uint32_t last_last_prime);

        bool division_random_simd(uint32_t num_divisions, uint32_t divisions_per_divisor);

        bool random_unsigned_division();

    }

}
