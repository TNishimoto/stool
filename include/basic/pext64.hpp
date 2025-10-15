// pext64_runtime.hpp
#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <mutex>

#if (defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86))
#define PEXT_RUNTIME_X86 1
#else
#define PEXT_RUNTIME_X86 0
#endif

#if PEXT_RUNTIME_X86
#if defined(_MSC_VER)
#include <intrin.h> // __cpuidex, _pext_u64
#include <immintrin.h>
#else
#include <cpuid.h>     // __get_cpuid_count
#include <immintrin.h> // _pext_u64
#endif
#endif

// pext64_tables_cxx17.hpp  (C++17)
#pragma once
#include <array>
#include <cstdint>

namespace stool
{
    namespace pext64
    {

        // 8bit popcount（constexpr）
        constexpr uint8_t popcount8(uint8_t m)
        {
            uint8_t c = 0;
            while (m)
            {
                m = static_cast<uint8_t>(m & (m - 1));
                ++c;
            }
            return c;
        }

        // 8bit 圧縮: x のうち mask=1 のビットを LSB から詰める（PEXTの1バイト版）
        constexpr uint8_t compress_byte(uint8_t x, uint8_t mask)
        {
            uint8_t out = 0, k = 0;
            for (uint8_t b = 0; b < 8; ++b)
            {
                if (mask & static_cast<uint8_t>(1u << b))
                {
                    out |= static_cast<uint8_t>(((x >> b) & 1u) << k);
                    ++k;
                }
            }
            return out;
        }

        struct Tables
        {
            std::array<uint8_t, 16> pop4{};
            // comp[mask][byte] = compress_byte(byte, mask)
            std::array<uint8_t, 256> comp{};
        };

        // 64KB のテーブルを constexpr で構築
        constexpr Tables make_tables()
        {
            Tables t{};
            for (int m = 0; m < 16; ++m)
            {
                t.pop4[m] = popcount8(static_cast<uint8_t>(m));
                for (int x = 0; x < 16; ++x)
                {
                    t.comp[(m << 4) + x] = compress_byte(static_cast<uint8_t>(x), static_cast<uint8_t>(m));
                }
            }
            return t;
        }

        // ヘッダ内に置く実体。
        // C++17 の inline 変数 + constexpr により ODR安定 & 定数初期化。
        inline constexpr Tables kTables = make_tables();

        // ついでに自己検査（コンパイル時評価）
        /*
        static_assert(kTables.pop8[0x00] == 0, "pop8[0x00]");
        static_assert(kTables.pop8[0xFF] == 8, "pop8[0xFF]");
        static_assert(kTables.comp[0b10110010][0b11010100] == 0b1010, "compress check");
        */
    }

    class Pext64
    {
    public:
        static uint64_t pext_portable(uint64_t x, uint64_t mask)
        {
            const auto &tab = pext64::kTables;
            uint64_t out = 0;
            unsigned shift = 0;
            for (unsigned i = 0; i < 16; ++i)
            {
                uint8_t mb = (static_cast<uint8_t>(mask >> (i * 4))) & 0b1111;
                if (!mb)
                    continue;
                uint8_t xb = (static_cast<uint8_t>(x >> (i * 4))) & 0b1111;
                uint8_t packed = tab.comp[(mb << 4) + xb];
                out |= (static_cast<uint64_t>(packed) << shift);
                shift += tab.pop4[mb];
            }
            return out;
        }

        static uint64_t naive_pext(uint64_t X, uint64_t Y)
        {
            uint64_t out = 0;
            unsigned i = 0; // out のビット位置（LSBから順に詰める）
            while (Y)
            {
                unsigned b = __builtin_ctzll(Y); // Y の最下位の 1 の位置 = y_i
                out |= ((X >> b) & 1ull) << i;   // X[y_i] を out の i ビット目へ
                Y &= Y - 1;                      // 最下位の 1 を落とす
                ++i;
            }
            return out;
        }

        static bool has_bmi2_runtime()
        {
#if PEXT_RUNTIME_X86
            unsigned eax = 0, ebx = 0, ecx = 0, edx = 0;
#if defined(_MSC_VER)
            int regs[4];
            __cpuidex(regs, 7, 0);
            ebx = static_cast<unsigned>(regs[1]);
#else
            if (!__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx))
                return false;
#endif
            return (ebx & (1u << 8)) != 0; // BMI2
#else
            return false;
#endif
        }

        static bool bmi2_available()
        {
            static std::once_flag once;
            static bool ok = false;
            std::call_once(once, []
                           { ok = has_bmi2_runtime(); });
            return ok;
        }

        static uint64_t pext64(uint64_t x, uint64_t mask)
        {
#if PEXT_RUNTIME_X86
            if (bmi2_available())
                return _pext_u64(x, mask);
            return pext_portable(x, mask);
#else
            return pext_portable(x, mask);
#endif
        }
    };

} // namespace pext_rt::ct