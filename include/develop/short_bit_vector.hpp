#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include "../basic/byte.hpp"
#include "../basic/lsb_byte.hpp"
#include "../basic/msb_byte.hpp"

namespace stool
{

    class ShortBitVector
    {

    public:
        // bits_with_gapはビット列Sを64ビットごとに分割したもの。
        std::vector<uint64_t> bits_with_gap;



        ShortBitVector()
        {
            this->bits_with_gap.clear();
            this->bits_with_gap.push_back(1ULL << 63);
        }

        ShortBitVector(const std::vector<bool> &bv)
        {
            uint64_t i = 0;
            while (true)
            {
                uint64_t bits = 0;
                uint64_t start_pos = i * 64;
                uint64_t end_pos = ((i + 1) * 64) - 1;

                if (end_pos < bv.size())
                {
                    for (uint64_t x = start_pos; x <= end_pos; x++)
                    {
                        bits = (bits << 1) | (bv[x] ? 1 : 0);
                    }
                    bits_with_gap.push_back(bits);
                    i++;
                }
                else
                {
                    for (uint64_t x = start_pos; x <= end_pos; x++)
                    {
                        if (x < bv.size())
                        {
                            bits = (bits << 1) | (bv[x] ? 1 : 0);
                        }
                        else if (x == bv.size())
                        {
                            bits = (bits << 1) | 1;
                        }
                        else
                        {
                            bits = (bits << 1) | 0;
                        }
                    }
                    bits_with_gap.push_back(bits);
                    break;
                }
            }
        }
        uint64_t end_bit_index_in_last_block() const
        {
            uint64_t last = this->bits_with_gap[this->bits_with_gap.size() - 1];
            // uint64_t count = stool::Byte::popcount(last);
            uint64_t sizex = 64 - stool::LSBByte::select1(last, 0);
            return sizex - 1;
        }
        size_t size() const
        {
            return 64 * (this->bits_with_gap.size() - 1) + this->end_bit_index_in_last_block();
        }
        bool at(uint64_t i) const
        {
            uint64_t block_index = i / 64;
            uint64_t bit_index = i % 64;
            return ((bits_with_gap[block_index] >> (63 - bit_index)) & 0b1) == 1;
        }
        bool operator[](size_t index) const
        {
            return this->at(index);
        }



        std::string to_string() const
        {
            std::string s = stool::Byte::to_bit_string(bits_with_gap, treu);
            uint64_t size = this->size();
            while (s.size() > size)
            {
                s.pop_back();
            }
            return s;
        }

        uint64_t psum(uint64_t i) const
        {
            int64_t current_len = i + 1;
            uint64_t block_index = 0;
            uint64_t sum = 0;
            while (current_len > 0)
            {
                if(current_len >= 64){
                    sum += stool::Byte::popcount(bits_with_gap[block_index]);
                    block_index++;
                    current_len -= 64;
                }else{
                    uint64_t bits = this->bits_with_gap[block_index] >> (64 - current_len);                    
                    sum += stool::Byte::popcount(bits);
                    current_len = 0;
                }
            }
            return sum;
        }


        // S[0, i]の1の個数を返す
        uint64_t rank1(uint64_t i) const
        {
            return this->psum(i);
        }
        uint64_t rank1() const
        {
            uint64_t size = this->size();
            if (size == 0)
            {
                return 0;
            }
            else
            {
                return this->rank1(size - 1);
            }
        }
        uint64_t rank0(uint64_t i) const
        {
            return (i+1) - this->rank1(i);
        }

        int64_t select1(uint64_t nth) const
        {
            uint64_t counter = 0;
            for (uint64_t i = 0; i < this->bits_with_gap.size(); i++)
            {
                uint64_t count = stool::Byte::popcount(bits_with_gap[i]);
                if (counter + count > nth)
                {
                    return i * 64 + stool::MSBByte::select1(bits_with_gap[i], nth - counter);
                }
                counter += count;
            }
            return -1;
        }
        int64_t successor1(uint64_t i) const {
            uint64_t block_index = (i+1) / 64;
            uint64_t bit_index = (i+1) % 64;
            uint64_t current_pos = (i+1);
            uint64_t size = this->size();

            while(current_pos < size){
                uint64_t mask = UINT64_MAX >> bit_index;
                uint64_t new_bits = bits_with_gap[block_index] & mask;

                if(new_bits == 0){
                    block_index++;
                    current_pos += (64 - bit_index);
                    bit_index = 0;
                }else{
                    current_pos += stool::Byte::number_of_leading_zero(new_bits) - bit_index;
                    if(current_pos == size){
                        return -1;
                    }else{
                        assert(i <= current_pos);
                        return current_pos;
                    }
                }                    
            }
            return -1;
        }
        size_t capacity() const{
            return this->bits_with_gap.capacity();
        }
        void reserve(size_t new_capacity){
            this->bits_with_gap.reserve(new_capacity);
        }
        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if(only_extra_bytes){
                return this->capacity() * sizeof(uint64_t);
            }else{
                return sizeof(std::vector<uint64_t>) + (this->capacity() * sizeof(uint64_t));
            }
        }
        void swap(ShortBitVector &item){
            this->bits_with_gap.swap(item.bits_with_gap);
        }
        uint64_t unused_size_in_bytes() const
        {
            return (this->capacity() - (this->size()/64) ) * sizeof(uint64_t);
        }


        void clear()
        {
            this->bits_with_gap.clear();
            this->bits_with_gap.push_back(1ULL << 63);
        }
        void resize(uint64_t new_size){
            if(this->size() <= new_size){
                this->extend_right(new_size - this->size());
            }else{
                this->pop_back(this->size() - new_size);
            }
        }
        void push_back(bool b)
        {
            this->extend_right(1);
            this->replace(this->size() - 1, b);
        }
        void push_back(uint64_t len, uint64_t value)
        {
            this->extend_right(len);
            this->replace(this->size() - len, len, value);            
        }

        uint64_t copy_to(uint64_t i, uint64_t len) const{

            uint64_t block_index = i / 64;
            uint64_t bit_index = i % 64;

            if(bit_index + len <= 64){
                uint64_t rightlen = 64 - (bit_index + len);
                uint64_t leftlen = bit_index;
                return (this->bits_with_gap[block_index] >> rightlen) << (leftlen + rightlen);
            }else{
                uint64_t xlen = 64 - bit_index;
                uint64_t ylen = len - xlen;

                uint64_t x = this->bits_with_gap[block_index] << (64 - xlen);

                uint64_t mask = UINT64_MAX << (64 - ylen);
                uint64_t y = (this->bits_with_gap[block_index + 1] & mask) >> xlen;

                return x | y;
            }
        }

        void pop_back()
        {
            if (this->bits_with_gap.size() == 0)
            {
                throw std::runtime_error("ShortBitVector is empty");
            }
            uint64_t i = this->end_bit_index_in_last_block();

            if (i > 0)
            {
                uint64_t last = this->bits_with_gap[this->bits_with_gap.size() - 1];
                uint64_t last1 = stool::MSBByte::write_bit(last, i, false);
                uint64_t last2 = stool::MSBByte::write_bit(last1, i - 1, true);
                this->bits_with_gap[this->bits_with_gap.size() - 1] = last2;
            }
            else
            {
                this->bits_with_gap.pop_back();
                uint64_t last = this->bits_with_gap[this->bits_with_gap.size() - 1];
                uint64_t last1 = stool::MSBByte::write_bit(last, 63, true);
                this->bits_with_gap[this->bits_with_gap.size() - 1] = last1;
            }
        }
        void pop_back(uint64_t len)
        {
            uint64_t block_index = this->bits_with_gap.size() - 1;
            uint64_t xbit_size = this->end_bit_index_in_last_block();
            uint64_t current_len = len;

            while (current_len > 0)
            {
                if (xbit_size == 63)
                {
                    if (current_len >= 64)
                    {
                        this->bits_with_gap.pop_back();
                        block_index--;
                        this->bits_with_gap[block_index] = stool::MSBByte::write_bit(this->bits_with_gap[block_index], 63, true);
                        current_len -= 64;
                    }
                    else
                    {

                        uint64_t ysize = xbit_size - current_len;
                        if (ysize > 0)
                        {
                            uint64_t mask = UINT64_MAX << (64 - ysize);
                            uint64_t new_bits = this->bits_with_gap[block_index] & mask;
                            new_bits = stool::MSBByte::write_bit(new_bits, ysize, true);
                            this->bits_with_gap[block_index] = new_bits;
                            xbit_size -= current_len;
                            current_len = 0;
                        }
                        else
                        {
                            this->bits_with_gap[block_index] = 1ULL << 63;
                            current_len = 0;
                        }
                    }
                }
                else
                {
                    if (current_len > xbit_size)
                    {
                        this->bits_with_gap.pop_back();
                        block_index--;
                        current_len -= xbit_size + 1;
                        xbit_size = 63;
                        this->bits_with_gap[block_index] = stool::MSBByte::write_bit(this->bits_with_gap[block_index], 63, true);
                    }
                    else
                    {
                        uint64_t ysize = xbit_size - current_len;
                        if (ysize > 0)
                        {
                            uint64_t mask = UINT64_MAX << (64 - ysize);
                            uint64_t new_bits = this->bits_with_gap[block_index] & mask;
                            new_bits = stool::MSBByte::write_bit(new_bits, ysize, true);
                            this->bits_with_gap[block_index] = new_bits;
                        }
                        else
                        {
                            this->bits_with_gap[block_index] = 1ULL << 63;
                        }
                        current_len = 0;
                    }
                }
            }
        }

        void replace(uint64_t i, bool b)
        {
            stool::MSBByte::replace(this->bits_with_gap, i, b);
        }
        void replace(uint64_t i, uint64_t len, uint64_t value)
        {

            if (len <= 64)
            {
                uint64_t block_index = i / 64;
                uint64_t bit_index = i % 64;
    
                if (bit_index + len <= 64)
                {
                    this->bits_with_gap[block_index] = stool::MSBByte::write_bits(this->bits_with_gap[block_index], bit_index, len, value);
                }
                else
                {
                    uint64_t sufLen = 64 - bit_index;
                    uint64_t prefLen = len - sufLen;
                    this->bits_with_gap[block_index] = stool::MSBByte::write_suffix(this->bits_with_gap[block_index], sufLen, value);
                    this->bits_with_gap[block_index + 1] = stool::MSBByte::write_prefix(this->bits_with_gap[block_index + 1], prefLen, (value << sufLen));
                }
            }
            else
            {
                throw std::runtime_error("len is too large");
            }
        }
        void insert(uint64_t i, uint64_t len, uint64_t value)
        {
            if(len <= 64){
                this->extend_right(i, len);
                this->replace(i, len, value);    
            }else{
                throw std::runtime_error("len is too large");
            }
        }

        void extend_right(uint64_t i, uint64_t len)
        {
            uint64_t new_size = this->size() + len;
            this->extend_right(len);
            this->copy_right(i, new_size - (i + len), i + len);
            stool::MSBByte::fill(this->bits_with_gap, i, len, false);
        }
        void copy_left(uint64_t starting_position, uint64_t len, uint64_t new_starting_position)
        {
            uint64_t current_len = len;
            uint64_t current_block_index = starting_position / 64;
            uint64_t current_bit_index = starting_position % 64;
            uint64_t current_new_starting_position = new_starting_position;

            while (current_len > 0)
            {
                if (current_bit_index == 0)
                {
                    if (current_len >= 64)
                    {
                        this->replace(current_new_starting_position, 64, this->bits_with_gap[current_block_index]);
                        current_len -= 64;
                        current_block_index++;
                        current_new_starting_position += 64;
                    }
                    else
                    {
                        uint64_t mask = UINT64_MAX << (64 - current_len);
                        uint64_t new_bits = this->bits_with_gap[current_block_index] & mask;
                        this->replace(current_new_starting_position, current_len, new_bits);
                        current_bit_index += current_len;
                        current_new_starting_position += current_len;
                        current_len = 0;
                    }
                }
                else
                {
                    if (current_bit_index + current_len >= 64)
                    {
                        uint64_t sufLen = 64 - current_bit_index;
                        uint64_t new_bits = this->bits_with_gap[current_block_index] << current_bit_index;
                        this->replace(current_new_starting_position, sufLen, new_bits);
                        current_new_starting_position += sufLen;
                        current_len -= sufLen;
                        current_bit_index = 0;
                        current_block_index++;
                    }
                    else
                    {
                        uint64_t new_bits = this->bits_with_gap[current_block_index] << current_bit_index;
                        this->replace(current_new_starting_position, current_len, new_bits);
                        current_new_starting_position += current_len;
                        current_bit_index = current_len;
                        current_len = 0;
                    }
                }
            }
        }

        void copy_right(uint64_t starting_position, uint64_t len, uint64_t new_starting_position)
        {
            uint64_t ending_position = starting_position + len - 1;

            uint64_t current_len = len;
            uint64_t current_block_index = ending_position / 64;
            uint64_t current_bit_index = ending_position % 64;

            while (current_len > 0)
            {
                uint64_t current_new_ending_position = new_starting_position + current_len - 1;
                if (current_bit_index == 63)
                {
                    if (current_len >= 64)
                    {

                        this->replace(current_new_ending_position - 63, 64, this->bits_with_gap[current_block_index]);
                        current_len -= 64;
                        current_block_index--;
                    }
                    else
                    {
                        uint64_t new_bits = this->bits_with_gap[current_block_index] << (64 - current_len);
                        this->replace(current_new_ending_position - current_len + 1, current_len, new_bits);
                        current_bit_index -= current_len;
                        current_len = 0;
                    }
                }
                else
                {
                    if (current_bit_index < current_len)
                    {
                        uint64_t mask = UINT64_MAX << (63 - current_bit_index);
                        uint64_t new_bits = this->bits_with_gap[current_block_index] & mask;
                        this->replace(current_new_ending_position - current_bit_index, current_bit_index + 1, new_bits);
                        current_len -= (current_bit_index + 1);
                        current_block_index--;
                        current_bit_index = 63;
                    }
                    else
                    {
                        uint64_t xpos = current_bit_index - current_len + 1;
                        uint64_t mask = UINT64_MAX << (63 - current_bit_index);
                        uint64_t new_bits = (this->bits_with_gap[current_block_index] << xpos) & mask;
                        this->replace(current_new_ending_position - current_len + 1, current_len, new_bits);
                        current_bit_index -= current_len;
                        current_len = 0;
                    }
                }
            }
        }

        void erase(uint64_t i, uint64_t len)
        {
            uint64_t npos = i + len;

            if (npos >= this->size())
            {
                this->pop_back(this->size() - i);
            }
            else
            {
                uint64_t xlen = this->size() - (i + len);
                this->copy_left(i + len, xlen, i);
                this->pop_back(len);

            }
        }

        void extend_right(uint64_t len)
        {
            uint64_t i = this->end_bit_index_in_last_block();

            uint64_t counter = len;
            while (counter > 0)
            {
                if (i == 0)
                {
                    if (counter >= 64)
                    {
                        this->bits_with_gap[this->bits_with_gap.size() - 1] = 0;
                        this->bits_with_gap.push_back(1ULL << 63);
                        counter -= 64;
                    }
                    else
                    {
                        this->bits_with_gap[this->bits_with_gap.size() - 1] = (1ULL << (63 - counter));
                        counter = 0;
                    }
                }
                else
                {
                    uint64_t last = this->bits_with_gap[this->bits_with_gap.size() - 1];

                    if (i + counter >= 64)
                    {
                        uint64_t last1 = stool::MSBByte::write_bit(last, i, false);
                        this->bits_with_gap[this->bits_with_gap.size() - 1] = last1;
                        this->bits_with_gap.push_back(1ULL << 63);
                        counter -= (64 - i);
                        i = 0;
                    }
                    else
                    {
                        uint64_t last1 = stool::MSBByte::write_bit(last, i, false);
                        uint64_t last2 = stool::MSBByte::write_bit(last1, i + counter, true);
                        this->bits_with_gap[this->bits_with_gap.size() - 1] = last2;
                        counter = 0;
                        i += counter;
                    }
                }
            }
        }
    };
}
