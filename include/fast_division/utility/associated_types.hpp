/**
*  Fast Division Library
*  Created by Stefan Ivanov
*/
#pragma once

namespace fast_division {
    namespace utility {

        /// Meta-function for linear search.

        template <typename... T>
        struct is_one_of;

        template <typename T>
        struct is_one_of <T> {
            constexpr static bool value = false;
        };

        template <typename T, typename... Ts>
        struct is_one_of <T, T, Ts...> {
            constexpr static bool value = true;
        };

        template <typename T, typename U, typename... Ts>
        struct is_one_of <T, U, Ts...> {
            constexpr static bool value = is_one_of<T, Ts...>::value;
        };

        /// Helper for determining if a type a SIMD vector.
        // TODO Add detection of available SIMD types.

        template <typename T>
        struct is_simd {
            constexpr static bool value = is_one_of<T, __m128i, __m256i>::value;
        };

        /// Promotion for integer types. Must be specialized to work properly.
        template <typename T>
        struct promotion;

        template <typename T>
        using promotion_t = typename promotion<T>::type;

        /// Specialization for several types

        template <>
        struct promotion<uint8_t> {
            using type = uint16_t;
        };

        template <>
        struct promotion<uint16_t> {
            using type = uint32_t;
        };

        template <>
        struct promotion<uint32_t> {
            using type = uint64_t;
        };

        template <>
        struct promotion<int8_t> {
            using type = int16_t;
        };

        template <>
        struct promotion<int16_t> {
            using type = int32_t;
        };

        template <>
        struct promotion<int32_t> {
            using type = int64_t;
        };

    }
}