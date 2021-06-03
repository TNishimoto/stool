#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sdsl/bit_vectors.hpp>
#include "stool/src/value_array.hpp"

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
        void check2(){
            
            uint64_t one_count = 0;
            for (uint64_t i = 0; i < this->upper_bits.size(); i++)
            {
                one_count += this->upper_bits[i] ?  1 : 0;
            }
            assert(this->current_element_count == one_count);
            if(this->current_element_count != one_count){
                throw -1;
            }
        }

        void check(){
            assert(this->current_element_count == _size);
            
            uint64_t one_count = 0;
            for (uint64_t i = 0; i < this->upper_bits.size(); i++)
            {
                one_count += this->upper_bits[i];
            }
            assert(one_count == _size);
            if(one_count != _size){
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

    class EliasFanoVector
    {
    public:
        class iterator
        {
        private:
            uint64_t index = 0;
            const EliasFanoVector *efs;

        public:
            using difference_type = uint64_t;
            using value_type = uint64_t;
            using pointer = uint64_t *;
            using reference = uint64_t &;
            using iterator_category = std::random_access_iterator_tag;
            //using value_type = uint64_t;

            iterator(const EliasFanoVector *_efs, uint64_t _index) : index(_index), efs(_efs)
            {
            }

            iterator &operator++()
            {
                this->index++;
                return *this;
            }
            uint64_t operator*()
            {
                return efs->access(this->index);
            }
            bool operator!=(const iterator &rhs)
            {
                return (index != rhs.index);
            }
            bool operator==(const iterator &rhs)
            {
                return (index == rhs.index);
            }

            iterator &operator+=(int64_t p)
            {
                this->index += p;
                return *this;
            }
            iterator operator+(int64_t p)
            {
                iterator r(this->efs, this->index + p);
                return r;
            }

            bool operator<(const iterator &rhs)
            {
                return (index < rhs.index);
            }
            bool operator>(const iterator &rhs)
            {
                return (index > rhs.index);
            }
            bool operator<=(const iterator &rhs)
            {
                return (index <= rhs.index);
            }
            bool operator>=(const iterator &rhs)
            {
                return (index >= rhs.index);
            }
            uint64_t operator-(const iterator &rhs)
            {
                if (this->index < rhs.index)
                {
                    return rhs.index - this->index;
                }
                else
                {
                    return this->index - rhs.index;
                }
            }
        };

    private:
        uint64_t _size = 0;
        //stool::ValueArray lower_bits;
        sdsl::int_vector<> lower_bits;
        sdsl::bit_vector upper_bits;
        sdsl::bit_vector::select_1_type upper_selecter;
        sdsl::bit_vector::select_0_type upper_0selecter;

        uint8_t upper_bit_size;
        uint8_t lower_bit_size;
        uint64_t max_value = 0;

        //template <typename VEC = std::vector<uint64_t>>

        /*
        void construct_lower_data_structure(const VEC &seq)
        {

            this->lower_bits.set(seq, true);
        }
        void build_lower_data_structure_from_bit_vector(const std::vector<bool> &seq, uint64_t item_count)
        {
            uint64_t zero_counter = 0;
            uint64_t max_value = 0;
            for (size_t i = 0; i < seq.size(); i++)
            {
                if (seq[i])
                {
                    uint64_t lower_value = get_upper_and_lower_bits(zero_counter).second;
                    if (max_value < lower_value)
                        max_value = lower_value;
                }
                else
                {
                    zero_counter++;
                }
            }
            if (max_value <= UINT8_MAX)
            {
                this->lower_bits.resize(item_count, 1);
            }
            else if (max_value <= UINT16_MAX)
            {
                this->lower_bits.resize(item_count, 2);
            }
            else if (max_value <= UINT32_MAX)
            {

                this->lower_bits.resize(item_count, 4);
            }
            else
            {

                this->lower_bits.resize(item_count, 8);
            }
            zero_counter = 0;
            uint64_t ith = 0;
            for (size_t i = 0; i < seq.size(); i++)
            {
                if (seq[i])
                {
                    uint64_t lower_value = get_upper_and_lower_bits(zero_counter).second;
                    this->lower_bits.change(ith, lower_value);
                    ith++;
                }
                else
                {
                    zero_counter++;
                }
            }
        }

        template <typename VEC = std::vector<uint64_t>>
        void construct_upper_data_structure(const VEC &seq)
        {

            vector<bool> tmp2;
            uint64_t tmp2_pos = 0;
            for (size_t i = 0; i < seq.size(); i++)
            {
                uint64_t upper_value = get_upper_and_lower_bits(seq[i]).first;
                if (tmp2_pos == upper_value)
                {
                    tmp2.push_back(true);
                }
                else if (tmp2_pos < upper_value)
                {
                    while (tmp2_pos < upper_value)
                    {
                        tmp2.push_back(false);
                        ++tmp2_pos;
                    }
                    tmp2.push_back(true);
                }
                else
                {
                    std::runtime_error("error");
                }
                //std::cout << upper_bits[i] << std::endl;
            }
            tmp2.push_back(false);

            sdsl::bit_vector b(tmp2.size(), 0);
            for (uint64_t i = 0; i < tmp2.size(); i++)
                b[i] = tmp2[i] ? 1 : 0;
            //sdsl::bit_vector::select_1_type b_sel(&b);
            upper_bits.swap(b);
            sdsl::bit_vector::select_1_type b_sel(&upper_bits);
            upper_selecter.set_vector(&upper_bits);
            upper_selecter.swap(b_sel);

            sdsl::bit_vector::select_0_type b0_sel(&upper_bits);
            upper_0selecter.set_vector(&upper_bits);
            upper_0selecter.swap(b0_sel);
        }
        void build_upper_data_structure_from_bit_vector(const std::vector<bool> &seq)
        {

            vector<bool> tmp2;
            uint64_t tmp2_pos = 0;
            uint64_t zero_counter = 0;
            for (size_t i = 0; i < seq.size(); i++)
            {
                if (seq[i])
                {
                    uint64_t upper_value = get_upper_and_lower_bits(zero_counter).first;
                    if (tmp2_pos == upper_value)
                    {
                        tmp2.push_back(true);
                    }
                    else if (tmp2_pos < upper_value)
                    {
                        while (tmp2_pos < upper_value)
                        {
                            tmp2.push_back(false);
                            ++tmp2_pos;
                        }
                        tmp2.push_back(true);
                    }
                    else
                    {
                        std::runtime_error("error");
                    }
                }
                else
                {
                    zero_counter++;
                }
                //std::cout << upper_bits[i] << std::endl;
            }
            tmp2.push_back(false);
            sdsl::bit_vector b(tmp2.size(), 0);
            for (uint64_t i = 0; i < tmp2.size(); i++)
                b[i] = tmp2[i] ? 1 : 0;
            //sdsl::bit_vector::select_1_type b_sel(&b);
            upper_bits.swap(b);
            sdsl::bit_vector::select_1_type b_sel(&upper_bits);
            upper_selecter.set_vector(&upper_bits);
            upper_selecter.swap(b_sel);

            sdsl::bit_vector::select_0_type b0_sel(&upper_bits);
            upper_0selecter.set_vector(&upper_bits);
            upper_0selecter.swap(b0_sel);
        }
        */

        std::pair<uint64_t, uint64_t> get_upper_and_lower_bits(uint64_t value) const
        {
            uint64_t upper = value >> this->lower_bit_size;
            uint64_t upper_filter = 64 - this->lower_bit_size;
            uint64_t lower = (value << upper_filter) >> upper_filter;
            return std::pair<uint64_t, uint64_t>(upper, lower);
        }

        uint64_t recover(uint64_t lower, uint64_t upper) const
        {
            return (upper << lower_bit_size) | lower;
        }

    public:
        using value_type = uint64_t;
        using const_iterator = iterator;

        EliasFanoVector()
        {
        }

        EliasFanoVector(EliasFanoVector &&obj) : lower_bits(std::move(obj.lower_bits))
        {

            this->_size = obj._size;
            this->upper_bit_size = obj.upper_bit_size;
            this->lower_bit_size = obj.lower_bit_size;
            this->max_value = obj.max_value;
            this->upper_bits.swap(obj.upper_bits);
            this->upper_selecter.set_vector(&this->upper_bits);
            this->upper_selecter.swap(obj.upper_selecter);

            this->upper_0selecter.set_vector(&this->upper_bits);
            this->upper_0selecter.swap(obj.upper_0selecter);
        }
        void build_from_builder(EliasFanoVectorBuilder &builder)
        {
            if (!builder.finished)
            {
                std::cout << "ERROR" << std::endl;
                throw -1;
            }
            this->upper_bit_size = builder.upper_bit_size;
            this->lower_bit_size = builder.lower_bit_size;
            this->_size = builder._size;
            this->max_value = builder.max_value;

            sdsl::bit_vector b(builder.upper_bits.size(), 0);
            for (uint64_t i = 0; i < builder.upper_bits.size(); i++)
            {
                b[i] = builder.upper_bits[i] ? 1 : 0;
            }

#if DEBUG
            uint64_t one_count = 0;
            for (uint64_t i = 0; i < builder.upper_bits.size(); i++)
            {
                one_count += builder.upper_bits[i];
            }
            assert(one_count == _size);
#endif

            upper_bits.swap(b);

            sdsl::bit_vector::select_1_type b_sel(&upper_bits);
            upper_selecter.set_vector(&upper_bits);
            upper_selecter.swap(b_sel);

            sdsl::bit_vector::select_0_type b0_sel(&upper_bits);
            upper_0selecter.set_vector(&upper_bits);
            upper_0selecter.swap(b0_sel);

            this->lower_bits.swap(builder.lower_bits);
/*
#if DEBUG
            uint64_t k = 0;
            for (uint64_t x = 0; x < this->_size; x++)
            {
                k += this->upper_selecter(x + 1);
            }
#endif
*/
        }
        template <typename VEC = std::vector<uint64_t>>
        void construct(VEC *seq)
        {
            uint64_t _max_value = 0;
            for (auto it : *seq)
            {
                if (_max_value < it)
                    _max_value = it;
            }

            EliasFanoVectorBuilder builder;
            builder.initialize(_max_value, seq->size());

            for (auto it : *seq)
            {
                builder.push(it);
            }
            builder.finish();
            this->build_from_builder(builder);

            /*

            for (auto it : *seq)
            {
                if (max_value < it)
                    max_value = it;
            }

            this->upper_bit_size = builder.upper_bit_size;
            this->lower_bit_size = builder.lower_bit_size;
            this->_size = builder._size;
            this->max_value = builder.max_value;

            this->upper_bit_size = std::log2(seq->size()) + 1;
            this->lower_bit_size = (std::log2(max_value) + 1) - upper_bit_size;

            this->construct_upper_data_structure(*seq);

            for (uint64_t i = 0; i < _size; i++)
            {
                (*seq)[i] = get_upper_and_lower_bits((*seq)[i]).second;
            }
            this->construct_lower_data_structure(*seq);
            for (uint64_t i = 0; i < _size; i++)
            {
                (*seq)[i] = this->access(i);
            }
            */

            //this->check(*seq);
        }
        void build_from_bit_vector(const std::vector<bool> &seq)
        {
            uint64_t _max_value = 0;
            uint64_t _element_num = 0;
            for (auto it : seq)
            {
                if (it)
                {
                    _element_num++;
                }
                else
                {
                    _max_value++;
                }
            }
            EliasFanoVectorBuilder builder;
            builder.initialize(_max_value, _element_num);
            for (auto it : seq)
            {
                builder.push_bit(it);
            }
            builder.finish();
            this->build_from_builder(builder);

            /*
            uint64_t zero_counter = 0;
            uint64_t max_value = 0;
            uint64_t item_count = 0;
            for (size_t i = 0; i < seq.size(); i++)
            {
                if (seq[i])
                {
                    uint64_t value = zero_counter;
                    item_count++;
                    if (max_value < value)
                        max_value = value;
                }
                else
                {
                    zero_counter++;
                }
            }
            _size = item_count;
            this->upper_bit_size = std::log2(item_count) + 1;
            this->lower_bit_size = (std::log2(max_value) + 1) - upper_bit_size;

            this->build_upper_data_structure_from_bit_vector(seq);
            this->build_lower_data_structure_from_bit_vector(seq, item_count);
            */
        }

        uint64_t access(uint64_t i) const
        {
            assert(i < this->_size);

            uint64_t upper = (upper_selecter(i + 1) - i);

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
        /*
        Return the number of 1 in T[0..value-1]
        */
        uint64_t rank(uint64_t value) const
        {
            auto min_value = this->access(0);
            if (value > min_value)
            {
                std::pair<uint64_t, uint64_t> ul = get_upper_and_lower_bits(value);

                uint64_t l = ul.first == 0 ? 0 : upper_0selecter(ul.first) - (ul.first - 1);
                uint64_t lpos = l == 0 ? 0 : (l - 1);
                uint64_t r = upper_0selecter(ul.first + 1) - (ul.first);
                uint64_t rpos = r == 0 ? 0 : (r - 1);

                if (rpos - lpos < 16)
                {
                    for (uint64_t i = lpos; i <= rpos; i++)
                    {
                        if (this->access(i) >= value)
                        {
                            return i;
                        }
                    }
                    return rpos + 1;
                }
                else
                {
                    auto p2 = std::lower_bound(this->begin() + lpos, this->begin() + (rpos + 1), value);
                    uint64_t pos2 = std::distance(this->begin(), p2);

                    return pos2;
                }
            }
            else
            {
                return 0;
            }
        }

        template <typename VEC = std::vector<uint64_t>>
        void check(const VEC &seq)
        {
            for (uint64_t i = 0; i < seq.size(); i++)
            {
                uint64_t p = this->access(i);
                if (seq[i] != p)
                {
                    throw std::logic_error("Something error!");
                }
            }
            std::cout << "OK!" << std::endl;
        }
        uint64_t operator[](uint64_t i) const
        {
            return this->access(i);
        }
        std::vector<uint64_t> to_vector()
        {
            std::vector<uint64_t> r;
            for (uint64_t i = 0; i < _size; i++)
            {
                r.push_back(this->access(i));
            }

            return r;
        }
        uint64_t size() const
        {
            return this->_size;
        }

        iterator begin() const
        {
            auto p = iterator(this, 0);
            return p;
        }
        iterator end() const
        {
            auto p = iterator(this, _size);
            return p;
        }

        uint64_t get_using_memory() const
        {
            return sdsl::size_in_bytes(lower_bits) + (this->upper_bits.size() / 8) + 18;
        }

        void print() const
        {
            uint64_t upper_size = upper_bits.size();
            std::cout << "Upper bits: ";
            for (uint64_t i = 0; i < upper_size; i++)
            {
                std::cout << (upper_bits[i] ? "1" : "0");
            }
            std::cout << std::endl;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                uint64_t p = this->access(i);
                std::pair<uint64_t, uint64_t> ul = get_upper_and_lower_bits(p);
                std::cout << "[" << p << ", " << ul.first << ", " << ul.second << "]";
            }
            std::cout << std::endl;
        }
    };

} // namespace stool