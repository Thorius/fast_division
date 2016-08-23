#include <iostream>
#include <cstdint>
#include <immintrin.h>

#include <fast_division/fast_division.hpp>
#include <fast_division/fast_division_simd.hpp>
#include <fast_division/division_policy.hpp>
#include <fast_division/utility/high_multiplication.hpp>

using namespace std;

void interactive_division_unsigned_simd()
{
    using fast_division::constant_divider;
    using fast_division::decomposition_policy;
    uint32_t d;
    uint32_t a[4];
    __m128i n, q;
    uint32_t print[4];
    while (!cin.eof()) {
        cout << "Enter divisor:\n";
        cin >> d;
        if (cin.eof()) {
            cin.clear();
            break;
        }
        constant_divider<uint32_t> divider(d);
        cout << "Enter 4 dividends:\n";
        cin >> a[0] >> a[1] >> a[2] >> a[3];
        n = _mm_setr_epi32(a[0], a[1], a[2], a[3]);
        q = divider(n);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(print), q);
        for (int i = 0; i != 4; ++i) {
            cout << print[i] << " ";
        }
        cout << "\n\n";
    }
}

void interactive_division_unsigned()
{
    using fast_division::constant_divider;
    using fast_division::decomposition_policy;
    uint32_t divisor;
    uint32_t dividends[4];
    while (!cin.eof()) {
        cout << "Enter divisor:\n";
        cin >> divisor;
        if (cin.eof()) {
            cin.clear();
            break;
        }
        constant_divider<uint32_t, decomposition_policy> divider(divisor);
        cout << "Enter 4 dividends:\n";
        for (auto& d : dividends) {
            cin >> d;
        }
        for (auto& d : dividends) {
            cout << (d / divider) << " ";
        }                       
        cout << "\n\n";
    }
}

void interactive_division_signed()
{
    using fast_division::constant_divider;
    using fast_division::promotion_policy;
    int32_t divisor;
    int32_t dividends[4];
    while (!cin.eof()) {
        cout << "Enter divisor:\n";
        cin >> divisor;
        if (cin.eof()) {
            cin.clear();
            break;
        }
        constant_divider<int32_t, promotion_policy> divider(divisor);
        cout << "Enter 4 dividends:\n";
        for (auto& d : dividends) {
            cin >> d;
        }
        for (auto& d : dividends) {
            cout << (d / divider) << " ";
        }                       
        cout << "\n\n";
    }
}

void interactive_high_multiplication()
{
    using fast_division::constant_divider;
    using fast_division::utility::high_mult;
    int64_t a, b;
    while (!cin.eof()) {
        cout << "Enter two numbers to multiply:\n";
        cin >> a >> b;
        if (cin.eof()) {
            cin.clear();
            break;
        }
        cout << "High bits of multiplication: " << high_mult(a, b);
        cout << "\n\n";
    }
}

int main()
{
    char choice;
    cout << "Choose mode:\n 1) Unsigned division.\n 2) Signed division."
            "\n 3) Unsigned division SIMD.\n 4) 64bit signed high multiplication.\n ";
    while (cin >> choice) {
        switch (choice) {
        case '1':
            interactive_division_unsigned();
            break;
        case '2':
            interactive_division_signed();
            break;
        case '3':
            interactive_division_unsigned_simd();
            break;
        case '4':
            interactive_high_multiplication();
        default:
            cout << "Invalid option.\n";
        }
         cout << "Choose mode:\n 1) Unsigned division.\n 2) Signed division."
                 "\n 3) Unsigned division SIMD.\n 4) 64bit signed high multiplication.\n ";
   }

    return 0;
}