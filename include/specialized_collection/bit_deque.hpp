#pragma once
#include "circular_bit_pointer.hpp"

namespace stool
{

    /**
     * @brief A simple circular buffer-based deque implementation
     *
     * @tparam T The type of elements stored in the deque
     * @tparam INDEX_TYPE The type used for indexing (default: uint16_t)
     *
     * This class provides a memory-efficient deque implementation using a circular buffer.
     * It supports O(1) push/pop operations at both ends and random access to elements.
     * The buffer size is automatically managed to maintain optimal memory usage.
     */
    class BitDeque
    {
        inline static std::vector<int> size_array{1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 33, 40, 48, 58, 70, 84, 101, 122, 147, 177, 213, 256, 308, 370, 444, 533, 640, 768, 922, 1107, 1329, 1595, 1914, 2297, 2757, 3309, 3971, 4766};

    public:
        using INDEX_TYPE = uint16_t;
        uint64_t *circular_buffer_ = nullptr;
        uint16_t circular_buffer_size_;
        uint16_t first_block_index_;
        uint16_t last_block_index_;
        uint8_t first_bit_index_;
        uint8_t last_bit_index_;

        class BitDequeIterator
        {

        public:
            const BitDeque *_m_deq;
            uint16_t index;
            uint16_t block_index;
            uint16_t size = 0;
            uint8_t bit_index;

            using iterator_category = std::random_access_iterator_tag;
            using value_type = bool;
            using difference_type = std::ptrdiff_t;

            BitDequeIterator(const BitDeque *_deque, uint16_t _index, uint16_t _block_index, uint8_t _bit_index, uint16_t _size) : 
            _m_deq(_deque), index(_index), block_index(_block_index), size(_size), bit_index(_bit_index) {}

            bool operator*() const
            {
                return ((*_m_deq).circular_buffer_[this->block_index] >> (63 - this->bit_index)) & 1;
            }

            BitDequeIterator &operator++()
            {
                if (this->index + 1 < this->size)
                {
                    this->index++;
                    CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                    bp.add(1);
                    this->block_index = bp.block_index_;
                    this->bit_index = bp.bit_index_;
                }
                else if (this->index + 1 == this->size)
                {
                    this->index = UINT16_MAX;
                    this->block_index = UINT16_MAX;
                    this->bit_index = UINT8_MAX;
                }
                else
                {
                    throw std::invalid_argument("Error: BitDequeIterator::operator++()");
                }

                return *this;
            }

            BitDequeIterator operator++(int)
            {
                BitDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            BitDequeIterator &operator--()
            {
                if (this->index > 1)
                {
                    this->index--;
                    CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                    bp.subtract(1);
                    this->block_index = bp.block_index_;
                    this->bit_index = bp.bit_index_;
                }
                else
                {
                    throw std::invalid_argument("Error: BitDequeIterator::operator--()");
                }

                return *this;
            }

            BitDequeIterator operator--(int)
            {
                BitDequeIterator temp = *this;
                --(*this);
                return temp;
            }

            BitDequeIterator operator+(difference_type n) const
            {
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                bp.add(n);
                if (this->index + n < this->size)
                {
                    return BitDequeIterator(this->_m_deq, this->index + n, bp.block_index_, bp.bit_index_, this->size);
                }
                else
                {
                    return BitDequeIterator(this->_m_deq, UINT16_MAX, UINT16_MAX, UINT8_MAX, this->size);
                }
            }

            BitDequeIterator &operator+=(difference_type n)
            {
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                bp.add(n);
                this->index += n;
                this->block_index = bp.block_index_;
                this->bit_index = bp.bit_index_;
                if (this->index >= this->size)
                {
                    this->index = UINT16_MAX;
                    this->block_index = UINT16_MAX;
                    this->bit_index = UINT8_MAX;
                }
                return *this;
            }

            BitDequeIterator operator-(difference_type n) const
            {
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                bp.subtract(n);
                if (n <= this->index)
                {
                    return BitDequeIterator(this->_m_deq, this->index - n, bp.block_index_, bp.bit_index_, this->size);
                }
                else
                {
                    throw std::invalid_argument("Error: BitDequeIterator::operator-()");
                }
            }

            BitDequeIterator &operator-=(difference_type n)
            {
                if (this->index < n)
                {
                    throw std::invalid_argument("Error: BitDequeIterator::operator-()");
                }
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                bp.subtract(n);
                this->index -= n;
                this->block_index = bp.block_index_;
                this->bit_index = bp.bit_index_;
                return *this;

            }

            uint64_t read_64_bit_string() const
            {
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                return bp.read64(this->_m_deq->circular_buffer_);
            }

            difference_type operator-(const BitDequeIterator &other) const
            {
                return (int16_t)this->index - (int16_t)other.index;
            }

            /**
             * @brief Equality comparison
             */
            bool operator==(const BitDequeIterator &other) const { return this->index == other.index; }

            /**
             * @brief Inequality comparison
             */
            bool operator!=(const BitDequeIterator &other) const { return this->index != other.index; }

            /**
             * @brief Less than comparison
             */
            bool operator<(const BitDequeIterator &other) const { return this->index < other.index; }

            /**
             * @brief Greater than comparison
             */
            bool operator>(const BitDequeIterator &other) const { return this->index > other.index; }

            /**
             * @brief Less than or equal comparison
             */
            bool operator<=(const BitDequeIterator &other) const { return this->index <= other.index; }

            /**
             * @brief Greater than or equal comparison
             */
            bool operator>=(const BitDequeIterator &other) const { return this->index >= other.index; }
        };

        // INDEX_TYPE deque_size_;

        static uint64_t get_appropriate_circular_buffer_size_index(int64_t size)
        {
            for (uint64_t i = 0; i < size_array.size(); i++)
            {
                int64_t xsize = size_array[i] * 64;
                if (xsize > size)
                {
                    return i;
                }
            }
            throw std::runtime_error("size is too large");
        }
        int64_t get_current_circular_buffer_size_index() const
        {
            if (this->circular_buffer_size_ == 0)
            {
                return -1;
            }
            else
            {
                for (uint64_t i = 0; i < size_array.size(); i++)
                {
                    if (this->circular_buffer_size_ == size_array[i])
                    {
                        return i;
                    }
                }
            }
            throw std::runtime_error("circular_buffer_size_ is not found");
        }

    public:
        /**
         * @brief Get the maximum possible deque size for the given index type
         *
         * @return uint64_t The maximum number of elements that can be stored
         */
        static uint64_t max_deque_size()
        {
            return (uint64_t)UINT16_MAX * 64;
        }

        /**
         * @brief Calculate the total memory usage in bytes
         *
         * @return uint64_t Total memory usage including object overhead and buffer
         */
        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if (only_extra_bytes)
            {
                return sizeof(uint64_t) * this->circular_buffer_size_;
            }
            else
            {
                return sizeof(BitDeque) + (sizeof(uint64_t) * this->circular_buffer_size_);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return (this->capacity() - this->size()) * sizeof(uint64_t);
        }

        /**
         * @brief Copy constructor
         *
         * @param other The BitDeque to copy from
         */
        BitDeque(const BitDeque &other) noexcept
        {
            this->circular_buffer_size_ = other.circular_buffer_size_;
            this->first_block_index_ = other.first_block_index_;
            this->last_block_index_ = other.last_block_index_;
            this->first_bit_index_ = other.first_bit_index_;
            this->last_bit_index_ = other.last_bit_index_;
            this->circular_buffer_ = new uint64_t[this->circular_buffer_size_];

            std::memcpy(this->circular_buffer_, other.circular_buffer_, this->circular_buffer_size_ * sizeof(uint64_t));
        }

        BitDeque(const std::vector<bool> &bv) noexcept
        {
            if (bv.size() == 0)
            {
                this->initialize();
            }
            else
            {
                this->initialize();
                this->update_size_if_needed(bv.size());

                for (uint64_t i = 0; i < bv.size(); i++)
                {
                    this->push_back(bv[i]);
                }
                assert(this->size() == bv.size());
            }
        }

        /**
         * @brief Move constructor
         *
         * @param other The BitDeque to move from
         */
        BitDeque(BitDeque &&other) noexcept
            : circular_buffer_(other.circular_buffer_),
              circular_buffer_size_(other.circular_buffer_size_),
              first_block_index_(other.first_block_index_),
              last_block_index_(other.last_block_index_),
              first_bit_index_(other.first_bit_index_),
              last_bit_index_(other.last_bit_index_)
        {
            // Reset the source object
            other.circular_buffer_ = nullptr;
            other.circular_buffer_size_ = 0;
            other.first_block_index_ = 0;
            other.last_block_index_ = 0;
            other.first_bit_index_ = 0;
            other.last_bit_index_ = 0;
        }

        /**
         * @brief Move assignment operator
         *
         * @param other The BitDeque to move from
         * @return BitDeque& Reference to this object
         */
        BitDeque &operator=(BitDeque &&other) noexcept
        {
            if (this != &other)
            {
                this->circular_buffer_ = other.circular_buffer_;
                this->circular_buffer_size_ = other.circular_buffer_size_;
                this->first_block_index_ = other.first_block_index_;
                this->last_block_index_ = other.last_block_index_;
                this->first_bit_index_ = other.first_bit_index_;
                this->last_bit_index_ = other.last_bit_index_;

                other.circular_buffer_ = nullptr;
                other.circular_buffer_size_ = 0;
                other.first_block_index_ = 0;
                other.last_block_index_ = 0;
                other.first_bit_index_ = 0;
                other.last_bit_index_ = 0;
            }
            return *this;
        }

        /**
         * @brief Get the current buffer capacity
         *
         * @return size_t The number of elements the buffer can hold
         */
        size_t capacity() const
        {
            return this->circular_buffer_size_ * 64;
        }

        /**
         * @brief Remove all elements from the deque
         */
        void clear()
        {
            this->first_block_index_ = 0;
            this->first_bit_index_ = 0;
            this->last_block_index_ = UINT16_MAX;
            this->last_bit_index_ = UINT8_MAX;
            this->shrink_to_fit(0);
        }

        /**
         * @brief Default constructor
         *
         * Creates an empty deque with initial capacity of 2 elements.
         */
        BitDeque()
        {
            this->initialize();
        }

        void initialize()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
            this->circular_buffer_ = new uint64_t[2];
            this->circular_buffer_[0] = 0;
            this->circular_buffer_[1] = 0;

            this->first_block_index_ = UINT16_MAX;
            this->last_block_index_ = UINT16_MAX;
            this->first_bit_index_ = UINT8_MAX;
            this->last_bit_index_ = UINT8_MAX;
            this->circular_buffer_size_ = 2;
        }

        /**
         * @brief Constructor with specified buffer size
         *
         * @param _circular_buffer_size Initial capacity of the circular buffer
         */
        BitDeque(uint64_t _circular_buffer_size)
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
            this->circular_buffer_ = new uint64_t[_circular_buffer_size];
            this->first_block_index_ = UINT16_MAX;
            this->last_block_index_ = UINT16_MAX;
            this->first_bit_index_ = UINT8_MAX;
            this->last_bit_index_ = UINT8_MAX;
            this->circular_buffer_size_ = _circular_buffer_size;
        }

        /**
         * @brief Destructor
         *
         * Frees the allocated circular buffer memory.
         */
        ~BitDeque()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
        }

        /**
         * @brief Update buffer size if needed based on current usage
         *
         * Automatically resizes the buffer to maintain optimal memory efficiency.
         */
        void update_size_if_needed(int64_t new_size)
        {
            this->shrink_to_fit(new_size);
        }

        /**
         * @brief Check if the deque is empty
         *
         * @return bool True if the deque contains no elements
         */
        bool empty() const
        {
            return this->last_bit_index_ == UINT8_MAX;
        }

        /**
         * @brief Reduce buffer size to fit current content
         *
         * Resizes the buffer to the minimum size needed to hold current elements.
         */
        void shrink_to_fit(int64_t new_size)
        {
            new_size = std::max(new_size, (int64_t)0);

            int64_t current_size_index = this->get_current_circular_buffer_size_index();
            int64_t appropriate_size_index = BitDeque::get_appropriate_circular_buffer_size_index(new_size);

            uint64_t old_buffer_size = this->circular_buffer_size_;
            uint64_t old_size = this->size();

            if (appropriate_size_index + 1 < current_size_index || appropriate_size_index > current_size_index)
            {
                std::array<uint64_t, 4096> tmp_array;
                std::memcpy(tmp_array.data(), this->circular_buffer_, this->circular_buffer_size_ * sizeof(uint64_t));

                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
                assert(appropriate_size_index < (int64_t)size_array.size());
                uint64_t new_size = size_array[appropriate_size_index];
                this->circular_buffer_ = new uint64_t[new_size];
                this->circular_buffer_size_ = new_size;

                this->special_copy(tmp_array, this->first_block_index_, this->first_bit_index_, 0, old_buffer_size, old_size);
            }

            assert(this->size() == old_size);
        }

        uint64_t read_64_bit_string() const
        {
            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            uint64_t value = bp.read64(this->circular_buffer_);
            return value;
        }

        /**
         * @brief Add an element to the end of the deque
         *
         * @param value The element to add
         * @throws std::invalid_argument If the deque would exceed maximum size
         */
        void push_back(bool value)
        {
#if DEBUG
            uint64_t prev_size = this->size();
#endif
            uint64_t size = this->size();
            this->update_size_if_needed(size + 1);

            if (size == 0)
            {

                this->first_block_index_ = 0;
                this->first_bit_index_ = 0;
                this->last_block_index_ = this->first_block_index_;
                this->last_bit_index_ = this->first_bit_index_;
                this->circular_buffer_[this->first_block_index_] = stool::MSBByte::write_bit(this->circular_buffer_[this->first_block_index_], this->first_bit_index_, value);
            }
            else
            {
                if (this->last_bit_index_ < 63)
                {
                    this->circular_buffer_[this->last_block_index_] = stool::MSBByte::write_bit(this->circular_buffer_[this->last_block_index_], this->last_bit_index_ + 1, value);
                    this->last_bit_index_++;
                }
                else
                {
                    if (this->last_block_index_ + 1 < this->circular_buffer_size_)
                    {
                        this->circular_buffer_[this->last_block_index_ + 1] = stool::MSBByte::write_bit(this->circular_buffer_[this->last_block_index_ + 1], 0, value);
                        this->last_block_index_++;
                        this->last_bit_index_ = 0;
                    }
                    else
                    {
                        this->circular_buffer_[0] = stool::MSBByte::write_bit(this->circular_buffer_[0], 0, value);
                        this->last_block_index_ = 0;
                        this->last_bit_index_ = 0;
                    }
                }
            }
#if DEBUG
            assert(this->size() == prev_size + 1);
#endif
        }

        /**
         * @brief Add an element to the beginning of the deque
         *
         * @param value The element to add
         * @throws std::invalid_argument If the deque would exceed maximum size
         */
        void push_front(bool value)
        {
            uint64_t size = this->size();
            this->update_size_if_needed(size + 1);

            if (size == 0)
            {
                this->push_back(value);
            }
            else
            {
                if (this->first_bit_index_ > 0)
                {
                    this->circular_buffer_[this->first_block_index_] = stool::MSBByte::write_bit(this->circular_buffer_[this->first_block_index_], this->first_bit_index_ - 1, value);
                    this->first_bit_index_--;
                }
                else
                {
                    if (this->first_block_index_ > 0)
                    {
                        this->circular_buffer_[this->first_block_index_ - 1] = stool::MSBByte::write_bit(this->circular_buffer_[this->first_block_index_ - 1], 63, value);
                        this->first_block_index_--;
                        this->first_bit_index_ = 63;
                    }
                    else
                    {
                        this->circular_buffer_[this->circular_buffer_size_ - 1] = stool::MSBByte::write_bit(this->circular_buffer_[this->circular_buffer_size_ - 1], 63, value);
                        this->first_block_index_ = this->circular_buffer_size_ - 1;
                        this->first_bit_index_ = 63;
                    }
                }
            }
        }

        /**
         * @brief Remove the last element from the deque
         */
        void pop_back()
        {
            uint64_t size = this->size();
            if (size == 0)
            {
                throw std::invalid_argument("Error: pop_back()");
            }
            else if (size == 1)
            {
                this->clear();
            }
            else
            {
                if (this->last_bit_index_ > 0)
                {
                    this->last_bit_index_--;
                }
                else
                {
                    if (this->last_block_index_ > 0)
                    {
                        this->last_block_index_--;
                        this->last_bit_index_ = 63;
                    }
                    else
                    {
                        this->last_block_index_ = this->circular_buffer_size_ - 1;
                        this->last_bit_index_ = 63;
                    }
                }

                this->update_size_if_needed(this->size());
            }
        }

        /**
         * @brief Remove the first element from the deque
         */
        void pop_front()
        {
            uint64_t size = this->size();
            if (size == 0)
            {
                throw std::invalid_argument("Error: pop_back()");
            }
            else if (size == 1)
            {
                this->clear();
            }
            else
            {
                if (this->first_bit_index_ < 63)
                {
                    this->first_bit_index_++;
                }
                else
                {
                    if (this->first_block_index_ + 1 < this->circular_buffer_size_)
                    {
                        this->first_block_index_++;
                        this->first_bit_index_ = 0;
                    }
                    else
                    {
                        this->first_block_index_ = 0;
                        this->first_bit_index_ = 0;
                    }
                }
                this->update_size_if_needed(this->size());
            }
        }

        void push_back64(uint64_t value, uint8_t len = 64)
        {
            if (len == 0)
                return;
            uint64_t size = this->size();
            this->update_size_if_needed(size + len);
            if (size == 0)
            {
                this->circular_buffer_[0] = value;
                this->first_block_index_ = 0;
                this->first_bit_index_ = 0;
                if (len == 64)
                {
                    this->last_block_index_ = 1;
                    this->last_bit_index_ = 0;
                }
                else
                {
                    this->last_block_index_ = 0;
                    this->last_bit_index_ = len - 1;
                }
            }
            else
            {
                CircularBitPointer bp(this->circular_buffer_size_, this->last_block_index_, this->last_bit_index_);
                bp.add(1);
                bp.write_bits(this->circular_buffer_, value, len);
                bp.add(len - 1);
                this->last_block_index_ = bp.block_index_;
                this->last_bit_index_ = bp.bit_index_;
            }
        }
        void push_front64(uint64_t value, uint8_t len = 64)
        {
#if DEBUG
            std::string suf = this->to_string();
            std::string pref = stool::MSBByte::to_bit_string(value, len);
            std::string bit_string1 = pref + suf;
#endif
            if (len == 0)
                return;
            uint64_t size = this->size();
            this->update_size_if_needed(size + len);
            if (size == 0)
            {
                this->push_back64(value, len);
            }
            else
            {
                CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
                bp.subtract(len);

                bp.write_bits(this->circular_buffer_, value, len);

                this->first_block_index_ = bp.block_index_;
                this->first_bit_index_ = bp.bit_index_;
            }

#if DEBUG
            std::string bit_string2 = this->to_string();

            if (bit_string1 != bit_string2)
            {
                assert(false);
            }
#endif
        }
        void pop_back(uint64_t len)
        {

            if (len == 0)
                return;
            if (len == 1)
            {
                this->pop_back();
            }
            else
            {
                uint64_t size = this->size();
                if (size < len)
                {
                    throw std::invalid_argument("Error: pop_back()");
                }
                else if (size == len)
                {
                    this->clear();
                }
                else
                {
                    CircularBitPointer bp(this->circular_buffer_size_, this->last_block_index_, this->last_bit_index_);
                    bp.subtract(len);
                    this->last_block_index_ = bp.block_index_;
                    this->last_bit_index_ = bp.bit_index_;
                    this->update_size_if_needed(this->size());
                }
            }
        }
        void pop_front(uint64_t len)
        {

            if (len == 0)
                return;
            if (len == 1)
            {
                this->pop_front();
            }
            else
            {
                uint64_t size = this->size();
                if (size < len)
                {
                    throw std::invalid_argument("Error: pop_front()");
                }
                else if (size == len)
                {
                    this->clear();
                }
                else
                {
                    CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
                    bp.add(len);
                    this->first_block_index_ = bp.block_index_;
                    this->first_bit_index_ = bp.bit_index_;
                    this->update_size_if_needed(this->size());
                }
            }
        }

        void replace(uint64_t position, bool value)
        {
            this->replace(position, value ? (1ULL << 63) : 0, 1);
        }
        void replace(uint64_t position, uint64_t value, uint64_t len)
        {
            if (position + len > this->size())
            {
                throw std::invalid_argument("Error: replace()");
            }
#if DEBUG
            std::string bit_string1 = this->to_string();
            std::string pattern = stool::MSBByte::to_bit_string(value, len);
            for (uint64_t i = 0; i < len; i++)
            {
                bit_string1[position + i] = pattern[i];
            }

#endif

            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            bp.add(position);
            bp.write_bits(this->circular_buffer_, value, len);

#if DEBUG
            std::string bit_string2 = this->to_string();
            if (bit_string1 != bit_string2)
            {
                std::cout << "bit_string1: " << bit_string1 << std::endl;
                std::cout << "bit_string2: " << bit_string2 << std::endl;
                assert(false);
            }
#endif
        }
        template <typename T>
        void replace(uint64_t position, const T &values, uint64_t bit_size)
        {
            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            bp.add(position);
            uint64_t i = 0;
            while (bit_size > 0)
            {
                if (bit_size >= 64)
                {
                    stool::MSBByte::write_64bit_string(this->circular_buffer_, this->circular_buffer_size_, values[i], bp.block_index_, bp.bit_index_, 64, true);
                    bit_size -= 64;
                    bp.add(64);
                    i++;
                }
                else
                {
                    stool::MSBByte::write_64bit_string(this->circular_buffer_, this->circular_buffer_size_, values[i], bp.block_index_, bp.bit_index_, bit_size, true);
                    bp.add(bit_size);
                    bit_size = 0;
                }
            }
        }

        bool is_cyclic() const
        {
            if (this->first_block_index_ == this->last_block_index_)
            {
                return this->last_bit_index_ < this->first_bit_index_;
            }
            else
            {
                return this->last_block_index_ < this->first_block_index_;
            }
        }

        /**
         * @brief Insert an element at the specified position
         *
         * @param position Index where to insert the element
         * @param value The element to insert
         * @throws std::invalid_argument If the deque would exceed maximum size
         * @throws std::out_of_range If position is out of range
         */
        void insert(size_t position, bool value)
        {
            uint64_t value64 = value ? (1ULL << 63) : 0;
            this->insert_64bit_string(position, value64, 1);
        }
        void insert_64bit_string(size_t position, uint64_t value, uint64_t len)
        {
            uint64_t size = this->size();
            if (position == size)
            {
                this->push_back64(value, len);
            }
            else if (position < size)
            {
                this->shift_right(position, len);
                assert(position + len <= this->size());
                this->replace(position, value, len);
            }
            else
            {
                throw std::invalid_argument("Error: insert64()");
            }
        }
        template <typename T>
        void insert_64bit_string(size_t position, const T &values, uint64_t bit_size)
        {
            this->shift_right(position, bit_size);
            this->replace(position, values, bit_size);
        }

        /**
         * @brief Remove element at the specified position
         *
         * @param position Index of the element to remove
         */
        void erase(size_t position)
        {
            this->erase(position, 1);
        }
        void remove(size_t position)
        {
            this->erase(position, 1);
        }

        void erase(size_t position, size_t len)
        {
            uint64_t size = this->size();
            if (position + len == size)
            {
                this->pop_back(len);
            }
            else if (position == 0)
            {
                this->pop_front(len);
            }
            else
            {
                this->shift_left(position + len, len);
            }
        }

        /**
         * @brief Get the current number of elements
         *
         * @return size_t Number of elements in the deque
         */
        size_t size() const
        {
            if (this->empty())
            {
                return 0;
            }
            else
            {
                if (this->is_cyclic())
                {
                    return (64 - this->first_bit_index_) + (1 + this->last_bit_index_) + ((this->circular_buffer_size_ - this->first_block_index_ - 1) * 64) + (this->last_block_index_ * 64);
                }
                else
                {
                    if (this->first_block_index_ < this->last_block_index_)
                    {
                        return (64 - this->first_bit_index_) + (1 + this->last_bit_index_) + ((this->last_block_index_ - this->first_block_index_ - 1) * 64);
                    }
                    else
                    {
                        return this->last_bit_index_ - this->first_bit_index_ + 1;
                    }
                }
            }
        }

        std::string get_circular_buffer_bit_string() const
        {
            std::vector<uint64_t> bits;
            for (uint64_t i = 0; i < this->circular_buffer_size_; i++)
            {
                bits.push_back(this->circular_buffer_[i]);
            }

            return stool::Byte::to_bit_string(bits);
        }

        /**
         * @brief Print debug information about the deque
         */
        void print_info() const
        {
            std::cout << "BitDeque = {" << std::endl;
            std::cout << "S = (" << this->first_block_index_ << ", " << (int)this->first_bit_index_ << ")" << std::endl;
            std::cout << "E = (" << this->last_block_index_ << ", " << (int)this->last_bit_index_ << ")" << std::endl;
            std::cout << "size = " << this->size() << std::endl;
            std::cout << "capacity = " << this->capacity() << std::endl;
            std::cout << "Circular Buffer: " << this->get_circular_buffer_bit_string() << std::endl;
            std::cout << "Content: " << this->to_string() << std::endl;

            std::cout << "}" << std::endl;
        }

        /*
        void reserve(size_t capacity_bit_size)
        {
            this->shrink_to_fit(capacity_bit_size);
        }
        */

        /**
         * @brief Swap contents with another BitDeque
         *
         * @param item The BitDeque to swap with
         */
        void swap(BitDeque &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            std::swap(this->circular_buffer_size_, item.circular_buffer_size_);
            std::swap(this->first_block_index_, item.first_block_index_);
            std::swap(this->last_block_index_, item.last_block_index_);
            std::swap(this->first_bit_index_, item.first_bit_index_);
            std::swap(this->last_bit_index_, item.last_bit_index_);
        }

        std::pair<uint16_t, uint8_t> get_block_index_and_bit_index(uint64_t index) const
        {
            if ((uint64_t)this->first_bit_index_ + index < 64)
            {
                return std::make_pair(this->first_block_index_, this->first_bit_index_ + index);
            }
            else
            {
                uint64_t x = index - (64 - this->first_bit_index_);
                uint64_t offset_block = (x / 64) + 1;
                uint64_t block_index = this->first_block_index_ + offset_block;
                uint64_t bit_index = x - (offset_block * 64);

                if (block_index >= this->circular_buffer_size_)
                {
                    block_index -= this->circular_buffer_size_;
                }
                return std::make_pair(block_index, bit_index);
            }
        }
        uint64_t psum() const
        {
            uint64_t size = this->size();
            if (size == 0)
            {
                return 0;
            }
            else
            {
                return this->rank1(size - 1);
            }
        }

        uint64_t psum(uint64_t i) const
        {
            return this->rank1(i);
        }

        uint64_t reverse_psum(uint64_t i) const
        {
            uint64_t size = this->size();
            if (i + 1 < size)
            {
                return this->psum() - this->rank1(size - i - 1);
            }
            else
            {
                return this->psum();
            }
        }
        int64_t search(uint64_t x) const noexcept
        {
            if (x == 0)
            {
                return 0;
            }
            else
            {
                uint64_t count = this->psum();
                if (x <= count)
                {
                    return this->select1(count - 1);
                }
                else
                {
                    return -1;
                }
            }
        }
        uint64_t rank1(uint64_t i) const
        {
            uint64_t num = 0;
            int64_t counter = i + 1;
            uint64_t block_index = this->first_block_index_;
            uint64_t bit_index = this->first_bit_index_;

            while (counter > 0)
            {
                if (bit_index == 0)
                {
                    if (counter >= 64)
                    {
                        num += stool::Byte::count_bits(this->circular_buffer_[block_index]);
                        block_index++;
                        counter -= 64;
                        if (block_index == this->circular_buffer_size_)
                        {
                            block_index = 0;
                        }
                    }
                    else
                    {
                        num += stool::MSBByte::count_bits(this->circular_buffer_[block_index], counter - 1);
                        bit_index += counter;
                        counter = 0;
                    }
                }
                else
                {
                    if (bit_index + counter >= 64)
                    {
                        num += stool::Byte::count_bits(this->circular_buffer_[block_index]) - stool::MSBByte::count_bits(this->circular_buffer_[block_index], bit_index - 1);
                        block_index++;
                        counter -= (64 - bit_index);
                        bit_index = 0;
                        if (block_index == this->circular_buffer_size_)
                        {
                            block_index = 0;
                        }
                    }
                    else
                    {
                        num += stool::MSBByte::count_bits(this->circular_buffer_[block_index], bit_index + counter - 1) - stool::MSBByte::count_bits(this->circular_buffer_[block_index], bit_index - 1);
                        bit_index += counter;
                        counter = 0;
                    }
                }
            }

            return num;
        }
        int64_t select1(uint64_t i) const
        {
            if (this->empty())
            {
                return -1;
            }
            int64_t counter = i + 1;
            uint64_t block_index = this->first_block_index_;
            uint64_t bit_index = this->first_bit_index_;

            // uint64_t size = this->size();
            uint64_t current_pos = 0;

            bool is_end = false;

            while (!is_end && counter > 0)
            {
                uint64_t bits = this->circular_buffer_[block_index];
                uint64_t bitsize = 64;

                if (block_index == this->last_block_index_ && bit_index <= this->last_bit_index_)
                {
                    uint64_t rightLen = (64 - this->last_bit_index_ - 1);
                    bits = (bits >> rightLen) << rightLen;
                    assert(bitsize >= rightLen);
                    bitsize -= rightLen;
                    is_end = true;
                }
                if (bit_index > 0)
                {
                    bits = bits << bit_index;
                    assert(bitsize >= bit_index);
                    bitsize -= bit_index;
                }

                int64_t num = stool::Byte::count_bits(bits);

                if (num < counter)
                {
                    counter -= num;
                    bit_index += bitsize;
                    current_pos += bitsize;
                }
                else
                {
                    int64_t p = stool::MSBByte::select1(bits, counter - 1);

                    /*
                    std::bitset<64> bs(bits);
                    std::cout << "Bits: " << bs.to_string() << "/num = " << num << std::endl;
                    std::cout << "YYY/" << counter << std::endl;
                    */
                    assert(p != -1);
                    return current_pos + p;
                }

                if (bit_index >= 64)
                {
                    block_index++;
                    bit_index -= 64;
                    if (block_index == this->circular_buffer_size_)
                    {
                        block_index = 0;
                    }
                }
            }

            if (counter > 0)
            {
                return current_pos;
            }
            else
            {
                return -1;
            }
        }

        std::string to_string(bool use_partition = false) const
        {
            std::string s;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                s += std::to_string(this->at(i));
                if (use_partition && i % 64 == 63)
                {
                    s += " ";
                }
            }
            return s;
        }

        void increment(uint64_t i, int64_t delta)
        {
            if (delta >= 1)
            {
                this->replace(i, true);
            }
            else if (delta <= -1)
            {
                this->replace(i, false);
            }
        }

        void shift_right(uint64_t position, uint64_t len)
        {
            uint64_t size = this->size();

            if (size == 0)
            {
                while (len > 0)
                {
                    if (len >= 64)
                    {
                        this->push_back64(0, len);
                        len -= 64;
                    }
                    else
                    {
                        this->push_back64(0, len);
                        len = 0;
                    }
                }
            }
            else
            {
                this->update_size_if_needed(size + len);
                this->change_starting_position(0);

                stool::MSBByte::block_shift_right(this->circular_buffer_, position, len, this->circular_buffer_size_);

                CircularBitPointer bp(this->circular_buffer_size_, this->last_block_index_, this->last_bit_index_);
                bp.add(len);
                this->last_block_index_ = bp.block_index_;
                this->last_bit_index_ = bp.bit_index_;
            }
        }
        void shift_left(uint64_t position, uint64_t len)
        {
            uint64_t size = this->size();

            if (position == size)
            {
                this->pop_back(len);
            }
            else
            {
                this->change_starting_position(0);
                stool::MSBByte::block_shift_left(this->circular_buffer_, position, len, this->circular_buffer_size_);

                CircularBitPointer bp(this->circular_buffer_size_, this->last_block_index_, this->last_bit_index_);
                bp.subtract(len);
                this->last_block_index_ = bp.block_index_;
                this->last_bit_index_ = bp.bit_index_;

                this->update_size_if_needed(size + len);
            }
        }

        void special_copy(std::array<uint64_t, 4096> &tmp_array, uint64_t old_first_block_index, uint64_t old_first_bit_index, uint64_t new_starting_position, uint64_t old_buffer_size, uint64_t bit_size)
        {
            uint64_t block_index = new_starting_position / 64;
            uint64_t bit_index = new_starting_position % 64;
            CircularBitPointer bp1(old_buffer_size, old_first_block_index, old_first_bit_index);
            CircularBitPointer bp2(old_buffer_size, block_index, bit_index);

            if (bit_size == 0)
            {
                this->first_block_index_ = block_index;
                this->first_bit_index_ = bit_index;
            }
            else
            {
                uint64_t counter = bit_size;

                while (counter > 0)
                {
                    if (counter >= 64)
                    {
                        uint64_t bits = bp1.read64(tmp_array);
                        bp2.write64(this->circular_buffer_, bits);
                        bp1.add(64);
                        bp2.add(64);
                        counter -= 64;
                    }
                    else
                    {
                        uint64_t bits = bp1.read64(tmp_array);
                        bp2.write_bits(this->circular_buffer_, bits, counter);
                        bp2.add(counter);
                        counter = 0;
                    }
                }
                bp2.subtract(1);
                this->first_block_index_ = block_index;
                this->first_bit_index_ = bit_index;
                this->last_block_index_ = bp2.block_index_;
                this->last_bit_index_ = bp2.bit_index_;
            }
        }

        void change_starting_position(uint64_t new_starting_position)
        {
            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            if (bp.get_position() == new_starting_position)
                return;

            std::array<uint64_t, 4096> tmp_array;
            std::memcpy(tmp_array.data(), this->circular_buffer_, this->circular_buffer_size_ * sizeof(uint64_t));

#if DEBUG
            std::string PX = this->to_string();
#endif

            this->special_copy(tmp_array, this->first_block_index_, this->first_bit_index_, new_starting_position, this->circular_buffer_size_, this->size());

#if DEBUG
            std::string PY = this->to_string();
            assert(PX == PY);
#endif
        }

        /**
         * @brief Const subscript operator for element access
         *
         * @param index Position of the element to access
         * @return const T& Const reference to the element
         */
        bool operator[](size_t index) const
        {
            std::pair<uint16_t, uint8_t> p = this->get_block_index_and_bit_index(index);
            return stool::MSBByte::get_bit(this->circular_buffer_[p.first], p.second);
        }

        /**
         * @brief Get element at specified position
         *
         * @param i Index of the element
         * @return T Copy of the element at position i
         */
        bool at(uint64_t i) const
        {
            return (*this)[i];
        }

        /**
         * @brief Save deque to a byte vector
         *
         * @param item The deque to save
         * @param output Vector to store the serialized data
         * @param pos Current position in the output vector (will be updated)
         */
        static void save(const BitDeque &item, std::vector<uint8_t> &output, uint64_t &pos)
        {
            uint64_t bs = 0;
            bs = bs | (((uint64_t)item.circular_buffer_size_) << 48);
            bs = bs | (((uint64_t)item.first_block_index_) << 32);
            bs = bs | (((uint64_t)item.last_block_index_) << 16);
            bs = bs | (((uint64_t)item.first_bit_index_) << 8);
            bs = bs | (((uint64_t)item.last_bit_index_) << 0);
            std::memcpy(output.data() + pos, &bs, sizeof(bs));
            pos += sizeof(bs);

            std::memcpy(output.data() + pos, item.circular_buffer_, item.circular_buffer_size_ * sizeof(uint64_t));
            pos += item.circular_buffer_size_ * sizeof(uint64_t);
        }

        /**
         * @brief Save deque to a file stream
         *
         * @param item The deque to save
         * @param os Output file stream
         */
        static void save(const BitDeque &item, std::ofstream &os)
        {
            os.write(reinterpret_cast<const char *>(&item.circular_buffer_size_), sizeof(item.circular_buffer_size_));
            os.write(reinterpret_cast<const char *>(&item.first_block_index_), sizeof(item.first_block_index_));
            os.write(reinterpret_cast<const char *>(&item.first_bit_index_), sizeof(item.first_bit_index_));
            os.write(reinterpret_cast<const char *>(&item.last_block_index_), sizeof(item.last_block_index_));
            os.write(reinterpret_cast<const char *>(&item.last_bit_index_), sizeof(item.last_bit_index_));
            os.write(reinterpret_cast<const char *>(item.circular_buffer_), item.circular_buffer_size_ * sizeof(uint64_t));
        }

        /**
         * @brief Load deque from a byte vector
         *
         * @param data Vector containing the serialized data
         * @param pos Current position in the data vector (will be updated)
         * @return BitDeque The loaded deque
         */
        static BitDeque load(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint16_t _circular_buffer_size;
            uint16_t _first_block_index;
            uint16_t _last_block_index;
            uint8_t _first_bit_index;
            uint8_t _last_bit_index;

            uint64_t tmp = 0;

            std::memcpy(&tmp, data.data() + pos, sizeof(uint64_t));
            pos += sizeof(uint64_t);
            _circular_buffer_size = tmp >> 48;
            _first_block_index = (tmp >> 32) & 0xFFFF;
            _last_block_index = (tmp >> 16) & 0xFFFF;
            _first_bit_index = (tmp >> 8) & 0xFF;
            _last_bit_index = tmp & 0xFF;

            BitDeque r(_circular_buffer_size);
            r.first_block_index_ = _first_block_index;
            r.first_bit_index_ = _first_bit_index;
            r.last_block_index_ = _last_block_index;
            r.last_bit_index_ = _last_bit_index;

            std::memcpy(r.circular_buffer_, data.data() + pos, sizeof(uint64_t) * (size_t)_circular_buffer_size);
            pos += sizeof(uint64_t) * _circular_buffer_size;

            return r;
        }

        /**
         * @brief Load deque from a file stream
         *
         * @param ifs Input file stream
         * @return BitDeque The loaded deque
         */
        static BitDeque load(std::ifstream &ifs)
        {
            uint16_t _circular_buffer_size;
            uint16_t _first_block_index;
            uint16_t _last_block_index;
            uint8_t _first_bit_index;
            uint8_t _last_bit_index;

            ifs.read(reinterpret_cast<char *>(&_circular_buffer_size), sizeof(uint16_t));
            ifs.read(reinterpret_cast<char *>(&_first_block_index), sizeof(uint16_t));
            ifs.read(reinterpret_cast<char *>(&_last_block_index), sizeof(uint16_t));
            ifs.read(reinterpret_cast<char *>(&_first_bit_index), sizeof(uint8_t));
            ifs.read(reinterpret_cast<char *>(&_last_bit_index), sizeof(uint8_t));

            BitDeque r(_circular_buffer_size);
            r.first_block_index_ = _first_block_index;
            r.first_bit_index_ = _first_bit_index;
            r.last_block_index_ = _last_block_index;
            r.last_bit_index_ = _last_bit_index;
            ifs.read(reinterpret_cast<char *>(r.circular_buffer_), sizeof(uint64_t) * (size_t)_circular_buffer_size);

            return r;
        }

        /**
         * @brief Calculate the serialized size of a deque
         *
         * @param item The deque to measure
         * @return uint64_t Size in bytes when serialized
         */
        static uint64_t get_byte_size(const BitDeque &item)
        {
            uint64_t bytes = (sizeof(uint64_t)) + (item.circular_buffer_size_ * sizeof(uint64_t));
            return bytes;
        }
        static uint64_t get_byte_size(const std::vector<BitDeque> &items)
        {
            uint64_t size = sizeof(uint64_t);
            for (const auto &item : items)
            {
                size += get_byte_size(item);
            }
            return size;
        }
        static void save(const std::vector<BitDeque> &items, std::vector<uint8_t> &output, uint64_t &pos)
        {
            uint64_t size = get_byte_size(items);
            if (pos + size > output.size())
            {
                output.resize(pos + size);
            }

            uint64_t items_size = items.size();
            std::memcpy(output.data() + pos, &items_size, sizeof(uint64_t));
            pos += sizeof(uint64_t);

            for (const auto &item : items)
            {
                BitDeque::save(item, output, pos);
            }
        }
        static void save(const std::vector<BitDeque> &items, std::ofstream &os)
        {
            uint64_t items_size = items.size();
            os.write(reinterpret_cast<const char *>(&items_size), sizeof(uint64_t));

            for (const auto &item : items)
            {
                BitDeque::save(item, os);
            }
        }
        uint64_t psum(uint64_t i, uint64_t j) const
        {
            if (i == j)
            {
                return this->at(i);
            }
            else
            {
                throw std::runtime_error("No implementation");
            }
        }
        static std::vector<BitDeque> load_vector(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t size = 0;
            std::memcpy(&size, data.data() + pos, sizeof(uint64_t));
            pos += sizeof(uint64_t);

            std::vector<BitDeque> output;
            output.resize(size);
            for (uint64_t i = 0; i < size; i++)
            {
                output[i] = BitDeque::load(data, pos);
            }
            return output;
        }
        static std::vector<BitDeque> load_vector(std::ifstream &ifs)
        {
            uint64_t size = 0;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(uint64_t));

            std::vector<BitDeque> output;
            output.resize(size);
            for (uint64_t i = 0; i < size; i++)
            {
                output[i] = BitDeque::load(ifs);
            }

            return output;
        }

        BitDequeIterator begin() const
        {
            if(!this->empty()){
                return BitDequeIterator(this, 0, this->first_block_index_, this->first_bit_index_, this->size());
            }else{
                return this->end();
            }
        }
        BitDequeIterator end() const
        {
            return BitDequeIterator(this, UINT16_MAX, UINT16_MAX, UINT8_MAX, this->size());
        }
    };
}
