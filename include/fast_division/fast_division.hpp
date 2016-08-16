/**
 *  Fast Division Library
 *  Created by Stefan Ivanov
 *
 *  Using ideas from
 *  Division by Invariant Integers Using Multiplication (1994)
 *  by Torbjörn Granlund, Peter L. Montgomery
 */
#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

#include <fast_division/fast_division_base.hpp>
#include <fast_division/division_policy.hpp>


namespace fast_division {

    template <typename Integer, template <typename I, bool S> class DivisionPolicy = promotion_policy>
    class constant_divider : constant_divider_base<Integer, std::is_signed<Integer>::value,
                                                   DivisionPolicy> {
    public:
        using base = constant_divider_base<Integer, std::is_signed<Integer>::value, DivisionPolicy>;
        using base::word_size;
        using value_type = Integer;
        
        explicit constant_divider(Integer divisor)
            : divisor_(divisor), constant_divider_base(divisor)
        {}

        const Integer& divisor() const { return divisor_; }

        template <typename T>
        auto operator()(T&& input) const
        {
            return base::operator()(std::forward<T>(input));
        }

        /// Equality and comparison operators delegating to the underlying divisor.

        friend
        bool operator== (const constant_divider& x, const constant_divider& y)
        {
            return x.divisor_ == y.divisor_;
        }
        friend
        bool operator!= (const constant_divider& x, const constant_divider& y)
        {
            return x.divisor_ != y.divisor_;
        }
        friend
        bool operator<  (const constant_divider& x, const constant_divider& y)
        {
            return x.divisor_ < y.divisor_;
        }
        friend
        bool operator>  (const constant_divider& x, const constant_divider& y)
        {
            return x.divisor_ > y.divisor_;
        }
        friend
        bool operator<= (const constant_divider& x, const constant_divider& y)
        {
            return x.divisor_ <= y.divisor_;
        }
        friend
        bool operator>= (const constant_divider& x, const constant_divider& y)
        {
            return x.divisor_ >= y.divisor_;
        }

    private:
        Integer divisor_;
    };

    /// Operator overload so that the fast_divider can be used in a generic context.
    /// Furthermore, this provides a more natural syntax for use.

    template <typename Integer, typename T>
    inline
    Integer operator/ (T&& divident, const constant_divider<Integer>& divisor)
    {
        return divisor(std::forward<T>(divident));
    }

}