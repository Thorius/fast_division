/**
*  Fast Division Library
*  Created by Stefan Ivanov
*
*  Using ideas from
*  Division by Invariant Integers Using Multiplication (1994)
*  by Torbjörn Granlund, Peter L. Montgomery
*/
#pragma once

#include <algorithm>

#include <fast_division/division_policy.hpp>
#include <fast_division/utility/log2i.hpp>
#include <fast_division/utility/high_multiplication.hpp>

namespace fast_division {

    template<typename Integer, bool Signed, template <typename I, bool S> class DivisionPolicy>
    class constant_divider_base {
    public:
        constexpr static const auto word_size = sizeof(Integer) * 8;
        using division_policy = DivisionPolicy<Integer, Signed>;

        explicit constant_divider_base(Integer divisor)
        {
            if (divisor == 1) { // A no-op.
                multiplier_ = shift_1_ = shift_2_ = 0;
            }
            else if ((divisor & (divisor - Integer(1))) == 0) { // Power of 2
                multiplier_ = shift_1_ = 0;
                shift_2_ = utility::log2i(divisor);
            }
            else {
                Integer l = utility::log2i(divisor - 1) + 1;
                // The bellow works for most types but not for uint8_t for some strange reason.
                // Check for overflow
                //Integer l2 = l < word_size ? (Integer(1) << l) : 0;
                //multiplier = 1 + Integer((p_type(l2 - divisor) << word_size) / divisor);
                // Alternatively:  multiplier = ((2 << (N + l)) / d) - (2 << N) + 1;
                // or: multiplier = p_type(1 << (word_size + l)) / divisor - p_type(1 << word_size) + 1;
                //multiplier_ = 1 + Integer(((p_type(1) << word_size) * ((p_type(1) << l) - divisor)) / divisor);
                multiplier_ = division_policy::calculate_multiplier(divisor, l);
                shift_1_ = std::min(l, Integer(1));
                shift_2_ = l - shift_1_;  //max(l - 1, 0)
            }
        }

        Integer operator()(Integer input)  const
        {
            Integer q = utility::high_mult(multiplier_, input);
            q = (q + ((input - q) >> shift_1_)) >> shift_2_;
            return q;
        }


        template <typename Simd, typename = std::enable_if_t<utility::is_simd<Simd>::value>>
        Simd operator()(Simd input) const
        {
            static_assert(false, "Division by a simd vector must use a specialization");
            return input;
        }

    private:
        Integer multiplier_;
        Integer shift_1_;
        Integer shift_2_;
    };

    template <typename Integer, template <typename I, bool S> class DivisionPolicy>
    class constant_divider_base<Integer, true, DivisionPolicy> {
    public:
        constexpr static const auto word_size = sizeof(Integer) * 8;
        using division_policy = DivisionPolicy<Integer, true>;

        explicit constant_divider_base(Integer divisor)
        {
            std::make_unsigned_t<Integer> abs_divisor;
            if (divisor < 0) {
                sign_ = Integer(-1);
                abs_divisor = -divisor;
            }
            else {
                sign_ = Integer(0);
                abs_divisor = divisor;
            }

            Integer l = utility::log2i(abs_divisor - 1) + 1;
            l = std::max(l, Integer(1));
            multiplier_ = division_policy::calculate_multiplier(abs_divisor, l);
            shift_ = l - 1;
        }

        Integer operator()(Integer input) const
        {
            Integer q = input + utility::high_mult(multiplier_, input);
            q = (q >> shift_) - (input >= 0 ? 0 : -1);
            return (q ^ sign_) - sign_;
        }

        template <typename Simd, typename = std::enable_if_t<utility::is_simd<Simd>::value>>
        Simd operator()(Simd input) const
        {
            static_assert(false, "Division by a simd vector must use a specialization");
            return input;
        }

    private:
        Integer multiplier_;
        Integer shift_;
        Integer sign_;

    };

}
