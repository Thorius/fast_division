/** 
 *  Fast Division Library
 *  Created by Stefan Ivanov
 */
#pragma once

#include <cstdint>
#include <cstddef>
#include <utility>

#include <fast_division/utility/high_multiplication.hpp>

namespace fast_division {
    namespace utility {

        struct uint128_t;
        using word_type = std::uint64_t;

        uint128_t full_multiplication(word_type x, word_type y);

        std::pair<uint128_t, uint128_t> quotient_and_remainder(uint128_t x, uint128_t y);

        struct uint128_t {
            using word_type = std::uint64_t;
            constexpr static const auto word_size = 8*sizeof(word_type);

            constexpr uint128_t() 
                : low_{}, high_{} {}
            
            constexpr uint128_t(word_type val)
                : low_{val}, high_{} {}

            constexpr uint128_t(word_type high, word_type low)
                : low_{low}, high_{high} {}
            
            template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
            constexpr explicit operator T() const
            {
                return low_;
            }
 
            uint128_t& operator+= (const uint128_t& val)
            {
                word_type prev = low_;
                word_type carry = 0;
                low_ += val.low_;
                if (low_ < prev) {
                    carry = 1;
                }
                high_ += val.high_ + carry;
                return *this;
            }

            uint128_t& operator-= (const uint128_t& val)
            {
                word_type prev = low_;
                word_type borrow = 0;
                low_ -= val.low_;
                if (low_ > prev) {
                    borrow = 1;
                }
                high_ -= (val.high_ + borrow);
                return *this;
            }

            uint128_t& operator*= (const uint128_t& val)
            {
                auto low_mult = full_multiplication(low_, val.low_);
                low_mult.high_ += (low_*val.high_ + high_*val.low_);
                *this = low_mult;
                return *this; 
            }

            uint128_t& operator/= (const uint128_t val)
            {
                *this = quotient_and_remainder(*this, val).first;
                return *this;
            }
            
            uint128_t& operator%= (const uint128_t val)
            {
                *this = quotient_and_remainder(*this, val).second;
                return *this;
            }

            uint128_t& operator&= (const uint128_t& val)
            {
                low_ &= val.low_;
                high_ &= val.high_;
                return *this;
            }

            uint128_t& operator|= (const uint128_t& val)
            {
                low_ |= val.low_;
                high_ |= val.high_;
                return *this;
            }

            uint128_t& operator^= (const uint128_t& val)
            {
                low_ ^= val.low_;
                high_ ^= val.high_;
                return *this;
            }

            uint128_t& operator~ ()
            {
                low_ = ~low_;
                high_ = ~high_;
                return *this;
            }

            uint128_t& operator>>= (unsigned long long sh)
            {
                low_ = (sh < word_size) ? ((low_ >> sh) | (high_ << (word_size - sh))) 
                                        : (high_ >> (sh - word_size));
                high_ = high_ >> sh;
                return *this;
            }

            uint128_t& operator<<= (unsigned long long sh)
            {
                high_ = (sh < word_size) ? ((high_ << sh) | (low_ >> (word_size - sh))) 
                                         : (low_ << (sh - word_size));
                low_ = low_ << sh;
                return *this;
            }

            word_type low_;
            word_type high_;
        };

        uint128_t full_multiplication(word_type x, word_type y)
        {
            using fast_division::utility::low_bits;
            using fast_division::utility::high_bits;
            uint128_t result;
            word_type x_low = low_bits(x);
            word_type y_low = low_bits(y);
            word_type x_high = high_bits(x);
            word_type y_high = high_bits(y);
            result.low_ = x_low * y_low;
            result.high_ = x_high * y_high; 
            word_type xl_yh = x_low * y_high;
            word_type xh_yl = x_high * y_low;
            result += (low_bits(xl_yh) << uint128_t::word_size/2);
            result += (low_bits(xh_yl) << uint128_t::word_size/2);
            result.high_ += high_bits(xl_yh);
            result.high_ += high_bits(xh_yl);
            return result;
        }

        uint128_t operator+ (const uint128_t& x, const uint128_t& y) {
            auto temp{x};
            return temp += y;
        }

        uint128_t operator- (const uint128_t& x, const uint128_t& y)
        {
            auto temp{x};
            return temp -= y;
        }

        uint128_t operator* (const uint128_t& x, const uint128_t& y)
        {
            auto temp{x};
            return temp *= y;
        }

        uint128_t operator/ (const uint128_t& x, const uint128_t& y)
        {
            auto temp{x};
            return temp /= y;
        }

        uint128_t operator% (const uint128_t& x, const uint128_t& y)
        {
            auto temp{x};
            return temp %= y;
        }

        uint128_t operator& (const uint128_t& x, const uint128_t& y)
        {
            auto temp{x};
            return temp &= y;
        }

        uint128_t operator| (const uint128_t& x, const uint128_t& y)
        {
            auto temp{x};
            return temp |= y;
        }

        uint128_t operator^ (const uint128_t& x, const uint128_t& y)
        {
            auto temp{x};
            return temp ^= y;
        }

        uint128_t operator<< (const uint128_t& x, unsigned long long sh)
        {
            auto temp{x};
            return temp <<= sh;
        }

        uint128_t operator>> (const uint128_t& x, unsigned long long sh)
        {
            auto temp{x};
            return temp >>= sh;
        }

        constexpr 
        bool operator== (const uint128_t& x, const uint128_t& y)
        {
            return (x.high_ == y.high_) && (x.low_ == y.low_);
        }

        constexpr 
        bool operator!= (const uint128_t& x, const uint128_t& y)
        {
            return (x.high_ != y.high_) || (x.low_ != y.low_);
        }

        constexpr 
        bool operator< (const uint128_t& x, const uint128_t& y)
        {
            return (x.high_ < y.high_) ? true : ((y.high_ < x.high_) ? false : x.low_ < y.low_);
        }

        constexpr 
        bool operator> (const uint128_t& x, const uint128_t& y)
        {
            return (x.high_ > y.high_) ? true : ((y.high_ > x.high_) ? false : x.low_ > y.low_);
        }

        constexpr 
        bool operator<= (const uint128_t& x, const uint128_t& y)
        {
            return !(x > y);
        }

        constexpr 
        bool operator>= (const uint128_t& x, const uint128_t& y)
        {
            return !(x < y);
        }

        uint128_t double_while_smaller(const uint128_t& bound, uint128_t elem)
        {
            while (bound - elem >= elem) {
                elem = elem + elem;
            }
            return elem;
        }

        std::pair<uint128_t, uint128_t> quotient_and_remainder(uint128_t x, uint128_t y)
        {
            if (x < y) {
                return std::make_pair(uint128_t{0}, x);
            }
            uint128_t z = double_while_smaller(x, y);
            uint128_t q {1};
            x = x - z;
            while (z != y) {
                z = (z >> 1);
                q = q + q;
                if (z <= x) {
                    x = x - z;
                    q = q + 1;
                }
            }
            return std::make_pair(q, x);
        }
    }
}