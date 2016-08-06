#include <iostream>
#include <cstdint>
#include <immintrin.h>

#include <fast_division/fast_division.hpp>

void interactive_division()
{
    using namespace std;
    uint32_t d;
    uint32_t a[4];
    __m128i n, q;
    uint32_t print[4];
    while (!cin.eof()) {
        cout << "Enter divisor:\n";
        cin >> d;
        if (cin.eof())
            break;
        fast_division::constant_divider_uint32 divider(d);
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

int main()
{
    interactive_division();
    return 0;
}