#pragma once
#include "./packed_psum.hpp"


namespace stool
{

    /*!
     * @brief A class for managing packed partial sums with variable bit widths [Unchecked AI's Comment]
     * 
     * This class provides functionality to store and process sequences of integers using bit packing techniques.
     * It supports different bit widths (1,2,4,8,16,32,64) to efficiently store values based on their magnitude.
     * The class includes methods for:
     * - Determining optimal bit width for values
     * - Packing/unpacking values
     * - Computing partial sums
     * - Searching within packed sequences
     */
    class PackedSearch
    {
    public:
        /**
         * @brief Enumeration of supported bit widths for packed storage [Unchecked AI's Comment]
         * 
         * Defines the available bit widths for packing integer values.
         * Each type corresponds to a specific number of bits used per value.
         */
        enum class PackedBitType
        {
            BIT_1 = 0,   ///< 1-bit per value (values 0-1)
            BIT_2 = 1,   ///< 2-bits per value (values 0-3)
            BIT_4 = 2,   ///< 4-bits per value (values 0-15)
            BIT_8 = 3,   ///< 8-bits per value (values 0-255)
            BIT_16 = 4,  ///< 16-bits per value (values 0-65535)
            BIT_32 = 5,  ///< 32-bits per value (values 0-4294967295)
            BIT_64 = 6   ///< 64-bits per value (full uint64_t range)
        };

        /**
         * @brief Finds the first position where prefix sum >= y for 2-bit values (branchless) [Unchecked AI's Comment]
         * 
         * Efficiently searches for the first position in a 64-bit word containing
         * thirty-two 2-bit values where the prefix sum reaches or exceeds the target value y.
         * Uses branchless bit manipulation for optimal performance.
         * 
         * @param X 64-bit word containing 32 packed 2-bit values
         * @param y Target sum value to search for
         * @return uint32_t Position where prefix sum >= y, or 32 if not found
         * 
         * @note Returns 32 as sentinel value if target sum exceeds total sum
         */
        static inline uint32_t find_prefix_ge_y_2b32_branchless(uint64_t X, uint32_t y)
        {
            const uint64_t M64 = 0x5555555555555555ULL; // ...0101
            const uint32_t M32 = 0x55555555u;
            const uint32_t M16 = 0x5555u;
            const uint32_t M8 = 0x55u;
            const uint32_t M4 = 0x5u;

            uint32_t total = (uint32_t)__builtin_popcountll(X & M64) + ((uint32_t)__builtin_popcountll((X >> 1) & M64) << 1);

            uint32_t y0 = y;
            uint32_t idx = 0;
            uint64_t W = X;

            uint32_t up32 = (uint32_t)(W >> 32);
            uint32_t sum16 = (uint32_t)__builtin_popcount(up32 & M32) + ((uint32_t)__builtin_popcount((up32 >> 1) & M32) << 1);

            uint32_t c1 = (y > sum16); 
            uint32_t m1 = 0u - c1;
            idx += (c1 << 4); // +16
            y -= (sum16 & m1);
            W >>= ((1u - c1) * 32);

            uint32_t up16 = (uint32_t)((W >> 16) & 0xFFFFu);
            uint32_t sum8 = (uint32_t)__builtin_popcount(up16 & M16) + ((uint32_t)__builtin_popcount((up16 >> 1) & M16) << 1);

            uint32_t c2 = (y > sum8);
            uint32_t m2 = 0u - c2;
            idx += (c2 << 3); // +8
            y -= (sum8 & m2);
            W >>= ((1u - c2) * 16);

            uint32_t up8 = (uint32_t)((W >> 8) & 0xFFu);
            uint32_t sum4 = (uint32_t)__builtin_popcount(up8 & M8) + ((uint32_t)__builtin_popcount((up8 >> 1) & M8) << 1);

            uint32_t c3 = (y > sum4);
            uint32_t m3 = 0u - c3;
            idx += (c3 << 2); // +4
            y -= (sum4 & m3);
            W >>= ((1u - c3) * 8);

            uint32_t up4 = (uint32_t)((W >> 4) & 0xFu);
            uint32_t sum2 = (uint32_t)__builtin_popcount(up4 & M4) + ((uint32_t)__builtin_popcount((up4 >> 1) & M4) << 1);

            uint32_t c4 = (y > sum2);
            uint32_t m4 = 0u - c4;
            idx += (c4 << 1); // +2
            y -= (sum2 & m4);
            W >>= ((1u - c4) * 4);

            uint32_t v_first = (uint32_t)((W >> 2) & 0x3u); 
            uint32_t c5 = (y > v_first);
            uint32_t p = idx + c5;

            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (32u & mf);
        }

        /**
         * @brief Finds the first position where prefix sum >= y for 4-bit values (branchless) [Unchecked AI's Comment]
         * 
         * Efficiently searches for the first position in a 64-bit word containing
         * sixteen 4-bit values where the prefix sum reaches or exceeds the target value y.
         * Uses branchless bit manipulation for optimal performance.
         * 
         * @param X 64-bit word containing 16 packed 4-bit values
         * @param y Target sum value to search for
         * @return uint32_t Position where prefix sum >= y, or 16 if not found
         * 
         * @note Returns 16 as sentinel value if target sum exceeds total sum
         */
        static inline uint32_t find_prefix_ge_y_nib4x16_branchless(uint64_t X, uint32_t y)
        {
            const uint64_t NIB = 0x0F0F0F0F0F0F0F0FULL;

            uint64_t bytes = (X & NIB) + ((X >> 4) & NIB); // b0..b7
            uint64_t s16 = (bytes & 0x00FF00FF00FF00FFULL) + ((bytes >> 8) & 0x00FF00FF00FF00FFULL); // [b0+b1, b2+b3, b4+b5, b6+b7]
            uint64_t s32 = (s16 & 0x0000FFFF0000FFFFULL) + ((s16 >> 16) & 0x0000FFFF0000FFFFULL); // [b0..b3, b4..b7]

            uint32_t total = (uint32_t)((bytes * 0x0101010101010101ULL) >> 56);

            uint32_t y0 = y;  // 番兵判定用に保存
            uint32_t idx = 0; // 先頭(MSB)から何ニブル進んだか

            uint32_t sum_front8 = (uint32_t)(s32 >> 32); // b4+b5+b6+b7
            uint32_t c1 = (y > sum_front8);              // 0:前半, 1:後半へ
            uint32_t m1 = 0u - c1;                       // 0 or 0xFFFFFFFF
            idx += (c1 << 3);                            // +8
            y -= (sum_front8 & m1);

            uint32_t shift16 = 16u + ((1u - c1) << 5); // 16 + (1-c1)*32
            uint32_t sum_front4 = (uint32_t)((s16 >> shift16) & 0xFFFFu);
            uint32_t c2 = (y > sum_front4);
            uint32_t m2 = 0u - c2;
            idx += (c2 << 2); // +4
            y -= (sum_front4 & m2);

            uint32_t byte_idx_from_lsb = 7u - (idx >> 1);
            uint32_t firstByteSum = (uint32_t)((bytes >> (byte_idx_from_lsb * 8u)) & 0xFFu);
            uint32_t c3 = (y > firstByteSum);
            uint32_t m3 = 0u - c3;
            idx += (c3 << 1); // +2
            y -= (firstByteSum & m3);

            uint32_t nib_shift = (15u - idx) << 2;
            uint32_t loNib = (uint32_t)((X >> nib_shift) & 0xFu);
            uint32_t c4 = (y > loNib); // 0/1
            uint32_t p = idx + c4;

            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (16u & mf);
        }
        /**
         * @brief Finds the first position where prefix sum >= y for 8-bit values (branchless) [Unchecked AI's Comment]
         * 
         * Efficiently searches for the first position in a 64-bit word containing
         * eight 8-bit values where the prefix sum reaches or exceeds the target value y.
         * Uses branchless bit manipulation for optimal performance.
         * 
         * @param X 64-bit word containing 8 packed 8-bit values
         * @param y Target sum value to search for
         * @return uint32_t Position where prefix sum >= y, or 8 if not found
         * 
         * @note Returns 8 as sentinel value if target sum exceeds total sum
         */
        static inline uint32_t find_prefix_ge_y_nib8x8_branchless(uint64_t X, uint32_t y)
        {
            uint64_t s16 = (X & 0x00FF00FF00FF00FFULL) + ((X >> 8) & 0x00FF00FF00FF00FFULL); // [b7+b6, b5+b4, b3+b2, b1+b0] (LSB→MSB)

            uint64_t s32 = (s16 & 0x0000FFFF0000FFFFULL) + ((s16 >> 16) & 0x0000FFFF0000FFFFULL); // low32: b7..b4, high32: b3..b0

            uint32_t sum_back4 = (uint32_t)(s32);        // b4..b7
            uint32_t sum_front4 = (uint32_t)(s32 >> 32); // b0..b3
            uint32_t total = sum_front4 + sum_back4;

            uint32_t y0 = y;
            uint32_t idx = 0;

            uint32_t c1 = (y > sum_front4); // 0: 前半, 1: 後半へ
            uint32_t m1 = 0u - c1;          // 0 or 0xFFFFFFFF
            idx += (c1 << 2);               // +4
            y -= (sum_front4 & m1);

            uint32_t shift16 = 16u + ((1u - c1) << 5); // c1=0→48, c1=1→16
            uint32_t sum_front2 = (uint32_t)((s16 >> shift16) & 0xFFFFu);

            uint32_t c2 = (y > sum_front2); // 0: 先頭ペア, 1: 後ろのペアへ
            uint32_t m2 = 0u - c2;
            idx += (c2 << 1); // +2
            y -= (sum_front2 & m2);

            uint32_t byte_idx_from_lsb = 7u - idx; // LSB基準インデックス
            uint32_t firstByte = (uint32_t)((X >> (byte_idx_from_lsb * 8u)) & 0xFFu);

            uint32_t c3 = (y > firstByte); // 0: そこで到達, 1: 次の1要素へ
            uint32_t p = idx + c3;

            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (8u & mf);
        }

        /**
         * @brief Finds the first position where prefix sum >= y for 16-bit values (branchless) [Unchecked AI's Comment]
         * 
         * Efficiently searches for the first position in a 64-bit word containing
         * four 16-bit values where the prefix sum reaches or exceeds the target value y.
         * Uses branchless bit manipulation for optimal performance.
         * 
         * @param X 64-bit word containing 4 packed 16-bit values
         * @param y Target sum value to search for
         * @return uint32_t Position where prefix sum >= y, or 4 if not found
         * 
         * @note Returns 4 as sentinel value if target sum exceeds total sum
         */
        static inline uint32_t find_prefix_ge_y_nib16x4_branchless(uint64_t X, uint32_t y)
        {
            uint32_t e0 = (uint32_t)((X >> 48) & 0xFFFFu);
            uint32_t e1 = (uint32_t)((X >> 32) & 0xFFFFu);
            uint32_t e2 = (uint32_t)((X >> 16) & 0xFFFFu);
            uint32_t e3 = (uint32_t)(X & 0xFFFFu);

            uint32_t s0 = e0;
            uint32_t s1 = s0 + e1;
            uint32_t s2 = s1 + e2;
            uint32_t s3 = s2 + e3;

            uint32_t m = ((s0 >= y) ? 1u : 0u) | ((s1 >= y) ? 2u : 0u) | ((s2 >= y) ? 4u : 0u) | ((s3 >= y) ? 8u : 0u);

            m |= ((m == 0u) ? 16u : 0u);

            return (uint32_t)__builtin_ctz(m);
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 1-bit values [Unchecked AI's Comment]
         * 
         * Finds the position in a packed 1-bit array where the cumulative sum
         * first reaches or exceeds the target value i. This is equivalent to
         * finding the i-th set bit.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 1-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Bit position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, otherwise searches for the i-th occurrence
         */
        static int64_t search64x1bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            sum = 0;
            uint64_t k = 0;
            uint64_t v = stool::Byte::popcount(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = stool::Byte::popcount(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = stool::MSBByte::select1(bits[k], diff - 1);
            sum = i - 1;
            return (k * 64) + f;
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 2-bit values [Unchecked AI's Comment]
         * 
         * Finds the position in a packed 2-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 2-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 2-bit value indices
         */
        static int64_t search32x2bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = PackedPSum::sum32x2bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = PackedPSum::sum32x2bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_2b32_branchless(bits[k], diff);
            return (k * 32) + f;
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 4-bit values [Unchecked AI's Comment]
         * 
         * Finds the position in a packed 4-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 4-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 4-bit value indices
         */
        static int64_t search16x4bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = PackedPSum::sum16x4bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = PackedPSum::sum16x4bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib4x16_branchless(bits[k], diff);
            return (k * 16) + f;
        }

        /**
         * @brief Searches for the position where cumulative sum reaches target for 8-bit values [Unchecked AI's Comment]
         * 
         * Finds the position in a packed 8-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 8-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 8-bit value indices
         */
        static int64_t search8x8bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = PackedPSum::sum8x8bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = PackedPSum::sum8x8bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib8x8_branchless(bits[k], diff);
            return (k * 8) + f;
        }

        /**
         * @brief Searches for the position where cumulative sum reaches target for 16-bit values [Unchecked AI's Comment]
         * 
         * Finds the position in a packed 16-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 16-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 16-bit value indices
         */
        static int64_t search4x16bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = PackedPSum::sum4x16bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = PackedPSum::sum4x16bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib16x4_branchless(bits[k], diff);
            return (k * 4) + f;
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 32-bit values [Unchecked AI's Comment]
         * 
         * Finds the position in a packed 32-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 32-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 32-bit value indices
         */
        static int64_t search2x32bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = PackedPSum::sum2x32bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = PackedPSum::sum2x32bits(bits[k]);
            }

            // uint64_t diff = i - sum;
            uint64_t L = bits[k] >> 32;
            // uint64_t R = bits[k] & 0xFFFFFFFF;
            if (sum + L >= i)
            {
                return (k * 2);
            }
            else
            {
                return (k * 2) + 1;
            }
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 64-bit values [Unchecked AI's Comment]
         * 
         * Finds the position in a 64-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Array index where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is the array index
         */
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

        /**
         * @brief Generic search function for finding cumulative sum position [Unchecked AI's Comment]
         * 
         * Searches for the position where the cumulative sum first reaches or
         * exceeds the target value i, based on the specified bit type.
         * This function dispatches to the appropriate specialized search function.
         * 
         * @param bits Pointer to array of packed values
         * @param i Target cumulative sum value
         * @param bit_type The bit width type of packed values
         * @param total_sum Total sum of all values (for bounds checking)
         * @param array_size Size of the bits array
         * @return int64_t Position where cumulative sum reaches i, or -1 if not found
         * 
         * @throws std::invalid_argument If bit_type is invalid
         * @note Returns -1 if i > total_sum
         */
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
