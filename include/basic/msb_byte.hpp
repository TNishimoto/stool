#pragma once
#include "./lsb_byte.hpp"
#include <cassert>
#include <cstring>

namespace stool
{
    namespace __MSB_BYTE
    {
        inline static constexpr int8_t get_1bit_position(uint8_t value, int rank)
        {
            int count = 0;
            for (int bit = 0; bit < 8; ++bit)
            {
                if (value & (1 << (7 - bit)))
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

        inline static constexpr std::array<std::array<int8_t, 8>, 256> select1_table = stool::__MSB_BYTE::build_lookup_table_for_select1();
    }

    /*!
     * @brief A class for handling most significant bit (MSB) operations on bytes [in progress]
     * @note This implementation includes optimized lookup tables and bit manipulation techniques
     *       for improved performance on MSB-oriented byte operations
     */

    class MSBByte
    {

    public:
        /*!
         * @brief Counts the number of 1 bits in B[0..i] for a given bit sequence B.
         */
        inline static int64_t popcount(uint64_t B, uint64_t i)
        {
            return __builtin_popcountll(B >> (63 - i));
        }

        /*!
         * @brief Returns the i-th bit of bit sequence B[0..].
         */
        inline static bool get_bit(uint64_t B, int64_t i)
        {
            return ((B >> (63 - i)) & 0b1) == 1;
        }

        /*!
         * @brief Replaces the i-th bit of bit sequence B[0..63] with b and returns the new B.
         */
        static uint64_t write_bit(uint64_t B, int64_t i, bool b)
        {
            return LSBByte::write_bit(B, 63 - i, b);
        }

        /*!
         * @brief Replaces the last len bits of 64 bits B[0..63] with the first len bits of 64 bits Q[0..63] and returns the new B.
         */
        static uint64_t write_suffix(uint64_t B, uint8_t len, uint64_t Q)
        {
            assert(len <= 64 && len > 0);
            uint64_t maskR = (UINT64_MAX >> (64 - len));
            uint64_t maskL = ~maskR;
            return (B & maskL) | ((Q >> (64 - len)) & maskR);
        }

        /*!
         * @brief Replaces the first len bits of 64 bits B[0..63] with the first len bits of 64 bits Q[0..63] and returns the new B.
         */
        static uint64_t write_prefix(uint64_t B, uint8_t len, uint64_t Q)
        {
            assert(len <= 64 && len > 0);
            uint64_t maskL = (UINT64_MAX << (64 - len));
            uint64_t maskR = ~maskL;
            return (B & maskR) | (Q & maskL);
        }

        /*!
         * @brief Replaces B[pos..pos+len-1] of 64 bits B[0..63] with the first len bits of 64 bits Q[0..63] and returns the new B.
         */
        static uint64_t write_bits(uint64_t B, uint8_t pos, uint8_t len, uint64_t Q)
        {
            assert(pos + len <= 64);
            uint64_t maskA = (UINT64_MAX >> pos) & (UINT64_MAX << (64 - len - pos));
            uint64_t maskB = ~maskA;
            return (B & maskB) | ((Q >> pos) & maskA);
        }

        /*!
         * @brief Shifts the suffix B[pos..63] of 64 bits B[0..63] to the right by len bits and returns the new B, i.e., B[0..pos-1] | 0^{len} | B[pos..63 - len].
         */
        static uint64_t shift_right(uint64_t B, uint8_t pos, uint8_t len)
        {
            assert(len > 0 && len <= 64);

            if (pos + len >= 64)
            {
                uint64_t mask = ~(UINT64_MAX >> pos);
                return B & mask;
            }
            else
            {
                if (pos > 0)
                {
                    uint64_t tmp = B >> len;

                    uint64_t maskL = UINT64_MAX << (64 - pos);
                    uint64_t maskR = UINT64_MAX >> (pos + len);
                    return (B & maskL) | (tmp & maskR);
                }
                else
                {
                    return B >> len;
                }
            }
        }

        /*!
         * @brief Shifts the suffix B[pos..63] of 64 bits B[0..63] to the left by len bits and returns the new B, i.e., B[0..pos-1-len] | B[pos..63] | 0^{len}.
         */
        static uint64_t shift_left(uint64_t B, uint8_t pos, uint8_t len)
        {
            assert(len > 0 && len <= 64);

            if (len >= pos || pos == 0)
            {
                return B << len;
            }
            else
            {
                uint64_t strL = B;
                uint64_t strR = B << len;
                uint64_t maskL = UINT64_MAX << (64 - (pos - len));
                uint64_t maskR = UINT64_MAX >> (pos - len);

                return (strL & maskL) | (strR & maskR);
            }
        }

        /*!
         * @brief Replaces the i-th bit of 64-bit vector B with b 
         */
        static void replace(std::vector<uint64_t> &B, uint64_t i, bool b)
        {
            uint64_t block_index = i / 64;
            uint64_t bit_index = i % 64;
            uint64_t block = B[block_index];
            uint64_t new_block = stool::MSBByte::write_bit(block, bit_index, b);
            B[block_index] = new_block;
        }

        /*
        uint64_t get_LSB(uint64_t B) const
        {
            if (B != 0)
            {
                // uint64_t count = stool::Byte::popcount(bits);
                uint64_t sizex = 64 - stool::LSBByte::select1(B, 0);
                return sizex - 1;
            }
            else
            {
                throw std::runtime_error("bits is 0");
            }
        }
        */

        
        /*!
         * @brief Replaces the bits B[I..I+len-1] in 64-bit sequence B with the first len bits of 64 bits Q[0..63], where I = block_index * 64 + bit_index.
         * @param array_size the length of the 64-bit sequence B, i.e., the number of 64-bit blocks in B.
         */
        template <typename BIT64_SEQUENCE>
        static void write_bits(BIT64_SEQUENCE &B, uint64_t Q, uint64_t len, uint16_t block_index, uint8_t bit_index, [[maybe_unused]] uint64_t array_size)
        {
            if (bit_index + len <= 64)
            {
                B[block_index] = stool::MSBByte::write_bits(B[block_index], bit_index, len, Q);
            }
            else
            {
                uint64_t Lvalue = Q;
                uint64_t Rvalue = Q << (64 - bit_index);

                uint64_t diff = 64 - bit_index;

                B[block_index] = stool::MSBByte::write_bits(B[block_index], bit_index, diff, Lvalue);

                assert(block_index + 1 < array_size);
                B[block_index + 1] = stool::MSBByte::write_bits(B[block_index + 1], 0, len - diff, Rvalue);
            }
        }

        /*!
         * @brief Fills the bits B[pos..pos+len-1] of 64-bit B with b and returns the new B.
         */
        static uint64_t fill(uint64_t B, uint64_t pos, uint64_t len, bool b)
        {
            uint64_t end_pos = pos + len - 1;

            if (b)
            {
                uint64_t mask1 = (UINT64_MAX >> (63 - end_pos)) << (63 - end_pos);
                uint64_t mask2 = (mask1 << pos) >> pos;
                return B | mask2;
            }
            else
            {
                if (pos > 0)
                {
                    uint64_t pref = UINT64_MAX << (64 - pos);

                    if (end_pos >= 63)
                    {
                        uint64_t mask = pref;
                        return B & mask;
                    }
                    else
                    {
                        uint64_t suf = UINT64_MAX >> (end_pos + 1);
                        uint64_t mask = pref | suf;
                        return B & mask;
                    }
                }
                else
                {
                    if (end_pos >= 63)
                    {
                        return 0ULL;
                    }
                    else
                    {
                        uint64_t mask = UINT64_MAX >> (end_pos + 1);
                        return B & mask;
                    }
                }
            }
        }


        /*!
         * @brief Fills the bits B[pos..pos+len-1] of 64-bit integer sequence B with b and returns the new B.
         */
        template <typename BIT64_SEQUENCE>
        static void fill(BIT64_SEQUENCE &B, uint64_t pos, uint64_t len, bool b)
        {
            uint64_t current_block_index = pos / 64;
            uint64_t current_bit_index = pos % 64;

            uint64_t counter = len;
            while (counter > 0)
            {
                if (current_bit_index == 0)
                {
                    if (counter >= 64)
                    {
                        B[current_block_index] = b ? UINT64_MAX : 0ULL;
                        current_block_index++;
                        counter -= 64;
                    }
                    else
                    {

                        B[current_block_index] = fill(B[current_block_index], current_bit_index, counter, b);
                        current_bit_index += counter;
                        counter = 0;
                    }
                }
                else
                {
                    if (current_bit_index + counter >= 64)
                    {

                        uint64_t mask_size = 64 - current_bit_index;

                        B[current_block_index] = fill(B[current_block_index], current_bit_index, mask_size, b);
                        current_block_index++;
                        current_bit_index = 0;
                        counter -= mask_size;
                    }
                    else
                    {

                        B[current_block_index] = fill(B[current_block_index], current_bit_index, counter, b);
                        current_bit_index += counter;
                        counter = 0;
                    }
                }
            }
        }

        /*
        static int64_t old_select1(uint64_t B, uint64_t i)
        {
            uint64_t nth = i + 1;
            uint64_t mask = UINT8_MAX;
            uint64_t counter = 0;
            for (int64_t i = 7; i >= 0; i--)
            {
                uint64_t bs = i * 8;
                uint64_t mask2 = mask << bs;
                uint64_t v = B & mask2;
                uint64_t c = Byte::popcount(v);
                if (counter + c >= nth)
                {
                    uint64_t pos = (7 - i) * 8;
                    uint8_t B_8 = (B >> (56 - pos)) & UINT8_MAX;
                    uint64_t X = pos + __MSB_BYTE::select1_table[B_8][(nth - counter - 1)];
                    return X;
                }
                counter += c;
            }
            return -1;
        }
        */

        /*!
         * @brief Returns the position of the i-th 1 in 64-bit B if such bit exists; otherwise return -1.
         */
        static int64_t select_ith_1(uint64_t B, uint64_t i)
        {
            uint64_t num = Byte::popcount(B);
            if (num < i + 1)
            {
                return -1;
            }
            else
            {
                uint64_t j = num - (i + 1);
                return 63 - LSBByte::select1(B, j);
            }
        }

        /*!
         * @brief Returns the position of the first 1 in 64-bit B if such bit exists; otherwise return -1.
         */
        static int64_t select1(uint64_t B)
        {
            if (B != 0)
            {
                return __builtin_clzll(B);
            }
            else
            {
                return -1;
            }
        }


        /*!
         * @brief Returns the position of the (i+1)-th 1 in 8-bit B if such bit exists; otherwise return -1.
         */
        static int64_t select1_for_8bits(uint8_t B, uint8_t i)
        {
            return __MSB_BYTE::select1_table[B][i];
        }

        /*!
         * @brief Returns the position of the i-th 0 in 64-bit B if such bit exists; otherwise return -1.
         */
        static int64_t select_ith_0(uint64_t B, uint64_t i)
        {
            return select_ith_1(~B, i);
        }

        /*!
         * @brief Returns the bits B[I..I+63] of 64-bit sequence B as a 64-bit integer, where I = block_index * 64 + bit_index.
         * @param array_size the length of the 64-bit sequence B, i.e., the number of 64-bit blocks in B.
         */
        template <typename BIT64_SEQUENCE>
        static uint64_t access_64bits(BIT64_SEQUENCE &B, uint64_t block_index, uint64_t bit_index, uint64_t array_size)
        {
            if (bit_index == 0)
            {
                return B[block_index];
            }
            else if (block_index + 1 < array_size)
            {

                uint64_t L = B[block_index] << bit_index;
                uint64_t R = B[block_index + 1] >> (64 - bit_index);
                return L | R;
            }
            else
            {
                uint64_t L = B[block_index] << bit_index;
                return L;
            }
        }

        /*!
         * @brief Returns the bits B[i..i+len-1] of 64-bit sequence B as a 64-bit integer, i.e., B[i..i+len-1] | 0^{64-len}.
         */
        static uint64_t access_bits(uint64_t B, uint8_t i, uint8_t len)
        {
            uint64_t mask = UINT64_MAX << (64 - len);
            uint64_t value = (B << i) & mask;
            return value;
        }



        /*!
         * @brief Counts the number of 1 bits in the bits B[I..J] of 64-bit sequence B, where I = start_block_index * 64 + start_bit_index and J = end_block_index * 64 + end_bit_index.
         * @param array_size the length of the 64-bit sequence B, i.e., the number of 64-bit blocks in B.
         */
        template <typename BIT64_SEQUENCE>
        static uint64_t rank1(BIT64_SEQUENCE &B, uint64_t start_block_index, uint8_t start_bit_index, uint64_t end_block_index, uint8_t end_bit_index, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t num = 0;

            assert(start_block_index <= end_block_index);

            if (start_block_index == end_block_index)
            {
                assert(start_bit_index <= end_bit_index);
                uint64_t block = B[start_block_index];
                uint64_t R = stool::MSBByte::popcount(block, end_bit_index);

                if (start_bit_index != 0)
                {
                    uint64_t L = stool::MSBByte::popcount(block, start_bit_index - 1);
                    num += R - L;
                }
                else
                {
                    num += R;
                }
            }
            else
            {

                if (start_bit_index != 0)
                {
                    uint64_t block = B[start_block_index];
                    uint64_t R = start_block_index != end_block_index ? stool::Byte::popcount(block) : stool::MSBByte::popcount(block, end_bit_index);
                    uint64_t L = stool::MSBByte::popcount(block, start_bit_index - 1);
                    num += R - L;
                }
                else
                {
                    num += stool::Byte::popcount(B[start_block_index]);
                }

                for (uint64_t j = start_block_index + 1; j < end_block_index; j++)
                {
                    num += stool::Byte::popcount(B[j]);
                }

                {
                    num += stool::MSBByte::popcount(B[end_block_index], end_bit_index);
                }
            }
            return num;
        }

        

        /*!
         * @brief Returns the first k bits of 64-bit B as a binary string
         */
        static std::string to_bit_string(uint64_t B, uint64_t k)
        {
            std::string s = Byte::to_bit_string(B);
            while (s.size() > k)
            {
                s.pop_back();
            }
            return s;
        }

        

        /*!
         * @brief Shifts the suffix B[pos..] of 64 bit sequence B[0..] to the right by len bits, i.e., B is changed to B[0..pos-1] | 0^{len} | B[pos..63 - len].
         * @param array_size the length of the 64-bit sequence B, i.e., the number of 64-bit blocks in B.
         */
        static void shift_right(uint64_t *B, uint64_t pos, uint64_t len, uint64_t array_size)
        {
            uint64_t block_index = pos / 64;
            uint64_t bit_index = pos % 64;

            uint64_t dst_block_index = (pos + len) / 64;
            uint64_t dst_bit_index = (pos + len) % 64;

            uint64_t curry_bitsize = 0;
            uint64_t mid_block_index = 0;

            if (dst_block_index >= array_size)
            {
                throw std::runtime_error("shift_rightX: dst_block_index >= array_size");
                return;
            }
            else if (bit_index <= dst_bit_index)
            {
                curry_bitsize = dst_bit_index - bit_index;
                mid_block_index = dst_block_index;
                // uint64_t shift_block_size = dst_block_index - block_index;
                uint64_t suffix_block_size = array_size - dst_block_index;
                std::memmove(&B[dst_block_index], &B[block_index], suffix_block_size * sizeof(uint64_t));
            }
            else
            {

                mid_block_index = dst_block_index - 1;
                curry_bitsize = dst_bit_index + 64 - bit_index;
                // uint64_t shift_block_size = (dst_block_index - 1) - block_index;
                uint64_t suffix_block_size = array_size - (dst_block_index - 1);
                std::memmove(&B[dst_block_index - 1], &B[block_index], suffix_block_size * sizeof(uint64_t));
            }

            if (curry_bitsize > 0)
            {
                assert(curry_bitsize < 64);

                uint64_t curried_bits = B[mid_block_index];
                {
                    B[mid_block_index] = stool::MSBByte::shift_right(B[mid_block_index], bit_index, curry_bitsize);
                }

                for (uint64_t j = mid_block_index + 1; j < array_size; j++)
                {
                    uint64_t L = curried_bits << (64 - curry_bitsize);
                    uint64_t R = B[j] >> curry_bitsize;
                    curried_bits = B[j];
                    B[j] = L | R;
                }
            }
            fill(B, pos, len, false);
        }

        /*!
         * @brief Shifts the suffix B[pos..] of 64-bit integer sequence B[0..] to the left by len bits, i.e., B is changed to B[0..pos-1-len] | B[pos..63] | 0^{len}.
         * @param array_size the length of the 64-bit sequence B, i.e., the number of 64-bit blocks in B.
         */
        static void shift_left(uint64_t *B, uint64_t pos, uint64_t len, uint64_t array_size)
        {
            if (len > pos)
            {
                throw std::runtime_error("shift_leftX: shift_bitsize > shift_pos");
                return;
            }

            uint64_t block_index = pos / 64;
            uint64_t bit_index = pos % 64;

            uint64_t dst_block_index = (pos - len) / 64;
            uint64_t dst_bit_index = (pos - len) % 64;

            uint64_t curry_bitsize = 0;
            uint64_t mid_block_index = 0;
            uint64_t prefix_block_size = array_size - block_index;

            if (bit_index >= dst_bit_index)
            {
                curry_bitsize = bit_index - dst_bit_index;
                mid_block_index = dst_block_index;
                // uint64_t shift_block_size = block_index - dst_block_index;
                uint64_t tmp = B[dst_block_index];
                std::memmove(&B[dst_block_index], &B[block_index], prefix_block_size * sizeof(uint64_t));

                uint64_t maskL = dst_bit_index > 0 ? (UINT64_MAX << (64 - dst_bit_index)) : 0ULL;
                uint64_t maskR = UINT64_MAX >> bit_index;
                B[dst_block_index] = (tmp & maskL) | ((B[dst_block_index] & maskR) << curry_bitsize);
            }
            else
            {

                mid_block_index = dst_block_index + 1;
                curry_bitsize = bit_index + (64 - dst_bit_index);
                // uint64_t shift_block_size = block_index - (dst_block_index + 1);
                std::memmove(&B[dst_block_index + 1], &B[block_index], prefix_block_size * sizeof(uint64_t));
            }

            if (curry_bitsize > 0)
            {
                assert(curry_bitsize < 64);
                assert(mid_block_index + prefix_block_size - 1 < array_size);

                uint64_t curried_bits = B[mid_block_index + prefix_block_size - 1];

                if (mid_block_index + prefix_block_size - 1 != dst_block_index)
                {
                    B[mid_block_index + prefix_block_size - 1] = B[mid_block_index + prefix_block_size - 1] << curry_bitsize;
                }

                for (int64_t j = mid_block_index + prefix_block_size - 2; j >= (int64_t)(dst_block_index + 1); j--)
                {
                    assert(j < (int64_t)array_size);
                    uint64_t L = B[j] << curry_bitsize;
                    uint64_t R = curried_bits >> (64 - curry_bitsize);
                    curried_bits = B[j];
                    B[j] = L | R;
                }

                if (bit_index >= dst_bit_index)
                {

                    uint64_t L = B[dst_block_index];
                    uint64_t R = curried_bits >> (64 - curry_bitsize);
                    B[dst_block_index] = L | R;
                }
                else
                {
                    uint64_t Lsize = 64 - dst_bit_index;
                    uint64_t Rsize = curry_bitsize - Lsize;
                    assert(dst_bit_index > 0);

                    uint64_t maskL = UINT64_MAX << Lsize;
                    uint64_t maskR = UINT64_MAX << (64 - Lsize);

                    uint64_t L = B[dst_block_index] & maskL;
                    uint64_t R = (curried_bits << Rsize) & maskR;
                    B[dst_block_index] = L | (R >> (64 - Lsize));
                }
            }

            uint64_t len1 = (array_size * 64) - pos;
            uint64_t xpos = pos - len + len1;
            uint64_t xlen = (array_size * 64) - xpos;
            fill(B, xpos, xlen, false);
        }
    };

} // namespace stool