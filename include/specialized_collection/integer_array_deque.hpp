#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include <cassert>
#include "./bit_array_deque.hpp"

namespace stool
{
    template <typename T = uint64_t>
    class IntegerArrayDeque
    {
        BitArrayDeque deque;

    public:
        /**
         * @brief Iterator class for IntegerDeque
         *
         * Provides random access iteration over the deque elements
         */
        class IntegerDequeIterator
        {

        public:
            BitArrayDeque::BitArrayDequeIterator _m_bit_iterator;
            // uint16_t _m_idx;

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
            IntegerDequeIterator(BitArrayDeque::BitArrayDequeIterator _bit_iterator) : _m_bit_iterator(_bit_iterator)
            {
            }

            uint16_t get_index() const
            {
                if (this->_m_bit_iterator.is_end())
                {
                    return UINT16_MAX;
                }
                else
                {
                    return this->_m_bit_iterator.index / VALUE_BIT_SIZE;
                }
            }
            uint16_t get_size() const
            {
                return this->_m_bit_iterator.size / VALUE_BIT_SIZE;
            }

            /**
             * @brief Dereference operator
             *
             * @return T The value at the current position
             */
            T operator*() const
            {
                uint64_t value = this->_m_bit_iterator.read_64bit_MSB_string();
                return value >> (64 - VALUE_BIT_SIZE);
            }

            /**
             * @brief Pre-increment operator
             *
             * @return IntegerDequeIterator& Reference to the incremented iterator
             */
            IntegerDequeIterator &operator++()
            {
                this->_m_bit_iterator += VALUE_BIT_SIZE;
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
                this->_m_bit_iterator -= VALUE_BIT_SIZE;
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
                int16_t sum = (int16_t)this->get_index() + (int16_t)(n * VALUE_BIT_SIZE);
                return IntegerDequeIterator(this->_m_bit_iterator + sum);
            }

            /**
             * @brief Addition assignment operator
             *
             * @param n The offset to add
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            IntegerDequeIterator &operator+=(difference_type n)
            {
                this->_m_bit_iterator += n * VALUE_BIT_SIZE;
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
                int16_t sum = (int16_t)this->get_index() - (int16_t)(n * VALUE_BIT_SIZE);
                return IntegerDequeIterator(this->_m_bit_iterator - sum);
            }

            /**
             * @brief Subtraction assignment operator
             *
             * @param n The offset to subtract
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            IntegerDequeIterator &operator-=(difference_type n)
            {
                this->_m_bit_iterator -= n * VALUE_BIT_SIZE;
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
                return (int16_t)this->get_index() - (int16_t)other.get_index();
            }

            /**
             * @brief Equality comparison operator
             */
            bool operator==(const IntegerDequeIterator &other) const { return this->get_index() == other.get_index(); }

            /**
             * @brief Inequality comparison operator
             */
            bool operator!=(const IntegerDequeIterator &other) const { return this->get_index() != other.get_index(); }

            /**
             * @brief Less than comparison operator
             */
            bool operator<(const IntegerDequeIterator &other) const { return this->get_index() < other.get_index(); }

            /**
             * @brief Greater than comparison operator
             */
            bool operator>(const IntegerDequeIterator &other) const { return this->get_index() > other.get_index(); }

            /**
             * @brief Less than or equal comparison operator
             */
            bool operator<=(const IntegerDequeIterator &other) const { return this->get_index() <= other.get_index(); }

            /**
             * @brief Greater than or equal comparison operator
             */
            bool operator>=(const IntegerDequeIterator &other) const { return this->get_index() >= other.get_index(); }
        };

        BitArrayDeque *get_bit_array_deque_pointer()
        {
            return &this->deque;
        }

        /**
         * @brief Calculate the total memory usage in bytes
         *
         * @return uint64_t The total size in bytes
         */
        uint64_t size_in_bytes() const
        {
            return this->deque.size_in_bytes();
        }
        inline static constexpr uint64_t VALUE_BIT_SIZE = sizeof(T) * 8;

        /**
         * @brief Move constructor
         *
         * @param other The source IntegerArrayDeque to move from
         */
        IntegerArrayDeque(IntegerArrayDeque &&other) noexcept
            : deque(other.deque)
        {
            // Reset the source object
            other.deque = BitArrayDeque();
        }

        IntegerArrayDeque(const std::vector<T> &seq) noexcept
        {
            for (auto it : seq)
            {
                this->push_back(it);
            }
        }
        std::vector<T> to_vector() const
        {
            std::vector<T> seq;
            for(auto it : *this){
                seq.push_back(it);
            }
            return seq;
        }
        std::string to_string() const
        {
            return stool::DebugPrinter::to_integer_string(this->to_vector());
        }

        /**
         * @brief Get the current capacity of the deque
         *
         * @return size_t The number of elements that can be stored
         */
        size_t capacity() const
        {
            return this->deque.capacity() / (sizeof(T) * 8);
        }

        /**
         * @brief Clear all elements from the deque
         */
        void clear()
        {
            this->deque.clear();
        }

        /**
         * @brief Default constructor
         *
         * Initializes an empty deque with default capacity
         */
        IntegerArrayDeque()
        {
            this->deque = BitArrayDeque();
        }

        /**
         * @brief Destructor
         *
         * Frees the allocated memory
         */
        ~IntegerArrayDeque()
        {
            this->deque = BitArrayDeque();
        }

        bool empty() const
        {
            return this->deque.empty();
        }

        void shrink_to_fit()
        {
            this->deque.shrink_to_fit();
        }

        /**
         * @brief Add an element to the back of the deque
         *
         * @param value The value to add
         */
        void push_back(const T value)
        {
            uint64_t value2 = value;
            uint64_t bitsize = sizeof(T) * 8;
            value2 = value2 << (64 - bitsize);

            this->deque.push_back64(value2, bitsize);
        }

        /**
         * @brief Add an element to the front of the deque
         *
         * @param value The value to add
         */
        void push_front(const T &value)
        {
            uint64_t value2 = value;
            uint64_t bitsize = sizeof(T) * 8;
            value2 = value2 << (64 - bitsize);
            this->deque.push_front64(value2, bitsize);
        }

        /**
         * @brief Remove the last element from the deque
         */
        void pop_back()
        {
            this->deque.pop_back(VALUE_BIT_SIZE);
        }

        /**
         * @brief Remove the first element from the deque
         */
        void pop_front()
        {
            this->deque.pop_front(VALUE_BIT_SIZE);
        }

        /**
         * @brief Insert an element at a specific iterator position
         *
         * @param position Iterator pointing to the insertion position
         * @param value The value to insert
         */
        void insert(size_t position, T value)
        {
            uint64_t value2 = value;
            uint64_t bitsize = sizeof(T) * 8;
            value2 = value2 << (64 - bitsize);
            this->deque.insert_64bit_string(position * bitsize, value2, bitsize);
        }

        void erase(size_t position)
        {
            uint64_t bitsize = sizeof(T) * 8;
            this->deque.erase(position * bitsize, VALUE_BIT_SIZE);
        }

        /**
         * @brief Get the current number of elements
         *
         * @return size_t The number of elements in the deque
         */
        size_t size() const
        {
            return this->deque.size() / (sizeof(T) * 8);
        }

        /**
         * @brief Swap contents with another IntegerArrayDeque
         *
         * @param item The other deque to swap with
         */
        void swap(IntegerArrayDeque &item)
        {
            std::swap(this->deque, item.deque);
        }

        /**
         * @brief Access element by index (const version)
         *
         * @param index The index of the element
         * @return T The value at the specified index
         */
        T operator[](size_t index) const
        {
            uint64_t p = index * VALUE_BIT_SIZE;
            CircularBitPointer bp = this->deque.get_position_pointer(p);
            uint64_t value = this->deque.read_64_bit_string(bp);
            return value >> (64 - VALUE_BIT_SIZE);
        }

        /**
         * @brief Set a value at a specific index
         *
         * @param index The index where to set the value
         * @param value The value to set
         */
        void set_value(int64_t index, uint64_t value)
        {
            uint64_t bitsize = VALUE_BIT_SIZE;
            uint64_t p = index * bitsize;
            uint64_t value2 = value;
            value2 = value2 << (64 - bitsize);
            this->deque.replace_64bit_string(p, value2, bitsize);
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

        IntegerDequeIterator begin() const
        {
            return IntegerDequeIterator(this->deque.begin());
        }

        IntegerDequeIterator end() const
        {
            return IntegerDequeIterator(this->deque.end());
        }
        /**
         * @brief Calculate the total memory usage in bytes
         * 
         * @return uint64_t Total memory usage including object overhead and buffer
         */
        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            return this->deque.size_in_bytes(only_extra_bytes);
        }

        void increment(uint64_t pos, int64_t delta){
            CircularBitPointer bp = this->deque.get_position_pointer(pos * VALUE_BIT_SIZE);
            T value = this->deque.read_64_bit_string(bp);
            value += delta;
            uint64_t value2 = value;
            this->deque.replace(bp, value2, VALUE_BIT_SIZE);
        }
        void decrement(uint64_t pos, int64_t delta){
            CircularBitPointer bp = this->deque.get_position_pointer(pos * VALUE_BIT_SIZE);
            T value = this->deque.read_64_bit_string(bp);
            value -= delta;
            uint64_t value2 = value;
            this->deque.replace(bp, value2, VALUE_BIT_SIZE);
        }
        std::deque<T> to_deque() const{
            std::deque<T> deq;
            for(auto it : *this){
                deq.push_back(it);
            }
            return deq;
        }

    };

    /**
     * @brief Type alias for IntegerArrayDeque with uint16_t index type
     */
    using IntegerArrayDeque16 = IntegerArrayDeque<uint16_t>;

    /**
     * @brief Type alias for IntegerArrayDeque with uint32_t index type
     */
    using IntegerArrayDeque32 = IntegerArrayDeque<uint32_t>;

    /**
     * @brief Type alias for IntegerArrayDeque with uint64_t index type
     */
    using IntegerArrayDeque64 = IntegerArrayDeque<uint64_t>;

}
