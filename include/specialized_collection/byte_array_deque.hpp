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
    class ByteArrayDeque
    {
        inline static std::vector<int> size_array{0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
        enum class ByteType
        {
            U8 = 1,
            U16 = 2,
            U32 = 3,
            U64 = 4
        };

        using T = uint64_t;
        using BUFFER_POS = INDEX_TYPE;
        using DEQUE_POS = INDEX_TYPE;
        uint8_t *circular_buffer_ = nullptr;
        INDEX_TYPE circular_buffer_size_ = 0;
        INDEX_TYPE starting_position_ = 0;
        INDEX_TYPE deque_size_ = 0;
        // uint8_t value_byte_size_ = 1;
        uint8_t value_byte_type_ = 1;

        static uint64_t get_appropriate_circular_buffer_size_index(int64_t size)
        {
            for (uint64_t i = 0; i < size_array.size(); i++)
            {
                if (size_array[i] >= size)
                {
                    return i;
                }
            }
            throw std::runtime_error("size is too large");
        }

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
         * @brief Calculate the total memory usage in bytes
         *
         * @return uint64_t The total size in bytes
         */
        uint64_t size_in_bytes() const
        {
            return sizeof(this->circular_buffer_size_) + sizeof(this->starting_position_) + sizeof(this->deque_size_) + sizeof(this->circular_buffer_) + (sizeof(uint8_t) * this->circular_buffer_size_);
        }

        /**
         * @brief Move constructor
         *
         * @param other The source IntegerDeque to move from
         */
        ByteArrayDeque(ByteArrayDeque &&other) noexcept
            : circular_buffer_(other.circular_buffer_),
              circular_buffer_size_(other.circular_buffer_size_),
              starting_position_(other.starting_position_),
              deque_size_(other.deque_size_),
              value_byte_type_(other.value_byte_type_)
        {
            // Reset the source object
            other.circular_buffer_ = nullptr;
            other.circular_buffer_size_ = 0;
            other.starting_position_ = 0;
            other.deque_size_ = 0;
            other.value_byte_type_ = 0;
        }

        ByteArrayDeque(const std::vector<uint64_t> &items)
        {
            this->clear();

            for (uint64_t i = 0; i < items.size(); i++)
            {
                this->push_back(items[i]);

                assert(this->at(i) == items[i]);
                assert(this->at(0) == items[0]);
            }
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
            ByteArrayDeque tmp;
            this->swap(tmp);
        }

        /**
         * @brief Iterator class for IntegerDeque
         *
         * Provides random access iteration over the deque elements
         */
        class ByteArrayDequeIterator
        {

        public:
            ByteArrayDeque *_m_deq;
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
            ByteArrayDequeIterator(ByteArrayDeque *_deque, INDEX_TYPE _idx) : _m_deq(_deque), _m_idx(_idx) {}

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
            ByteArrayDequeIterator &operator++()
            {
                ++this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-increment operator
             *
             * @return IntegerDequeIterator The iterator before increment
             */
            ByteArrayDequeIterator operator++(int)
            {
                ByteArrayDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            /**
             * @brief Pre-decrement operator
             *
             * @return IntegerDequeIterator& Reference to the decremented iterator
             */
            ByteArrayDequeIterator &operator--()
            {
                --this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-decrement operator
             *
             * @return IntegerDequeIterator The iterator before decrement
             */
            ByteArrayDequeIterator operator--(int)
            {
                ByteArrayDequeIterator temp = *this;
                --(*this);
                return temp;
            }

            /**
             * @brief Addition operator (iterator + offset)
             *
             * @param n The offset to add
             * @return IntegerDequeIterator The new iterator position
             */
            ByteArrayDequeIterator operator+(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return ByteArrayDequeIterator(this->_m_deq, sum);
            }

            /**
             * @brief Addition assignment operator
             *
             * @param n The offset to add
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            ByteArrayDequeIterator &operator+=(difference_type n)
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
            ByteArrayDequeIterator operator-(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx - (int16_t)n;
                return ByteArrayDequeIterator(this->_m_deq, sum);
            }

            /**
             * @brief Subtraction assignment operator
             *
             * @param n The offset to subtract
             * @return IntegerDequeIterator& Reference to the modified iterator
             */
            ByteArrayDequeIterator &operator-=(difference_type n)
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
            difference_type operator-(const ByteArrayDequeIterator &other) const
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
            bool operator==(const ByteArrayDequeIterator &other) const { return _m_idx == other._m_idx; }

            /**
             * @brief Inequality comparison operator
             */
            bool operator!=(const ByteArrayDequeIterator &other) const { return _m_idx != other._m_idx; }

            /**
             * @brief Less than comparison operator
             */
            bool operator<(const ByteArrayDequeIterator &other) const { return this->_m_idx < other._m_idx; }

            /**
             * @brief Greater than comparison operator
             */
            bool operator>(const ByteArrayDequeIterator &other) const { return this->_m_idx > other._m_idx; }

            /**
             * @brief Less than or equal comparison operator
             */
            bool operator<=(const ByteArrayDequeIterator &other) const { return this->_m_idx <= other._m_idx; }

            /**
             * @brief Greater than or equal comparison operator
             */
            bool operator>=(const ByteArrayDequeIterator &other) const { return this->_m_idx >= other._m_idx; }
        };

        /**
         * @brief Default constructor
         *
         * Initializes an empty deque with default capacity
         */
        ByteArrayDeque()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
            this->circular_buffer_ = nullptr;
            // this->circular_buffer_[0] = 0;
            /// this->circular_buffer_[1] = 0;

            this->starting_position_ = 0;
            this->circular_buffer_size_ = 0;
            this->deque_size_ = 0;
            this->value_byte_type_ = 1;
        }

        /**
         * @brief Destructor
         *
         * Frees the allocated memory
         */
        ~ByteArrayDeque()
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
        ByteArrayDequeIterator begin() const
        {
            return ByteArrayDequeIterator(const_cast<ByteArrayDeque<INDEX_TYPE> *>(this), 0);
        }

        /**
         * @brief Get iterator to the end
         *
         * @return IntegerDequeIterator Iterator pointing past the last element
         */
        ByteArrayDequeIterator end() const
        {
            return ByteArrayDequeIterator(const_cast<ByteArrayDeque<INDEX_TYPE> *>(this), this->deque_size_);
        }

        /*
        uint64_t get_buffer_bit() const
        {
            return stool::LSBByte::get_code_length(this->circular_buffer_size_ / this->value_byte_size_);
        }
        */

        /**
         * @brief Update the size if needed based on current usage
         *
         * Automatically resizes the deque when it's too full or too empty
         */
        void update_size_if_needed()
        {
            uint64_t new_capacity_size_index = get_appropriate_circular_buffer_size_index(this->deque_size_);
            uint64_t old_capacity_size_index = get_appropriate_circular_buffer_size_index(this->circular_buffer_size_ >> (this->value_byte_type_ - 1));

            if (new_capacity_size_index > old_capacity_size_index)
            {
                this->shrink_to_fit2(new_capacity_size_index, this->value_byte_type_);
            }
            else if (new_capacity_size_index + 1 < old_capacity_size_index)
            {
                this->shrink_to_fit2(new_capacity_size_index + 1, this->value_byte_type_);
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
        /*
        void shrink_to_fit()
        {
            uint64_t bit_size = stool::LSBByte::get_code_length(this->deque_size_ + 1);
            this->shrink_to_fit(bit_size);
        }
        */

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

        static uint8_t get_byte_size2(uint8_t value_type)
        {
            return 1 << (value_type - 1);
        }

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

        /**
         * @brief Add an element to the back of the deque
         *
         * @param value The value to add
         */
        void push_back(const T &value)
        {

            uint64_t new_byte_type = std::max((uint8_t)get_byte_type(value), this->value_byte_type_);
            uint64_t new_capacity_size_index = get_appropriate_circular_buffer_size_index(this->deque_size_ + 1);
            uint64_t old_capacity_size_index = get_appropriate_circular_buffer_size_index(this->circular_buffer_size_ >> (this->value_byte_type_ - 1));

            if (new_byte_type > this->value_byte_type_ || new_capacity_size_index > old_capacity_size_index)
            {
                this->shrink_to_fit2(new_capacity_size_index, new_byte_type);
            }
            uint64_t pos = this->size();
            this->deque_size_++;
            this->set_value(pos, value);

            uint64_t deque_total_byte_size = this->deque_size_ << (this->value_byte_type_ - 1);

            if (deque_total_byte_size > this->circular_buffer_size_)
            {
                std::cout << new_byte_type << "/" << new_capacity_size_index << "/" << old_capacity_size_index << std::endl;
                this->print_info();
                throw std::invalid_argument("push_back");
            }
            assert(deque_total_byte_size <= this->circular_buffer_size_);
        }

        /**
         * @brief Add an element to the front of the deque
         *
         * @param value The value to add
         */
        void push_front(const T &value)
        {
            uint64_t new_byte_type = std::max((uint8_t)get_byte_type(value), this->value_byte_type_);
            uint64_t new_capacity_size_index = get_appropriate_circular_buffer_size_index(this->deque_size_ + 1);
            uint64_t old_capacity_size_index = get_appropriate_circular_buffer_size_index(this->circular_buffer_size_ >> (this->value_byte_type_ - 1));

            if (new_byte_type > this->value_byte_type_ || new_capacity_size_index > old_capacity_size_index)
            {
                this->shrink_to_fit2(new_capacity_size_index, new_byte_type);
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

                this->starting_position_ = this->circular_buffer_size_ - value_byte_size;
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
            // std::cout << "POP" << std::endl;
            this->deque_size_--;
            this->update_size_if_needed();
        }

        /**
         * @brief Remove the first element from the deque
         */
        void pop_front()
        {
            uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);
            if (this->starting_position_ + value_byte_size < this->circular_buffer_size_)
            {
                this->starting_position_ += value_byte_size;
                this->deque_size_--;
            }
            else if (this->starting_position_ + value_byte_size == this->circular_buffer_size_)
            {
                this->starting_position_ = 0;
                this->deque_size_--;
            }
            else
            {
                throw std::invalid_argument("pop_front");
            }
            this->update_size_if_needed();
        }

        /**
         * @brief Insert an element at a specific iterator position
         *
         * @param position Iterator pointing to the insertion position
         * @param value The value to insert
         */
        void insert(const ByteArrayDequeIterator &position, const T &value)
        {
            uint64_t pos = position._m_idx;
            this->insert(pos, value);
        }

        /**
         * @brief Erase an element at a specific iterator position
         *
         * @param position Iterator pointing to the element to erase
         */
        void erase(const ByteArrayDequeIterator &position)
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
                uint64_t new_byte_type = std::max((uint8_t)get_byte_type(value), this->value_byte_type_);
                uint64_t new_capacity_size_index = get_appropriate_circular_buffer_size_index(this->deque_size_ + 1);
                uint64_t old_capacity_size_index = get_appropriate_circular_buffer_size_index(this->circular_buffer_size_ >> (this->value_byte_type_ - 1));

                this->reset_starting_position();

                if (new_byte_type > this->value_byte_type_ || new_capacity_size_index > old_capacity_size_index)
                {
                    this->shrink_to_fit2(new_capacity_size_index, new_byte_type);
                }
                uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);

                uint64_t src_pos = position * value_byte_size;
                uint64_t dst_pos = src_pos + value_byte_size;
                uint64_t move_size = this->circular_buffer_size_ - dst_pos;

                memmove(this->circular_buffer_ + dst_pos, this->circular_buffer_ + src_pos, move_size);

                this->deque_size_++;
                this->set_value(position, value);
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
                uint64_t new_capacity_size_index = get_appropriate_circular_buffer_size_index(this->deque_size_ - 1);
                uint64_t old_capacity_size_index = get_appropriate_circular_buffer_size_index(this->circular_buffer_size_ >> (this->value_byte_type_ - 1));

                this->reset_starting_position();
                if (new_capacity_size_index + 1 < old_capacity_size_index)
                {
                    this->shrink_to_fit2(new_capacity_size_index + 1, this->value_byte_type_);
                }

                uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);

                uint64_t dst_pos = position * value_byte_size;
                uint64_t src_pos = dst_pos + value_byte_size;
                uint64_t move_size = this->circular_buffer_size_ - src_pos;

                memmove(this->circular_buffer_ + dst_pos, this->circular_buffer_ + src_pos, move_size);
                this->deque_size_--;
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

        void reset_starting_position()
        {
            if (this->starting_position_ != 0)
            {
                std::array<uint8_t, 65536> tmp_array;
                std::memcpy(tmp_array.data(), this->circular_buffer_, this->circular_buffer_size_);

                uint64_t prefix_size = this->circular_buffer_size_ - this->starting_position_;
                uint64_t suffix_size = this->circular_buffer_size_ - prefix_size;
                std::memcpy(this->circular_buffer_, &tmp_array[this->starting_position_], prefix_size);
                if (this->starting_position_ > 0)
                {
                    std::memcpy(this->circular_buffer_ + prefix_size, tmp_array.data(), suffix_size);
                }
                this->starting_position_ = 0;
            }
        }

        void shrink_to_fit2(uint64_t capacity_size_index, uint8_t new_byte_type)
        {
            uint64_t new_byte_size = get_byte_size2(new_byte_type);
            uint64_t new_capacity_byte_size = size_array[capacity_size_index] * new_byte_size;
            uint64_t old_byte_size = get_byte_size2(this->value_byte_type_);

            if (new_capacity_byte_size > ByteArrayDeque<INDEX_TYPE>::max_deque_size())
            {
                assert(new_capacity_byte_size > ByteArrayDeque<INDEX_TYPE>::max_deque_size());

                throw std::invalid_argument("shrink_to_fit");
            }
            else if (old_byte_size != new_byte_size)
            {
                std::array<uint8_t, 65536> tmp_array;
                uint64_t i = 0;
                for (uint64_t it : *this)
                {
                    std::memcpy(&tmp_array[i * new_byte_size], &it, new_byte_size);
                    i++;
                }

                uint8_t *new_data = new uint8_t[new_capacity_byte_size];
                std::memcpy(new_data, tmp_array.data(), new_capacity_byte_size);

                if (this->circular_buffer_ != nullptr)
                {
                    delete[] this->circular_buffer_;
                    this->circular_buffer_ = nullptr;
                }

                this->circular_buffer_ = new_data;
                this->starting_position_ = 0;
                this->circular_buffer_size_ = new_capacity_byte_size;
                this->value_byte_type_ = new_byte_type;
            }
            else if (new_capacity_byte_size > this->circular_buffer_size_ || new_capacity_byte_size < this->circular_buffer_size_)
            {

                this->reset_starting_position();

                uint8_t *new_data = new uint8_t[new_capacity_byte_size];

                if (new_capacity_byte_size > this->circular_buffer_size_)
                {
                    std::memcpy(new_data, this->circular_buffer_, this->circular_buffer_size_);
                }
                else
                {
                    std::memcpy(new_data, this->circular_buffer_, new_capacity_byte_size);
                }

                if (this->circular_buffer_ != nullptr)
                {
                    delete[] this->circular_buffer_;
                    this->circular_buffer_ = nullptr;
                }

                this->circular_buffer_ = new_data;
                this->starting_position_ = 0;
                this->circular_buffer_size_ = new_capacity_byte_size;
                this->value_byte_type_ = new_byte_type;
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

            for (ByteArrayDequeIterator it = this->begin(); it != this->end(); ++it)
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
            std::cout << "ByteArrayDeque ===============" << std::endl;
            std::string buffer_str = "";
            for (uint64_t t = 0; t < this->circular_buffer_size_; t++)
            {
                std::bitset<8> p(this->circular_buffer_[t]);
                buffer_str += p.to_string();
                buffer_str += " ";
            }
            std::deque<uint64_t> deque_values = this->to_deque();
            stool::DebugPrinter::print_integers(deque_values, "Deque");
            std::cout << "Buffer: " << buffer_str << std::endl;
            std::cout << "Buffer size: " << (int64_t)this->circular_buffer_size_ << std::endl;
            std::cout << "Value byte type: " << (int64_t)this->value_byte_type_ << std::endl;
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
        void swap(ByteArrayDeque &item)
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
            uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);
            uint64_t pos = this->starting_position_ + (index << (this->value_byte_type_ - 1));
            uint64_t mask = this->circular_buffer_size_ - 1;
            uint64_t pos2 = pos & mask;

            uint64_t B = 0;
            ByteType v = (ByteType)this->value_byte_type_;

            switch (v)
            {
            case ByteType::U8:
                B = this->circular_buffer_[pos2];
                break;
            case ByteType::U16:
                B = static_cast<uint64_t>(this->circular_buffer_[pos2 + 0]) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 1]) << 8);
                break;
            case ByteType::U32:
                B = static_cast<uint64_t>(this->circular_buffer_[pos2 + 0]) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 1]) << 8) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 2]) << 16) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 3]) << 24);
                break;
            case ByteType::U64:
                B = static_cast<uint64_t>(this->circular_buffer_[pos2 + 0]) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 1]) << 8) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 2]) << 16) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 3]) << 24) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 4]) << 32) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 5]) << 40) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 6]) << 48) |
                    (static_cast<uint64_t>(this->circular_buffer_[pos2 + 7]) << 56);
                break;
            default:
                break;
            }
            return B;

            // std::memcpy(&B, this->circular_buffer_ + pos2, value_byte_size);
        }

        /**
         * @brief Set a value at a specific index
         *
         * @param index The index where to set the value
         * @param value The value to set
         */
        void set_value(int64_t index, uint64_t value)
        {
            uint64_t new_byte_type = std::max((uint8_t)get_byte_type(value), this->value_byte_type_);
            if (new_byte_type > this->value_byte_type_)
            {
                this->shrink_to_fit2(get_appropriate_circular_buffer_size_index(this->deque_size_), new_byte_type);
            }

            uint64_t pos = this->starting_position_ + (index << (this->value_byte_type_ - 1));
            uint64_t mask = this->circular_buffer_size_ - 1;
            uint64_t pos2 = pos & mask;
            uint64_t B = value;

            ByteType v = (ByteType)this->value_byte_type_;

            switch (v)
            {
            case ByteType::U8:
                this->circular_buffer_[pos2] = B;
                break;
            case ByteType::U16:
                std::memcpy(this->circular_buffer_ + pos2, &B, 2);
                break;
            case ByteType::U32:
                std::memcpy(this->circular_buffer_ + pos2, &B, 4);
                break;
            case ByteType::U64:
                std::memcpy(this->circular_buffer_ + pos2, &B, 8);

                break;
            }
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

        uint64_t psum(uint64_t i) const
        {
            uint64_t x = 0;
            uint64_t sum = 0;
            for (ByteArrayDequeIterator it = this->begin(); it != this->end(); ++it)
            {
                sum += *it;

                if (i == x)
                {
                    break;
                }
                else
                {
                    x++;
                }
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
            uint64_t x = 0;
            sum = 0;

            uint64_t size = this->size();
            for (uint64_t i = 0; i < size; i++)
            {
                sum += this->at(i);
                if (sum >= value)
                {
                    return i;
                }
            }
            return -1;

        }
        int64_t search2(uint64_t value, uint64_t &sum) const
        {
            uint64_t x = 0;
            sum = 0;

            uint64_t size = this->size();
            for (uint64_t i = 0; i < size; i++)
            {
                uint64_t v = this->at(i);
                if (value < sum + v)
                {
                    return i;
                }
                sum += v;
            }
            return -1;

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
            if (only_extra_bytes)
            {
                return sizeof(uint8_t) * this->circular_buffer_size_;
            }
            else
            {
                return sizeof(ByteArrayDeque) + (sizeof(uint8_t) * this->circular_buffer_size_);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            uint64_t sum = this->circular_buffer_size_;

            uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);
            uint32_t used_byte_size = this->size() * value_byte_size;
            return sum - used_byte_size;
        }
    };

    /**
     * @brief Type alias for IntegerDeque with uint16_t index type
     */
    using ByteArrayDeque16 = ByteArrayDeque<uint16_t>;

    /**
     * @brief Type alias for IntegerDeque with uint32_t index type
     */
    using ByteArrayDeque32 = ByteArrayDeque<uint32_t>;

    /**
     * @brief Type alias for IntegerDeque with uint64_t index type
     */
    using ByteArrayDeque64 = ByteArrayDeque<uint64_t>;

}
