#pragma once
#include "./byte.hpp"
#include <cassert>
#if defined(__x86_64__) || defined(_M_X64)
#if defined(__BMI2__)
#include <immintrin.h> // これで _pdep_u64 が宣言される
                       // （必要なら）#include <bmi2intrin.h> でもOK
#endif
#endif

namespace stool
{
    namespace __LSB_BYTE
    {
        inline static constexpr int8_t get_1bit_position(uint8_t value, int rank)
        {
            int count = 0;
            for (int bit = 0; bit < 8; ++bit)
            {
                if (value & (1 << bit))
                {
                    if (count == rank)
                        return bit;
                    ++count;
                }
            }
            return -1;
        }

        inline static constexpr std::array<std::array<int8_t, 8>, 256> build_lookup_table_for_select1()
        {
            std::array<std::array<int8_t, 8>, 256> table{};

            for (int i = 0; i < 256; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    table[i][j] = get_1bit_position(static_cast<uint8_t>(i), j);
                }
            }

            return table;
        }

        inline static constexpr std::array<std::array<int8_t, 8>, 256> select1_table = stool::__LSB_BYTE::build_lookup_table_for_select1();
    }

    /*!
     * @brief A class for handling least significant bit (LSB) operations on bytes
     * @note This implementation includes optimized lookup tables and bit manipulation techniques
     *       for improved performance on LSB-oriented byte operations
     * \ingroup BasicClasses
     */
    class LSBByte
    {
    public:
        /*!
         * @brief Returns the i-th bit of 64-bit B[63..0].
         */

        static bool get_bit(uint64_t B, int8_t nth)
        {
            return ((B >> nth) & 0x00000001) > 0;
        }
        /*!
         * @brief Inserts a given bit v into 64-bit B[63..0] at position i
         *
         * @return The new bits B
         */
        static uint64_t insert_bit(uint64_t B, uint8_t i, bool v)
        {
            if (i == 0)
            {
                return (B << 1) | (v ? 1ULL : 0ULL);
            }
            else if (i == 63)
            {
                return (B >> 1) | ((v ? 1ULL : 0ULL) << 63);
            }
            else
            {
                uint64_t left = (B >> (i)) << (i + 1);
                uint64_t right = (B << (64 - i)) >> (64 - i);
                uint64_t center = (v ? 1ULL : 0ULL) << i;

                return left | center | right;
            }
        }

        /*!
         * @brief Replaces the i-th bit of 64-bit B[63..0] with v
         *
         * @return The new bits B
         */
        static uint64_t write_bit(uint64_t B, int64_t i, bool v)
        {
            if (i == 0)
            {
                return ((B >> 1) << 1) | (v ? 1ULL : 0ULL);
            }
            else if (i == 63)
            {
                return ((B << 1) >> 1) | ((v ? 1ULL : 0ULL) << 63);
            }
            else
            {
                uint64_t left = (B >> (i + 1)) << (i + 1);
                uint64_t right = (B << (64 - i)) >> (64 - i);
                uint64_t center = (v ? 1ULL : 0ULL) << i;
                return left | center | right;
            }
        }

        /*!
         * @brief Removes the i-th bit from 64-bit B[63..0]
         *
         * @return The new bits B
         */
        static uint64_t remove_bit(uint64_t B, uint64_t i)
        {
            if (i == 0)
            {
                return B >> 1;
            }
            else if (i == 63)
            {
                return (B << 1) >> 1;
            }
            else
            {
                uint64_t left = (B >> (i + 1)) << i;
                uint64_t right = (B << (64 - i)) >> (64 - i);
                return left | right;
            }
        }

        /*!
         * @brief Returns the position of the (i+1)-th 1 in 64-bit B[63..0].
         *
         * @return The position of the (i+1)-th 1 in 64-bit B[63..0], or -1 if not found.
         */
        static int64_t naive_select1(uint64_t B, uint64_t i)
        {
            uint64_t ith = 0;
            for (uint64_t x = 0; x < 64; x++)
            {
                bool b = get_bit(B, x);
                if (b)
                {
                    ith++;
                }
                if (ith == i + 1)
                {
                    return x;
                }
            }
            return -1;
        }

        /*!
         * @brief Returns the position of the (i+1)-th 1 in 64-bit B[63..0].
         *
         * @return The position of the (i+1)-th 1 in 64-bit B[63..0], or -1 if not found.
         */
        static int64_t select1(uint64_t B, uint64_t i)
        {

#if defined(__BMI2__)

            unsigned cnt = Byte::popcount(B);
            if (i >= cnt)
                return -1;                                              // 存在しない
            uint64_t src = 1ull << i;                                   // r番目の1を表す単一ビット
            uint64_t bit = _pdep_u64(src, B);                           // xの中の該当位置に配置
            uint64_t _result = static_cast<int>(LSBByte::select1(bit)); // その位置を返す

            return _result;

#else

            // 事前チェック（総1数 < i+1 は失敗）
            // sの最終バイト = 総1数 なので、本当は後段だけで検出できるが、
            // 早期終了しておくと無駄を抑えられる（popcountは1命令）。
            if (i >= static_cast<unsigned>(Byte::popcount(B)))
                return -1;

            // 1) byte popcount（各バイトに 0..8）
            uint64_t b = B - ((B & 0xAAAAAAAAAAAAAAAAULL) >> 1);
            b = (b & 0x3333333333333333ULL) + ((b >> 2) & 0x3333333333333333ULL);
            b = (b + (b >> 4)) & 0x0F0F0F0F0F0F0F0FULL;

            // 2) byte prefix sums
            constexpr uint64_t L8 = 0x0101010101010101ULL;
            uint64_t s = b * L8; // 各バイト: 先頭からの累積個数

            // 3) 最初に累積 >= i+1 となるバイトを検出
            constexpr uint64_t H8 = 0x8080808080808080ULL;
            uint64_t rank = static_cast<uint64_t>(i + 1) * L8;
            uint64_t mask = ((s | H8) - rank) & H8; // 各バイトのMSBが (s_byte >= rank_byte) なら1

            // 万一該当なし（理論上ここには来ない）は -1
            if (mask == 0)
            {
                assert(naive_select1(B, i) == -1);
                return -1;
            }

            int byte_index = __builtin_ctzll(mask) / 8;

            // 4) バイト内ランク r とバイト値
            uint64_t sb = (byte_index == 0) ? 0 : ((s >> ((byte_index - 1) * 8)) & 0xFF);
            unsigned r = i - static_cast<unsigned>(sb);
            uint8_t byte = static_cast<uint8_t>(B >> (byte_index * 8));

            // 5) バイト内 select（テーブル）
            int bit_in_byte = __LSB_BYTE::select1_table[byte][r];
            int64_t result = static_cast<int64_t>(byte_index * 8 + bit_in_byte);

            assert(result == naive_select1(B, i));
            return result;
#endif
        }

        /*!
         * @brief Returns the position of the first 1 in 64-bit B[63..0].
         *
         * @return The position of the first 1 in 64-bit B[63..0], or -1 if not found.
         */
        static int64_t select1(uint64_t B)
        {
            if (B != 0)
            {
                return __builtin_ffsll(B) - 1;
            }
            else
            {
                return -1;
            }
        }

        /*!
         * @brief Returns the position of the (i+1)-th 0 in 64-bit B[63..0].
         *
         * @return The position of the (i+1)-th 0 in 64-bit B[63..0], or -1 if not found.
         */
        static int64_t select0(uint64_t B, uint64_t i)
        {
            return select1(~B, i);
        }

        
        /*!
         * @brief Returns the bit size of 64-bit B when B is represented as an integer (e.g., get_code_length(1000) = 4)
         *
         * @return Returns the bit size of 64-bit B when B is represented as an integer if B != 0; otherwise returns 0.
         */
        static uint8_t get_code_length(uint64_t B)
        {
            if (B > 0)
            {

                return 64 - __builtin_clzll(B);
            }
            else
            {
                return 0;
            }
        }

        /*!
         * @brief Returns the first k bits of 64-bit B as a binary string
         */
        static std::string to_bit_string(uint64_t B, uint64_t k)
        {
            std::string s = Byte::to_bit_string(B, true);
            if (k == 0)
            {
                return "";
            }
            else
            {
                return s.substr(s.size() - k, k);
            }
        }
    };
}
