#pragma once
#include "./lsb_byte.hpp"
#include <cassert>

namespace stool
{
    namespace __MSB_BYTE{
        inline static constexpr int8_t get_1bit_position(uint8_t value, int rank) {
            int count = 0;
            for (int bit = 0; bit < 8; ++bit) {
                if (value & (1 << (7-bit))) {
                    if (count == rank) return bit;
                    ++count;
                }
            }
            return -1;
        }
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
        
        inline static constexpr std::array<std::array<int8_t, 8>, 256> build_lookup_table_for_select1() {
            std::array<std::array<int8_t, 8>, 256> table{};
        
            for (int i = 0; i < 256; ++i) {
                for (int j = 0; j < 8; ++j) {
                    table[i][j] = get_1bit_position(static_cast<uint8_t>(i), j);
                }
            }
        
            return table;
        }
        inline static constexpr std::array<std::array<int8_t, 8>, 256> build_lookup_table_for_select0() {
            std::array<std::array<int8_t, 8>, 256> table{};
        
            for (int i = 0; i < 256; ++i) {
                for (int j = 0; j < 8; ++j) {
                    table[i][j] = get_0bit_position(static_cast<uint8_t>(i), j);
                }
            }
        
            return table;
        }

        
        inline static constexpr std::array<std::array<int8_t, 8>, 256> select1_table = stool::__MSB_BYTE::build_lookup_table_for_select1();    
        inline static constexpr std::array<std::array<int8_t, 8>, 256> select0_table = stool::__MSB_BYTE::build_lookup_table_for_select0();    
    }


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
            if(len < 64){
                uint64_t mask1 = UINT64_MAX << len;
                uint64_t result = (bits & mask1) | (suffix_bits >> (64 - len));
    
                return result;                    
            }else{
                return suffix_bits;
            }
        }
        static uint64_t write_prefix(uint64_t bits, uint8_t len, uint64_t prefix_bits)
        {

            if (len < 64 && len > 0)
            {
                uint64_t mask1 = UINT64_MAX >> len;
                uint64_t mask2 = UINT64_MAX << (64 - len);

                uint64_t result = (bits & mask1) | (prefix_bits & mask2);

                return result;
            }
            else if(len == 0){
                return bits;
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
                if(next_block_index == array_size){
                    if(is_cyclic){
                        next_block_index = 0;
                        assert(next_block_index < array_size);
                        bits_array[next_block_index] = MSBByte::write_prefix(bits_array[next_block_index], right_len, right_bits);
                    }else{
                        return;
                    }
                }else{
                    assert(next_block_index < array_size);
                    bits_array[next_block_index] = MSBByte::write_prefix(bits_array[next_block_index], right_len, right_bits);    
                }
            }
        }


        template <typename T, uint64_t BUFFER_SIZE>
        static void move_suffix_blocks_to_a_right_bit_position(T &bits_array, uint64_t block_index, uint64_t dst_bit_index, uint64_t array_size){
            if(block_index >= array_size){
                throw std::runtime_error("block_index >= array_size");
            }
            if(dst_bit_index >= 64){
                throw std::runtime_error("dst_bit_index >= 64");
            }

            if(dst_bit_index > 0){
                std::array<uint64_t, BUFFER_SIZE> tmp_array;

                uint64_t L_size = dst_bit_index;
                uint64_t R_size = 64 - dst_bit_index;
                uint64_t fst_block = bits_array[block_index] >> L_size;
                tmp_array[0] = fst_block;

                for(uint64_t i = block_index + 1; i < array_size; i++){
                    uint64_t prefix = bits_array[i-1] << R_size;
                    uint64_t suffix = bits_array[i] >> L_size;                    
                    uint64_t new_block = prefix | suffix;                    
                    tmp_array[i-block_index] = new_block;
                }

                std::memcpy(&bits_array[block_index], &tmp_array[0], (array_size - block_index) * sizeof(uint64_t));
            }else{
                return;
            }
        }


        
        template <typename T, uint64_t BUFFER_SIZE>
        static void move_suffix_blocks_to_a_left_bit_position(T &bits_array, uint64_t block_index, uint64_t dst_bit_index, uint64_t array_size)
        {
            if(block_index == 0){
                throw std::runtime_error("block_index == 0");
            }
            if(dst_bit_index >= 64){
                throw std::runtime_error("dst_bit_index >= 64");
            }

            if(dst_bit_index > 0){
                uint8_t suffix_size = 64 - dst_bit_index;
                {
                    uint64_t newBlock = bits_array[block_index];
                    bits_array[block_index-1] = stool::MSBByte::write_suffix(bits_array[block_index-1], suffix_size, newBlock);
                }
                move_suffix_blocks_to_a_block_position<T, BUFFER_SIZE>(bits_array, block_index, suffix_size, block_index, array_size);;
            }else{
                uint64_t block_size = array_size - block_index;
                std::memmove(&bits_array[block_index-1], &bits_array[block_index], block_size * sizeof(uint64_t));
            }
        }
        


        template <typename T, uint64_t BUFFER_SIZE>
        static void move_suffix_blocks_to_a_block_position(T &bits_array, uint64_t block_index, uint8_t bit_index, uint64_t dst_block_index, uint64_t array_size)
        {
            uint64_t block_size = array_size - block_index;

            if(dst_block_index < block_index){
                if(bit_index == 0){
                    std::memmove(&bits_array[dst_block_index], &bits_array[block_index], block_size * sizeof(uint64_t));
                }else{
                    std::array<uint64_t, BUFFER_SIZE> tmp_array;
                    for(uint64_t i = 0; i + 1 < block_size; i++){
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
            }else if(block_index < dst_block_index){
                uint64_t dst_block_size = array_size - dst_block_index;
                if(bit_index > 0){
                    std::array<uint64_t, BUFFER_SIZE> tmp_array;
                    uint64_t L_size = bit_index;
                    uint64_t R_size = 64 - bit_index;

                    for(uint64_t i = 0; i  < dst_block_size; i++){
                        uint64_t LBlock = bits_array[block_index + i] << L_size;
                        uint64_t RBlock = bits_array[block_index + i + 1] >> R_size;
                        uint64_t new_block = LBlock | RBlock;
                        tmp_array[i] = new_block;
                    }
                    std::memcpy(&bits_array[dst_block_index], &tmp_array[0], dst_block_size * sizeof(uint64_t));

                    bits_array[block_index] = (bits_array[block_index] >> R_size) << R_size; 

                    for(uint64_t i = block_index+1; i < dst_block_index;i++){
                        bits_array[i] = 0;
                    }

    
                }else{
                    std::memmove(&bits_array[dst_block_index], &bits_array[block_index], dst_block_size * sizeof(uint64_t));
                    for(uint64_t i = block_index; i < dst_block_index;i++){
                        bits_array[i] = 0;
                    }    
                }
            }else{
                if(bit_index == 0){
                    return;
                }else{
                    for(uint64_t i = 0; i + 1 < block_size; i++){
                        uint64_t prefix = bits_array[block_index + i] << bit_index;
                        uint8_t prefix_len = 64 - bit_index;
                        uint64_t suffix = bits_array[block_index + i + 1] >> prefix_len;
                        uint64_t new_block = prefix | suffix;

                        bits_array[block_index+i] = new_block;
                    }

                    {
                        uint64_t prefix = bits_array[block_index + block_size - 1] << bit_index;
                        bits_array[block_index + block_size - 1] = prefix;
                    }
                }
            }
        }

        template <typename T, uint64_t BUFFER_SIZE>
        static void move_suffix_blocks_to_a_block_position(T &bits_array, uint64_t block_index, uint8_t bit_index, uint64_t dst_block_index, uint8_t dst_bit_index, uint64_t array_size)
        {
            //uint64_t block_size = array_size - block_index;

            if(block_index < dst_block_index){
                move_suffix_blocks_to_a_block_position<T, BUFFER_SIZE>(bits_array, block_index, bit_index, dst_block_index, array_size);
                move_suffix_blocks_to_a_right_bit_position<T, BUFFER_SIZE>(bits_array, dst_block_index, dst_bit_index, array_size);

            }else if(block_index > dst_block_index){
                move_suffix_blocks_to_a_block_position<T, BUFFER_SIZE>(bits_array, block_index, bit_index, dst_block_index+1, array_size);
                move_suffix_blocks_to_a_left_bit_position<T, BUFFER_SIZE>(bits_array, dst_block_index+1, dst_bit_index, array_size);
            }else{
                if(bit_index < dst_bit_index){
                    uint64_t copy_start_index = block_index + 1;
                    uint64_t diff = dst_bit_index - bit_index;
                    
                    uint64_t block = bits_array[block_index];
                    uint64_t W = block << (64 - diff);
                    
                    if(copy_start_index < array_size){
                        move_suffix_blocks_to_a_right_bit_position<T, BUFFER_SIZE>(bits_array, copy_start_index, diff, array_size);
                        bits_array[copy_start_index] = stool::MSBByte::write_prefix(bits_array[copy_start_index], diff, W);
                    }

                    uint64_t block_suffix = (block << bit_index) >> dst_bit_index;
                    uint64_t block_prefix_size = bit_index;
                    uint64_t new_block = stool::MSBByte::write_prefix(block_suffix, block_prefix_size, block);
                    bits_array[block_index] = new_block;
                }else if(bit_index > dst_bit_index){
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

                    if(src_block_index < array_size){
                        move_suffix_blocks_to_a_block_position<T, BUFFER_SIZE>(bits_array, src_block_index, src_bit_index, src_block_index, array_size);
                    }
                }else{
                    return;
                }
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
                    uint8_t bits8 = (bits >> (56 - pos)) & UINT8_MAX;
                    uint64_t X = pos + __MSB_BYTE::select1_table[bits8][(nth-counter-1)];
                    return X;
                }
                counter += c;
            }
            return -1;
        }
        static int64_t select0(uint64_t bits, uint64_t i)
        {
            uint64_t nth = i + 1;
            uint64_t mask = UINT8_MAX;
            uint64_t counter = 0;
            for (int64_t i = 7; i >= 0; i--)
            {
                uint64_t bs = i * 8;
                uint64_t mask2 = mask << bs;
                uint64_t v = bits & mask2;
                uint64_t c = 8 -Byte::count_bits(v);
                if (counter + c >= nth)
                {
                    uint64_t pos = (7 - i) * 8;
                    uint8_t bits8 = (bits >> (56 - pos)) & UINT8_MAX;
                    uint64_t X = pos + __MSB_BYTE::select0_table[bits8][(nth-counter-1)];
                    return X;
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