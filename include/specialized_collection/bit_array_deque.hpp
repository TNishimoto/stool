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
    class BitArrayDeque
    {
        inline static std::vector<int> size_array{1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 33, 40, 48, 58, 70, 84, 101, 122, 147, 177, 213, 256, 308, 370, 444, 533, 640, 768, 922, 1107, 1329, 1595, 1914, 2297, 2757, 3309, 3971, 4766};
        inline static const uint64_t TMP_BUFFER_SIZE = 4096;

    public:
        using INDEX_TYPE = uint16_t;
        uint64_t *circular_buffer_ = nullptr;
        uint16_t num1_;
        uint16_t circular_buffer_size_;
        uint16_t first_block_index_;
        uint16_t last_block_index_;
        uint8_t first_bit_index_;
        uint8_t last_bit_index_;

        class BitArrayDequeIterator
        {

        public:
            const BitArrayDeque *_m_deq;
            uint16_t index;
            uint16_t block_index;
            uint16_t size = 0;
            uint8_t bit_index;

            using iterator_category = std::random_access_iterator_tag;
            using value_type = bool;
            using difference_type = std::ptrdiff_t;

            BitArrayDequeIterator() : _m_deq(nullptr), index(UINT16_MAX), block_index(UINT16_MAX), size(UINT16_MAX), bit_index(UINT8_MAX)
            {
            }

            BitArrayDequeIterator(const BitArrayDeque *_deque, uint16_t _index, uint16_t _block_index, uint8_t _bit_index, uint16_t _size) : _m_deq(_deque), index(_index), block_index(_block_index), size(_size), bit_index(_bit_index) {}

            bool operator*() const
            {
                return ((*_m_deq).circular_buffer_[this->block_index] >> (63 - this->bit_index)) & 1;
            }

            BitArrayDequeIterator &operator++()
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
                    throw std::invalid_argument("Error: BitArrayDequeIterator::operator++()");
                }

                return *this;
            }

            BitArrayDequeIterator operator++(int)
            {
                BitArrayDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            BitArrayDequeIterator &operator--()
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
                    throw std::invalid_argument("Error: BitArrayDequeIterator::operator--()");
                }

                return *this;
            }

            BitArrayDequeIterator operator--(int)
            {
                BitArrayDequeIterator temp = *this;
                --(*this);
                return temp;
            }

            BitArrayDequeIterator operator+(difference_type n) const
            {
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                bp.add(n);
                if (this->index + n < this->size)
                {
                    return BitArrayDequeIterator(this->_m_deq, this->index + n, bp.block_index_, bp.bit_index_, this->size);
                }
                else
                {
                    return BitArrayDequeIterator(this->_m_deq, UINT16_MAX, UINT16_MAX, UINT8_MAX, this->size);
                }
            }

            BitArrayDequeIterator &operator+=(difference_type n)
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

            BitArrayDequeIterator operator-(difference_type n) const
            {
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                bp.subtract(n);
                if (n <= this->index)
                {
                    return BitArrayDequeIterator(this->_m_deq, this->index - n, bp.block_index_, bp.bit_index_, this->size);
                }
                else
                {
                    throw std::invalid_argument("Error: BitArrayDequeIterator::operator-()");
                }
            }

            BitArrayDequeIterator &operator-=(difference_type n)
            {
                if (this->index < n)
                {
                    throw std::invalid_argument("Error: BitArrayDequeIterator::operator-()");
                }
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                bp.subtract(n);
                this->index -= n;
                this->block_index = bp.block_index_;
                this->bit_index = bp.bit_index_;
                return *this;
            }

            uint64_t read_64bit_MSB_string() const
            {
                CircularBitPointer bp(this->_m_deq->circular_buffer_size_, this->block_index, this->bit_index);
                return bp.read64(this->_m_deq->circular_buffer_);
            }

            difference_type operator-(const BitArrayDequeIterator &other) const
            {
                return (int16_t)this->index - (int16_t)other.index;
            }
            uint64_t get_size() const
            {
                return this->size;
            }
            bool is_end() const
            {
                return this->index == UINT16_MAX;
            }

            /**
             * @brief Equality comparison
             */
            bool operator==(const BitArrayDequeIterator &other) const { return this->index == other.index; }

            /**
             * @brief Inequality comparison
             */
            bool operator!=(const BitArrayDequeIterator &other) const { return this->index != other.index; }

            /**
             * @brief Less than comparison
             */
            bool operator<(const BitArrayDequeIterator &other) const { return this->index < other.index; }

            /**
             * @brief Greater than comparison
             */
            bool operator>(const BitArrayDequeIterator &other) const { return this->index > other.index; }

            /**
             * @brief Less than or equal comparison
             */
            bool operator<=(const BitArrayDequeIterator &other) const { return this->index <= other.index; }

            /**
             * @brief Greater than or equal comparison
             */
            bool operator>=(const BitArrayDequeIterator &other) const { return this->index >= other.index; }
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
                return sizeof(BitArrayDeque) + (sizeof(uint64_t) * this->circular_buffer_size_);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return (this->capacity() - this->size() / 64) * sizeof(uint64_t);
        }

        /**
         * @brief Copy constructor
         *
         * @param other The BitArrayDeque to copy from
         */
        BitArrayDeque(const BitArrayDeque &other) noexcept
        {
            this->circular_buffer_size_ = other.circular_buffer_size_;
            this->num1_ = other.num1_;
            this->first_block_index_ = other.first_block_index_;
            this->last_block_index_ = other.last_block_index_;
            this->first_bit_index_ = other.first_bit_index_;
            this->last_bit_index_ = other.last_bit_index_;
            this->circular_buffer_ = new uint64_t[this->circular_buffer_size_];

            std::memcpy(this->circular_buffer_, other.circular_buffer_, this->circular_buffer_size_ * sizeof(uint64_t));
        }

        BitArrayDeque(const std::vector<bool> &bv) noexcept
        {
            if (bv.size() == 0)
            {
                this->initialize();
            }
            else
            {
                this->initialize();
                this->update_size_if_needed(bv.size());

                uint64_t bv_size = bv.size();

                assert(this->size() == 0);

                for (uint64_t i = 0; i < bv_size; i++)
                {
                    this->push_back(bv[i]);

                    assert(this->size() == i + 1);
                }
                assert(this->size() == bv.size());
            }
        }

        /**
         * @brief Move constructor
         *
         * @param other The BitArrayDeque to move from
         */
        BitArrayDeque(BitArrayDeque &&other) noexcept
            : circular_buffer_(other.circular_buffer_),
              num1_(other.num1_),
              circular_buffer_size_(other.circular_buffer_size_),
              first_block_index_(other.first_block_index_),
              last_block_index_(other.last_block_index_),
              first_bit_index_(other.first_bit_index_),
              last_bit_index_(other.last_bit_index_)
        {
            // Reset the source object
            other.circular_buffer_ = nullptr;
            other.circular_buffer_size_ = 0;
            other.num1_ = 0;
            other.first_block_index_ = 0;
            other.last_block_index_ = 0;
            other.first_bit_index_ = 0;
            other.last_bit_index_ = 0;
        }

        /**
         * @brief Move assignment operator
         *
         * @param other The BitArrayDeque to move from
         * @return BitArrayDeque& Reference to this object
         */
        BitArrayDeque &operator=(BitArrayDeque &&other) noexcept
        {
            if (this != &other)
            {
                this->circular_buffer_ = other.circular_buffer_;
                this->circular_buffer_size_ = other.circular_buffer_size_;
                this->num1_ = other.num1_;
                this->first_block_index_ = other.first_block_index_;
                this->last_block_index_ = other.last_block_index_;
                this->first_bit_index_ = other.first_bit_index_;
                this->last_bit_index_ = other.last_bit_index_;

                other.circular_buffer_ = nullptr;
                other.circular_buffer_size_ = 0;
                other.num1_ = 0;
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
            this->num1_ = 0;
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
        BitArrayDeque()
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
            this->num1_ = 0;

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
        BitArrayDeque(uint64_t _circular_buffer_size)
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
            this->circular_buffer_ = new uint64_t[_circular_buffer_size];
            this->num1_ = 0;
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
        ~BitArrayDeque()
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
            int64_t appropriate_size_index = BitArrayDeque::get_appropriate_circular_buffer_size_index(new_size);

            uint64_t old_buffer_size = this->circular_buffer_size_;
            uint64_t old_size = this->size();

            if (appropriate_size_index + 1 < current_size_index || appropriate_size_index > current_size_index)
            {
                this->reset_starting_position();
                uint64_t *tmp = this->circular_buffer_;
                assert(tmp != nullptr);

                assert(appropriate_size_index < (int64_t)size_array.size());
                uint64_t new_size = size_array[appropriate_size_index];
                this->circular_buffer_ = new uint64_t[new_size];
                this->circular_buffer_size_ = new_size;

                std::array<uint64_t, TMP_BUFFER_SIZE> tmp_array;
                std::memcpy(this->circular_buffer_, tmp, (std::min(old_size, new_size)) * sizeof(uint64_t));
            }

            // assert(this->size() == old_size);
        }

        uint64_t read_64_bit_string() const
        {
            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            return this->read_64_bit_string(bp);
        }
        uint64_t read_64_bit_string(const CircularBitPointer &bp) const
        {
            uint64_t value = bp.read64(this->circular_buffer_);
            return value;
        }
        void replace(const CircularBitPointer &pointer, uint64_t value, uint64_t len)
        {
            pointer.write_bits(this->circular_buffer_, value, len);
        }
        CircularBitPointer get_position_pointer(uint64_t position) const
        {
            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            bp.add(position);
            return bp;
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
            if (value)
            {
                this->num1_++;
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

                if (value)
                {
                    this->num1_++;
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
                bool value = this->at(size - 1);
                if (value)
                {
                    this->num1_--;
                }

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
                bool value = this->at(0);
                if (value)
                {
                    this->num1_--;
                }

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

            uint64_t num1 = stool::MSBByte::count_bits(value, len - 1);
            this->num1_ += num1;

            if (size == 0)
            {
                this->circular_buffer_[0] = value;
                this->first_block_index_ = 0;
                this->first_bit_index_ = 0;
                if (len == 64)
                {
                    this->last_block_index_ = 0;
                    this->last_bit_index_ = 63;
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
                uint64_t num1 = stool::MSBByte::count_bits(value, len - 1);
                this->num1_ += num1;

                CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
                bp.subtract(len);

                bp.write_bits(this->circular_buffer_, value, len);

                this->first_block_index_ = bp.block_index_;
                this->first_bit_index_ = bp.bit_index_;
            }
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

                    uint64_t removed_num1 = this->rank1(size - len, size - 1);
                    this->num1_ -= removed_num1;

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
                    uint64_t removed_num1 = this->rank1(len - 1);
                    this->num1_ -= removed_num1;

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
            this->replace_64bit_string(position, value ? (1ULL << 63) : 0, 1);
        }
        void replace_64bit_string(uint64_t position, uint64_t value, uint64_t len)
        {
            if (position + len > this->size())
            {
                throw std::invalid_argument("Error: replace()");
            }
            if (len == 0)
            {
                return;
            }

            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            bp.add(position);
            uint64_t removed_bits = bp.read64(this->circular_buffer_);

            uint64_t added_num1 = stool::MSBByte::count_bits(value, len - 1);
            uint64_t removed_num1 = stool::MSBByte::count_bits(removed_bits, len - 1);
            this->num1_ += added_num1;
            this->num1_ -= removed_num1;

            bp.write_bits(this->circular_buffer_, value, len);

            assert(this->num1_ == this->rank1(0, this->size() - 1));
        }
        template <typename T>
        void replace_64bit_string_sequence(uint64_t position, const T &values, uint64_t bit_size)
        {
            if(bit_size){
                return;
            }

            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            bp.add(position);
            uint64_t i = 0;
            while (bit_size > 0)
            {
                if (bit_size >= 64)
                {
                    uint64_t removed_bits = bp.read64(this->circular_buffer_);
                    uint64_t removed_num1 = stool::Byte::count_bits(removed_bits);
                    uint64_t added_num1 = stool::Byte::count_bits(values[i]);
                    this->num1_ += added_num1 - removed_num1;

                    stool::MSBByte::write_64bit_string(this->circular_buffer_, this->circular_buffer_size_, values[i], bp.block_index_, bp.bit_index_, 64, true);

                    bit_size -= 64;
                    bp.add(64);
                    i++;
                }
                else
                {
                    uint64_t removed_bits = bp.read64(this->circular_buffer_);
                    uint64_t removed_num1 = stool::MSBByte::count_bits(removed_bits, bit_size - 1);
                    uint64_t added_num1 = stool::MSBByte::count_bits(values[i], bit_size - 1);
                    this->num1_ += added_num1;
                    this->num1_ -= removed_num1;

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
                this->replace_64bit_string(position, value, len);
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

            this->replace_64bit_string_sequence(position, values, bit_size);
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
            bits.resize(this->circular_buffer_size_);
            for (uint64_t i = 0; i < this->circular_buffer_size_; i++)
            {
                bits[i] = this->circular_buffer_[i];
            }

            return stool::Byte::to_bit_string(bits);
        }

        /**
         * @brief Print debug information about the deque
         */
        void print_info() const
        {
            std::cout << "BitArrayDeque = {" << std::endl;
            std::cout << "S = (" << this->first_block_index_ << ", " << (int)this->first_bit_index_ << ")" << std::endl;
            std::cout << "E = (" << this->last_block_index_ << ", " << (int)this->last_bit_index_ << ")" << std::endl;
            std::cout << "size = " << this->size() << std::endl;
            std::cout << "capacity = " << this->capacity() << std::endl;
            std::cout << "circular_buffer_size = " << this->circular_buffer_size_ << std::endl;

            if (this->circular_buffer_ != nullptr)
            {
                std::cout << "Circular Buffer: " << this->get_circular_buffer_bit_string() << std::endl;
            }
            else
            {
                std::cout << "Circular Buffer: nullptr" << std::endl;
            }
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
         * @brief Swap contents with another BitArrayDeque
         *
         * @param item The BitArrayDeque to swap with
         */
        void swap(BitArrayDeque &item)
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
            return this->rank1();
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
                uint64_t v = this->psum() - this->rank1(size - i - 2);
                return v;
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
                    uint64_t v = this->select1(x - 1);
                    return v;
                }
                else
                {
                    return -1;
                }
            }
        }

        uint64_t rank1(uint64_t i, uint64_t j) const
        {
            uint64_t len = j - i + 1;
            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            bp.add(i);

            return this->rank1(bp.block_index_, bp.bit_index_, len);
        }
        uint64_t rank1() const
        {
            return this->num1_;
        }

        uint64_t rank1(uint16_t block_index, uint8_t bit_index, uint16_t len) const
        {
            uint64_t num = 0;

            if (len == 0)
            {
                return 0;
            }
            CircularBitPointer start_bp(this->circular_buffer_size_, block_index, bit_index);
            CircularBitPointer end_bp(this->circular_buffer_size_, block_index, bit_index);
            end_bp.add(len - 1);

            if (start_bp.get_position_on_circular_buffer() <= end_bp.get_position_on_circular_buffer())
            {
                {
                    uint64_t block = this->circular_buffer_[start_bp.block_index_];
                    uint64_t R = start_bp.block_index_ != end_bp.block_index_ ? stool::Byte::count_bits(block) : stool::MSBByte::count_bits(block, end_bp.bit_index_);
                    uint64_t L = start_bp.bit_index_ == 0 ? 0 : stool::MSBByte::count_bits(block, start_bp.bit_index_ - 1);
                    num += R - L;
                }

                for (uint64_t i = start_bp.block_index_ + 1; i < end_bp.block_index_; i++)
                {
                    num += stool::Byte::count_bits(this->circular_buffer_[i]);
                }

                if (start_bp.block_index_ != end_bp.block_index_)
                {
                    num += stool::MSBByte::count_bits(this->circular_buffer_[end_bp.block_index_], end_bp.bit_index_);
                }
            }
            else
            {
                {
                    uint64_t block = this->circular_buffer_[start_bp.block_index_];
                    uint64_t R = stool::Byte::count_bits(block);
                    uint64_t L = start_bp.bit_index_ == 0 ? 0 : stool::MSBByte::count_bits(block, start_bp.bit_index_ - 1);
                    num += R - L;
                }
                for (uint64_t i = start_bp.block_index_ + 1; i < this->circular_buffer_size_; i++)
                {
                    num += stool::Byte::count_bits(this->circular_buffer_[i]);
                }
                for (uint64_t i = 0; i < end_bp.block_index_; i++)
                {
                    num += stool::Byte::count_bits(this->circular_buffer_[i]);
                }
                num += stool::MSBByte::count_bits(this->circular_buffer_[end_bp.block_index_], end_bp.bit_index_);
            }
            return num;
        }

        uint64_t rank1(uint64_t i) const
        {
            return this->rank1(this->first_block_index_, this->first_bit_index_, i + 1);
        }
        uint64_t rank0(uint64_t i) const
        {
            return (i + 1) - this->rank1(i);
        }
        uint64_t rank0() const
        {
            return this->size() - this->rank1();
        }

        int64_t select1(uint64_t i) const
        {
            if (this->empty())
            {
                return -1;
            }

            uint64_t size = this->size();

            if (i + 1 > this->num1_)
            {
                return -1;
            }
            else
            {
                int64_t counter = i + 1;
                uint64_t block_index = this->first_block_index_;
                uint64_t bit_index = this->first_bit_index_;

                // uint64_t size = this->size();
                uint64_t current_pos = 0;

                bool is_end = false;
                CircularBitPointer bp(this->circular_buffer_size_, block_index, bit_index);

                while (!is_end && counter > 0)
                {
                    uint64_t bits = bp.read64(this->circular_buffer_);
                    uint64_t bitsize = 64;

                    if (bp.bit_index_ == 0)
                    {
                        int64_t num = stool::Byte::count_bits(bits);
                        if (num < counter)
                        {
                            counter -= num;
                        }
                        else
                        {
                            int64_t p = stool::MSBByte::select1(bits, counter - 1);
                            return current_pos + p;
                        }
                        current_pos += bitsize;
                        bp.add(bitsize);
                    }
                    else
                    {
                        uint64_t nokori = size - current_pos;
                        uint64_t nokori2 = 64 - bit_index;
                        if (nokori >= nokori2)
                        {
                            bitsize = nokori2;
                        }
                        else
                        {
                            bitsize = nokori;
                        }
                        bits = (bits >> (64 - bitsize)) << (64 - bitsize);
                        int64_t num = stool::Byte::count_bits(bits);
                        if (num < counter)
                        {
                            counter -= num;
                        }
                        else
                        {
                            int64_t p = stool::MSBByte::select1(bits, counter - 1);
                            return current_pos + p;
                        }
                        current_pos += bitsize;
                        bp.add(bitsize);
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
        }
        int64_t select0(uint64_t i) const
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
            uint64_t size = this->size();

            bool is_end = false;
            CircularBitPointer bp(this->circular_buffer_size_, block_index, bit_index);

            while (!is_end && counter > 0)
            {
                uint64_t bits = bp.read64(this->circular_buffer_);
                uint64_t bitsize = 64;

                if (bp.bit_index_ == 0)
                {
                    int64_t num = bitsize - stool::Byte::count_bits(bits);
                    if (num < counter)
                    {
                        counter -= num;
                    }
                    else
                    {
                        int64_t p = stool::MSBByte::select0(bits, counter - 1);
                        return current_pos + p;
                    }
                    current_pos += bitsize;
                    bp.add(bitsize);
                }
                else
                {
                    uint64_t nokori = size - current_pos;
                    uint64_t nokori2 = 64 - bit_index;
                    if (nokori >= nokori2)
                    {
                        bitsize = nokori2;
                    }
                    else
                    {
                        bitsize = nokori;
                    }
                    bits = (bits >> (64 - bitsize)) << (64 - bitsize);
                    int64_t num = bitsize - stool::Byte::count_bits(bits);
                    if (num < counter)
                    {
                        counter -= num;
                    }
                    else
                    {
                        int64_t p = stool::MSBByte::select0(bits, counter - 1);
                        return current_pos + p;
                    }
                    current_pos += bitsize;
                    bp.add(bitsize);
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
        std::vector<bool> to_bit_vector() const
        {
            std::vector<bool> bv;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                bv.push_back(this->at(i));
            }
            return bv;
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
                this->reset_starting_position();


                CircularBitPointer src_bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
                CircularBitPointer dst_bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
                src_bp.add(position);
                dst_bp.add(position + len);



                stool::MSBByte::move_suffix_blocks_to_a_block_position<uint64_t*, TMP_BUFFER_SIZE>(this->circular_buffer_, src_bp.block_index_, src_bp.bit_index_, dst_bp.block_index_, dst_bp.bit_index_, this->circular_buffer_size_);

                //stool::MSBByte::block_shift_right(this->circular_buffer_, position, len, this->circular_buffer_size_);
                
                CircularBitPointer bp(this->circular_buffer_size_, this->last_block_index_, this->last_bit_index_);
                bp.add(len);
                this->last_block_index_ = bp.block_index_;
                this->last_bit_index_ = bp.bit_index_;



#if DEBUG
                uint64_t xnum1 = this->rank1(this->size() - 1);
                if (this->num1_ != xnum1)
                {
                    std::cout << "shift_right error @@@@@@@@@@@@@@@@@" << std::endl;
                    std::cout << "this->num1_ = " << this->num1_ << std::endl;
                    std::cout << "xnum1 = " << xnum1 << std::endl;
                    std::cout << "position = " << position << std::endl;
                    std::cout << "len = " << len << std::endl;
                    std::cout << "this->size() = " << this->size() << std::endl;
                    this->print_info();
                    assert(false);
                }
#endif
            }
        }
        void shift_left(uint64_t position, uint64_t len)
        {
            uint64_t size = this->size();

            if (position == size)
            {
                this->pop_back(len);
            }
            else if (len == 0)
            {
                return;
            }
            else if (position == 0)
            {
                throw std::runtime_error("shift_left is not implemented for position == 0");
            }
            else
            {
                int64_t posL = (int64_t)position - (int64_t)len;
                uint64_t removed_num1 = this->rank1(posL, position - 1);


                this->reset_starting_position();

                CircularBitPointer bpDST(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
                bpDST.add(posL);

                CircularBitPointer bpSRC(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
                bpSRC.add(position);

                assert(bpDST.block_index_ <= bpSRC.block_index_);

                stool::MSBByte::move_suffix_blocks_to_a_block_position<uint64_t*, TMP_BUFFER_SIZE>(this->circular_buffer_, bpSRC.block_index_, bpSRC.bit_index_, bpDST.block_index_, bpDST.bit_index_, this->circular_buffer_size_);

                // stool::MSBByte::block_shift_left(this->circular_buffer_, position, len, this->circular_buffer_size_);

                this->num1_ -= removed_num1;

                CircularBitPointer bp(this->circular_buffer_size_, this->last_block_index_, this->last_bit_index_);
                bp.subtract(len);
                this->last_block_index_ = bp.block_index_;
                this->last_bit_index_ = bp.bit_index_;

                this->update_size_if_needed(size + len);

#if DEBUG
                uint64_t xnum1 = this->rank1(this->size() - 1);
                if (this->num1_ != xnum1)
                {
                    std::cout << "shift_left error @@@@@@@@@@@@@@@@@" << std::endl;
                    std::cout << "\t this->num1_ = " << this->num1_ << std::endl;
                    std::cout << "\t xnum1 = " << xnum1 << std::endl;
                    std::cout << "\t removed_num1 = " << removed_num1 << std::endl;
                    std::cout << "\t position = " << position << std::endl;
                    std::cout << "\t len = " << len << std::endl;
                    std::cout << "\t this->size() = " << this->size() << std::endl;
                    this->print_info();
                    assert(false);
                }
#endif
            }
        }

        void reset_starting_position()
        {
            if (this->first_block_index_ == 0 && this->first_bit_index_ == 0)
            {
                return;
            }

            uint64_t size = this->size();

            if (size > 0)
            {
                assert(this->num1_ == this->rank1(0, this->size() - 1));

#if DEBUG
                {
                    if (this->size() == 3006)
                    {
                        this->print_info();
                    }
                }
#endif

                std::array<uint64_t, TMP_BUFFER_SIZE> tmp_array;
                CircularBitPointer start_bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
                CircularBitPointer end_bp(this->circular_buffer_size_, this->last_block_index_, this->last_bit_index_);

                if (start_bp.get_position_on_circular_buffer() <= end_bp.get_position_on_circular_buffer())
                {
                    uint64_t block_count = end_bp.block_index_ - start_bp.block_index_ + 1;
                    std::memcpy(&tmp_array[0], &this->circular_buffer_[start_bp.block_index_], block_count * sizeof(uint64_t));
                    stool::MSBByte::move_suffix_blocks_to_a_block_position<std::array<uint64_t, TMP_BUFFER_SIZE>, TMP_BUFFER_SIZE>(tmp_array, 0, start_bp.bit_index_, 0, this->circular_buffer_size_);
                }
                else
                {

                    if (this->circular_buffer_size_ == 1 && start_bp.block_index_ == 0 && end_bp.block_index_ == 0)
                    {
                        uint64_t blockL = this->circular_buffer_[0] << start_bp.bit_index_;
                        uint8_t blockLsize = 64 - start_bp.bit_index_;
                        uint64_t blockR = this->circular_buffer_[0] >> blockLsize;
                        tmp_array[0] = blockL | blockR;
                    }
                    else
                    {
                        uint64_t suffix_block_count = this->circular_buffer_size_ - start_bp.block_index_;
                        uint64_t prefix_block_count = end_bp.block_index_ + 1;
                        std::memcpy(&tmp_array[0], &this->circular_buffer_[start_bp.block_index_], suffix_block_count * sizeof(uint64_t));
                        std::memcpy(&tmp_array[suffix_block_count], &this->circular_buffer_[0], prefix_block_count * sizeof(uint64_t));
                        stool::MSBByte::move_suffix_blocks_to_a_block_position<std::array<uint64_t, TMP_BUFFER_SIZE>, TMP_BUFFER_SIZE>(tmp_array, 0, start_bp.bit_index_, 0, prefix_block_count + suffix_block_count);
                    }
                }
                std::memcpy(this->circular_buffer_, tmp_array.data(), this->circular_buffer_size_ * sizeof(uint64_t));

                this->first_block_index_ = 0;
                this->first_bit_index_ = 0;
                CircularBitPointer new_bp(this->circular_buffer_size_, 0, 0);
                new_bp.add(size - 1);
                this->last_block_index_ = new_bp.block_index_;
                this->last_bit_index_ = new_bp.bit_index_;

#if DEBUG
                {
                    if (this->num1_ != this->rank1(0, this->size() - 1))
                    {
                        std::cout << "reset_starting_position error @@@@@@@@@@@@@@@@@" << std::endl;
                        std::cout << "this->num1_ = " << this->num1_ << std::endl;
                        std::cout << "this->rank1(0, this->size() - 1) = " << this->rank1(0, this->size() - 1) << std::endl;
                        this->print_info();
                        assert(false);
                    }
                }
#endif
            }
            else
            {
                this->first_block_index_ = 0;
                this->first_bit_index_ = 0;
                this->last_block_index_ = UINT16_MAX;
                this->last_bit_index_ = UINT8_MAX;
            }
        }
        /*
        void special_copy(std::array<uint64_t, TMP_BUFFER_SIZE> &tmp_array, uint64_t old_first_block_index, uint64_t old_first_bit_index, uint64_t old_buffer_size, uint64_t bit_size)
        {
            uint64_t block_index = 0;
            uint64_t bit_index = 0;
            CircularBitPointer tmp_bp(old_buffer_size, old_first_block_index, old_first_bit_index);
            CircularBitPointer buffer_bp(old_buffer_size, block_index, bit_index);

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
                        uint64_t bits = tmp_bp.read64(tmp_array);
                        buffer_bp.write64(this->circular_buffer_, bits);
                        tmp_bp.add(64);
                        buffer_bp.add(64);
                        counter -= 64;
                    }
                    else
                    {
                        uint64_t bits = tmp_bp.read64(tmp_array);
                        buffer_bp.write_bits(this->circular_buffer_, bits, counter);
                        buffer_bp.add(counter);
                        counter = 0;
                    }
                }
                buffer_bp.subtract(1);
                this->first_block_index_ = block_index;
                this->first_bit_index_ = bit_index;
                this->last_block_index_ = buffer_bp.block_index_;
                this->last_bit_index_ = buffer_bp.bit_index_;
            }
        }

        void reset_starting_position()
        {
            CircularBitPointer bp(this->circular_buffer_size_, this->first_block_index_, this->first_bit_index_);
            if (bp.get_position_on_circular_buffer() == 0)
                return;

            std::array<uint64_t, TMP_BUFFER_SIZE> tmp_array;
            assert(this->circular_buffer_size_ < TMP_BUFFER_SIZE);
            std::memcpy(tmp_array.data(), this->circular_buffer_, this->circular_buffer_size_ * sizeof(uint64_t));

            this->special_copy(tmp_array, this->first_block_index_, this->first_bit_index_, this->circular_buffer_size_, this->size());
        }
        */

        void __change_starting_position_for_debug(uint64_t new_starting_position)
        {
            std::vector<bool> bv = this->to_bit_vector();
            int64_t p = new_starting_position % bv.size();

            this->clear();

            uint64_t counter = 0;
            for (uint64_t i = p; i < bv.size(); i++)
            {
                this->push_back(bv[i]);
                if (bv[i])
                {
                    counter++;
                }
                assert(this->num1_ == counter);
            }

            for (int64_t i = p - 1; i >= 0; i--)
            {
                this->push_front(bv[i]);
                if (bv[i])
                {
                    counter++;
                }
                assert(this->num1_ == counter);
            }
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
        static void save(const BitArrayDeque &item, std::vector<uint8_t> &output, uint64_t &pos)
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
        static void save(const BitArrayDeque &item, std::ofstream &os)
        {
            os.write(reinterpret_cast<const char *>(&item.circular_buffer_size_), sizeof(item.circular_buffer_size_));
            os.write(reinterpret_cast<const char *>(&item.first_block_index_), sizeof(item.first_block_index_));
            os.write(reinterpret_cast<const char *>(&item.last_block_index_), sizeof(item.last_block_index_));
            os.write(reinterpret_cast<const char *>(&item.first_bit_index_), sizeof(item.first_bit_index_));
            os.write(reinterpret_cast<const char *>(&item.last_bit_index_), sizeof(item.last_bit_index_));
            os.write(reinterpret_cast<const char *>(item.circular_buffer_), item.circular_buffer_size_ * sizeof(uint64_t));
        }

        /**
         * @brief Load deque from a byte vector
         *
         * @param data Vector containing the serialized data
         * @param pos Current position in the data vector (will be updated)
         * @return BitArrayDeque The loaded deque
         */
        static BitArrayDeque load(const std::vector<uint8_t> &data, uint64_t &pos)
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

            BitArrayDeque r(_circular_buffer_size);
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
         * @return BitArrayDeque The loaded deque
         */
        static BitArrayDeque load(std::ifstream &ifs)
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

            BitArrayDeque r(_circular_buffer_size);
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
        static uint64_t get_byte_size(const BitArrayDeque &item)
        {
            uint64_t bytes = (sizeof(uint64_t)) + (item.circular_buffer_size_ * sizeof(uint64_t));
            return bytes;
        }
        static uint64_t get_byte_size(const std::vector<BitArrayDeque> &items)
        {
            uint64_t size = sizeof(uint64_t);
            for (const auto &item : items)
            {
                size += get_byte_size(item);
            }
            return size;
        }
        static void save(const std::vector<BitArrayDeque> &items, std::vector<uint8_t> &output, uint64_t &pos)
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
                BitArrayDeque::save(item, output, pos);
            }
        }
        static void save(const std::vector<BitArrayDeque> &items, std::ofstream &os)
        {
            uint64_t items_size = items.size();
            os.write(reinterpret_cast<const char *>(&items_size), sizeof(uint64_t));

            for (const auto &item : items)
            {
                BitArrayDeque::save(item, os);
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
        static std::vector<BitArrayDeque> load_vector(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t size = 0;
            std::memcpy(&size, data.data() + pos, sizeof(uint64_t));
            pos += sizeof(uint64_t);

            std::vector<BitArrayDeque> output;
            output.resize(size);
            for (uint64_t i = 0; i < size; i++)
            {
                output[i] = BitArrayDeque::load(data, pos);
            }
            return output;
        }
        static std::vector<BitArrayDeque> load_vector(std::ifstream &ifs)
        {
            uint64_t size = 0;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(uint64_t));

            std::vector<BitArrayDeque> output;
            output.resize(size);
            for (uint64_t i = 0; i < size; i++)
            {
                output[i] = BitArrayDeque::load(ifs);
            }

            return output;
        }

        BitArrayDequeIterator begin() const
        {
            if (!this->empty())
            {
                return BitArrayDequeIterator(this, 0, this->first_block_index_, this->first_bit_index_, this->size());
            }
            else
            {
                return this->end();
            }
        }
        BitArrayDequeIterator end() const
        {
            return BitArrayDequeIterator(this, UINT16_MAX, UINT16_MAX, UINT8_MAX, this->size());
        }
    };
}
