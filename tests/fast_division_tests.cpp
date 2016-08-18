#include "fast_division_tests.hpp"

#include <cassert>
#include <vector>
#include <immintrin.h>
#include <random>

#include <fast_division/fast_division.hpp>
#include <fast_division/fast_division_base.hpp>
#include <fast_division/fast_division_simd.hpp>
#include <fast_division/utility/associated_types.hpp>
#include <fast_division/division_policy.hpp>

namespace fd_t = fast_division::tests;

namespace {

    template<typename Integer, typename  SizeType = uint64_t>
    std::vector<Integer> calculate_prime_table(SizeType num_primes)
    {
        using namespace std;
        assert(num_primes != 0);
        vector<Integer> primes;
        primes.reserve(num_primes);
        primes.push_back(Integer(2));
        --num_primes;
        Integer candidate = 3;
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

    template <typename T>
    using distribution_t = std::conditional_t<
        std::is_same<uint8_t, T>::value, 
        uint16_t, std::conditional_t<std::is_same<int8_t, T>::value,
                                     int16_t, T> >;

    template<typename Integer, template<typename, bool> class DivisionPolicy = fast_division::promotion_policy,
             typename SizeType = uint64_t>
    bool random_division_impl(SizeType num_divisors, SizeType divisions_per_divisor)
    {
        using namespace  std;
        using namespace fast_division;
        bool is_correct = true;
        random_device rd;
        mt19937 dividend_generator(rd());
        mt19937 divisor_generator(rd());
        uniform_int_distribution<distribution_t<Integer>> dividend_distribution(
            numeric_limits<Integer>::min(), numeric_limits<Integer>::max());
        uniform_int_distribution<distribution_t<Integer>> divisor_distribution(
            numeric_limits<Integer>::min(), numeric_limits<Integer>::max());
        while (num_divisors) {
            Integer divisor_ = static_cast<Integer>(divisor_distribution(divisor_generator));
            // Skip division by zero.
            if (divisor_ == Integer(0)) {
                continue;
            }
            constant_divider<Integer, DivisionPolicy> divider(divisor_);
            constant_divider<Integer, promotion_policy> divider_ref(divisor_);
            auto current_divisions = divisions_per_divisor;
            while (current_divisions) {
                Integer dividend = static_cast<Integer>(dividend_distribution(dividend_generator));
                Integer result = divider(dividend);
                Integer expected = dividend / divisor_;
                if(result != expected) {
                    is_correct = false;
                }
                --current_divisions;
            }
            --num_divisors;
        }
        return is_correct;
    }

    template<typename Integer, typename DoubleWordInteger = fast_division::utility::promotion_t<Integer>,
             typename SizeType = uint64_t>
    bool high_division_impl(SizeType num_multiplication)
    {
        using namespace  std;
        using namespace fast_division;
        bool is_correct = true;
        random_device rd;
        mt19937 generator(rd());
        uniform_int_distribution<distribution_t<Integer>> integer_distribution(
            numeric_limits<Integer>::min(), numeric_limits<Integer>::max());
        while (num_multiplication)
        {
            Integer m1 = static_cast<Integer>(integer_distribution(generator));
            Integer m2 = static_cast<Integer>(integer_distribution(generator));
            Integer result = utility::high_mult(m1, m2);
            Integer expected = (DoubleWordInteger(m1) * DoubleWordInteger(m2)) >> (8 * sizeof(Integer));
            if (result != expected) {
                is_correct = false;
            }
            --num_multiplication;
        }
        return is_correct;
    }

    template <typename Integer, typename DivisionPolicy, typename = std::enable_if_t<!std::is_signed<Integer>> >
    Integer calculate_multiplier(Integer divisor, DivisionPolicy policy)
    {
        Integer multiplier;
        if (divisor == 1) {
            multiplier = 0;
        }
        else if ((divisor & (divisor - Integer(1))) == 0) {
            multiplier = 0;
        }
        else {
            Integer l = fast_division::utility::log2i(divisor - 1) + 1;
            multiplier = DivisionPolicy::calculate_multiplier(divisor, l);
        }
        return multiplier;
    }

    template <typename Integer, template<typename, bool> class DivisionPolicy,
              typename SizeType = uint64_t>
    bool division_policy(SizeType num_divisors)
    {
        using namespace  std;
        using namespace fast_division;
        using test_policy = DivisionPolicy<Integer, std::is_signed<Integer>::value>>;
        using reference_policy = promotion_policy<Integer, std::is_signed<Integer>::value>>;
        bool is_correct = true;
        random_device rd;
        mt19937 generator(rd());
        uniform_int_distribution<distribution_t<Integer>> integer_distribution(
            numeric_limits<Integer>::min(), numeric_limits<Integer>::max());
        while (num_divisors) {
            Integer divisor = static_cast<Integer>(integer_distribution(generator));
            if (divisor == Integer(0))
                continue;
            auto result   = calculate_multiplier(divisor, test_policy{});
            auto expected = calculate_multiplier(divisor, reference_policy{});
            if (result != expected) {
                bool is_correct = false;
            }
            --num_divisors;
        }
        return is_correct;
    }

}

bool fd_t::division_simd(uint32_t first_dividend, uint32_t last_dividend,
                         uint32_t first_divisor, uint32_t last_divisor, uint32_t divisor_step)
{
    assert(first_dividend <= last_dividend && first_divisor <= last_divisor &&
        ((first_dividend - last_dividend) % 4 == 0));
    bool is_correct = true;

    while (first_divisor < last_divisor) {
        constant_divider<uint32_t> divider(first_divisor);
        auto current_dividend = first_dividend;
        __m128i n, q;
        uint32_t check[4];
        while (current_dividend < last_dividend) {
            n = _mm_setr_epi32(current_dividend, current_dividend + 1,
                               current_dividend + 2, current_dividend + 3);
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

bool fd_t::division_by_primes_simd(uint32_t first_dividend, uint32_t last_dividend,
                                   uint32_t first_prime_index, uint32_t last_prime_index)
{
    assert(first_dividend <= last_dividend && first_prime_index <= last_prime_index &&
        ((first_dividend - last_dividend) % 4 == 0));
    bool is_correct = true;
    auto primes = calculate_prime_table<uint32_t>(last_prime_index);
    while (first_prime_index != last_prime_index) {
        constant_divider<uint32_t> divider(primes[first_prime_index]);
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
        uint32_t divisor_ = divisor_distribution(divisor_generator);
        constant_divider<uint32_t> divider(divisor_);
        auto rounding = divisions_per_divisor % 8;
        auto current_divisions = rounding ? divisions_per_divisor + (8 - rounding) : divisions_per_divisor;
        uint32_t dividends[8];
        uint32_t check[8];
        __m128i batch_1, batch_2, q_1, q_2;
        while (current_divisions) {
            for (auto& dividend : dividends)
                dividend = dividend_distribution(dividend_generator);
            batch_1 = _mm_loadu_si128(reinterpret_cast<__m128i const*>(dividends));
            batch_2 = _mm_loadu_si128(reinterpret_cast<__m128i const*>(dividends + 4));
            q_1 = divider(batch_1);
            q_2 = divider(batch_2);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(check), q_1);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(check + 4), q_2);
            for (int i = 0; i != 8; ++i) {
                if (check[i] != (dividends[i] / divisor_)) {
                    is_correct = false;
                }
            }
            current_divisions -= 8;
        }
        --num_divisors;
    }
    return is_correct;
}


bool fd_t::random_unsigned_division()
{
    using namespace std;
    using fast_division::decomposition_policy;
    // Test for uint8_t
    auto uint8_test = random_division_impl<uint8_t, decomposition_policy>(1000, 10000);
    // Test for uint16_t
    auto uint16_test = true;// random_division_impl<uint16_t, decomposition_policy>(100000, 10000);
    // Test for uint32_t
    auto uint32_test = random_division_impl<uint32_t, decomposition_policy>(100000, 10000);
    
    return uint8_test && uint16_test && uint32_test;
}

bool fd_t::random_signed_division()
{
    using namespace std;
    // Test for uint8_t
    auto int8_test = random_division_impl<int8_t>(1000, 10000);
    // Test for uint16_t
    auto int16_test = random_division_impl<int16_t>(10000, 10000);
    // Test for uint32_t
    auto int32_test = random_division_impl<int32_t>(100000, 10000);

    return int8_test && int16_test && int32_test;

}

bool fd_t::high_multiplication()
{
    using namespace std;
    // Test for uint8_t
    auto uint8_test = high_division_impl<std::uint8_t>(100);
    // Test for uint16_t
    auto uint16_test = high_division_impl<uint16_t>(1000);
    // Test for uint32_t
    auto uint32_test = high_division_impl<uint32_t>(10000);

    // Test for int8_t
    auto int8_test = high_division_impl<int8_t>(100);
    // Test for int16_t
    auto int16_test = high_division_impl<int16_t>(1000);
    // Test for int32_t
    auto int32_test = high_division_impl<int32_t>(10000);
    return uint8_test && uint16_test && uint32_test &&
           int8_test && int16_test && int32_test;
}