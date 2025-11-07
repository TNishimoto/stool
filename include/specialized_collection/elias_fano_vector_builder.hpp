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

    /**
     * @brief Builder class for Elias-Fano encoded vectors [Unchecked AI's Comment] 
     * 
     * This class implements a builder for Elias-Fano encoded vectors, which provide
     * space-efficient storage for monotonically increasing sequences of integers.
     * The encoding splits each value into upper and lower bits, where upper bits
     * are stored as a unary-coded bit vector and lower bits are stored directly.
     * 
     * @tparam T The type of values to be stored (typically uint64_t)
     * \ingroup CollectionClasses
     */
    class EliasFanoVectorBuilder
    {
    public:
        /** @brief Total number of elements to be stored */
        uint64_t _size = 0;
        /** @brief Storage for lower bits of each value */
        sdsl::int_vector<> lower_bits;
        /** @brief Bit vector for upper bits (unary encoding) */
        std::vector<bool> upper_bits;

        /** @brief Number of bits used for upper part of each value */
        uint8_t upper_bit_size;
        /** @brief Number of bits used for lower part of each value */
        uint8_t lower_bit_size;
        /** @brief Maximum value in the sequence */
        uint64_t max_value = 0;

        /** @brief Current number of zeros in upper bits */
        uint64_t current_zero_num_on_upper_bits = 0;
        /** @brief Current number of elements added */
        uint64_t current_element_count = 0;
        /** @brief Temporary value for bit-by-bit insertion */
        uint64_t tmp_value = 0;
        /** @brief Universe size (maximum possible value + 1) */
        uint64_t universe = 0;
        /** @brief Flag indicating if building is finished */
        bool finished = false;

        /**
         * @brief Calculate memory usage in bytes
         * @return Memory usage in bytes
         */
        uint64_t get_using_memory() const
        {
            return sdsl::size_in_bytes(lower_bits) + (this->upper_bits.size() / 8) + 42;
        }
        
        /**
         * @brief Default constructor
         */
        EliasFanoVectorBuilder()
        {
        }
        
        /**
         * @brief Initialize the builder with universe size and element count
         * @param _universe The universe size (maximum value + 1)
         * @param element_num Number of elements to be stored
         */
        void initialize(uint64_t _universe, uint64_t element_num)
        {
            uint64_t x = element_num == 0 ? 1 : std::ceil(std::log2(element_num));

            this->initialize(_universe, element_num, x);
        }
        
        /**
         * @brief Initialize the builder with universe size, element count, and upper bit size
         * @param _universe The universe size (maximum value + 1)
         * @param element_num Number of elements to be stored
         * @param _upper_bit_size Number of bits to use for upper part
         */
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
        
        /**
         * @brief Swap contents with another builder
         * @param builder The builder to swap with
         */
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
        
        /**
         * @brief Merge another builder's contents with an offset
         * @param builder The builder to merge from
         * @param add_value Value to add to each element from the other builder
         */
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

        /**
         * @brief Split a value into upper and lower bits
         * @param value The value to split
         * @return Pair of (upper_bits, lower_bits)
         */
        std::pair<uint64_t, uint64_t> get_upper_and_lower_bits(uint64_t value) const
        {
            uint64_t upper = value >> this->lower_bit_size;
            uint64_t upper_filter = 64 - this->lower_bit_size;
            uint64_t lower = (value << upper_filter) >> upper_filter;
            return std::pair<uint64_t, uint64_t>(upper, lower);
        }
        
        /**
         * @brief Push a single bit (for bit-by-bit construction)
         * @param bit The bit to push (true for 1, false for 0)
         */
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
        
        /**
         * @brief Add a value to the sequence
         * @param value The value to add (must be monotonically increasing)
         */
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
        /**
         * @brief Debug function to check upper bits consistency
         */
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

        /**
         * @brief Debug function to check overall consistency
         */
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
        /**
         * @brief Finalize the construction by adding the final zero bit
         */
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

        /**
         * @brief Find the position of the i-th 1 in upper bits
         * @param i The index of the 1 to find (1-based)
         * @return Position of the i-th 1, or UINT64_MAX if not found
         */
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
        
        /**
         * @brief Access the i-th element in the sequence
         * @param i The index of the element to access (0-based)
         * @return The value at index i
         */
        uint64_t access(uint64_t i) const
        {
            uint64_t upper = (upper_selecter(i + 1) - i);
            return access(i, upper);
        }
        
        /**
         * @brief Access the i-th element with precomputed upper bits
         * @param i The index of the element to access (0-based)
         * @param upper The upper bits value
         * @return The reconstructed value
         */
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

        /**
         * @brief Print the current state for debugging
         */
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

        /**
         * @brief Convert the encoded sequence to a standard vector
         * @param output The output vector to store the decoded values
         */
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