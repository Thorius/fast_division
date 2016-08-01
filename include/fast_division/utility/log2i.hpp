/** 
 *  Fast Division Library
 *  Created by Stefan Ivanov
 */
#pragma once

#include <cassert>

namespace fast_division
{
    namespace utility
    {
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
                _BitScanReverse(&result, static_cast<unsigned long>(value));
            else
                _BitScanReverse64(&result, static_cast<unsigned long long>(value));
            return T(result);
        #else
            T r = 0;
            while ((value >> r) != 0)
                r++;
            return r - 1;
        #endif
        }     

    }
}
