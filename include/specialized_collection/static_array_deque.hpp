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
    class StaticArrayDeque
    {
        enum class ByteType
        {
            U8 = 1,
            U16 = 2,
            U32 = 3,
            U64 = 4
        };
        static constexpr uint64_t BUFFER_SIZE = SIZE * 8;
        using BufferIndex = uint64_t;
        using ElementIndex = uint64_t;

        std::array<uint8_t, BUFFER_SIZE> circular_buffer_;
        uint64_t starting_position_ = 0;
        uint64_t deque_size_ = 0;
        uint8_t value_byte_type_ = 1;

        static constexpr bool is_power_of_two = (SIZE != 0) && ((SIZE & (SIZE - 1)) == 0);

        static ByteType get_byte_type(uint64_t value)
        {
            ByteType new_byte_type = ByteType::U8;
            if (value <= (uint64_t)UINT8_MAX)
            {
                new_byte_type = ByteType::U8;
            }
            else if (value <= (uint64_t)UINT16_MAX)
            {
                new_byte_type = ByteType::U16;
            }
            else if (value <= (uint64_t)UINT32_MAX)
            {
                new_byte_type = ByteType::U32;
            }
            else
            {
                new_byte_type = ByteType::U64;
            }
            return new_byte_type;
        }

    public:
        /**
         * @brief Calculate the total memory usage in bytes
         *
         * @return uint64_t The total size in bytes
         */
        uint64_t size_in_bytes() const
        {
            return sizeof(StaticArrayDeque);
        }

        StaticArrayDeque(const std::vector<uint64_t> &items)
        {
            if constexpr (!is_power_of_two)
            {
                throw std::invalid_argument("SIZE must be a power of two");
            }
            this->clear();

            uint64_t test_sum = 0;

            for (uint64_t i = 0; i < items.size(); i++)
            {
                this->push_back(items[i]);
                test_sum += items[i];
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
            this->starting_position_ = 0;
            this->value_byte_type_ = 1;
        }

        /**
         * @brief Iterator class for IntegerDeque
         *
         * Provides random access iteration over the deque elements
         */
        class StaticArrayDequeIterator
        {

        public:
            StaticArrayDeque *_m_deq;
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
            StaticArrayDequeIterator(StaticArrayDeque *_deque, uint64_t _idx) : _m_deq(_deque), _m_idx(_idx) {}

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
            StaticArrayDequeIterator &operator++()
            {
                ++this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-increment operator
             *
             * @return IntegerDequeIterator The iterator before increment
             */
            StaticArrayDequeIterator operator++(int)
            {
                StaticArrayDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            /**
             * @brief Pre-decrement operator
             *
             * @return IntegerDequeIterator& Reference to the decremented iterator
             */
            StaticArrayDequeIterator &operator--()
            {
                --this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-decrement operator
             *
             * @return IntegerDequeIterator The iterator before decrement
             */
            StaticArrayDequeIterator operator--(int)
            {
                StaticArrayDequeIterator temp = *this;
                --(*this);
                return temp;
            }

            /**
             * @brief Addition operator (iterator + offset)
             *
             * @param n The offset to add
             * @return IntegerDequeIterator The new iterator position
             */
            StaticArrayDequeIterator operator+(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return StaticArrayDequeIterator(this->_m_deq, sum);
            }

            /**
             * @brief Addition assignment operator
             *
             * @param n The offset to add
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            StaticArrayDequeIterator &operator+=(difference_type n)
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
            StaticArrayDequeIterator operator-(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx - (int16_t)n;
                return StaticArrayDequeIterator(this->_m_deq, sum);
            }

            /**
             * @brief Subtraction assignment operator
             *
             * @param n The offset to subtract
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            StaticArrayDequeIterator &operator-=(difference_type n)
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
            difference_type operator-(const StaticArrayDequeIterator &other) const
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
            bool operator==(const StaticArrayDequeIterator &other) const { return _m_idx == other._m_idx; }

            /**
             * @brief Inequality comparison operator
             */
            bool operator!=(const StaticArrayDequeIterator &other) const { return _m_idx != other._m_idx; }

            /**
             * @brief Less than comparison operator
             */
            bool operator<(const StaticArrayDequeIterator &other) const { return this->_m_idx < other._m_idx; }

            /**
             * @brief Greater than comparison operator
             */
            bool operator>(const StaticArrayDequeIterator &other) const { return this->_m_idx > other._m_idx; }

            /**
             * @brief Less than or equal comparison operator
             */
            bool operator<=(const StaticArrayDequeIterator &other) const { return this->_m_idx <= other._m_idx; }

            /**
             * @brief Greater than or equal comparison operator
             */
            bool operator>=(const StaticArrayDequeIterator &other) const { return this->_m_idx >= other._m_idx; }
        };

        /**
         * @brief Default constructor
         *
         * Initializes an empty deque with default capacity
         */
        StaticArrayDeque()
        {
            this->starting_position_ = 0;
            this->deque_size_ = 0;
        }

        /**
         * @brief Destructor
         *
         * Frees the allocated memory
         */
        ~StaticArrayDeque()
        {
            this->clear();
        }

        /**
         * @brief Get iterator to the beginning
         *
         * @return IntegerDequeIterator Iterator pointing to the first element
         */
        StaticArrayDequeIterator begin() const
        {
            return StaticArrayDequeIterator(const_cast<StaticArrayDeque<SIZE> *>(this), 0);
        }

        /**
         * @brief Get iterator to the end
         *
         * @return IntegerDequeIterator Iterator pointing past the last element
         */
        StaticArrayDequeIterator end() const
        {
            return StaticArrayDequeIterator(const_cast<StaticArrayDeque<SIZE> *>(this), this->deque_size_);
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

            if (this->deque_size_ + 1 >= SIZE)
            {
                throw std::out_of_range("Size out of range");
            }

            uint64_t new_byte_type = std::max((uint8_t)get_byte_type(value), this->value_byte_type_);
            if (new_byte_type != this->value_byte_type_)
            {
                this->change_element_byte_type(new_byte_type);
            }

            uint64_t pos = this->size();
            this->deque_size_++;
            this->set_value(pos, value);
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

            if (this->deque_size_ + 1 >= SIZE)
            {
                throw std::out_of_range("Size out of range");
            }

            uint64_t new_byte_type = std::max((uint8_t)get_byte_type(value), this->value_byte_type_);
            if (new_byte_type != this->value_byte_type_)
            {
                this->change_element_byte_type(new_byte_type);
            }

            uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);

            if (this->starting_position_ >= value_byte_size)
            {
                this->starting_position_ -= value_byte_size;
                this->deque_size_++;
                this->set_value(0, value);
            }
            else if (this->starting_position_ == 0)
            {
                this->starting_position_ = BUFFER_SIZE - value_byte_size;
                this->deque_size_++;
                this->set_value(0, value);
            }
            else
            {
                throw std::invalid_argument("push_front");
            }

        }

        /**
         * @brief Remove the last element from the deque
         */
        void pop_back()
        {
            if (this->deque_size_ == 0)
            {
                throw std::out_of_range("Size out of range");
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
                throw std::out_of_range("Size out of range");
            }

            uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);

            if (this->starting_position_ + value_byte_size != BUFFER_SIZE)
            {
                this->starting_position_ += value_byte_size;
                this->deque_size_--;
            }
            else if (this->starting_position_ + value_byte_size == BUFFER_SIZE)
            {
                this->starting_position_ = 0;
                this->deque_size_--;
            }
            else
            {
                throw std::invalid_argument("pop_front");
            }
        }

        /**
         * @brief Insert an element at a specific iterator position
         *
         * @param position Iterator pointing to the insertion position
         * @param value The value to insert
         */
        void insert(const StaticArrayDequeIterator &position, const uint64_t &value)
        {
            uint64_t pos = position._m_idx;
            this->insert(pos, value);
        }

        /**
         * @brief Erase an element at a specific iterator position
         *
         * @param position Iterator pointing to the element to erase
         */
        void erase(const StaticArrayDequeIterator &position)
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

            if (size + 1 >= SIZE)
            {
                throw std::out_of_range("Size out of range");
            }

            if (position > SIZE + 1)
            {
                throw std::out_of_range("Position out of range");
            }

            if (position == 0)
            {
                this->push_front(value);
            }
            else if (position == size)
            {
                this->push_back(value);
            }
            else
            {

                this->reset_starting_position();

                uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);

                this->deque_size_++;
                uint64_t deque_byte_size = this->deque_size_ << (this->value_byte_type_ - 1);
                uint64_t src_pos = position << (this->value_byte_type_ - 1);
                uint64_t dst_pos = src_pos + value_byte_size;
                uint64_t move_size = deque_byte_size - dst_pos;

                memmove(&this->circular_buffer_[dst_pos], &this->circular_buffer_[src_pos], move_size);

                this->set_value(position, value);
            }

            assert(this->at(position) == value);
        }

        /**
         * @brief Erase an element at a specific position
         *
         * @param position The position of the element to erase
         */
        void erase(ElementIndex position)
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

                this->reset_starting_position();



                uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);
                uint64_t deque_byte_size = this->deque_size_ << (this->value_byte_type_ - 1);
                uint64_t dst_pos = position << (this->value_byte_type_ - 1);
                uint64_t src_pos = dst_pos + value_byte_size;
                uint64_t move_size = deque_byte_size - src_pos;

                memmove(&this->circular_buffer_[dst_pos], &this->circular_buffer_[src_pos], move_size);
                this->deque_size_--;


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

        void reset_starting_position()
        {
            if (this->starting_position_ != 0)
            {
                std::array<uint8_t, BUFFER_SIZE> tmp_array;
                std::memcpy(tmp_array.data(), this->circular_buffer_.data(), BUFFER_SIZE);

                uint64_t prefix_size = BUFFER_SIZE - this->starting_position_;
                uint64_t suffix_size = BUFFER_SIZE - prefix_size;
                std::memcpy(this->circular_buffer_.data(), &tmp_array[this->starting_position_], prefix_size);
                if (this->starting_position_ > 0)
                {
                    std::memcpy(&this->circular_buffer_[prefix_size], tmp_array.data(), suffix_size);
                }
                this->starting_position_ = 0;
            }
        }

        uint64_t naive_psum() const
        {
            uint64_t sum = 0;
            uint64_t size = this->deque_size_;

            for (uint64_t i = 0; i < size; i++)
            {
                sum += this->at(i);
            }
            return sum;
        }

        /**
         * @brief Convert the deque to a std::deque
         *
         * @return std::deque<T> A standard deque containing the same elements
         */
        std::deque<uint64_t> to_deque() const
        {

            std::deque<uint64_t> r;

            for (StaticArrayDequeIterator it = this->begin(); it != this->end(); ++it)
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
            std::cout << "StaticArrayDeque ===============" << std::endl;
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
            //std::cout << "Buffer: " << buffer_str << std::endl;
            std::cout << "Buffer size: " << (int64_t)SIZE << std::endl;
            std::cout << "Starting position: " << (int64_t)this->starting_position_ << std::endl;
            std::cout << "Deque size: " << (int64_t)this->deque_size_ << std::endl;
            std::cout << "==============================" << std::endl;
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
        void swap(StaticArrayDeque &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            std::swap(this->starting_position_, item.starting_position_);
            std::swap(this->deque_size_, item.deque_size_);
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
         * @brief Set a value at a specific index
         *
         * @param index The index where to set the value
         * @param value The value to set
         */
        void set_value(ElementIndex index, uint64_t value)
        {
            uint64_t new_byte_type = std::max((uint8_t)get_byte_type(value), this->value_byte_type_);
            if (new_byte_type > this->value_byte_type_)
            {
                this->change_element_byte_type(new_byte_type);
            }

            uint64_t pos = this->starting_position_ + (index << (this->value_byte_type_ - 1));
            uint64_t mask = BUFFER_SIZE - 1;
            uint64_t pos2 = pos & mask;
            uint64_t B = value;

            ByteType v = (ByteType)this->value_byte_type_;

            switch (v)
            {
            case ByteType::U8:
                this->circular_buffer_[pos2] = B;
                break;
            case ByteType::U16:
                std::memcpy(&this->circular_buffer_[pos2], &B, 2);
                break;
            case ByteType::U32:
                std::memcpy(&this->circular_buffer_[pos2], &B, 4);
                break;
            case ByteType::U64:
                std::memcpy(&this->circular_buffer_[pos2], &B, 8);

                break;
            }
        }

        /**
         * @brief Access element by index with bounds checking
         *
         * @param i The index of the element
         * @return T The value at the specified index
         */
        uint64_t at(ElementIndex index) const
        {
            assert(index < this->size());
            uint64_t pos = this->starting_position_ + (index << (this->value_byte_type_ - 1));
            uint64_t mask = BUFFER_SIZE - 1;
            uint64_t pos2 = pos & mask;

            uint64_t B = 0;
            ByteType v = (ByteType)this->value_byte_type_;

            switch (v)
            {
            case ByteType::U8:
                B = this->circular_buffer_[pos2];
                break;
            case ByteType::U16:
                B = this->at16(pos2);
                break;
            case ByteType::U32:
                B = this->at32(pos2);
                break;
            case ByteType::U64:
                B = this->at64(pos2);
                break;
            default:
                break;
            }
            return B;
        }
        uint64_t at16(BufferIndex pos) const
        {
            return static_cast<uint64_t>(this->circular_buffer_[pos + 0]) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 1]) << 8);
        }
        uint64_t at32(BufferIndex pos) const
        {
            return static_cast<uint64_t>(this->circular_buffer_[pos + 0]) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 1]) << 8) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 2]) << 16) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 3]) << 24);
        }
        uint64_t at64(BufferIndex pos) const
        {
            return static_cast<uint64_t>(this->circular_buffer_[pos + 0]) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 1]) << 8) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 2]) << 16) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 3]) << 24) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 4]) << 32) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 5]) << 40) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 6]) << 48) |
                   (static_cast<uint64_t>(this->circular_buffer_[pos + 7]) << 56);
        }

        static uint8_t get_byte_size2(uint8_t value_type)
        {
            return 1 << (value_type - 1);
        }
        void change_element_byte_type(uint8_t new_byte_type)
        {
            uint64_t new_byte_size = get_byte_size2(new_byte_type);
            uint64_t old_byte_size = get_byte_size2(this->value_byte_type_);


            if (old_byte_size != new_byte_size)
            {
                std::array<uint8_t, BUFFER_SIZE> tmp_array;
                uint64_t i = 0;
                for (uint64_t it : *this)
                {
                    std::memcpy(&tmp_array[i * new_byte_size], &it, new_byte_size);
                    i++;
                }

                std::memcpy(this->circular_buffer_.data(), tmp_array.data(), this->deque_size_ * new_byte_size);
                this->starting_position_ = 0;
                this->value_byte_type_ = new_byte_type;
            }
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
                sum += this->at(x);
            }
            return sum;
        }
        int64_t search(uint64_t value) const
        {
            uint64_t sum = 0;
            return this->search(value, sum);
        }

        int64_t naive_search(uint64_t value, uint64_t &sum) const
        {
            sum = 0;

            uint64_t size = this->size();
            uint64_t pos = this->starting_position_;
            uint64_t mask = BUFFER_SIZE - 1;
            ByteType type = (ByteType)this->value_byte_type_;
            switch (type)
            {
            case ByteType::U8:
                for (uint64_t i = 0; i < size; i++)
                {
                    uint64_t v = this->circular_buffer_[pos & mask];
                    if (value <= sum + v)
                    {
                        return i;
                    }
                    pos++;
                    sum += v;
                }
                break;
            case ByteType::U16:
                for (uint64_t i = 0; i < size; i++)
                {
                    uint64_t v = this->at16(pos & mask);
                    if (value <= sum + v)
                    {
                        return i;
                    }
                    pos += 2;
                    sum += v;
                }
                break;
            case ByteType::U32:
                for (uint64_t i = 0; i < size; i++)
                {
                    uint64_t v = this->at32(pos & mask);
                    if (value <= sum + v)
                    {
                        return i;
                    }
                    pos += 4;
                    sum += v;
                }
                break;
            case ByteType::U64:
                for (uint64_t i = 0; i < size; i++)
                {
                    uint64_t v = this->at64(pos & mask);
                    if (value <= sum + v)
                    {
                        return i;
                    }
                    pos += 8;
                    sum += v;
                }
                break;
            default:
                break;
            }
            return -1;
        }
        int64_t search(uint64_t value, uint64_t &sum) const
        {
            return this->naive_search(value, sum);
        }

        void increment(uint64_t pos, int64_t delta)
        {
            this->set_value(pos, this->at(pos) + delta);
        }

        void decrement(uint64_t pos, int64_t delta)
        {
            this->set_value(pos, this->at(pos) - delta);
        }

        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            throw std::runtime_error("size_in_bytes is not supported");
        }

        uint64_t unused_size_in_bytes() const
        {
            throw std::runtime_error("unused_size_in_bytes is not supported");
        }
    };

}
