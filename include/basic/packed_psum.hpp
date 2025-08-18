#pragma once
#include "./lsb_byte.hpp"
#include "./msb_byte.hpp"
#include <cassert>
#include <cstring>

namespace stool
{
    enum class PackedBitType
    {
        BIT_1 = 0,
        BIT_2 = 1,
        BIT_4 = 2,
        BIT_8 = 3,
        BIT_16 = 4,
        BIT_32 = 5,
        BIT_64 = 6
    };

    class PackedPsum
    {
    public:
        static PackedBitType get_code_type(uint64_t value)
        {
            if (value <= 1)
            {
                return PackedBitType::BIT_1;
            }
            else if (value <= 3)
            {
                return PackedBitType::BIT_2;
            }
            else if (value <= 15)
            {
                return PackedBitType::BIT_4;
            }
            else if (value <= UINT8_MAX)
            {
                return PackedBitType::BIT_8;
            }
            else if (value <= UINT16_MAX)
            {
                return PackedBitType::BIT_16;
            }
            else if (value <= UINT32_MAX)
            {
                return PackedBitType::BIT_32;
            }
            else
            {
                return PackedBitType::BIT_64;
            }
        }

        static uint64_t sum32x2bits(uint64_t bits)
        {
            uint64_t lsb = bits & 0x5555555555555555ULL;

            uint64_t msb = (bits >> 1) & 0x5555555555555555ULL;

            uint64_t lsb_sum = stool::Byte::count_bits(lsb);
            uint64_t msb_sum = stool::Byte::count_bits(msb);

            return lsb_sum + (2 * msb_sum);
        }
        static inline uint64_t sum16x4bits(uint64_t x)
        {
            const uint64_t mask = 0x0F0F0F0F0F0F0F0FULL;
            uint64_t lo = x & mask;
            uint64_t hi = (x >> 4) & mask;
            uint64_t bytes = lo + hi;

            return (bytes * 0x0101010101010101ULL) >> 56;
        }
        static inline uint32_t sum8x8bits(uint64_t x)
        {
            x = (x & 0x00FF00FF00FF00FFULL) + ((x >> 8) & 0x00FF00FF00FF00FFULL);
            x = (x & 0x0000FFFF0000FFFFULL) + ((x >> 16) & 0x0000FFFF0000FFFFULL);
            x = (x & 0x00000000FFFFFFFFULL) + (x >> 32);
            return static_cast<uint32_t>(x); // or (uint32_t)(x & 0x7FF)
        }

        static inline uint32_t sum4x16bits(uint64_t x)
        {
            x = (x & 0x0000FFFF0000FFFFULL) + ((x >> 16) & 0x0000FFFF0000FFFFULL);
            x = (x & 0x00000000FFFFFFFFULL) + (x >> 32);
            return static_cast<uint32_t>(x);
        }
        static inline uint64_t sum2x32bits(uint64_t x)
        {
            uint64_t L = x >> 32;
            uint64_t R = x & (UINT64_MAX >> 32);
            return static_cast<uint64_t>(L + R);
        }

        static uint64_t psum64x1bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = i / 64;
            uint64_t bit_index = i % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += stool::Byte::count_bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += stool::Byte::count_bits(last_block);
            return sum;
        }
        static uint64_t psum64x1bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = i / 64;
            uint64_t start_bit_index = i % 64;
            uint64_t end_block_index = j / 64;
            uint64_t end_bit_index = j % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += stool::Byte::count_bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += stool::Byte::count_bits(bits[k]);
                }

                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += stool::Byte::count_bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += stool::Byte::count_bits(modified_last_block);
            }
            return sum;
        }

        static uint64_t psum32x2bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 2) / 64;
            uint64_t bit_index = ((i * 2) + 1) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum32x2bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum32x2bits(last_block);
            return sum;
        }
        static uint64_t psum32x2bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 2) / 64;
            uint64_t start_bit_index = (i * 2) % 64;
            uint64_t end_block_index = (j * 2) / 64;
            uint64_t end_bit_index = ((j * 2) + 1) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum32x2bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum32x2bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum32x2bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum32x2bits(modified_last_block);
            }

            return sum;
        }

        static uint64_t psum16x4bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 4) / 64;
            uint64_t bit_index = ((i * 4) + 3) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum16x4bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum16x4bits(last_block);
            return sum;
        }

        static uint64_t psum16x4bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 4) / 64;
            uint64_t start_bit_index = (i * 4) % 64;
            uint64_t end_block_index = (j * 4) / 64;
            uint64_t end_bit_index = ((j * 4) + 3) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum16x4bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum16x4bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum16x4bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum16x4bits(modified_last_block);
            }

            return sum;
        }

        static uint64_t psum8x8bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 8) / 64;
            uint64_t bit_index = ((i * 8) + 7) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum8x8bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum8x8bits(last_block);
            return sum;
        }

        static uint64_t psum8x8bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 8) / 64;
            uint64_t start_bit_index = (i * 8) % 64;
            uint64_t end_block_index = (j * 8) / 64;
            uint64_t end_bit_index = ((j * 8) + 7) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum8x8bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum8x8bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum8x8bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum8x8bits(modified_last_block);
            }

            return sum;
        }

        static uint64_t psum4x16bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 16) / 64;
            uint64_t bit_index = ((i * 16) + 15) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum4x16bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum4x16bits(last_block);
            return sum;
        }

        static uint64_t psum4x16bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 16) / 64;
            uint64_t start_bit_index = (i * 16) % 64;
            uint64_t end_block_index = (j * 16) / 64;
            uint64_t end_bit_index = ((j * 16) + 15) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum4x16bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum4x16bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum4x16bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum4x16bits(modified_last_block);
            }

            return sum;
        }

        static uint64_t psum2x32bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 32) / 64;
            uint64_t bit_index = ((i * 32) + 31) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum2x32bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum2x32bits(last_block);
            return sum;
        }
        static uint64_t psum2x32bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 32) / 64;
            uint64_t start_bit_index = (i * 32) % 64;
            uint64_t end_block_index = (j * 32) / 64;
            uint64_t end_bit_index = ((j * 32) + 31) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum2x32bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum2x32bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum2x32bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum2x32bits(modified_last_block);
            }

            return sum;
        }

        static uint64_t psum1x64bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            for (uint64_t j = 0; j <= i; j++)
            {
                sum += bits[j];
            }
            return sum;
        }
        static uint64_t psum1x64bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            for (uint64_t k = i; k <= j; k++)
            {
                sum += bits[k];
            }
            return sum;
        }

        static uint64_t psum(uint64_t *bits, uint64_t i, PackedBitType bit_type, [[maybe_unused]] uint64_t array_size)
        {
            switch (bit_type)
            {
            case PackedBitType::BIT_1:
                return psum64x1bits(bits, i, array_size);
            case PackedBitType::BIT_2:
                return psum32x2bits(bits, i, array_size);
            case PackedBitType::BIT_4:
                return psum16x4bits(bits, i, array_size);
            case PackedBitType::BIT_8:
                return psum8x8bits(bits, i, array_size);
            case PackedBitType::BIT_16:
                return psum4x16bits(bits, i, array_size);
            case PackedBitType::BIT_32:
                return psum2x32bits(bits, i, array_size);
            case PackedBitType::BIT_64:
                return psum1x64bits(bits, i, array_size);
            default:
                break;
            }
            throw std::invalid_argument("Invalid bit type");
        }
        static uint64_t psum(uint64_t *bits, uint64_t i, uint64_t j, PackedBitType bit_type, [[maybe_unused]] uint64_t array_size)
        {
            switch (bit_type)
            {
            case PackedBitType::BIT_1:
                return psum64x1bits(bits, i, j, array_size);
            case PackedBitType::BIT_2:
                return psum32x2bits(bits, i, j, array_size);
            case PackedBitType::BIT_4:
                return psum16x4bits(bits, i, j, array_size);
            case PackedBitType::BIT_8:
                return psum8x8bits(bits, i, j, array_size);
            case PackedBitType::BIT_16:
                return psum4x16bits(bits, i, j, array_size);
            case PackedBitType::BIT_32:
                return psum2x32bits(bits, i, j, array_size);
            case PackedBitType::BIT_64:
                return psum1x64bits(bits, i, j, array_size);
            default:
                break;
            }
            throw std::invalid_argument("Invalid bit type");
        }

        static inline uint32_t find_prefix_ge_y_2b32_branchless(uint64_t X, uint32_t y)
        {
            const uint64_t M64 = 0x5555555555555555ULL; // ...0101
            const uint32_t M32 = 0x55555555u;
            const uint32_t M16 = 0x5555u;
            const uint32_t M8 = 0x55u;
            const uint32_t M4 = 0x5u;

            // 全体の総和（番兵判定用）
            uint32_t total = (uint32_t)__builtin_popcountll(X & M64) + ((uint32_t)__builtin_popcountll((X >> 1) & M64) << 1);

            uint32_t y0 = y;
            uint32_t idx = 0;
            uint64_t W = X;

            // ---- 32→16（「現在の窓」の“前半”は上位32bit）----
            uint32_t up32 = (uint32_t)(W >> 32);
            uint32_t sum16 = (uint32_t)__builtin_popcount(up32 & M32) + ((uint32_t)__builtin_popcount((up32 >> 1) & M32) << 1);

            uint32_t c1 = (y > sum16); // 0:上位16を選ぶ, 1:下位16へ
            uint32_t m1 = 0u - c1;
            idx += (c1 << 4); // +16
            y -= (sum16 & m1);
            W >>= ((1u - c1) * 32); // 選んだ半分を下位へ寄せる（上位を選んだ時だけ 32 右シフト）

            // ---- 16→8（前半は上位16bit）----
            uint32_t up16 = (uint32_t)((W >> 16) & 0xFFFFu);
            uint32_t sum8 = (uint32_t)__builtin_popcount(up16 & M16) + ((uint32_t)__builtin_popcount((up16 >> 1) & M16) << 1);

            uint32_t c2 = (y > sum8);
            uint32_t m2 = 0u - c2;
            idx += (c2 << 3); // +8
            y -= (sum8 & m2);
            W >>= ((1u - c2) * 16);

            // ---- 8→4（前半は上位8bit）----
            uint32_t up8 = (uint32_t)((W >> 8) & 0xFFu);
            uint32_t sum4 = (uint32_t)__builtin_popcount(up8 & M8) + ((uint32_t)__builtin_popcount((up8 >> 1) & M8) << 1);

            uint32_t c3 = (y > sum4);
            uint32_t m3 = 0u - c3;
            idx += (c3 << 2); // +4
            y -= (sum4 & m3);
            W >>= ((1u - c3) * 8);

            // ---- 4→2（前半は上位4bit）----
            uint32_t up4 = (uint32_t)((W >> 4) & 0xFu);
            uint32_t sum2 = (uint32_t)__builtin_popcount(up4 & M4) + ((uint32_t)__builtin_popcount((up4 >> 1) & M4) << 1);

            uint32_t c4 = (y > sum2);
            uint32_t m4 = 0u - c4;
            idx += (c4 << 1); // +2
            y -= (sum2 & m4);
            W >>= ((1u - c4) * 4);

            // ---- 2→1（前半は上位2bit = 単一要素）----
            uint32_t v_first = (uint32_t)((W >> 2) & 0x3u); // 先頭要素（窓内前半）の値
            uint32_t c5 = (y > v_first);
            uint32_t p = idx + c5;

            // 未達なら 32 を返す（分岐なし）
            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (32u & mf);
        }

        static inline uint32_t find_prefix_ge_y_nib4x16_branchless(uint64_t X, uint32_t y)
        {
            const uint64_t NIB = 0x0F0F0F0F0F0F0F0FULL;

            // ニブル→バイト（各バイト = 2ニブルの和 0..30）
            uint64_t bytes = (X & NIB) + ((X >> 4) & NIB); // b0..b7（LSB側が b0）
            // 8バイト→4×16bit 和
            uint64_t s16 = (bytes & 0x00FF00FF00FF00FFULL) + ((bytes >> 8) & 0x00FF00FF00FF00FFULL); // [b0+b1, b2+b3, b4+b5, b6+b7]
            // 4×16bit→2×32bit 和
            uint64_t s32 = (s16 & 0x0000FFFF0000FFFFULL) + ((s16 >> 16) & 0x0000FFFF0000FFFFULL); // [b0..b3, b4..b7]

            // 全体の総和（0..240）: 8バイトの和。各バイト<=30なので安全に畳める
            uint32_t total = (uint32_t)((bytes * 0x0101010101010101ULL) >> 56);

            uint32_t y0 = y;  // 番兵判定用に保存
            uint32_t idx = 0; // 先頭(MSB)から何ニブル進んだか

            // ── まず「先頭8ニブル(=上位32bit)」の和 ──
            uint32_t sum_front8 = (uint32_t)(s32 >> 32); // b4+b5+b6+b7
            uint32_t c1 = (y > sum_front8);              // 0:前半, 1:後半へ
            uint32_t m1 = 0u - c1;                       // 0 or 0xFFFFFFFF
            idx += (c1 << 3);                            // +8
            y -= (sum_front8 & m1);

            // ── 次に、その半分内の「先頭4ニブル」の和 ──
            // s16 の中で、前半を選んだときは上位16bit(b6+b7)、後半なら 16..31bit(b2+b3)
            // shift = 48 (c1=0) または 16 (c1=1) を分岐レスに計算
            uint32_t shift16 = 16u + ((1u - c1) << 5); // 16 + (1-c1)*32
            uint32_t sum_front4 = (uint32_t)((s16 >> shift16) & 0xFFFFu);
            uint32_t c2 = (y > sum_front4);
            uint32_t m2 = 0u - c2;
            idx += (c2 << 2); // +4
            y -= (sum_front4 & m2);

            // ── さらに、その4ニブル内の「先頭2ニブル(=1バイト)」の和 ──
            // 先頭から idx ニブル進んだ位置の“先頭バイト”は bytes の
            // byte_index = 7 - (idx/2) （LSB側 index）
            uint32_t byte_idx_from_lsb = 7u - (idx >> 1);
            uint32_t firstByteSum = (uint32_t)((bytes >> (byte_idx_from_lsb * 8u)) & 0xFFu);
            uint32_t c3 = (y > firstByteSum);
            uint32_t m3 = 0u - c3;
            idx += (c3 << 1); // +2
            y -= (firstByteSum & m3);

            // ── 最後に 1 ニブル ──
            // 上位が先頭なので、ニブル idx のビット位置は 4*(15 - idx)
            uint32_t nib_shift = (15u - idx) << 2;
            uint32_t loNib = (uint32_t)((X >> nib_shift) & 0xFu);
            uint32_t c4 = (y > loNib); // 0/1
            uint32_t p = idx + c4;

            // 総和<y なら 16 を返す（分岐レス）
            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (16u & mf);
        }
        static inline uint32_t find_prefix_ge_y_nib8x8_branchless(uint64_t X, uint32_t y)
        {
            // 8B → 4×16bit 和
            uint64_t s16 = (X & 0x00FF00FF00FF00FFULL) + ((X >> 8) & 0x00FF00FF00FF00FFULL); // [b7+b6, b5+b4, b3+b2, b1+b0] (LSB→MSB)

            // 4×16bit → 2×32bit 和
            uint64_t s32 = (s16 & 0x0000FFFF0000FFFFULL) + ((s16 >> 16) & 0x0000FFFF0000FFFFULL); // low32: b7..b4, high32: b3..b0

            // 合計（0..2040）
            uint32_t sum_back4 = (uint32_t)(s32);        // b4..b7
            uint32_t sum_front4 = (uint32_t)(s32 >> 32); // b0..b3
            uint32_t total = sum_front4 + sum_back4;

            uint32_t y0 = y;  // 番兵判定用
            uint32_t idx = 0; // 先頭(MSB)から進めた要素数

            // ── まず 4 要素塊（前半=MSB側 b0..b3）の判定 ──
            uint32_t c1 = (y > sum_front4); // 0: 前半, 1: 後半へ
            uint32_t m1 = 0u - c1;          // 0 or 0xFFFFFFFF
            idx += (c1 << 2);               // +4
            y -= (sum_front4 & m1);

            // ── 選んだ 4 要素内で 2 要素塊（最初の2要素）の和 ──
            // s16 の 16bit レーン: [b7+b6 | b5+b4 | b3+b2 | b1+b0] (下位→上位)
            // 前半を選んだ場合(=c1=0)は上位側の [b1+b0] を下位に、次が [b3+b2]
            // 後半を選んだ場合(=c1=1)は中位の [b5+b4] が下位、その次が [b7+b6]
            uint32_t shift16 = 16u + ((1u - c1) << 5); // c1=0→48, c1=1→16
            uint32_t sum_front2 = (uint32_t)((s16 >> shift16) & 0xFFFFu);

            uint32_t c2 = (y > sum_front2); // 0: 先頭ペア, 1: 後ろのペアへ
            uint32_t m2 = 0u - c2;
            idx += (c2 << 1); // +2
            y -= (sum_front2 & m2);

            // ── 選んだ 2 要素のうち、最初の 1 要素の値 ──
            // 先頭(MSB)から idx 個進んだ位置のバイトを取り出す
            uint32_t byte_idx_from_lsb = 7u - idx; // LSB基準インデックス
            uint32_t firstByte = (uint32_t)((X >> (byte_idx_from_lsb * 8u)) & 0xFFu);

            uint32_t c3 = (y > firstByte); // 0: そこで到達, 1: 次の1要素へ
            uint32_t p = idx + c3;

            // ── 未達なら 8 を返す（分岐レス）──
            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (8u & mf);
        }

        static inline uint32_t find_prefix_ge_y_nib16x4_branchless(uint64_t X, uint32_t y)
        {
            // 要素抽出（MSB→LSB）
            uint32_t e0 = (uint32_t)((X >> 48) & 0xFFFFu);
            uint32_t e1 = (uint32_t)((X >> 32) & 0xFFFFu);
            uint32_t e2 = (uint32_t)((X >> 16) & 0xFFFFu);
            uint32_t e3 = (uint32_t)(X & 0xFFFFu);

            // 累積和（32bitで安全：最大 262140）
            uint32_t s0 = e0;
            uint32_t s1 = s0 + e1;
            uint32_t s2 = s1 + e2;
            uint32_t s3 = s2 + e3;

            // 「s_i >= y」をビットに詰める（bit i が立つ）
            uint32_t m = ((s0 >= y) ? 1u : 0u) | ((s1 >= y) ? 2u : 0u) | ((s2 >= y) ? 4u : 0u) | ((s3 >= y) ? 8u : 0u);

            // 未達（m==0）のときは bit4 を立てて ctz=4 にする（分岐なしで番兵）
            m |= ((m == 0u) ? 16u : 0u);

            // 最下位1bitの位置 = 最小 p（m!=0 が保証されている）
            return (uint32_t)__builtin_ctz(m);
        }
        static int64_t search64x1bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            sum = 0;
            uint64_t k = 0;
            uint64_t v = stool::Byte::count_bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = stool::Byte::count_bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = stool::MSBByte::select1(bits[k], diff - 1);
            sum = i - 1;
            return (k * 64) + f;
        }
        static int64_t search32x2bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum32x2bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum32x2bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_2b32_branchless(bits[k], diff);
            return (k * 32) + f;
        }
        static int64_t search16x4bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum16x4bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum16x4bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib4x16_branchless(bits[k], diff);
            return (k * 16) + f;
        }

        static int64_t search8x8bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum8x8bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum8x8bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib8x8_branchless(bits[k], diff);
            return (k * 8) + f;
        }

        static int64_t search4x16bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum4x16bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum4x16bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib16x4_branchless(bits[k], diff);
            return (k * 4) + f;
        }
        static int64_t search2x32bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum2x32bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum2x32bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t L = bits[k] >> 32;
            uint64_t R = bits[k] & 0xFFFFFFFF;
            if (sum + L >= i)
            {
                return (k * 2);
            }
            else
            {
                return (k * 2) + 1;
            }
        }
        static int64_t search1x64bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = bits[k];

            while (sum + v < i)
            {
                sum += v;
                k++;
                v = bits[k];
            }
            return k;
        }

        static int64_t search(uint64_t *bits, uint64_t i, PackedBitType bit_type, uint64_t total_sum, [[maybe_unused]] uint64_t array_size)
        {
            if (i > total_sum)
            {
                return -1;
            }
            switch (bit_type)
            {
            case PackedBitType::BIT_1:
                return search64x1bits(bits, i, array_size);
            case PackedBitType::BIT_2:
                return search32x2bits(bits, i, array_size);
            case PackedBitType::BIT_4:
                return search16x4bits(bits, i, array_size);
            case PackedBitType::BIT_8:
                return search8x8bits(bits, i, array_size);
            case PackedBitType::BIT_16:
                return search4x16bits(bits, i, array_size);
            case PackedBitType::BIT_32:
                return search2x32bits(bits, i, array_size);
            case PackedBitType::BIT_64:
                return search1x64bits(bits, i, array_size);
            default:
                break;
            }
            throw std::invalid_argument("Invalid bit type");
        }
    };
}
