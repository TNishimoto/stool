#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include <cassert>
#include "../../basic/byte.hpp"
#include "../../debug/debug_printer.hpp"
#include "../../basic/simd.hpp"

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
    template <uint64_t MAX_ELEMENT_COUNT = 1024>
    class FasterStaticArrayDeque
    {
        public:

        using BufferIndex = uint64_t;
        using ElementIndex = uint64_t;
        static constexpr uint64_t BUFFER_SIZE = MAX_ELEMENT_COUNT * 2;

        protected:



        std::array<uint64_t, BUFFER_SIZE> circular_buffer_;
        uint64_t starting_position_ = 0;
        uint64_t deque_size_ = 0;
        uint64_t psum_ = 0;

        static constexpr bool is_power_of_two = (MAX_ELEMENT_COUNT != 0) && ((MAX_ELEMENT_COUNT & (MAX_ELEMENT_COUNT - 1)) == 0);


    public:
        /**
         * @brief Calculate the total memory usage in bytes
         *
         * @return uint64_t The total size in bytes
         */
        uint64_t size_in_bytes() const
        {
            return sizeof(FasterStaticArrayDeque);
        }

        FasterStaticArrayDeque(const std::vector<uint64_t> &items)
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
            return MAX_ELEMENT_COUNT;
        }

        /**
         * @brief Clear all elements from the deque
         */
        void clear()
        {
            this->deque_size_ = 0;
            this->starting_position_ = BUFFER_SIZE / 2;
            this->psum_ = 0;
        }

        /**
         * @brief Iterator class for IntegerDeque
         *
         * Provides random access iteration over the deque elements
         */
        class FasterStaticArrayDequeIterator
        {

        public:
            FasterStaticArrayDeque *_m_deq;
            uint64_t _m_idx;

            using iterator_category = std::random_access_iterator_tag;
            using value_type = uint64_t;
            using difference_type = std::ptrdiff_t;
            using pointer = uint64_t *;
            using reference = uint64_t &;

            /**
             * @brief Construct an iterator
             *
             * @param _deque Pointer to the deque
             * @param _idx Current index
             */
            FasterStaticArrayDequeIterator(FasterStaticArrayDeque *_deque, uint64_t _idx) : _m_deq(_deque), _m_idx(_idx) {}

            /**
             * @brief Dereference operator
             *
             * @return T The value at the current position
             */
            uint64_t operator*() const
            {
                return (*_m_deq)[this->_m_idx];
            }

            /**
             * @brief Pre-increment operator
             *
             * @return IntegerDequeIterator& Reference to the incremented iterator
             */
            FasterStaticArrayDequeIterator &operator++()
            {
                ++this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-increment operator
             *
             * @return IntegerDequeIterator The iterator before increment
             */
            FasterStaticArrayDequeIterator operator++(int)
            {
                FasterStaticArrayDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            /**
             * @brief Pre-decrement operator
             *
             * @return IntegerDequeIterator& Reference to the decremented iterator
             */
            FasterStaticArrayDequeIterator &operator--()
            {
                --this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-decrement operator
             *
             * @return IntegerDequeIterator The iterator before decrement
             */
            FasterStaticArrayDequeIterator operator--(int)
            {
                FasterStaticArrayDequeIterator temp = *this;
                --(*this);
                return temp;
            }

            /**
             * @brief Addition operator (iterator + offset)
             *
             * @param n The offset to add
             * @return IntegerDequeIterator The new iterator position
             */
            FasterStaticArrayDequeIterator operator+(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return FasterStaticArrayDequeIterator(this->_m_deq, sum);
            }

            /**
             * @brief Addition assignment operator
             *
             * @param n The offset to add
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            FasterStaticArrayDequeIterator &operator+=(difference_type n)
            {
                this->_m_idx += n;
                return *this;
            }

            /**
             * @brief Subtraction operator (iterator - offset)
             *
             * @param n The offset to subtract
             * @return IntegerDequeIterator The new iterator position
             */
            FasterStaticArrayDequeIterator operator-(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx - (int16_t)n;
                return FasterStaticArrayDequeIterator(this->_m_deq, sum);
            }

            /**
             * @brief Subtraction assignment operator
             *
             * @param n The offset to subtract
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            FasterStaticArrayDequeIterator &operator-=(difference_type n)
            {
                this->_m_idx -= n;
                return *this;
            }

            /**
             * @brief Difference operator (iterator - iterator)
             *
             * @param other The other iterator
             * @return difference_type The distance between iterators
             */
            difference_type operator-(const FasterStaticArrayDequeIterator &other) const
            {
                return (int16_t)this->_m_idx - (int16_t)other._m_idx;
            }

            /**
             * @brief Subscript operator (non-const version)
             *
             * @param n The offset
             * @return T& Reference to the element at offset n
             */
            /*
             T &operator[](difference_type n)
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }
            */

            /**
             * @brief Subscript operator (const version)
             *
             * @param n The offset
             * @return const T& Const reference to the element at offset n
             */
            const uint64_t &operator[](difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }

            /**
             * @brief Equality comparison operator
             */
            bool operator==(const FasterStaticArrayDequeIterator &other) const { return _m_idx == other._m_idx; }

            /**
             * @brief Inequality comparison operator
             */
            bool operator!=(const FasterStaticArrayDequeIterator &other) const { return _m_idx != other._m_idx; }

            /**
             * @brief Less than comparison operator
             */
            bool operator<(const FasterStaticArrayDequeIterator &other) const { return this->_m_idx < other._m_idx; }

            /**
             * @brief Greater than comparison operator
             */
            bool operator>(const FasterStaticArrayDequeIterator &other) const { return this->_m_idx > other._m_idx; }

            /**
             * @brief Less than or equal comparison operator
             */
            bool operator<=(const FasterStaticArrayDequeIterator &other) const { return this->_m_idx <= other._m_idx; }

            /**
             * @brief Greater than or equal comparison operator
             */
            bool operator>=(const FasterStaticArrayDequeIterator &other) const { return this->_m_idx >= other._m_idx; }
        };

        /**
         * @brief Default constructor
         *
         * Initializes an empty deque with default capacity
         */
        FasterStaticArrayDeque()
        {
            this->clear();
        }

        /**
         * @brief Destructor
         *
         * Frees the allocated memory
         */
        ~FasterStaticArrayDeque()
        {
            this->clear();
        }

        /**
         * @brief Get iterator to the beginning
         *
         * @return IntegerDequeIterator Iterator pointing to the first element
         */
        FasterStaticArrayDequeIterator begin() const
        {
            return FasterStaticArrayDequeIterator(const_cast<FasterStaticArrayDeque<MAX_ELEMENT_COUNT> *>(this), 0);
        }

        /**
         * @brief Get iterator to the end
         *
         * @return IntegerDequeIterator Iterator pointing past the last element
         */
        FasterStaticArrayDequeIterator end() const
        {
            return FasterStaticArrayDequeIterator(const_cast<FasterStaticArrayDeque<MAX_ELEMENT_COUNT> *>(this), this->deque_size_);
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

        uint64_t max_size() const {
            return MAX_ELEMENT_COUNT;
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

            if (this->deque_size_  >= MAX_ELEMENT_COUNT)
            {
                throw std::out_of_range("push_back, Size out of range");
            }

            if(this->starting_position_ + this->deque_size_ >= BUFFER_SIZE){
                this->starting_position_= reset_starting_position_of_array_deque(this->circular_buffer_, this->starting_position_, this->deque_size_); 
            }
            uint64_t pos = this->starting_position_ + this->deque_size_;
            this->circular_buffer_[pos] = value;
            this->psum_ += value;
            this->deque_size_++;

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

            if (this->deque_size_  >= MAX_ELEMENT_COUNT)
            {
                throw std::out_of_range("push_front, Size out of range");
            }


            if(this->starting_position_ == 0){
                this->starting_position_= reset_starting_position_of_array_deque(this->circular_buffer_, this->starting_position_, this->deque_size_); 
            }
            uint64_t pos = this->starting_position_ - 1;
            this->circular_buffer_[pos] = value;
            this->psum_ += value;
            this->deque_size_++;
            this->starting_position_ = pos;
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

            uint64_t pos = this->starting_position_ + this->deque_size_ - 1;
            this->psum_ -= this->circular_buffer_[pos];
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

            if(this->deque_size_ >= 2){
                this->psum_ -= this->circular_buffer_[this->starting_position_];
                this->deque_size_--;
                this->starting_position_++;    
            }else{
                this->clear();
            }
        }

        /**
         * @brief Insert an element at a specific iterator position
         *
         * @param position Iterator pointing to the insertion position
         * @param value The value to insert
         */
        void insert(const FasterStaticArrayDequeIterator &position, const uint64_t &value)
        {
            uint64_t pos = position._m_idx;
            this->insert(pos, value);
        }

        /**
         * @brief Erase an element at a specific iterator position
         *
         * @param position Iterator pointing to the element to erase
         */
        void erase(const FasterStaticArrayDequeIterator &position)
        {
            uint64_t pos = position._m_idx;
            this->erase(pos);
        }

        /**
         * @brief Insert an element at a specific position
         *
         * @param position The position to insert at
         * @param value The value to insert
         */
        void insert(ElementIndex position, uint64_t value)
        {
            uint64_t size = this->size();

            if (size  >= MAX_ELEMENT_COUNT)
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
                if(this->starting_position_ + this->deque_size_ >= BUFFER_SIZE){
                    this->starting_position_= reset_starting_position_of_array_deque(this->circular_buffer_, this->starting_position_, this->deque_size_);
                }
                
                uint64_t dst_pos = this->starting_position_ + position + 1;
                uint64_t src_pos = this->starting_position_ + position;
                uint64_t move_size = this->deque_size_ - position;

                memmove(&this->circular_buffer_[dst_pos], &this->circular_buffer_[src_pos], move_size * sizeof(uint64_t));

                this->circular_buffer_[src_pos] = value;
                this->psum_ += value;
                this->deque_size_++;
                assert(this->at(position) == value);

            }

        }

        /**
         * @brief Erase an element at a specific position
         *
         * @param position The position of the element to erase
         */
        void erase(ElementIndex position)
        {
            if(position >= this->deque_size_){
                throw std::out_of_range("erase, Position out of range");
            }

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

                uint64_t dst_pos = this->starting_position_ + position;
                uint64_t src_pos = this->starting_position_ + position + 1;
                uint64_t value = this->circular_buffer_[dst_pos];
                uint64_t move_size = this->deque_size_ - position;

                memmove(&this->circular_buffer_[dst_pos], &this->circular_buffer_[src_pos], move_size * sizeof(uint64_t));

                this->psum_ -= value;
                this->deque_size_--;
            }
        }
        void set_value(uint64_t index, uint64_t value)
        {
            uint64_t old_value = this->circular_buffer_[this->starting_position_ + index];
            this->circular_buffer_[this->starting_position_ + index] = value;
            this->psum_ -= old_value;
            this->psum_ += value;
        }

        /**
         * @brief Get the current number of elements
         *
         * @return size_t The number of elements in the deque
         */
        size_t size() const
        {
            return this->deque_size_;
        }

        uint64_t value_capacity() const {
            return UINT64_MAX;
        }

        static uint64_t reset_starting_position_of_array_deque(std::array<uint64_t, BUFFER_SIZE> &array, BufferIndex old_starting_position, uint64_t element_count)
        {
            uint64_t center_position = BUFFER_SIZE / 2;
            uint64_t new_starting_position = center_position - (element_count / 2);

            std::memmove(&array[new_starting_position], &array[old_starting_position], element_count * sizeof(uint64_t));

            return new_starting_position;

        }

        /**
         * @brief Convert the deque to a std::deque
         *
         * @return std::deque<T> A standard deque containing the same elements
         */
        std::deque<uint64_t> to_deque() const
        {

            std::deque<uint64_t> r;

            for (FasterStaticArrayDequeIterator it = this->begin(); it != this->end(); ++it)
            {
                r.push_back(*it);
            }
            assert(r.size() == this->size());

            return r;
        }

        /**
         * @brief Print debug information about the deque
         */
        void print_info() const
        {
            std::cout << "FasterStaticArrayDeque ===============" << std::endl;
            std::string buffer_str = "";

            /*
            for (uint64_t t = 0; t < BUFFER_SIZE; t++)
            {
                std::bitset<8> p(this->circular_buffer_[t]);
                buffer_str += p.to_string();
                buffer_str += " ";
            }
            */

            std::deque<uint64_t> deque_values = this->to_deque();
            stool::DebugPrinter::print_integers(deque_values, "Deque");
            // std::cout << "Buffer: " << buffer_str << std::endl;
            std::cout << "Buffer size: " << (int64_t)MAX_ELEMENT_COUNT << std::endl;
            std::cout << "Starting position: " << (int64_t)this->starting_position_ << std::endl;
            std::cout << "Deque size: " << (int64_t)this->deque_size_ << std::endl;
            std::cout << "==============================" << std::endl;
        }


        /**
         * @brief Swap contents with another IntegerDeque
         *
         * @param item The other deque to swap with
         */
        void swap(FasterStaticArrayDeque &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            std::swap(this->starting_position_, item.starting_position_);
            std::swap(this->deque_size_, item.deque_size_);
            std::swap(this->psum_, item.psum_);
        }

        /**
         * @brief Access element by index (const version)
         *
         * @param index The index of the element
         * @return T The value at the specified index
         */
        uint64_t operator[](ElementIndex index) const
        {
            return this->at(index);
        }

        /**
         * @brief Access element by index with bounds checking
         *
         * @param i The index of the element
         * @return T The value at the specified index
         */
        uint64_t at(ElementIndex index) const
        {
            return this->circular_buffer_[this->starting_position_ + index];
        }


        /**
         * @brief Convert the deque to a std::vector
         *
         * @return std::vector<uint64_t> A vector containing the same elements
         */
        std::vector<uint64_t> to_vector() const
        {
            std::vector<uint64_t> tmp(this->size());
            std::copy(this->begin(), this->end(), tmp.begin());
            return tmp;
        }

        std::string to_string() const
        {
            auto vec = this->to_vector();
            return stool::DebugPrinter::to_integer_string(vec);
        }

        uint64_t psum(uint64_t i) const
        {
            uint64_t sum = 0;
            for (uint64_t x = 0; x <= i; x++)
            {
                sum += this->circular_buffer_[this->starting_position_ + x];
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
            uint64_t psum = this->psum();
            uint64_t size = this->size();

            if(value <= psum && size > 0){
                uint64_t i = 0;
                uint64_t v = this->circular_buffer_[this->starting_position_ + i];
                        
                while (sum + v < value)
                {
                    i++;
                    sum += v;
                    v = this->circular_buffer_[this->starting_position_ + i];
                    assert(i < size);
                }
                return i;    
            }else if(size == 0){
                return -1;
            }else{
                sum = psum;
                return -1;
            }
        }


        void increment(uint64_t pos, int64_t delta)
        {
            this->circular_buffer_[this->starting_position_ + pos] += delta;
            this->psum_ += delta;
        }

        void decrement(uint64_t pos, int64_t delta)
        {
            this->circular_buffer_[this->starting_position_ + pos] -= delta;
            this->psum_ -= delta;
        }
        uint64_t psum() const
        {
            return this->psum_;
        }

        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if(only_extra_bytes){
                return 0;
            }else{
                return sizeof(FasterStaticArrayDeque<MAX_ELEMENT_COUNT>);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return (MAX_ELEMENT_COUNT - this->size()) * sizeof(uint64_t);
        }
    };
}
