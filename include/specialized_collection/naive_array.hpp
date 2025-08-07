#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include <cassert>
#include "../basic/byte.hpp"
#include "../debug/print.hpp"
#include "../basic/simd.hpp"

namespace stool
{

    /**
     * @brief A specialized deque implementation for storing integers with variable byte sizes
     *
     * This class provides a memory-efficient deque implementation that can store integers
     * using different byte sizes (1, 2, 4, or 8 bytes) based on the actual values stored.
     * It uses a circular buffer to efficiently handle front and back operations.
     *
     * @tparam INDEX_TYPE The type used for indexing (uint16_t, uint32_t, uint64_t)
     */
    template <uint64_t SIZE = 1024>
    class NaiveArray
    {
    public:
    protected:
        std::array<uint64_t, SIZE> circular_buffer_;
        std::array<uint64_t, SIZE> circular_sum_buffer_;
        uint64_t deque_size_ = 0;

        static constexpr bool is_power_of_two = (SIZE != 0) && ((SIZE & (SIZE - 1)) == 0);

    public:
        /**
         * @brief Calculate the total memory usage in bytes
         *
         * @return uint64_t The total size in bytes
         */
        uint64_t size_in_bytes() const
        {
            return sizeof(NaiveArray);
        }

        NaiveArray(const std::vector<uint64_t> &items)
        {
            if constexpr (!is_power_of_two)
            {
                throw std::invalid_argument("SIZE must be a power of two");
            }
            this->clear();

            for (uint64_t i = 0; i < items.size(); i++)
            {
                this->push_back(items[i]);
            }
        }

        /**
         * @brief Get the current capacity of the deque
         *
         * @return size_t The number of elements that can be stored
         */
        size_t capacity() const
        {
            return SIZE;
        }

        /**
         * @brief Clear all elements from the deque
         */
        void clear()
        {
            this->deque_size_ = 0;
        }

        /**
         * @brief Default constructor
         *
         * Initializes an empty deque with default capacity
         */
        NaiveArray()
        {
            this->deque_size_ = 0;
        }

        /**
         * @brief Destructor
         *
         * Frees the allocated memory
         */
        ~NaiveArray()
        {
            this->clear();
        }

        /*
        uint64_t get_buffer_bit() const
        {
            return stool::LSBByte::get_code_length(this->circular_buffer_size_ / this->value_byte_size_);
        }
        */

        /**
         * @brief Check if the deque is empty
         *
         * @return bool True if the deque contains no elements
         */
        bool empty() const
        {
            return this->deque_size_ == 0;
        }

        uint64_t max_size() const
        {
            return SIZE;
        }

        void update_sum_buffer()
        {
            uint64_t sum = 0;
            for (uint64_t i = 0; i < this->deque_size_; i++)
            {
                sum += this->circular_buffer_[i];
                this->circular_sum_buffer_[i] = sum;
            }
        }
        void update_buffer(std::vector<uint64_t> &seq)
        {
            for (uint64_t i = 0; i < seq.size(); i++)
            {
                this->circular_buffer_[i] = seq[i];
            }
            this->deque_size_ = seq.size();
            this->update_sum_buffer();
        }

        /**
         * @brief Add an element to the back of the deque
         *
         * @param value The value to add
         */
        void push_back(uint64_t value)
        {
            if constexpr (!is_power_of_two)
            {
                throw std::invalid_argument("SIZE must be a power of two");
            }

            if (this->deque_size_ >= SIZE)
            {
                throw std::out_of_range("push_back, Size out of range");
            }

            this->circular_buffer_[this->deque_size_] = value;
            this->deque_size_++;
            this->update_sum_buffer();
        }

        /**
         * @brief Add an element to the front of the deque
         *
         * @param value The value to add
         */
        void push_front(uint64_t value)
        {
            if constexpr (!is_power_of_two)
            {
                throw std::invalid_argument("SIZE must be a power of two");
            }

            if (this->deque_size_ >= SIZE)
            {
                throw std::out_of_range("push_front, Size out of range");
            }

            std::vector<uint64_t> tmp;
            tmp.push_back(value);
            for (uint64_t i = 0; i < this->deque_size_; i++)
            {
                tmp.push_back(this->circular_buffer_[i]);
            }
            this->update_buffer(tmp);
        }

        /**
         * @brief Remove the last element from the deque
         */
        void pop_back()
        {
            if (this->deque_size_ == 0)
            {
                throw std::out_of_range("pop_back, Size out of range");
            }
            this->deque_size_--;
        }

        /**
         * @brief Remove the first element from the deque
         */
        void pop_front()
        {
            if (this->deque_size_ == 0)
            {
                throw std::out_of_range("pop_front, Size out of range");
            }

            std::vector<uint64_t> tmp;
            for (uint64_t i = 0; i < this->deque_size_ - 1; i++)
            {
                tmp.push_back(this->circular_buffer_[i]);
            }
            this->update_buffer(tmp);
        }
        uint64_t size() const
        {
            return this->deque_size_;
        }

        /**
         * @brief Insert an element at a specific position
         *
         * @param position The position to insert at
         * @param value The value to insert
         */
        void insert(uint64_t position, uint64_t value)
        {
            uint64_t size = this->size();

            std::vector<uint64_t> tmp;
            for (uint64_t i = 0; i < position; i++)
            {
                tmp.push_back(this->circular_buffer_[i]);
            }
            tmp.push_back(value);
            for (uint64_t i = position; i < this->deque_size_; i++)
            {
                tmp.push_back(this->circular_buffer_[i]);
            }
            this->update_buffer(tmp);
        }

        /**
         * @brief Erase an element at a specific position
         *
         * @param position The position of the element to erase
         */
        void erase(uint64_t position)
        {
            if (position == 0)
            {
                this->pop_front();
            }
            else if (position == this->size() - 1)
            {
                this->pop_back();
            }
            else
            {
                std::vector<uint64_t> tmp;
                for (uint64_t i = 0; i < position; i++)
                {
                    tmp.push_back(this->circular_buffer_[i]);
                }
                for (uint64_t i = position + 1; i < this->deque_size_; i++)
                {
                    tmp.push_back(this->circular_buffer_[i]);
                }
                this->update_buffer(tmp);
            }
        }


        uint64_t value_capacity() const
        {
            return UINT64_MAX;
        }

        /**
         * @brief Print debug information about the deque
         */
        void print_info() const
        {
        }

        /**
         * @brief Reserve space for a specific capacity and byte size
         *
         * @param capacity_bit_size The bit size for the capacity
         * @param byte_size The byte size for storing values
         */
        /*
        void reserve(size_t capacity_bit_size, uint64_t byte_size)
        {
            this->shrink_to_fit(capacity_bit_size, byte_size);
        }
        */

        /**
         * @brief Swap contents with another IntegerDeque
         *
         * @param item The other deque to swap with
         */
        void swap(NaiveArray &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            std::swap(this->circular_sum_buffer_, item.circular_sum_buffer_);
            std::swap(this->deque_size_, item.deque_size_);
        }

        /**
         * @brief Access element by index (const version)
         *
         * @param index The index of the element
         * @return T The value at the specified index
         */
        uint64_t operator[](uint64_t index) const
        {
            return this->at(index);
        }

        /**
         * @brief Set a value at a specific index
         *
         * @param index The index where to set the value
         * @param value The value to set
         */
        void set_value(uint64_t index, uint64_t value)
        {
            this->circular_buffer_[index] = value;
            this->update_sum_buffer();
        }

        /**
         * @brief Access element by index with bounds checking
         *
         * @param i The index of the element
         * @return T The value at the specified index
         */
        uint64_t at(uint64_t index) const
        {
            return this->circular_buffer_[index];
        }

        uint64_t psum(uint64_t i) const
        {
            return this->circular_sum_buffer_[i];
        }
        int64_t search(uint64_t value) const
        {
            uint64_t sum = 0;
            return this->search(value, sum);
        }

        int64_t search(uint64_t value, uint64_t &sum) const
        {

            uint64_t size = this->size();
            uint64_t i = 0;

            uint64_t v = this->circular_buffer_[i];
                        
            while (sum + v < value)
            {
                i++;
                sum += v;
                v = this->circular_buffer_[i];
                assert(i < size);
            }
            
            sum = i > 0 ? this->circular_sum_buffer_[i-1] : 0;

            return i;
        }
        uint64_t psum() const
        {
            if(this->deque_size_ == 0){
                return 0;
            }
            return this->circular_sum_buffer_[this->deque_size_-1];
        }

        void increment(uint64_t pos, int64_t delta)
        {
            uint64_t value = this->circular_buffer_[pos];
            this->set_value(pos, value + delta);
        }

        void decrement(uint64_t pos, int64_t delta)
        {
            uint64_t value = this->circular_buffer_[pos];
            this->set_value(pos, value - delta);
        }

        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            return 0;
        }

        uint64_t unused_size_in_bytes() const
        {
            return 0;
        }
    };
}
