#pragma once
#include "./lsb_byte.hpp"

namespace stool
{
    class MSBByte
    {

    public:
        /*!
         * @brief Counts the number of 1 bits in S[0..i] for a given bit sequence S.
         *
         */
        inline static int64_t count_bits(uint64_t bits, uint64_t i)
        {
            return __builtin_popcountll(bits >> (63 - i));
        }
        inline static bool get_bit(uint64_t bits, int64_t nth)
        {
            return ((bits >> (63 - nth)) & 0b1) == 1;
        }

        static uint64_t write_bit(uint64_t bits, int64_t pos, bool value)
        {
            return LSBByte::write_bit(bits, 63 - pos, value);
        }

        static uint64_t write_suffix(uint64_t bits, uint8_t len, uint64_t suffix_bits)
        {
            uint64_t mask1 = UINT64_MAX << len;
            uint64_t result = (bits & mask1) | (suffix_bits >> (64 - len));

            return result;
        }
        static uint64_t write_prefix(uint64_t bits, uint8_t len, uint64_t prefix_bits)
        {
            if (len < 64)
            {
                uint64_t mask1 = UINT64_MAX >> len;
                uint64_t mask2 = UINT64_MAX << (64 - len);

                uint64_t result = (bits & mask1) | (prefix_bits & mask2);

                return result;
            }
            else
            {
                return prefix_bits;
            }
        }
        static uint64_t write_bits(uint64_t bits, uint8_t pos, uint8_t len, uint64_t new_bits)
        {
            if (pos != 0)
            {
                if (pos + len < 64)
                {
                    uint64_t mask1 = UINT64_MAX << (64 - pos);
                    uint64_t mask2 = UINT64_MAX >> (pos + len);
                    uint64_t mask3 = UINT64_MAX << (64 - len - pos);

                    return (bits & (mask1 | mask2)) | ((new_bits >> pos) & mask3);
                }
                else
                {
                    return write_suffix(bits, len, new_bits);
                }
            }
            else
            {
                return write_prefix(bits, len, new_bits);
            }
        }

        static uint64_t shift_right(uint64_t code, uint8_t pos, uint8_t len)
        {
            if (len > 0)
            {
                uint64_t pref = (code >> (63 - pos)) << (63 - pos);

                uint64_t suf_start_pos = pos + len;
                if (suf_start_pos == 64)
                {
                    return pref;
                }
                else
                {
                    uint64_t suf = (code << suf_start_pos) >> suf_start_pos;
                    return pref | suf;
                }
            }
            else
            {
                return code;
            }
        }
        static void replace(std::vector<uint64_t> &bits, uint64_t i, bool b)
        {
            uint64_t block_index = i / 64;
            uint64_t bit_index = i % 64;
            uint64_t block = bits[block_index];
            uint64_t new_block = stool::MSBByte::write_bit(block, bit_index, b);
            bits[block_index] = new_block;
        }

        uint64_t get_LSB(uint64_t bits) const
        {
            if (bits != 0)
            {
                // uint64_t count = stool::Byte::count_bits(bits);
                uint64_t sizex = 64 - stool::LSBByte::select1(bits, 0);
                return sizex - 1;
            }
            else
            {
                throw std::runtime_error("bits is 0");
            }
        }

        static void block_shift_right(std::vector<uint64_t> &bits, uint64_t block_size)
        {
            for (int64_t i = bits.size() - 1; i >= (int64_t)(bits.size() - block_size); i--)
            {
                bits[i] = 0;
            }

            for (int64_t i = bits.size() - block_size - 1; i >= 0; i--)
            {
                bits[i + block_size] = bits[i];
                bits[i] = 0;
            }
        }

        template <typename T>
        static void block_shift_right(T &bits_array, uint64_t block_index, uint64_t offset_block_index, uint8_t offset_bit_index, uint64_t array_size)
        {
            uint64_t start_index = block_index + offset_block_index;
            if (offset_bit_index == 0)
            {
                if (start_index < array_size)
                {
                    bits_array[start_index] = bits_array[block_index];
                }
            }
            else
            {
                uint64_t end_index = start_index + 1;

                uint64_t leftBlock = bits_array[block_index];
                uint64_t rightBlock = bits_array[block_index] << (64 - offset_bit_index);

                if (end_index < array_size)
                {
                    bits_array[start_index + 1] = MSBByte::write_prefix(bits_array[start_index + 1], offset_bit_index, rightBlock);
                }

                if (start_index < array_size)
                {
                    bits_array[start_index] = MSBByte::write_suffix(bits_array[start_index], 64 - offset_bit_index, leftBlock);
                }
            }
        }

        template <typename T>
        static void block_shift_right(T &bits_array, uint64_t bit_index, uint64_t offset_bit_index, uint64_t array_size)
        {
            int64_t xblock_index = bit_index / 64;
            int64_t xbit_index = bit_index % 64;

            int64_t yblock_index = offset_bit_index / 64;
            int64_t ybit_index = offset_bit_index % 64;

            if (xblock_index + 1 == array_size)
            {
                if (xbit_index + offset_bit_index < 64)
                {
                    uint64_t bits = bits_array[xblock_index] << xbit_index;
                    uint64_t bitsize = 64 - (xbit_index + offset_bit_index);
                    bits_array[xblock_index] = MSBByte::write_suffix(bits_array[xblock_index], bitsize, bits);
                }
            }
            else
            {
                if (xbit_index == 0)
                {
                    for (int64_t i = array_size - 1; i >= xblock_index; i--)
                    {
                        block_shift_right(bits_array, i, yblock_index, ybit_index, array_size);
                    }
                }
                else
                {
                    for (int64_t i = array_size - 1; i >= xblock_index + 1; i--)
                    {
                        block_shift_right(bits_array, i, yblock_index, ybit_index, array_size);
                    }

                    uint64_t zblock_index = (bit_index + offset_bit_index) / 64;
                    uint64_t zbit_index = (bit_index + offset_bit_index) % 64;

                    uint64_t zlen = 64 - xbit_index;
                    if (zbit_index + zlen <= 64)
                    {

                        if (zblock_index < array_size)
                        {

                            uint64_t bits = bits_array[xblock_index] << xbit_index;

                            bits_array[zblock_index] = MSBByte::write_bits(bits_array[zblock_index], zbit_index, zlen, bits);
                        }
                    }
                    else
                    {
                        uint64_t leftLen = 64 - zbit_index;
                        uint64_t rightLen = zlen - leftLen;
                        uint64_t leftBits = bits_array[xblock_index] << xbit_index;
                        uint64_t rightBits = bits_array[xblock_index] << (xbit_index + leftLen);


                        if (zblock_index + 1 < array_size)
                        {
                            bits_array[zblock_index + 1] = MSBByte::write_prefix(bits_array[zblock_index + 1], rightLen, rightBits);
                        }
                        if (zblock_index < array_size)
                        {
                            bits_array[zblock_index] = MSBByte::write_suffix(bits_array[zblock_index], leftLen, leftBits);
                        }
                    }
                }
            }
        }

        static void block_shift_left(std::vector<uint64_t> &bits, uint64_t block_size)
        {
            for (int64_t i = 0; i < (int64_t)block_size; i++)
            {
                bits[i] = 0;
            }
            for (int64_t i = (int64_t)block_size; i < (int64_t)bits.size(); i++)
            {
                bits[i - block_size] = bits[i];
                bits[i] = 0;
            }
        }

        static void shift_right(std::vector<uint64_t> &bits, uint64_t len)
        {
            if (len >= 64)
            {
                block_shift_right(bits, len / 64);
                len = len % 64;
            }
            if (bits.size() == 0)
            {
                return;
            }

            bits[bits.size() - 1] = bits[bits.size() - 1] >> len;

            for (int64_t i = bits.size() - 2; i >= 0; i--)
            {
                uint64_t suf = bits[i] << (64 - len);
                bits[i + 1] = bits[i + 1] | suf;
                bits[i] = bits[i] >> len;
            }
        }

        static void shift_left(std::vector<uint64_t> &bits, uint64_t len)
        {
            if (len >= 64)
            {
                block_shift_left(bits, len / 64);
                len = len % 64;
            }
            if (bits.size() == 0)
            {
                return;
            }

            bits[0] = bits[0] << len;

            for (int64_t i = 1; i < (int64_t)bits.size(); i++)
            {
                uint64_t pref = bits[i] >> (64 - len);
                bits[i - 1] = bits[i - 1] | pref;
                bits[i] = bits[i] << len;
            }
        }

        static uint64_t fill(uint64_t bits, uint64_t pos, uint64_t len, bool b)
        {
            uint64_t end_pos = pos + len - 1;

            if (b)
            {
                uint64_t mask1 = (UINT64_MAX >> (63 - end_pos)) << (63 - end_pos);
                uint64_t mask2 = (mask1 << pos) >> pos;
                return bits | mask2;
            }
            else
            {
                if (pos > 0)
                {
                    uint64_t pref = UINT64_MAX << (64 - pos);

                    if (end_pos >= 63)
                    {
                        uint64_t mask = pref;
                        return bits & mask;
                    }
                    else
                    {
                        uint64_t suf = UINT64_MAX >> (end_pos + 1);
                        uint64_t mask = pref | suf;
                        return bits & mask;
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
                        return bits & mask;
                    }
                }
            }
        }

        static void fill(std::vector<uint64_t> &bits, uint64_t pos, uint64_t len, bool b)
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
                        bits[current_block_index] = b ? UINT64_MAX : 0ULL;
                        current_block_index++;
                        counter -= 64;
                    }
                    else
                    {

                        bits[current_block_index] = fill(bits[current_block_index], current_bit_index, counter, b);
                        current_bit_index += counter;
                        counter = 0;
                    }
                }
                else
                {
                    if (current_bit_index + counter >= 64)
                    {

                        uint64_t mask_size = 64 - current_bit_index;

                        bits[current_block_index] = fill(bits[current_block_index], current_bit_index, mask_size, b);
                        current_block_index++;
                        current_bit_index = 0;
                        counter -= mask_size;
                    }
                    else
                    {

                        bits[current_block_index] = fill(bits[current_block_index], current_bit_index, counter, b);
                        current_bit_index += counter;
                        counter = 0;
                    }
                }
            }
        }

        /*!
         * @brief Finds the position of the (i+1)-th set bit (1) in a 64-bit integer
         *
         * Return the position of the (i+1)-th 1 in the given bits if such bit exists; otherwise return -1.
         * @param bits The 64-bit integer to search
         * @param i The zero-based index of the set bit to find (finds i+1 th occurrence)
         * @return The position (0-63) of the (i+1)-th set bit, or -1 if not found
         */
        static int64_t select1(uint64_t bits, uint64_t i)
        {
            uint64_t nth = i + 1;
            uint64_t mask = UINT8_MAX;
            uint64_t counter = 0;
            for (int64_t i = 7; i >= 0; i--)
            {
                uint64_t bs = i * 8;
                uint64_t mask2 = mask << bs;
                uint64_t v = bits & mask2;
                uint64_t c = Byte::count_bits(v);
                if (counter + c >= nth)
                {
                    uint64_t pos = (7 - i) * 8;

                    for (uint64_t j = 0; j < 8; j++)
                    {
                        if (get_bit(bits, pos + j))
                        {
                            counter++;
                            if (counter == nth)
                            {
                                return pos + j;
                            }
                        }
                    }
                    return -1;
                }
                counter += c;
            }
            return -1;
        }

        static std::string to_bit_string(uint64_t x, uint64_t bit_size)
        {
            std::string s = Byte::to_bit_string(x);
            while (s.size() > bit_size)
            {
                s.pop_back();
            }
            return s;
        }
    };

} // namespace stool