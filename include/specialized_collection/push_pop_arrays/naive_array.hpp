#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include <cassert>
#include <cstring>
#include "../../basic/byte.hpp"
#include "../../debug/debug_printer.hpp"

namespace stool
{

    /**
     * @brief A specialized deque implementation for storing integers with variable byte sizes [Unchecked AI's Comment] 
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
        //std::array<uint64_t, SIZE> circular_sum_buffer_;
        uint64_t deque_size_ = 0;
        uint64_t psum_ = 0;

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
            this->psum_ = 0;
            assert(this->verify());

        }

        /**
         * @brief Default constructor
         *
         * Initializes an empty deque with default capacity
         */
        NaiveArray()
        {
            this->clear();
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

        static NaiveArray build(const std::vector<uint64_t> &items){
            NaiveArray deque(items);
            return deque;
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
            this->psum_ += value;
            //this->update_sum_buffer();
            assert(this->verify());

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

            uint64_t src = 0;
            uint64_t dst = 1;
            std::memmove(&this->circular_buffer_[dst], &this->circular_buffer_[src], this->deque_size_ * sizeof(uint64_t));
            this->circular_buffer_[src] = value;
            this->psum_ += value;
            this->deque_size_++;
            assert(this->verify());

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
            uint64_t value = this->circular_buffer_[this->deque_size_-1];
            this->deque_size_--;
            this->psum_ -= value;
            assert(this->verify());

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

            if(this->deque_size_ > 1){
                uint64_t value = this->circular_buffer_[0];
                this->psum_ -= value;
    
                uint64_t src = 1;
                uint64_t dst = 0;
                std::memmove(&this->circular_buffer_[dst], &this->circular_buffer_[src], (this->deque_size_ - 1) * sizeof(uint64_t));    
                this->deque_size_--;
            }else{
                this->clear();
            }
            assert(this->verify());

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

            if (size  >= SIZE)
            {
                throw std::out_of_range("insert, Size out of range");
            }

            if (position > this->deque_size_ + 1)
            {
                throw std::out_of_range("Position out of range");
            }

            if (position == 0)
            {
                this->push_front(value);
                assert(this->at(position) == value);
            }
            else if (position == size)
            {
                this->push_back(value);
                assert(this->at(position) == value);
            }
            else
            {
                
                uint64_t dst_pos = position + 1;
                uint64_t src_pos = position;
                uint64_t move_size = this->deque_size_ - position;

                memmove(&this->circular_buffer_[dst_pos], &this->circular_buffer_[src_pos], move_size * sizeof(uint64_t));

                this->circular_buffer_[src_pos] = value;
                this->psum_ += value;
                this->deque_size_++;
                assert(this->at(position) == value);

            }
            assert(this->verify());

        }
        void remove(uint64_t position)
        {
            this->erase(position);
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


                uint64_t dst_pos = position;
                uint64_t src_pos = position + 1;
                uint64_t value = this->circular_buffer_[dst_pos];
                uint64_t move_size = this->deque_size_ - position;

                memmove(&this->circular_buffer_[dst_pos], &this->circular_buffer_[src_pos], move_size * sizeof(uint64_t));

                this->psum_ -= value;
                this->deque_size_--;
            }
            assert(this->verify());

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
            std::cout << "psum = " << this->psum_ << std::endl;
            std::cout << "deque_size = " << this->deque_size_ << std::endl;
            std::cout << "circular_buffer = ";
            for(uint64_t i = 0; i < this->deque_size_; i++){
                std::cout << this->circular_buffer_[i] << " ";
            }
            std::cout << std::endl;
        }
        std::string to_string() const
        {
            std::string r = "[";
            for(uint64_t i = 0; i < this->deque_size_; i++){
                r += std::to_string(this->circular_buffer_[i]);
                if(i < this->deque_size_ - 1){
                    r += ", ";
                }
            }
            r += "]";
            return r;
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
            //std::swap(this->circular_sum_buffer_, item.circular_sum_buffer_);
            std::swap(this->psum_, item.psum_);
            std::swap(this->deque_size_, item.deque_size_);
            assert(this->verify());

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
            uint64_t old_value = this->circular_buffer_[index];
            this->psum_ -= old_value;
            this->psum_ += value;
            this->circular_buffer_[index] = value;
            assert(this->verify());

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
            uint64_t sum = 0;
            uint64_t size = this->size();
            if(i < size){
                for(uint64_t x = 0; x <= i; x++){
                    sum += this->circular_buffer_[x];
                }    
            }else{
                throw std::out_of_range("psum, Index out of range");
            }
            return sum;
        }
        int64_t search(uint64_t value) const
        {
            uint64_t sum = 0;
            return this->search(value, sum);
        }

        int64_t search(uint64_t value, uint64_t &sum) const
        {
            assert(this->verify());

            uint64_t size = this->size();
            uint64_t i = 0;

            if(value > this->psum_ || size == 0)
            {
                return -1;
            }
            else{
            
                sum = 0;
                uint64_t v = this->circular_buffer_[i];
                        
                while (sum + v < value)
                {
                    i++;
                    sum += v;
                    v = this->circular_buffer_[i];
                    assert(i < size);
                }
                    
                return i;    
            }    
            


        }
        uint64_t psum() const
        {
            return this->psum_;
        }
        bool verify() const{
            uint64_t sum = 0;
            for(uint64_t i = 0; i < this->size(); i++){
                sum += this->circular_buffer_[i];
            }
            if(sum != this->psum_){
                std::cout << "sum: " << sum << " != psum: " << this->psum_ << std::endl;
                throw std::invalid_argument("verify, psum error");
            }

            return true;
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

        std::vector<uint64_t> to_vector() const
        {
            std::vector<uint64_t> r;
            for(uint64_t i = 0; i < this->size(); i++){
                r.push_back(this->circular_buffer_[i]);
            }
            return r;

        }
        uint64_t reverse_psum([[maybe_unused]] uint64_t i) const
        {
            throw std::runtime_error("reverse_psum is not supported for NaiveArray");
        }


        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if(only_extra_bytes){
                return 0;
            }else{
                return sizeof(NaiveArray<SIZE>);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return (SIZE - this->size()) * sizeof(uint64_t);
        }
    };
}
