#include "fast_division_tests.hpp"

#include <cassert>
#include <vector>
#include <immintrin.h>
#include <random>

#include "fast_division/fast_division.hpp"

namespace fd_t = fast_division::tests;

namespace {

    std::vector<uint32_t> calculate_prime_table(uint32_t num_primes)
    {
        using namespace std;
        assert(num_primes != 0);
        vector<uint32_t> primes;
        primes.reserve(num_primes);
        primes.push_back(2);
        --num_primes;
        uint32_t candidate = 3;
        while (num_primes != 0) {
            bool is_prime = true;
            for (auto& x : primes) {
                if (candidate % x == 0) {
                    is_prime = false;
                    break;
                }
            }
            if (is_prime) {
                primes.push_back(candidate);
                --num_primes;
            }
            candidate += 2;
        }
        return primes;
    }

}

bool fd_t::division_simd(uint32_t first_dividend, uint32_t last_dividend, uint32_t first_divisor, uint32_t last_divisor, uint32_t divisor_step)
{
    assert(first_dividend <= last_dividend && first_divisor <= last_divisor &&
        ((first_dividend - last_dividend) % 4 == 0));
    bool is_correct = true;

    while (first_divisor < last_divisor) {
        constant_divider_uint32 divider(first_divisor);
        auto current_dividend = first_dividend;
        __m128i n, q;
        uint32_t check[4];
        while (current_dividend < last_dividend) {
            n = _mm_setr_epi32(current_dividend, current_dividend + 1, current_dividend + 2, current_dividend + 3);
            q = divider(n);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(check), q);
            for (int i = 0; i != 4; ++i) {
                if (check[i] != (current_dividend + i) / first_divisor) {
                    is_correct = false;
                }

            }
            current_dividend += 4;
        }
        first_divisor += divisor_step;
    }
    return is_correct;
}

bool fd_t::division_by_primes_simd(uint32_t first_dividend, uint32_t last_dividend, uint32_t first_prime_index, uint32_t last_prime_index)
{
    assert(first_dividend <= last_dividend && first_prime_index <= last_prime_index &&
        ((first_dividend - last_dividend) % 4 == 0));
    bool is_correct = true;
    auto primes = calculate_prime_table(last_prime_index);
    while (first_prime_index != last_prime_index) {
        constant_divider_uint32 divider(primes[first_prime_index]);
        auto current_dividend = first_dividend;
        __m128i n, q;
        uint32_t check[4];
        while (current_dividend < last_dividend) {
            n = _mm_setr_epi32(current_dividend, current_dividend + 1, current_dividend + 2, current_dividend + 3);
            q = divider(n);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(check), q);
            for (int i = 0; i != 4; ++i) {
                if (check[i] != (current_dividend + i) / primes[first_prime_index]) {
                    is_correct = false;
                }

            }
            current_dividend += 4;
        }
        first_prime_index += 1;
    }

    return is_correct;
}

bool fd_t::division_random_simd(uint32_t num_divisors, uint32_t divisions_per_divisor)
{
    bool is_correct = true;
    std::random_device rd;
    std::mt19937 dividend_generator(rd());
    std::mt19937 divisor_generator(rd());
    std::uniform_int_distribution<uint32_t> dividend_distribution(1, std::numeric_limits<uint32_t>::max());
    std::uniform_int_distribution<uint32_t> divisor_distribution(1, std::numeric_limits<uint32_t>::max());
    while (num_divisors) {
        uint32_t divisor = divisor_distribution(divisor_generator);
        constant_divider_uint32 divider(divisor);
        auto rounding = divisions_per_divisor % 8;
        auto current_divisions = rounding ? divisions_per_divisor + (8 - rounding) : divisions_per_divisor;
        uint32_t dividends[8];
        uint32_t check[8];
        __m128i batch_1, batch_2, q_1, q_2;
        while (current_divisions) {
            for (auto& dividend : dividends)
                dividend = dividend_distribution(dividend_generator);
            //rng_dividend.nextUInt(dividends);
            batch_1 = _mm_loadu_si128(reinterpret_cast<__m128i const*>(dividends));
            batch_2 = _mm_loadu_si128(reinterpret_cast<__m128i const*>(dividends + 4));
            q_1 = divider(batch_1);
            q_2 = divider(batch_2);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(check), q_1);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(check + 4), q_2);
            for (int i = 0; i != 8; ++i) {
                if (check[i] != (dividends[i] / divisor)) {
                    is_correct = false;
                }
            }
            current_divisions -= 8;
        }
        --num_divisors;
    }
    return is_correct;
}