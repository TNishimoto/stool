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
        /*
        inline static constexpr int8_t get_0bit_position(uint8_t value, int rank) {
            int count = 0;
            for (int bit = 0; bit < 8; ++bit) {
                if (!(value & (1 << (7-bit))) ) {
                    if (count == rank) return bit;
                    ++count;
                }
            }
            return -1;
        }
        */

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
        // inline static constexpr std::array<std::array<int8_t, 8>, 256> select0_table = stool::__MSB_BYTE::build_lookup_table_for_select0();
    }

    /*!
     * @brief A class for handling most significant bit (MSB) operations on bytes
     * 
     * This class provides utility functions for manipulating and analyzing bits
     * in byte-level data, focusing on operations related to the most significant bits.
     * It includes functionality for:
     * - Bit counting and position finding
     * - Bit manipulation (reading, writing, shifting bits)
     * - Lookup table based optimizations for common operations
     * 
     * The class is particularly useful for:
     * - Big-endian bit processing
     * - MSB-first bit manipulation tasks
     * - Bit-level data structures requiring MSB operations
     * 
     * @note This implementation includes optimized lookup tables and bit manipulation techniques
     *       for improved performance on MSB-oriented byte operations
     */

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
            assert(len <= 64 && len > 0);
            uint64_t maskR = (UINT64_MAX >> (64 - len));
            uint64_t maskL = ~maskR;
            return (bits & maskL) | ((suffix_bits >> (64 - len)) & maskR);
        }
        static uint64_t write_prefix(uint64_t bits, uint8_t len, uint64_t prefix_bits)
        {
            assert(len <= 64 && len > 0);
            uint64_t maskL = (UINT64_MAX << (64 - len));
            uint64_t maskR = ~maskL;
            return (bits & maskR) | (prefix_bits & maskL);
        }
        static uint64_t write_bits(uint64_t bits, uint8_t pos, uint8_t len, uint64_t new_bits)
        {
            assert(pos + len <= 64);
            uint64_t maskA = (UINT64_MAX >> pos) & (UINT64_MAX << (64 - len - pos));
            uint64_t maskB = ~maskA;
            return (bits & maskB) | ((new_bits >> pos) & maskA);
        }

        static uint64_t shift_right(uint64_t code, uint8_t pos, uint8_t len)
        {
            assert(len > 0 && len <= 64);

            if(pos + len >= 64){
                uint64_t mask = ~(UINT64_MAX >> pos);
                return code & mask;
            }else{
                if(pos > 0){
                    uint64_t tmp = code >> len;

                    uint64_t maskL = UINT64_MAX << (64 - pos);
                    uint64_t maskR = UINT64_MAX >> (pos + len);
                    return (code & maskL) | (tmp & maskR);
    
                }else{
                    return code >> len;
                }    
            }
        }
        static uint64_t shift_left(uint64_t code, uint8_t pos, uint8_t len)
        {
            assert(len > 0 && len <= 64);

            if(len >= pos || pos == 0){
                return code << len;
            }else{
                uint64_t strL = code;
                uint64_t strR = code << len;
                uint64_t maskL = UINT64_MAX << (64 - (pos - len));
                uint64_t maskR = UINT64_MAX >> (pos - len);

                return (strL & maskL) | (strR & maskR);


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

        template <typename T>
        static void write_64bit_string(T &bits_array, uint64_t array_size, uint64_t bits, uint64_t block_index, uint8_t bit_index, uint8_t len, bool is_cyclic)
        {
            assert(block_index < array_size);

            if (bit_index + len <= 64)
            {
                bits_array[block_index] = MSBByte::write_bits(bits_array[block_index], bit_index, len, bits);
            }
            else
            {
                uint64_t left_len = 64 - bit_index;
                uint64_t right_len = len - left_len;
                uint64_t left_bits = bits;
                uint64_t right_bits = bits << left_len;

                bits_array[block_index] = MSBByte::write_suffix(bits_array[block_index], left_len, left_bits);
                uint64_t next_block_index = block_index + 1;
                if (next_block_index == array_size)
                {
                    if (is_cyclic)
                    {
                        next_block_index = 0;
                        assert(next_block_index < array_size);
                        assert(right_len <= 64 && right_len > 0);
                        bits_array[next_block_index] = MSBByte::write_prefix(bits_array[next_block_index], right_len, right_bits);
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    assert(next_block_index < array_size);
                    assert(right_len <= 64 && right_len > 0);
                    bits_array[next_block_index] = MSBByte::write_prefix(bits_array[next_block_index], right_len, right_bits);
                }
            }
        }

        /*
        template <typename T, uint64_t BUFFER_SIZE>
        static void move_suffix_blocks_to_a_right_bit_position(T &bits_array, uint64_t block_index, uint64_t dst_bit_index, uint64_t array_size)
        {
            if (block_index >= array_size)
            {
                throw std::runtime_error("block_index >= array_size");
            }
            if (dst_bit_index >= 64)
            {
                throw std::runtime_error("dst_bit_index >= 64");
            }

            if (dst_bit_index > 0)
            {
                std::array<uint64_t, BUFFER_SIZE> tmp_array;

                uint64_t L_size = dst_bit_index;
                uint64_t R_size = 64 - dst_bit_index;
                uint64_t fst_block = bits_array[block_index] >> L_size;
                tmp_array[0] = fst_block;

                for (uint64_t i = block_index + 1; i < array_size; i++)
                {
                    uint64_t prefix = bits_array[i - 1] << R_size;
                    uint64_t suffix = bits_array[i] >> L_size;
                    uint64_t new_block = prefix | suffix;
                    tmp_array[i - block_index] = new_block;
                }

                std::memcpy(&bits_array[block_index], &tmp_array[0], (array_size - block_index) * sizeof(uint64_t));
            }
            else
            {
                return;
            }
        }
        */
        /*

        template <typename T, uint64_t BUFFER_SIZE>
        static void move_suffix_blocks_to_a_left_bit_position(T &bits_array, uint64_t block_index, uint64_t dst_bit_index, uint64_t array_size)
        {
            if (block_index == 0)
            {
                throw std::runtime_error("block_index == 0");
            }
            if (dst_bit_index >= 64)
            {
                throw std::runtime_error("dst_bit_index >= 64");
            }

            if (dst_bit_index > 0)
            {
                uint8_t suffix_size = 64 - dst_bit_index;
                {
                    uint64_t newBlock = bits_array[block_index];
                    bits_array[block_index - 1] = stool::MSBByte::write_suffix(bits_array[block_index - 1], suffix_size, newBlock);
                }
                move_suffix_blocks_to_a_block_position<T, BUFFER_SIZE>(bits_array, block_index, suffix_size, block_index, array_size);
                ;
            }
            else
            {
                uint64_t block_size = array_size - block_index;
                std::memmove(&bits_array[block_index - 1], &bits_array[block_index], block_size * sizeof(uint64_t));
            }
        }
        */

        /*
        template <typename T, uint64_t BUFFER_SIZE>
        static void move_suffix_blocks_to_a_block_position(T &bits_array, uint64_t block_index, uint8_t bit_index, uint64_t dst_block_index, uint64_t array_size)
        {
            uint64_t block_size = array_size - block_index;

            if (dst_block_index < block_index)
            {
                if (bit_index == 0)
                {
                    std::memmove(&bits_array[dst_block_index], &bits_array[block_index], block_size * sizeof(uint64_t));
                }
                else
                {
                    std::array<uint64_t, BUFFER_SIZE> tmp_array;
                    for (uint64_t i = 0; i + 1 < block_size; i++)
                    {
                        uint64_t prefix = bits_array[block_index + i] << bit_index;
                        uint8_t prefix_len = 64 - bit_index;
                        uint64_t suffix = bits_array[block_index + i + 1] >> prefix_len;
                        uint64_t new_block = prefix | suffix;
                        tmp_array[i] = new_block;
                    }

                    std::memcpy(&bits_array[dst_block_index], &tmp_array[0], block_size * sizeof(uint64_t));

                    {
                        uint64_t prefix = bits_array[block_index + block_size - 1] << bit_index;
                        bits_array[dst_block_index + block_size - 1] = prefix;
                    }
                }
            }
            else if (block_index < dst_block_index)
            {
                uint64_t dst_block_size = array_size - dst_block_index;
                if (bit_index > 0)
                {
                    std::array<uint64_t, BUFFER_SIZE> tmp_array;
                    uint64_t L_size = bit_index;
                    uint64_t R_size = 64 - bit_index;

                    for (uint64_t i = 0; i < dst_block_size; i++)
                    {
                        uint64_t LBlock = bits_array[block_index + i] << L_size;
                        uint64_t RBlock = bits_array[block_index + i + 1] >> R_size;
                        uint64_t new_block = LBlock | RBlock;
                        tmp_array[i] = new_block;
                    }
                    std::memcpy(&bits_array[dst_block_index], &tmp_array[0], dst_block_size * sizeof(uint64_t));

                    bits_array[block_index] = (bits_array[block_index] >> R_size) << R_size;

                    for (uint64_t i = block_index + 1; i < dst_block_index; i++)
                    {
                        bits_array[i] = 0;
                    }
                }
                else
                {
                    std::memmove(&bits_array[dst_block_index], &bits_array[block_index], dst_block_size * sizeof(uint64_t));
                    for (uint64_t i = block_index; i < dst_block_index; i++)
                    {
                        bits_array[i] = 0;
                    }
                }
            }
            else
            {
                if (bit_index == 0)
                {
                    return;
                }
                else
                {
                    for (uint64_t i = 0; i + 1 < block_size; i++)
                    {
                        uint64_t prefix = bits_array[block_index + i] << bit_index;
                        uint8_t prefix_len = 64 - bit_index;
                        uint64_t suffix = bits_array[block_index + i + 1] >> prefix_len;
                        uint64_t new_block = prefix | suffix;

                        bits_array[block_index + i] = new_block;
                    }

                    {
                        uint64_t prefix = bits_array[block_index + block_size - 1] << bit_index;
                        bits_array[block_index + block_size - 1] = prefix;
                    }
                }
            }
        }
        */

        /*

        template <typename T, uint64_t BUFFER_SIZE>
        static void move_suffix_blocks_to_a_block_position(T &bits_array, uint64_t block_index, uint8_t bit_index, uint64_t dst_block_index, uint8_t dst_bit_index, uint64_t array_size)
        {
            // uint64_t block_size = array_size - block_index;

            if (block_index < dst_block_index)
            {
                move_suffix_blocks_to_a_block_position<T, BUFFER_SIZE>(bits_array, block_index, bit_index, dst_block_index, array_size);
                move_suffix_blocks_to_a_right_bit_position<T, BUFFER_SIZE>(bits_array, dst_block_index, dst_bit_index, array_size);
            }
            else if (block_index > dst_block_index)
            {
                move_suffix_blocks_to_a_block_position<T, BUFFER_SIZE>(bits_array, block_index, bit_index, dst_block_index + 1, array_size);
                move_suffix_blocks_to_a_left_bit_position<T, BUFFER_SIZE>(bits_array, dst_block_index + 1, dst_bit_index, array_size);
            }
            else
            {
                if (bit_index < dst_bit_index)
                {
                    uint64_t copy_start_index = block_index + 1;
                    uint64_t diff = dst_bit_index - bit_index;

                    uint64_t block = bits_array[block_index];
                    uint64_t W = block << (64 - diff);

                    if (copy_start_index < array_size)
                    {
                        move_suffix_blocks_to_a_right_bit_position<T, BUFFER_SIZE>(bits_array, copy_start_index, diff, array_size);
                        assert(diff <= 64);
                        if(diff > 0){
                            bits_array[copy_start_index] = stool::MSBByte::write_prefix(bits_array[copy_start_index], diff, W);
                        }
                    }

                    uint64_t block_suffix = (block << bit_index) >> dst_bit_index;
                    uint64_t block_prefix_size = bit_index;
                    assert(block_prefix_size <= 64);
                    if(block_prefix_size > 0){
                        uint64_t new_block = stool::MSBByte::write_prefix(block_suffix, block_prefix_size, block);
                        bits_array[block_index] = new_block;    
                    }else{
                        bits_array[block_index] = block_suffix; 
                    }
                }
                else if (bit_index > dst_bit_index)
                {
                    uint64_t readBlockL = bits_array[block_index] << bit_index;
                    uint64_t readBlockL_size = 64 - bit_index;
                    uint64_t readBlockR = block_index + 1 < array_size ? (bits_array[block_index + 1] >> readBlockL_size) : 0;
                    uint64_t readBlock = readBlockL | readBlockR;

                    uint8_t blockL_size = 64 - dst_bit_index;
                    {
                        uint64_t newBlock = stool::MSBByte::write_suffix(bits_array[block_index], blockL_size, readBlock);
                        bits_array[block_index] = newBlock;
                    }

                    uint64_t src_block_index = block_index + 1;
                    uint8_t src_bit_index = (bit_index + blockL_size) % 64;

                    if (src_block_index < array_size)
                    {
                        move_suffix_blocks_to_a_block_position<T, BUFFER_SIZE>(bits_array, src_block_index, src_bit_index, src_block_index, array_size);
                    }
                }
                else
                {
                    return;
                }
            }
        }
        */

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

        template <typename T>
        static void fill(T &bits, uint64_t pos, uint64_t len, bool b)
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

        static int64_t old_select1(uint64_t bits, uint64_t i)
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
                    uint8_t bits8 = (bits >> (56 - pos)) & UINT8_MAX;
                    uint64_t X = pos + __MSB_BYTE::select1_table[bits8][(nth - counter - 1)];
                    return X;
                }
                counter += c;
            }
            return -1;
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
            uint64_t num = Byte::count_bits(bits);
            if(num < i+1){
                return -1;
            }else{
                uint64_t j = num - (i+1);
                return 63 - LSBByte::select1(bits, j);
            }
        }
        static int64_t select1(uint64_t bits)
        {
            if (bits != 0)
            {
                return __builtin_clzll(bits);
            }
            else
            {
                return -1;
            }
        }
        template <typename T>
        static uint64_t read_64bit_string(T &bit_array, uint64_t block_index, uint64_t bit_index, uint64_t array_size)
        {
            if (bit_index == 0)
            {
                return bit_array[block_index];
            }
            else if (block_index + 1 < array_size)
            {

                uint64_t L = bit_array[block_index] << bit_index;
                uint64_t R = bit_array[block_index + 1] >> (64 - bit_index);
                return L | R;
            }
            else
            {
                uint64_t L = bit_array[block_index] << bit_index;
                return L;
            }
        }

        static uint64_t read_64bit_string(uint64_t block, uint8_t bit_index, uint8_t code_len)
        {
            uint64_t mask = UINT64_MAX << (64 - code_len);
            uint64_t value = (block << bit_index) & mask;
            return value;

        }
        static uint64_t read_as_64bit_integer(uint64_t block, uint8_t bit_index, uint8_t code_len)
        {
            uint64_t end_bit_index = bit_index + code_len - 1;
            uint64_t mask = UINT64_MAX >> (64 - code_len);

            uint64_t value = block >> (63 - end_bit_index);
            value = value & mask;

            return value;
        }


        static int64_t select1_for_8bits(uint8_t bits, uint8_t i)
        {
            return __MSB_BYTE::select1_table[bits][i];
        }
        static int64_t select0(uint64_t bits, uint64_t i)
        {
            return select1(~bits, i);
        }

        template <typename T>
        static uint64_t rank1(T &bit_array, uint64_t start_block_index, uint8_t start_bit_index, uint64_t end_block_index, uint8_t end_bit_index, [[maybe_unused]] uint64_t array_size){
            uint64_t num = 0;

            assert(start_block_index <= end_block_index);

            
            if(start_block_index == end_block_index){
                assert(start_bit_index <= end_bit_index);
                uint64_t block = bit_array[start_block_index];
                uint64_t R = stool::MSBByte::count_bits(block, end_bit_index);

                if(start_bit_index != 0){
                    uint64_t L = stool::MSBByte::count_bits(block, start_bit_index - 1);
                    num += R - L;
                }else{
                    num += R;
                }
            }
            else{

                if(start_bit_index != 0){
                    uint64_t block = bit_array[start_block_index];
                    uint64_t R = start_block_index != end_block_index ? stool::Byte::count_bits(block) : stool::MSBByte::count_bits(block, end_bit_index);
                    uint64_t L = stool::MSBByte::count_bits(block, start_bit_index - 1);
                    num += R - L;
                }else{
                    num += stool::Byte::count_bits(bit_array[start_block_index]);
                }

    
                for (uint64_t i = start_block_index + 1; i < end_block_index; i++)
                {
                    num += stool::Byte::count_bits(bit_array[i]);
                }

                {
                    num += stool::MSBByte::count_bits(bit_array[end_block_index], end_bit_index);
                }
    
            }
            return num;
        }

        static std::pair<uint64_t, uint8_t> add_bit_length(uint64_t block_index, uint64_t bit_index, uint64_t bit_length){
            block_index += bit_length / 64;
            bit_index += bit_length % 64;

            if(bit_index >= 64){
                bit_index -= 64;
                block_index++;
            }

            return std::make_pair(block_index, bit_index);

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

        template <typename T>
        static void write_bits(T &bits, uint64_t value, uint64_t len, uint16_t block_index, uint8_t bit_index, [[maybe_unused]] uint64_t array_size)
        {
            if (bit_index + len <= 64)
            {
                bits[block_index] = stool::MSBByte::write_bits(bits[block_index], bit_index, len, value);
            }
            else
            {
                uint64_t Lvalue = value;
                uint64_t Rvalue = value << (64 - bit_index);

                uint64_t diff = 64 - bit_index;

                bits[block_index] = stool::MSBByte::write_bits(bits[block_index], bit_index, diff, Lvalue);

                assert(block_index + 1 < array_size);
                bits[block_index + 1] = stool::MSBByte::write_bits(bits[block_index + 1], 0, len - diff, Rvalue);
            }
        }

        
        static void shift_right(uint64_t* bits, uint64_t shift_pos, uint64_t shift_bitsize, uint64_t array_size){
            uint64_t block_index = shift_pos / 64;
            uint64_t bit_index = shift_pos % 64;

            uint64_t dst_block_index = (shift_pos + shift_bitsize) / 64;
            uint64_t dst_bit_index = (shift_pos + shift_bitsize) % 64;

            uint64_t curry_bitsize = 0;
            uint64_t mid_block_index = 0;


            if(dst_block_index >= array_size){
                throw std::runtime_error("shift_rightX: dst_block_index >= array_size");
                return;            
            }else if(bit_index <= dst_bit_index){
                curry_bitsize = dst_bit_index - bit_index;
                mid_block_index = dst_block_index;
                //uint64_t shift_block_size = dst_block_index - block_index;
                uint64_t suffix_block_size = array_size - dst_block_index;
                std::memmove(&bits[dst_block_index], &bits[block_index], suffix_block_size * sizeof(uint64_t));

            }else{

                mid_block_index = dst_block_index - 1;
                curry_bitsize = dst_bit_index + 64 - bit_index;
                //uint64_t shift_block_size = (dst_block_index - 1) - block_index;
                uint64_t suffix_block_size = array_size - (dst_block_index - 1);
                std::memmove(&bits[dst_block_index-1], &bits[block_index], suffix_block_size * sizeof(uint64_t));
            }

            if(curry_bitsize > 0){
                assert(curry_bitsize < 64);

                uint64_t curried_bits = bits[mid_block_index];
                {
                    bits[mid_block_index] = stool::MSBByte::shift_right(bits[mid_block_index], bit_index, curry_bitsize);

                }


                for(uint64_t i = mid_block_index+1; i < array_size;i++){
                    uint64_t L = curried_bits << (64 - curry_bitsize);
                    uint64_t R = bits[i] >> curry_bitsize;                    
                    curried_bits = bits[i];
                    bits[i] = L | R;
                }
            }
            fill(bits, shift_pos, shift_bitsize, false);
        }
        static void shift_left(uint64_t* bits, uint64_t shift_pos, uint64_t shift_bitsize, uint64_t array_size){
            if(shift_bitsize > shift_pos){
                throw std::runtime_error("shift_leftX: shift_bitsize > shift_pos");
                return;            
            }
            
            uint64_t block_index = shift_pos / 64;
            uint64_t bit_index = shift_pos % 64;

            uint64_t dst_block_index = (shift_pos - shift_bitsize) / 64;
            uint64_t dst_bit_index = (shift_pos - shift_bitsize) % 64;

            uint64_t curry_bitsize = 0;
            uint64_t mid_block_index = 0;
            uint64_t prefix_block_size = array_size - block_index;



            if(bit_index >= dst_bit_index){
                curry_bitsize = bit_index - dst_bit_index;
                mid_block_index = dst_block_index;
                //uint64_t shift_block_size = block_index - dst_block_index;
                uint64_t tmp = bits[dst_block_index]; 
                std::memmove(&bits[dst_block_index], &bits[block_index], prefix_block_size * sizeof(uint64_t));

                uint64_t maskL = dst_bit_index > 0 ? (UINT64_MAX << (64 - dst_bit_index)) : 0ULL;
                uint64_t maskR = UINT64_MAX >> bit_index;
                bits[dst_block_index] = (tmp & maskL) | ((bits[dst_block_index] & maskR) << curry_bitsize);

            }else{

                mid_block_index = dst_block_index + 1;
                curry_bitsize = bit_index + (64 - dst_bit_index);
                //uint64_t shift_block_size = block_index - (dst_block_index + 1);
                std::memmove(&bits[dst_block_index+1], &bits[block_index], prefix_block_size * sizeof(uint64_t));
            }



            if(curry_bitsize > 0){
                assert(curry_bitsize < 64);
                assert(mid_block_index + prefix_block_size - 1 < array_size);

                uint64_t curried_bits = bits[mid_block_index + prefix_block_size - 1];

                if(mid_block_index + prefix_block_size - 1 != dst_block_index)
                {
                    bits[mid_block_index + prefix_block_size - 1] = bits[mid_block_index + prefix_block_size - 1] << curry_bitsize;
                }


                for(int64_t i = mid_block_index + prefix_block_size - 2; i >= (int64_t)(dst_block_index+1);i--){
                    assert(i < (int64_t)array_size);
                    uint64_t L = bits[i] << curry_bitsize;
                    uint64_t R = curried_bits >> (64 - curry_bitsize);                    
                    curried_bits = bits[i];
                    bits[i] = L | R;
                }

                if(bit_index >= dst_bit_index){

                    uint64_t L = bits[dst_block_index];
                    uint64_t R = curried_bits >> (64 - curry_bitsize);                    
                    bits[dst_block_index] = L | R;
                }else{
                    uint64_t Lsize = 64 - dst_bit_index;
                    uint64_t Rsize = curry_bitsize - Lsize;
                    assert(dst_bit_index > 0);

                    uint64_t maskL = UINT64_MAX << Lsize;
                    uint64_t maskR = UINT64_MAX << (64 - Lsize);

                    uint64_t L = bits[dst_block_index] & maskL;
                    uint64_t R = (curried_bits << Rsize) & maskR;                    
                    bits[dst_block_index] = L | (R >> (64 - Lsize));
                }

            }

            uint64_t len1 = (array_size * 64) - shift_pos;
            uint64_t xpos = shift_pos - shift_bitsize + len1;
            uint64_t xlen = (array_size * 64) - xpos;
            fill(bits, xpos, xlen, false);

        }

        
    };

} // namespace stool