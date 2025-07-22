#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include <cassert>
#include "../basic/byte.hpp"
#include "../debug/print.hpp"

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
    template <typename INDEX_TYPE = uint16_t>
    class IntegerDeque
    {
        using T = uint64_t;
        using BUFFER_POS = INDEX_TYPE;
        using DEQUE_POS = INDEX_TYPE;
        uint64_t *circular_buffer_ = nullptr;
        INDEX_TYPE circular_buffer_size_;
        INDEX_TYPE starting_position_;
        INDEX_TYPE deque_size_;
        uint8_t value_byte_size_;

    public:
        /**
         * @brief Get the maximum possible deque size based on INDEX_TYPE
         * 
         * @return uint64_t The maximum number of elements that can be stored
         */
        static uint64_t max_deque_size()
        {
            uint64_t b = stool::LSBByte::get_code_length(std::numeric_limits<INDEX_TYPE>::max());
            return (1 << (b - 1)) - 1;
        }
        
        /**
         * @brief Extract a value from a packed code at a specific position
         * 
         * @param code The packed code containing multiple values
         * @param pos The position within the code (0-based)
         * @param byte_size The number of bytes used per value
         * @return uint64_t The extracted value
         */
        static uint64_t access_value(uint64_t code, uint8_t pos, uint8_t byte_size)
        {
            uint64_t left_size = (8 * byte_size) * pos;
            uint64_t right_size = 64 - (left_size + (8 * byte_size));
            return (code << left_size) >> (left_size + right_size);
        }
        
        /**
         * @brief Set a value in a packed code at a specific position
         * 
         * @param code The original packed code
         * @param pos The position within the code (0-based)
         * @param byte_size The number of bytes used per value
         * @param value The value to set
         * @return uint64_t The modified packed code
         */
        static uint64_t set_code(uint64_t code, uint8_t pos, uint8_t byte_size, uint64_t value)
        {
            uint64_t left_size = (8 * byte_size) * pos;
            uint64_t right_size = 64 - (left_size + (8 * byte_size));

            uint64_t left_code = code >> (right_size + (8 * byte_size));
            uint64_t right_code = (code << (left_size + (8 * byte_size))) >> (left_size + (8 * byte_size));

            return (left_code << (right_size + (8 * byte_size))) | (value << right_size) | right_code;
        }

        /**
         * @brief Calculate the total memory usage in bytes
         * 
         * @return uint64_t The total size in bytes
         */
        uint64_t size_in_bytes() const
        {
            return sizeof(this->circular_buffer_size_) + sizeof(this->starting_position_) + sizeof(this->deque_size_) + sizeof(this->circular_buffer_) + (sizeof(T) * this->circular_buffer_size_);
        }

        /**
         * @brief Move constructor
         * 
         * @param other The source IntegerDeque to move from
         */
        IntegerDeque(IntegerDeque &&other) noexcept
            : circular_buffer_(other.circular_buffer_),
              circular_buffer_size_(other.circular_buffer_size_),
              starting_position_(other.starting_position_),
              deque_size_(other.deque_size_),
              value_byte_size_(other.value_byte_size_)
        {
            // Reset the source object
            other.circular_buffer_ = nullptr;
            other.circular_buffer_size_ = 0;
            other.starting_position_ = 0;
            other.deque_size_ = 0;
            other.value_byte_size_ = 0;
        }
        
        /**
         * @brief Get the first part of the starting position
         * 
         * @return uint64_t The buffer index
         */
        uint64_t get_starting_position_1() const
        {
            return this->starting_position_ / (8 / this->value_byte_size_);
        }
        
        /**
         * @brief Get the second part of the starting position
         * 
         * @return uint64_t The position within the buffer element
         */
        uint64_t get_starting_position_2() const
        {
            return this->starting_position_ % (8 / this->value_byte_size_);
        }

        /**
         * @brief Get the current capacity of the deque
         * 
         * @return size_t The number of elements that can be stored
         */
        size_t capacity() const
        {
            return this->circular_buffer_size_;
        }
        
        /**
         * @brief Clear all elements from the deque
         */
        void clear()
        {
            IntegerDeque tmp;
            this->swap(tmp);
        }

        /**
         * @brief Iterator class for IntegerDeque
         * 
         * Provides random access iteration over the deque elements
         */
        class IntegerDequeIterator
        {

        public:
            IntegerDeque *_m_deq;
            INDEX_TYPE _m_idx;

            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T *;
            using reference = T &;
            
            /**
             * @brief Construct an iterator
             * 
             * @param _deque Pointer to the deque
             * @param _idx Current index
             */
            IntegerDequeIterator(IntegerDeque *_deque, INDEX_TYPE _idx) : _m_deq(_deque), _m_idx(_idx) {}

            /**
             * @brief Dereference operator
             * 
             * @return T The value at the current position
             */
            T operator*() const
            {
                return (*_m_deq)[this->_m_idx];
            }

            /**
             * @brief Pre-increment operator
             * 
             * @return IntegerDequeIterator& Reference to the incremented iterator
             */
            IntegerDequeIterator &operator++()
            {
                ++this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-increment operator
             * 
             * @return IntegerDequeIterator The iterator before increment
             */
            IntegerDequeIterator operator++(int)
            {
                IntegerDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            /**
             * @brief Pre-decrement operator
             * 
             * @return IntegerDequeIterator& Reference to the decremented iterator
             */
            IntegerDequeIterator &operator--()
            {
                --this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-decrement operator
             * 
             * @return IntegerDequeIterator The iterator before decrement
             */
            IntegerDequeIterator operator--(int)
            {
                IntegerDequeIterator temp = *this;
                --(*this);
                return temp;
            }
            
            /**
             * @brief Addition operator (iterator + offset)
             * 
             * @param n The offset to add
             * @return IntegerDequeIterator The new iterator position
             */
            IntegerDequeIterator operator+(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return IntegerDequeIterator(this->_m_deq, sum);
            }
            
            /**
             * @brief Addition assignment operator
             * 
             * @param n The offset to add
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            IntegerDequeIterator &operator+=(difference_type n)
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
            IntegerDequeIterator operator-(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx - (int16_t)n;
                return IntegerDequeIterator(this->_m_deq, sum);
            }
            
            /**
             * @brief Subtraction assignment operator
             * 
             * @param n The offset to subtract
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            IntegerDequeIterator &operator-=(difference_type n)
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
            difference_type operator-(const IntegerDequeIterator &other) const
            {
                return (int16_t)this->_m_idx - (int16_t)other._m_idx;
            }

            /**
             * @brief Subscript operator (non-const version)
             * 
             * @param n The offset
             * @return T& Reference to the element at offset n
             */
            T &operator[](difference_type n)
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }

            /**
             * @brief Subscript operator (const version)
             * 
             * @param n The offset
             * @return const T& Const reference to the element at offset n
             */
            const T &operator[](difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }
            
            /**
             * @brief Equality comparison operator
             */
            bool operator==(const IntegerDequeIterator &other) const { return _m_idx == other._m_idx; }
            
            /**
             * @brief Inequality comparison operator
             */
            bool operator!=(const IntegerDequeIterator &other) const { return _m_idx != other._m_idx; }
            
            /**
             * @brief Less than comparison operator
             */
            bool operator<(const IntegerDequeIterator &other) const { return this->_m_idx < other._m_idx; }
            
            /**
             * @brief Greater than comparison operator
             */
            bool operator>(const IntegerDequeIterator &other) const { return this->_m_idx > other._m_idx; }
            
            /**
             * @brief Less than or equal comparison operator
             */
            bool operator<=(const IntegerDequeIterator &other) const { return this->_m_idx <= other._m_idx; }
            
            /**
             * @brief Greater than or equal comparison operator
             */
            bool operator>=(const IntegerDequeIterator &other) const { return this->_m_idx >= other._m_idx; }
        };
        
        /**
         * @brief Default constructor
         * 
         * Initializes an empty deque with default capacity
         */
        IntegerDeque()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
            this->circular_buffer_ = new T[2];
            this->circular_buffer_[0] = 0;
            this->circular_buffer_[1] = 0;

            this->starting_position_ = 0;
            this->circular_buffer_size_ = 2;
            this->deque_size_ = 0;
            this->value_byte_size_ = 1;
        }
        
        /**
         * @brief Destructor
         * 
         * Frees the allocated memory
         */
        ~IntegerDeque()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
        }
        
        /**
         * @brief Get iterator to the beginning
         * 
         * @return IntegerDequeIterator Iterator pointing to the first element
         */
        IntegerDequeIterator begin() const
        {
            return IntegerDequeIterator(const_cast<IntegerDeque<INDEX_TYPE> *>(this), 0);
        }
        
        /**
         * @brief Get iterator to the end
         * 
         * @return IntegerDequeIterator Iterator pointing past the last element
         */
        IntegerDequeIterator end() const
        {
            return IntegerDequeIterator(const_cast<IntegerDeque<INDEX_TYPE> *>(this), this->deque_size_);
        }

        /**
         * @brief Get the bit size needed for the buffer
         * 
         * @return uint64_t The number of bits needed
         */
        uint64_t get_buffer_bit() const
        {
            return stool::LSBByte::get_code_length(this->circular_buffer_size_ * (8 / this->value_byte_size_));
        }
        
        /**
         * @brief Update the size if needed based on current usage
         * 
         * Automatically resizes the deque when it's too full or too empty
         */
        void update_size_if_needed()
        {
            uint64_t deque_bit = stool::LSBByte::get_code_length(this->deque_size_ + 1);
            uint64_t buffer_bit = this->get_buffer_bit();

            // uint64_t max = 1 << deque_bit;
            assert(deque_bit <= buffer_bit);

            if (deque_bit == buffer_bit)
            {
                this->reserve(buffer_bit, this->value_byte_size_);
            }
            else if (deque_bit + 3 < buffer_bit)
            {
                this->reserve(buffer_bit - 2, this->value_byte_size_);
            }
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
        
        /**
         * @brief Shrink the deque to fit its current size
         */
        void shrink_to_fit()
        {
            uint64_t bit_size = stool::LSBByte::get_code_length(this->deque_size_ + 1);
            this->shrink_to_fit(bit_size);
        }
        
        /**
         * @brief Recompute the deque contents
         * 
         * Rebuilds the deque from its current elements
         */
        void recompute()
        {
            std::vector<uint64_t> tmp_vec = this->to_vector();
            tmp_vec.resize(this->size(), UINT64_MAX);
            int64_t i = 0;
            for (uint64_t x : this)
            {
                tmp_vec[i++] = x;
            }
        }
        
        /**
         * @brief Determine the appropriate byte size for a value
         * 
         * @param value The value to analyze
         * @return uint64_t The recommended byte size (1, 2, 4, or 8)
         */
        static uint64_t get_byte_size(uint64_t value)
        {
            uint64_t new_byte_size = 0;
            uint64_t vsize = stool::LSBByte::get_code_length(value);
            if (vsize <= 8)
            {
                new_byte_size = 1;
            }
            else if (vsize <= 16)
            {
                new_byte_size = 2;
            }
            else if (vsize <= 32)
            {
                new_byte_size = 4;
            }
            else
            {
                new_byte_size = 8;
            }
            return new_byte_size;
        }

        /**
         * @brief Add an element to the back of the deque
         * 
         * @param value The value to add
         */
        void push_back(const T &value)
        {
            // std::cout << "PUSH: " << value << std::endl;
            uint64_t vsize = stool::LSBByte::get_code_length(value);
            if (vsize > this->value_byte_size_ * 8)
            {
                uint64_t new_byte_size = get_byte_size(value);
                this->reserve(this->get_buffer_bit(), new_byte_size);
            }

            if (this->size() >= IntegerDeque<INDEX_TYPE>::max_deque_size())
            {
                throw std::invalid_argument("Error: push_back()");
            }
            this->update_size_if_needed();
            this->set_value(this->size(), value);
            this->deque_size_++;
        }
        
        /**
         * @brief Add an element to the front of the deque
         * 
         * @param value The value to add
         */
        void push_front(const T &value)
        {
            if (this->size() >= IntegerDeque<INDEX_TYPE>::max_deque_size())
            {
                throw std::invalid_argument("Error: push_front()");
            }

            this->update_size_if_needed();

            uint64_t pos = this->starting_position_ - 1;
            uint64_t mask = this->circular_buffer_size_ - 1;

            this->circular_buffer_[pos & mask] = value;
            this->starting_position_ = pos & mask;

            this->deque_size_++;
            assert(this->at(0) == value);
        }
        
        /**
         * @brief Remove the last element from the deque
         */
        void pop_back()
        {
            // std::cout << "POP" << std::endl;
            this->deque_size_--;
            this->update_size_if_needed();
        }
        
        /**
         * @brief Remove the first element from the deque
         */
        void pop_front()
        {
            uint64_t pos = this->starting_position_ + 1;
            uint64_t mask = this->circular_buffer_size_ - 1;
            this->starting_position_ = pos & mask;
            this->deque_size_--;
            this->update_size_if_needed();
        }
        
        /**
         * @brief Insert an element at a specific iterator position
         * 
         * @param position Iterator pointing to the insertion position
         * @param value The value to insert
         */
        void insert(const IntegerDequeIterator &position, const T &value)
        {
            uint64_t pos = position._m_idx;
            this->insert(pos, value);
        }
        
        /**
         * @brief Erase an element at a specific iterator position
         * 
         * @param position Iterator pointing to the element to erase
         */
        void erase(const IntegerDequeIterator &position)
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
        void insert(size_t position, const T &value)
        {
            if (position > this->size() + 1)
            {
                throw std::out_of_range("Position out of range");
            }

            // 容量が足りない場合は容量を増やす

            if (position == 0)
            {
                this->push_front(value);
            }
            else if (position == this->size())
            {
                this->push_back(value);
            }
            else
            {
                uint64_t vsize = stool::LSBByte::get_code_length(value);
                if (vsize > this->value_byte_size_ * 8)
                {
                    uint64_t new_byte_size = get_byte_size(value);
                    this->reserve(this->get_buffer_bit(), new_byte_size);
                }

                this->update_size_if_needed();


                for (size_t i = this->deque_size_; i > position; --i)
                {
                    uint64_t pos = this->starting_position_ + i;
                    if (pos >= this->circular_buffer_size_)
                    {
                        pos -= this->circular_buffer_size_;
                    }
                    this->circular_buffer_[pos] = pos > 0 ? this->circular_buffer_[pos - 1] : this->circular_buffer_[this->circular_buffer_size_ - 1];
                }

                uint64_t wpos = this->starting_position_ + position;
                if (wpos >= this->circular_buffer_size_)
                {
                    wpos -= this->circular_buffer_size_;
                }
                this->circular_buffer_[wpos] = value;

                this->deque_size_++;
            }

            assert(this->at(position) == value);
        }
        
        /**
         * @brief Erase an element at a specific position
         * 
         * @param position The position of the element to erase
         */
        void erase(size_t position)
        {
            if (position > 0)
            {
                for (int64_t i = position + 1; i < (int64_t)this->deque_size_; ++i)
                {
                    uint64_t pos = this->starting_position_ + i;
                    if (pos >= this->circular_buffer_size_)
                    {
                        pos -= this->circular_buffer_size_;
                    }
                    uint64_t pos2 = pos > 0 ? pos - 1 : this->circular_buffer_size_ - 1;
                    this->circular_buffer_[pos2] = this->circular_buffer_[pos];
                }
                this->deque_size_--;
                this->update_size_if_needed();
            }
            else
            {
                this->pop_front();
            }
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
        
        /**
         * @brief Shrink the deque to a specific capacity and byte size
         * 
         * @param capacity_bit_size The bit size for the new capacity
         * @param byte_size The byte size for storing values
         */
        void shrink_to_fit(uint64_t capacity_bit_size, uint8_t byte_size)
        {
            uint64_t size = 1 << capacity_bit_size;
            uint64_t tsize = size / (8 / byte_size);

            if (tsize > IntegerDeque<INDEX_TYPE>::max_deque_size())
            {
                assert(tsize > IntegerDeque<INDEX_TYPE>::max_deque_size());

                throw std::invalid_argument("shrink_to_fit");
            }
            else if (size > this->deque_size_ || this->value_byte_size_ != byte_size)
            {
                std::cout << "SHRINK/" << tsize << "/" << (uint64_t)byte_size << std::endl;

                T *new_data = new T[tsize];
                uint64_t i = 0;
                uint64_t shift = 64 - (byte_size * 8);
                for (uint64_t i = 0; i < tsize; i++)
                {
                    new_data[i] = 0;
                }
                for (IntegerDequeIterator it = this->begin(); it != this->end(); ++it)
                {
                    new_data[i] = new_data[i] | (*it << shift);
                    if (shift == 0)
                    {
                        i++;
                        shift = 64 - (byte_size * 8);
                    }
                    else
                    {
                        shift -= byte_size * 8;
                    }
                }

                if (this->circular_buffer_ != nullptr)
                {
                    delete[] this->circular_buffer_;
                    this->circular_buffer_ = nullptr;
                }

                this->circular_buffer_ = new_data;
                this->starting_position_ = 0;
                this->circular_buffer_size_ = tsize;
                this->value_byte_size_ = byte_size;
            }
        }
        
        /**
         * @brief Convert the deque to a std::deque
         * 
         * @return std::deque<T> A standard deque containing the same elements
         */
        std::deque<T> to_deque() const
        {

            std::deque<T> r;

            for (IntegerDequeIterator it = this->begin(); it != this->end(); ++it)
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
            for (uint64_t t = 0; t < this->circular_buffer_size_; t++)
            {
                std::bitset<64> p(this->circular_buffer_[t]);
                std::cout << p << std::flush;
                std::cout << " " << std::flush;
            }
            std::cout << std::endl;
            std::cout << "IntegerDeque: " << (uint64_t)this->circular_buffer_size_ << ", " << (uint64_t)this->deque_size_ << ", " << (uint64_t)this->starting_position_ << ", " << this->value_byte_size_ << std::endl;
        }
        
        /**
         * @brief Reserve space for a specific capacity and byte size
         * 
         * @param capacity_bit_size The bit size for the capacity
         * @param byte_size The byte size for storing values
         */
        void reserve(size_t capacity_bit_size, uint64_t byte_size)
        {
            this->shrink_to_fit(capacity_bit_size, byte_size);
        }
        
        /**
         * @brief Swap contents with another IntegerDeque
         * 
         * @param item The other deque to swap with
         */
        void swap(IntegerDeque &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            std::swap(this->circular_buffer_size_, item.circular_buffer_size_);
            std::swap(this->starting_position_, item.starting_position_);
            std::swap(this->deque_size_, item.deque_size_);
        }
        
        /**
         * @brief Access element by index (const version)
         * 
         * @param index The index of the element
         * @return T The value at the specified index
         */
        T operator[](size_t index) const
        {
            assert(index < this->size());
            uint64_t pos = this->starting_position_ + index;
            uint64_t pos1 = pos / (8 / this->value_byte_size_);
            uint64_t pos2 = pos % (8 / this->value_byte_size_);
            if (pos1 >= this->circular_buffer_size_)
            {
                pos1 -= this->circular_buffer_size_;
            }
            uint64_t result = access_value(this->circular_buffer_[pos1], pos2, this->value_byte_size_);
            // std::cout << "Access: " << index << "/" << result << std::endl;
            return result;
        }
    
        /**
         * @brief Set a value at a specific index
         * 
         * @param index The index where to set the value
         * @param value The value to set
         */
        void set_value(int64_t index, uint64_t value)
        {
            uint64_t pos = this->starting_position_ + index;
            uint64_t pos1 = pos / (8 / this->value_byte_size_);
            uint64_t pos2 = pos % (8 / this->value_byte_size_);
            if (pos1 >= this->circular_buffer_size_)
            {
                pos1 -= this->circular_buffer_size_;
            }
            uint64_t new_code = set_code(this->circular_buffer_[pos1], pos2, this->value_byte_size_, value);
            this->circular_buffer_[pos1] = new_code;
        }

        /**
         * @brief Access element by index with bounds checking
         * 
         * @param i The index of the element
         * @return T The value at the specified index
         */
        T at(uint64_t i) const
        {
            return (*this)[i];
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
    };

    /**
     * @brief Type alias for IntegerDeque with uint16_t index type
     */
    template <typename T>
    using IntegerDeque16 = IntegerDeque<uint16_t>;

    /**
     * @brief Type alias for IntegerDeque with uint32_t index type
     */
    template <typename T>
    using IntegerDeque32 = IntegerDeque<uint32_t>;

    /**
     * @brief Type alias for IntegerDeque with uint64_t index type
     */
    template <typename T>
    using IntegerDeque64 = IntegerDeque<uint64_t>;

}
