#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "./value_array.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"

#include <sdsl/bit_vectors.hpp>
#pragma GCC diagnostic pop

namespace stool
{
    /*
bool getBit(uint64_t x, int8_t nth)
{
    return ((x >> nth) & 0x00000001) > 0;
}
std::string toBinaryString(uint64_t x)
{
    std::string s;
    s.resize(64, '0');
    for (uint64_t i = 0; i < 64; i++)
    {
        s[i] = getBit(x, i) ? '1' : '0';
    }
    return s;
}
*/

    class EliasFanoVectorBuilder
    {
    public:
        uint64_t _size = 0;
        //stool::ValueArray lower_bits;
        sdsl::int_vector<> lower_bits;
        std::vector<bool> upper_bits;

        uint8_t upper_bit_size;
        uint8_t lower_bit_size;
        uint64_t max_value = 0;

        uint64_t current_zero_num_on_upper_bits = 0;
        uint64_t current_element_count = 0;
        uint64_t tmp_value = 0;
        uint64_t universe = 0;
        bool finished = false;

        //uint64_t base_bit_size = 0;

        uint64_t get_using_memory() const
        {
            return sdsl::size_in_bytes(lower_bits) + (this->upper_bits.size() / 8) + 42;
        }
        EliasFanoVectorBuilder()
        {
        }
        void initialize(uint64_t _universe, uint64_t element_num)
        {
            uint64_t x = element_num == 0 ? 1 : std::ceil(std::log2(element_num));

            this->initialize(_universe, element_num, x);
        }
        void initialize(uint64_t _universe, uint64_t element_num, uint64_t _upper_bit_size)
        {

            this->universe = _universe;
#if DEBUG
            uint64_t x = element_num == 0 ? 1 : std::ceil(std::log2(element_num));

            assert(x <= _upper_bit_size);

#endif
            _size = element_num;

            this->upper_bit_size = _upper_bit_size;
            this->lower_bit_size = std::ceil(std::log2(universe)) - upper_bit_size;

            if (this->lower_bit_size != 0)
            {
                this->lower_bits.width(this->lower_bit_size);
            }
            else
            {
                this->lower_bits.width(1);
            }
            this->lower_bits.resize(element_num);
            /*
            if (this->lower_bit_size <= 8)
            {
                this->lower_bits.resize(element_num, 1);
            }
            else if (this->lower_bit_size <= 16)
            {
                this->lower_bits.resize(element_num, 2);
            }
            else if (this->lower_bit_size <= 32)
            {
                this->lower_bits.resize(element_num, 4);
            }
            else
            {
                this->lower_bits.resize(element_num, 8);
            }
            */
        }
        void swap(EliasFanoVectorBuilder &builder)
        {
            this->_size = builder._size;
            this->lower_bits.swap(builder.lower_bits);
            this->upper_bits.swap(builder.upper_bits);

            std::swap(this->upper_bit_size, builder.upper_bit_size);
            std::swap(this->lower_bit_size, builder.lower_bit_size);
            std::swap(this->max_value, builder.max_value);
            std::swap(this->current_zero_num_on_upper_bits, builder.current_zero_num_on_upper_bits);
            std::swap(this->current_element_count, builder.current_element_count);
            std::swap(this->tmp_value, builder.tmp_value);
            std::swap(this->universe, builder.universe);
        }
        void merge(EliasFanoVectorBuilder &builder, uint64_t add_value)
        {
            //std::vector<uint64_t> output;
            //builder.to_vector(output);

            uint64_t p = 0;
            uint64_t index = 0;
            for (uint64_t i = 0; i < builder.upper_bits.size(); i++)
            {
                if (builder.upper_bits[i])
                {
                    uint64_t value = builder.access(index, p);
                    this->push(add_value + value);

                    index++;
                }
                else
                {
                    p++;
                }
            }

            /*
            for (auto &it : output)
            {
                assert(add_value + it <= this->universe);

            }
            */

            EliasFanoVectorBuilder _tmp;
            builder.swap(_tmp);
        }

        std::pair<uint64_t, uint64_t> get_upper_and_lower_bits(uint64_t value) const
        {
            uint64_t upper = value >> this->lower_bit_size;
            uint64_t upper_filter = 64 - this->lower_bit_size;
            uint64_t lower = (value << upper_filter) >> upper_filter;
            return std::pair<uint64_t, uint64_t>(upper, lower);
        }
        void push_bit(bool bit)
        {
            if (bit)
            {
                std::pair<uint64_t, uint64_t> lr = get_upper_and_lower_bits(tmp_value);
                if (this->lower_bit_size != 0)
                {
                    this->lower_bits[current_element_count] = lr.second;
                }

                //this->lower_bits.change(current_element_count, lr.second);

                uint64_t upper_value = lr.first;
                if (current_zero_num_on_upper_bits == upper_value)
                {
                    upper_bits.push_back(true);
                }
                else if (current_zero_num_on_upper_bits < upper_value)
                {
                    while (current_zero_num_on_upper_bits < upper_value)
                    {
                        upper_bits.push_back(false);
                        ++current_zero_num_on_upper_bits;
                    }
                    upper_bits.push_back(true);
                }
                else
                {
                    throw std::runtime_error("error");
                }
                current_element_count++;
            }
            else
            {
                tmp_value++;
            }
        }
        void push(uint64_t value)
        {
            //std::cout << "CHECK/" << value   << std::endl;

            assert(current_element_count < _size);
            assert(value <= this->universe);
            std::pair<uint64_t, uint64_t> lr = get_upper_and_lower_bits(value);

            uint64_t upper_value = lr.first;
            if (this->lower_bit_size != 0)
            {
                this->lower_bits[current_element_count] = lr.second;
            }

            //std::cout << "CHECK-" << current_zero_num_on_upper_bits << "/" << upper_value << std::endl;
            //this->lower_bits.change(current_element_count, lr.second);

            if (current_zero_num_on_upper_bits == upper_value)
            {
                upper_bits.push_back(true);
            }
            else if (current_zero_num_on_upper_bits < upper_value)
            {
                while (current_zero_num_on_upper_bits < upper_value)
                {
                    upper_bits.push_back(false);
                    ++current_zero_num_on_upper_bits;
                }
                upper_bits.push_back(true);
            }
            else
            {
                assert(false);
                throw std::runtime_error("error");
            }

            current_element_count++;
            //this->check3(value);
        }
        /*
        void check3(uint64_t last_value){
            uint64_t p = this->access(this->current_element_count-1);
            assert(p == last_value);
            if(p != last_value){
                throw -1;
            }
        }
        */
#if DEBUG
        void check2()
        {

            uint64_t one_count = 0;
            for (uint64_t i = 0; i < this->upper_bits.size(); i++)
            {
                one_count += this->upper_bits[i] ? 1 : 0;
            }
            assert(this->current_element_count == one_count);
            if (this->current_element_count != one_count)
            {
                throw -1;
            }
        }

        void check()
        {
            assert(this->current_element_count == _size);

            uint64_t one_count = 0;
            for (uint64_t i = 0; i < this->upper_bits.size(); i++)
            {
                one_count += this->upper_bits[i];
            }
            assert(one_count == _size);
            if (one_count != _size)
            {
                throw -1;
            }
        }
#endif
        void finish()
        {
#if DEBUG
            this->check();
#endif

            if (!this->finished)
            {
                upper_bits.push_back(false);
                this->finished = true;
            }
            else
            {
                std::cout << "EliasFanoVector Finish Error" << std::endl;
                throw -1;
            }
        }

        uint64_t upper_selecter(uint64_t i) const
        {
            uint64_t p = 0;
            for (uint64_t x = 0; x < this->upper_bits.size(); x++)
            {
                if (this->upper_bits[x])
                {
                    p++;
                }
                if (p == i)
                {
                    return x;
                }
            }
            return UINT64_MAX;
        }
        uint64_t access(uint64_t i) const
        {
            uint64_t upper = (upper_selecter(i + 1) - i);
            return access(i, upper);
        }
        uint64_t access(uint64_t i, uint64_t upper) const
        {
            if (lower_bit_size > 0)
            {
                uint64_t lower = lower_bits[i];
                return (upper << lower_bit_size) | lower;
            }
            else
            {
                return upper;
            }
        }

        void print()
        {
            std::cout << "print" << std::endl;
            for (uint64_t i = 0; i < this->upper_bits.size(); i++)
            {
                std::cout << (this->upper_bits[i] ? "1" : "0");
            }
            std::cout << std::endl;
            std::cout << this->current_element_count << std::endl;

            for (uint64_t i = 0; i < this->current_element_count; i++)
            {
                std::cout << this->access(i) << ", ";
            }
            std::cout << "print end" << std::endl;
        }

        void to_vector(std::vector<uint64_t> &output)
        {
            output.resize(this->current_element_count);

            uint64_t p = 0;
            uint64_t index = 0;
            for (uint64_t i = 0; i < this->upper_bits.size(); i++)
            {
                if (this->upper_bits[i])
                {
                    uint64_t upper = p;
                    uint64_t lower = lower_bits[index];
                    output[index] = (upper << lower_bit_size) | lower;
                    index++;
                }
                else
                {
                    p++;
                }
            }
        }
    };


} // namespace stool