#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include "../../basic/byte.hpp"
#include "../../basic/lsb_byte.hpp"
#include "../../basic/msb_byte.hpp"
#include "../../debug/debug_printer.hpp"

namespace stool
{
    /*!
     * @brief A simple bit vector B[0..n-1] implementation with push/pop operations [in progress]
     * @note The bits are stored in 64-bit integers S[0..m-1] (uint64_t *buffer_)
     * @tparam MAX_BIT_LENGTH Maximum number of bits that can be stored (default: 8092)
     */

    template <uint64_t MAX_BIT_LENGTH = 8092>
    class NaiveBitVector
    {
        inline static std::vector<int> size_array{1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 33, 40, 48, 58, 70, 84, 101, 122, 147, 177, 213, 256, 308, 370, 444, 533, 640, 768, 922, 1107, 1329, 1595, 1914, 2297, 2757, 3309, 3971, 4766};
        inline static const uint64_t TMP_BUFFER_SIZE = MAX_BIT_LENGTH / 64;

        static std::pair<uint64_t, uint8_t> add_bit_length(uint64_t block_index, uint64_t bit_index, uint64_t bit_length)
        {
            block_index += bit_length / 64;
            bit_index += bit_length % 64;

            if (bit_index >= 64)
            {
                bit_index -= 64;
                block_index++;
            }

            return std::make_pair(block_index, bit_index);
        }
        uint64_t *buffer_ = nullptr; // 64-bit integers S
        uint16_t bit_count_; // |B|
        uint16_t num1_; // The number of 1s in B
        uint16_t buffer_size_; // |S|


    public:
        using INDEX_TYPE = uint16_t;

        /*!
         * @brief Bit vector iterator
         */
        class NaiveBitVectorIterator
        {

        public:
            const NaiveBitVector *_m_deq;
            uint16_t index;
            uint16_t size = 0;

            // uint16_t block_index;
            // uint8_t bit_index;

            using iterator_category = std::random_access_iterator_tag;
            using value_type = bool;
            using difference_type = std::ptrdiff_t;

            NaiveBitVectorIterator() : _m_deq(nullptr), index(UINT16_MAX)
            {
            }

            NaiveBitVectorIterator(const NaiveBitVector *_deque, uint16_t _index, uint16_t _size) : _m_deq(_deque), index(_index), size(_size) {}

            bool operator*() const
            {
                uint64_t block_index = this->index / 64;
                uint64_t bit_index = this->index % 64;
                return ((*_m_deq).buffer_[block_index] >> (63 - bit_index)) & 1;
            }

            NaiveBitVectorIterator &operator++()
            {
                if (this->index + 1 <= this->size)
                {
                    this->index++;
                }
                else
                {
                    throw std::invalid_argument("Error: NaiveBitVectorIterator::operator++()");
                }

                return *this;
            }

            NaiveBitVectorIterator operator++(int)
            {
                NaiveBitVectorIterator temp = *this;

                ++(*this);
                return temp;
            }

            NaiveBitVectorIterator &operator--()
            {
                if (this->index >= 1)
                {
                    this->index--;
                }
                else
                {
                    throw std::invalid_argument("Error: NaiveBitVectorIterator::operator--()");
                }

                return *this;
            }

            NaiveBitVectorIterator operator--(int)
            {
                NaiveBitVectorIterator temp = *this;
                --(*this);
                return temp;
            }

            NaiveBitVectorIterator operator+(difference_type n) const
            {
                if (this->index + n <= this->size)
                {
                    return NaiveBitVectorIterator(this->_m_deq, this->index + n, this->size);
                }
                else
                {
                    throw std::invalid_argument("Error: NaiveBitVectorIterator::operator+()");
                }
            }

            NaiveBitVectorIterator &operator+=(difference_type n)
            {
                this->index += n;
                if (this->index > this->size)
                {
                    throw std::invalid_argument("Error: NaiveBitVectorIterator::operator+()");
                }
                return *this;
            }

            NaiveBitVectorIterator operator-(difference_type n) const
            {
                if (this->index >= n)
                {
                    return NaiveBitVectorIterator(this->_m_deq, this->index - n, this->size);
                }
                else
                {
                    throw std::invalid_argument("Error: NaiveBitVectorIterator::operator-()");
                }
            }

            NaiveBitVectorIterator &operator-=(difference_type n)
            {
                if (this->index < n)
                {
                    throw std::invalid_argument("Error: NaiveBitVectorIterator::operator-()");
                }
                this->index -= n;
                return *this;
            }

            difference_type operator-(const NaiveBitVectorIterator &other) const
            {
                return (int16_t)this->index - (int16_t)other.index;
            }
            uint64_t get_size() const
            {
                return this->size;
            }
            bool is_end() const
            {
                return this->index == this->size;
            }
            uint64_t read_64bits_string() const
            {
                uint64_t block_index = this->index / 64;
                uint64_t bit_index = this->index % 64;
                uint64_t value = this->_m_deq->read_as_64bit_integer(block_index, bit_index);

                return value;
            }

            /**
             * @brief Equality comparison
             */
            bool operator==(const NaiveBitVectorIterator &other) const { return this->index == other.index; }

            /**
             * @brief Inequality comparison
             */
            bool operator!=(const NaiveBitVectorIterator &other) const { return this->index != other.index; }

            /**
             * @brief Less than comparison
             */
            bool operator<(const NaiveBitVectorIterator &other) const { return this->index < other.index; }

            /**
             * @brief Greater than comparison
             */
            bool operator>(const NaiveBitVectorIterator &other) const { return this->index > other.index; }

            /**
             * @brief Less than or equal comparison
             */
            bool operator<=(const NaiveBitVectorIterator &other) const { return this->index <= other.index; }

            /**
             * @brief Greater than or equal comparison
             */
            bool operator>=(const NaiveBitVectorIterator &other) const { return this->index >= other.index; }
        };

        // INDEX_TYPE deque_size_;

    private: 
    int64_t get_current_buffer_size_index() const
    {
        if (this->buffer_size_ == 0)
        {
            return -1;
        }
        else
        {
            for (uint64_t i = 0; i < size_array.size(); i++)
            {
                if (this->buffer_size_ == size_array[i])
                {
                    return i;
                }
            }
        }
        throw std::runtime_error("buffer_size_ is not found");
    }


    public:
        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Constructors and Destructor
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Default constructor
         */
        NaiveBitVector()
        {
            this->initialize();
        }
        /**
         * @brief Constructor with buffer size |S| = m
         */
        NaiveBitVector(uint64_t _buffer_size_m)
        {
            if (this->buffer_ != nullptr)
            {
                delete[] this->buffer_;
                this->buffer_ = nullptr;
            }
            this->buffer_ = new uint64_t[_buffer_size_m];
            this->num1_ = 0;
            this->bit_count_ = 0;
            this->buffer_size_ = _buffer_size_m;
        }

        /**
         * @brief Constructor with B[0..n-1] = bv[0..n-1]
         */
        NaiveBitVector(const std::vector<bool> &bv) noexcept
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
         * @brief Copy constructor
         */
        NaiveBitVector(const NaiveBitVector &other) noexcept
        {
            this->buffer_size_ = other.buffer_size_;
            this->num1_ = other.num1_;
            this->buffer_ = new uint64_t[this->buffer_size_];
            this->bit_count_ = other.bit_count_;

            std::memcpy(this->buffer_, other.buffer_, this->buffer_size_ * sizeof(uint64_t));
        }

        /**
         * @brief Move constructor
         */
        NaiveBitVector(NaiveBitVector &&other) noexcept
            : buffer_(other.buffer_),
              bit_count_(other.bit_count_),
              num1_(other.num1_),
              buffer_size_(other.buffer_size_)

        {
            // Reset the source object
            other.buffer_ = nullptr;
            other.bit_count_ = 0;
            other.num1_ = 0;
            other.buffer_size_ = 0;
        }
        /**
         * @brief Destructor
         */
        ~NaiveBitVector()
        {
            if (this->buffer_ != nullptr)
            {
                delete[] this->buffer_;
                this->buffer_ = nullptr;
            }
        }

        /**
         * @brief Initialize the bit vector with |B| = 0 and |S| = 2
         */
        void initialize()
        {
            if (this->buffer_ != nullptr)
            {
                delete[] this->buffer_;
                this->buffer_ = nullptr;
            }
            this->buffer_ = new uint64_t[2];
            this->buffer_[0] = 0;
            this->buffer_[1] = 0;
            this->num1_ = 0;
            this->bit_count_ = 0;
            this->buffer_size_ = 2;
        }

        /**
         * @brief Initialize the bit vector with S = S_[0..m-1] and |B| = n_, and |S| = m_
         * @param num1 The number of 1s in the new B
         */
        template <typename T>
        void initialize(const T &bit64_array_S_, uint64_t bit_size_n_, uint64_t num1, uint64_t array_size_m)
        {
            this->update_size_if_needed(bit_size_n_);
            std::memcpy(this->buffer_, &bit64_array_S_[0], array_size_m * sizeof(uint64_t));
            this->bit_count_ = bit_size_n_;
            this->num1_ = num1;
        }


        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Begin and end iterators
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Get iterator to the first element
         */
        NaiveBitVectorIterator begin() const
        {
            if (!this->empty())
            {
                return NaiveBitVectorIterator(this, 0, this->size());
            }
            else
            {
                return this->end();
            }
        }

        /**
         * @brief Get iterator to the end element
         */
        NaiveBitVectorIterator end() const
        {
            return NaiveBitVectorIterator(this, this->size(), this->size());
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Lightweight functions for accessing to properties of this class
        ///   The properties of this class.
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Return the maximum number of bits that can be stored without resizing the buffer (i.e., |S| * 64)
         */
        size_t capacity() const
        {
            return this->buffer_size_ * 64;
        }

        /**
         * @brief Update the buffer so that its capacity is larger than \p m
         */
        void update_size_if_needed(int64_t m)
        {
            this->shrink_to_fit(m);
        }

        /**
         * @brief Check if the array is empty
         */
        bool empty() const
        {
            return this->bit_count_ == 0;
        }

        /**
         * @brief Get the current number of bits (i.e., |B|)
         */
        size_t size() const
        {
            return this->bit_count_;
        }

        /**
         * @brief Get the pointer to the buffer
         */
        uint64_t *get_buffer_pointer() const
        {
            return this->buffer_;
        }

        /**
         * @brief Returns the total memory usage in bytes
         * @param only_dynamic_memory If true, only the size of the dynamic memory is returned
         */
        uint64_t size_in_bytes(bool only_dynamic_memory = false) const
        {
            if (only_dynamic_memory)
            {
                return sizeof(uint64_t) * this->buffer_size_;
            }
            else
            {
                return sizeof(NaiveBitVector) + (sizeof(uint64_t) * this->buffer_size_);
            }
        }

        /**
         * @brief Returns the size of the unused memory in bytes (i.e., (|S| * 64 - |B|) / 64 * sizeof(uint64_t))
         */
        uint64_t unused_size_in_bytes() const
        {
            return (this->capacity() - this->size() / 64) * sizeof(uint64_t);
        }

        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Operators
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Move assignment operator
         */
        NaiveBitVector &operator=(NaiveBitVector &&other) noexcept
        {
            if (this != &other)
            {
                this->buffer_ = other.buffer_;
                this->buffer_size_ = other.buffer_size_;
                this->num1_ = other.num1_;
                this->bit_count_ = other.bit_count_;

                other.buffer_ = nullptr;
                other.buffer_size_ = 0;
                other.num1_ = 0;
                other.bit_count_ = 0;
            }
            return *this;
        }

        /**
         * @brief Returns bit value B[i]
         */
        bool operator[](size_t i) const
        {
            uint64_t block_index = i / 64;
            uint64_t bit_index = i % 64;
            return stool::MSBByte::get_bit(this->buffer_[block_index], bit_index);
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Access, search, psum, rank, and select operations
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Returns the number of 1s in the bit vector \p B
         * @note \p O(1) time
         */
        uint64_t psum() const
        {
            return this->rank1();
        }
        
        /**
         * @brief Returns the number of 1s in \p B[0..i] (i.e., rank1(i))
         * @note \p O(i) time
         */
        uint64_t psum(uint64_t i) const
        {
            return this->rank1(i);
        }

        /**
         * @brief Returns the number of 1s in \p B[i..j] (i.e., rank1(j) - rank1(i-1))
         * @note \p O(n) time
         */
        uint64_t psum(uint64_t i, uint64_t j) const
        {
            if (i == j)
            {
                return this->at(i);
            }
            else if (i > j)
            {
                return this->rank1(j) - this->rank1(i) - this->at(i);
            }
            else
            {
                throw std::runtime_error("No implementation");
            }
        }

        /**
         * @brief Returns the number of 1s in \p B[(n-1)-i..n-1]
         * @note \p O(n) time
         */
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

        /**
         * @brief Returns the first position \p p such that psum(p) >= x if such a position exists, otherwise returns -1
         * @note \p O(p) time
         */
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

        /**
         * @brief Returns the number of 0s in \p B[0..i]
         * @note \p O(i) time
         */
        uint64_t rank0(uint64_t i) const
        {
            return (i + 1) - this->rank1(i);
        }

        /**
         * @brief Returns the number of 0s in \p B
         * @note \p O(1) time
         */
        uint64_t rank0() const
        {
            return this->size() - this->rank1();
        }

        /**
         * @brief Returns the number of 1s in \p B[i..j]
         * @note \p O(j-i) time
         */
        uint64_t rank1(uint64_t i, uint64_t j) const
        {
            uint64_t len = j - i + 1;
            uint64_t block_index = i / 64;
            uint64_t bit_index = i % 64;

            return this->rank1(block_index, bit_index, len);
        }

        /**
         * @brief Returns the number of 1s in \p B
         * @note \p O(1) time
         */
        uint64_t rank1() const
        {
            return this->num1_;
        }

        /**
         * @brief Returns the number of 1s in \p B[(block_index / 64) + bit_index..(block_index / 64) + bit_index + len - 1]
         * @note \p O(len) time
         */
        uint64_t rank1(uint16_t block_index, uint8_t bit_index, uint16_t len) const
        {
            if (len == 0)
            {
                return 0;
            }

            std::pair<uint64_t, uint8_t> end_bpX = NaiveBitVector::add_bit_length(block_index, bit_index, len - 1);
            uint64_t num = stool::MSBByte::rank1(this->buffer_, block_index, bit_index, end_bpX.first, end_bpX.second, this->buffer_size_);

            return num;
        }

        /**
         * @brief Returns the number of 1s in \p B[0..i]
         * @note \p O(i) time
         */
        uint64_t rank1(uint64_t i) const
        {
            return this->rank1(0, 0, i + 1);
        }

        /**
         * @brief Returns the position \p p of the (i+1)-th 0 in \p B if such a position exists, otherwise returns -1
         * @note \p O(p) time
         */
        int64_t select0(uint64_t i) const
        {
            if (this->empty())
            {
                return -1;
            }

            // uint64_t size = this->size();
            uint64_t num0 = this->bit_count_ - this->num1_;

            if (i + 1 > num0)
            {
                return -1;
            }
            else
            {
                return stool::MSBByte::select0(this->buffer_, i, this->buffer_size_);
            }
        }

        /**
         * @brief Returns the position \p p of the (i+1)-th 1 in \p B if such a position exists, otherwise returns -1
         * @note \p O(p) time
         */
        int64_t select1(uint64_t i) const
        {
            // int64_t true_result = this->select1__(i);
            if (this->empty())
            {
                return -1;
            }

            // uint64_t size = this->size();

            if (i + 1 > this->num1_)
            {
                return -1;
            }
            else
            {
                return stool::MSBByte::select1(this->buffer_, i, this->buffer_size_);
            }
        }
    
        /**
         * @brief Returns the first occurrence position \p p of 1 in \p B such that p >= i (if such a position does not exist, returns -1)
         * @note \p O(p-i) time
         */
        int64_t select1_successor(uint64_t i) const
        {

            uint64_t block_index = (i + 1) / 64;
            uint8_t bit_index = (i + 1) % 64;

            uint64_t last_block_index = (this->bit_count_ - 1) / 64;
            uint64_t last_bit_index = (this->bit_count_ - 1) % 64;

            if (i + 1 >= this->size())
            {
                return -1;
            }
            else if (block_index == last_block_index)
            {
                uint64_t bits = this->buffer_[block_index] << bit_index;
                uint64_t bit_size = last_bit_index - bit_index + 1;
                bits = (bits >> (64 - bit_size)) << (64 - bit_size);
                uint64_t bit_count = stool::Byte::popcount(bits);
                if (bit_count > 0)
                {
                    uint64_t result = stool::MSBByte::select1(bits) + (i + 1);
                    assert(result > i);
                    assert(result <= this->size());
                    return result;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                {
                    uint64_t bits = this->buffer_[block_index] << bit_index;
                    uint64_t bit_count = stool::Byte::popcount(bits);
                    if (bit_count > 0)
                    {
                        uint64_t result = stool::MSBByte::select1(bits) + (i + 1);
                        assert(result > i);
                        assert(result <= this->size());
                        return result;
                    }
                }
                uint64_t gap = 64 - bit_index;

                for (uint64_t j = block_index + 1; j < last_block_index; j++)
                {
                    uint64_t bits = this->buffer_[j];
                    uint64_t bit_count = stool::Byte::popcount(bits);
                    if (bit_count > 0)
                    {
                        uint64_t result = stool::MSBByte::select1(bits) + (i + 1) + gap + ((j - (block_index + 1)) * 64);
                        assert(result > i);
                        assert(result <= this->size());

                        return result;
                    }
                }

                uint64_t last_bits = (this->buffer_[last_block_index] >> (63 - last_bit_index)) << (63 - last_bit_index);
                uint64_t bit_count = stool::Byte::popcount(last_bits);
                uint64_t gap2 = block_index + 1 < last_block_index ? (last_block_index - (block_index + 1)) : 0;
                if (bit_count > 0)
                {
                    uint64_t result = stool::MSBByte::select1(last_bits) + (i + 1) + gap + (gap2 * 64);

                    assert(result > i);
                    assert(result <= this->size());

                    return result;
                }
                else
                {
                    return -1;
                }
            }
        }
        /**
         * @brief Returns the last occurrence position \p p of 1 in \p B such that p <= i (if such a position does not exist, returns -1)
         * @note \p O(i-p) time
         */
        int64_t select1_predecessor(uint64_t i) const
        {
            uint64_t block_index = (i - 1) / 64;
            uint8_t bit_index = (i - 1) % 64;

            {
                uint64_t R_size = 63 - bit_index;
                uint64_t bits = (this->buffer_[block_index] >> R_size) << R_size;
                uint64_t bit_count = stool::Byte::popcount(bits);
                if (bit_count > 0)
                {
                    int64_t result = i - 1 - (stool::LSBByte::select1(bits) - R_size);
                    return result;
                }
            }
            int64_t gap = bit_index + 1;

            for (int64_t j = block_index - 1; j >= 0; j--)
            {
                uint64_t bits = this->buffer_[j];
                uint64_t bit_count = stool::Byte::popcount(bits);
                if (bit_count > 0)
                {
                    int64_t gap2 = (((block_index - 1) - j) * 64);
                    int64_t result = i - 1 - gap - gap2 - stool::LSBByte::select1(bits);
                    assert(result >= 0);
                    assert(result <= (int64_t)this->size());
                    return result;
                }
            }
            return -1;
        }


        /**
         * @brief Returns the position \p p of the (u - i)-th 1 in \p B if such a position exists, otherwise returns -1. Here, \p u is the number of 1s in \p B.
         * @note \p O(|B| - p) time
         */
        int64_t rev_select1(uint64_t i) const
        {
            uint64_t sum = this->rank1();
            uint64_t size = this->size();
            uint64_t counter1 = i + 1;
            if (sum == 0)
            {
                return -1;
            }
            else if (i >= sum)
            {
                return -1;
            }
            else
            {
                uint64_t last_block_index = (this->bit_count_ - 1) / 64;
                uint64_t last_bit_index = (this->bit_count_ - 1) % 64;

                {
                    uint64_t bits = this->buffer_[last_block_index];
                    bits = bits >> (63 - last_bit_index);

                    uint64_t byte_count1 = stool::Byte::popcount(bits);
                    if (byte_count1 < counter1)
                    {
                        counter1 -= byte_count1;
                    }
                    else
                    {
                        int64_t p = stool::LSBByte::select1(bits, counter1 - 1);
                        assert(p != -1);

                        return (size - 1) - p;
                    }
                }

                for (int64_t x = last_block_index - 1; x >= 0; x--)
                {
                    uint64_t bits = this->buffer_[x];
                    uint64_t byte_count1 = stool::Byte::popcount(bits);
                    if (byte_count1 < counter1)
                    {
                        counter1 -= byte_count1;
                    }
                    else
                    {
                        uint64_t gap = (((last_block_index - 1) - x) * 64) + last_bit_index + 1;
                        int64_t p = stool::LSBByte::select1(bits, counter1 - 1);
                        assert(p != -1);

                        return (size - 1) - gap - p;
                    }
                }
                throw std::runtime_error("rev_select1: invalid argument");
            }
        }

        /**
         * @brief Returns bit value B[i]
         */
        bool at(uint64_t i) const
        {
            return (*this)[i];
        }

        /*!
         * @brief Returns the bits B[I..I+63] as a 64-bit integer, where I = block_index * 64 + bit_index, and B[n..] = 0....0.
         */
        uint64_t read_as_64bit_integer(uint64_t block_index, uint8_t bit_index = 0) const
        {
            return stool::MSBByte::access_64bits(this->buffer_, block_index, bit_index, this->bit_count_, this->buffer_size_);
        }

        /*!
         * @brief Returns the bits B[I..I+L-1]0^{64-L} as a 64-bit integer, where I = block_index * 64 + bit_index, and B[n..] = 0....0.
         */
        uint64_t read_as_64bit_integer(uint64_t block_index, uint8_t bit_index, uint8_t code_len_L) const
        {
            uint64_t mask = UINT64_MAX << (64 - code_len_L);
            return stool::MSBByte::access_64bits(this->buffer_, block_index, bit_index, this->bit_count_, this->buffer_size_) & mask;
        }

        /*!
         * @brief Returns the bits 0^{64-L}B[I..I+L-1] as a 64-bit integer, where I = block_index * 64 + bit_index.
         */
        uint64_t read_as_right_alligned_64bit_integer(uint16_t block_index, uint8_t bit_index, uint64_t code_len_L) const
        {
            return stool::MSBByte::access_right_alligned_64bits(this->buffer_, block_index, bit_index, code_len_L, this->buffer_size_);
        }


        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Convertion functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /*!
         * @brief Returns the bits \p B as a binary string
         */
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

        /*!
         * @brief Returns the bits \p B as a bit vector
         */
        std::vector<bool> to_bit_vector() const
        {
            std::vector<bool> bv;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                bv.push_back(this->at(i));
            }
            return bv;
        }

        /*!
         * @brief Returns the 64-bit integers \p S as a binary string
         */
        std::string get_buffer_bit_string() const
        {
            std::vector<uint64_t> bits;
            bits.resize(this->buffer_size_);
            for (uint64_t i = 0; i < this->buffer_size_; i++)
            {
                bits[i] = this->buffer_[i];
            }

            return stool::Byte::to_bit_string(bits, true);
        }

        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Print and verification functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Print debug information about this instance
         */
        void print_info() const
        {
            std::cout << "NaiveBitVector = {" << std::endl;
            std::cout << "size = " << this->size() << std::endl;
            std::cout << "capacity = " << this->capacity() << std::endl;
            std::cout << "buffer_size = " << this->buffer_size_ << std::endl;

            if (this->buffer_ != nullptr)
            {
                std::cout << "Buffer: " << this->get_buffer_bit_string() << std::endl;
            }
            else
            {
                std::cout << "Buffer: nullptr" << std::endl;
            }
            std::cout << "Content: " << this->to_string() << std::endl;

            std::cout << "}" << std::endl;
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Load and save functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Save the given instance \p item to a byte vector \p output at the position \p pos
         */
        static void store_to_bytes(const NaiveBitVector &item, std::vector<uint8_t> &output, uint64_t &pos)
        {
            std::memcpy(output.data() + pos, &item.bit_count_, sizeof(item.bit_count_));
            pos += sizeof(item.bit_count_);
            std::memcpy(output.data() + pos, &item.num1_, sizeof(item.num1_));
            pos += sizeof(item.num1_);
            std::memcpy(output.data() + pos, &item.buffer_size_, sizeof(item.buffer_size_));
            pos += sizeof(item.buffer_size_);
            std::memcpy(output.data() + pos, item.buffer_, item.buffer_size_ * sizeof(uint64_t));
            pos += item.buffer_size_ * sizeof(uint64_t);
        }

        /**
         * @brief Save the given instance \p item to a file stream \p os
         */
        static void store_to_file(const NaiveBitVector &item, std::ofstream &os)
        {
            os.write(reinterpret_cast<const char *>(&item.bit_count_), sizeof(item.bit_count_));
            os.write(reinterpret_cast<const char *>(&item.num1_), sizeof(item.num1_));
            os.write(reinterpret_cast<const char *>(&item.buffer_size_), sizeof(item.buffer_size_));
            os.write(reinterpret_cast<const char *>(item.buffer_), item.buffer_size_ * sizeof(uint64_t));
        }

        /**
         * @brief Save the given vector of NaiveBitVector instances \p items to a byte vector \p output at the position \p pos
         */
        static void store_to_bytes(const std::vector<NaiveBitVector> &items, std::vector<uint8_t> &output, uint64_t &pos)
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
                NaiveBitVector::store_to_file(item, output, pos);
            }
        }
        /**
         * @brief Save the given vector of NaiveBitVector instances \p items to a file stream \p os
         */
        static void store_to_file(const std::vector<NaiveBitVector> &items, std::ofstream &os)
        {
            uint64_t items_size = items.size();
            os.write(reinterpret_cast<const char *>(&items_size), sizeof(uint64_t));

            for (const auto &item : items)
            {
                NaiveBitVector::store_to_file(item, os);
            }
        }

        /**
         * @brief Returns the NaiveBitVector instance loaded from a byte vector \p data at the position \p pos
         */
        static NaiveBitVector load_from_bytes(const std::vector<uint8_t> &data, uint64_t &pos)
        {

            uint16_t _bit_count;
            uint16_t _num1;
            uint16_t _buffer_size;

            std::memcpy(&_bit_count, data.data() + pos, sizeof(uint16_t));
            pos += sizeof(uint16_t);
            std::memcpy(&_num1, data.data() + pos, sizeof(uint16_t));
            pos += sizeof(uint16_t);
            std::memcpy(&_buffer_size, data.data() + pos, sizeof(uint16_t));
            pos += sizeof(uint16_t);

            NaiveBitVector r(_buffer_size);
            r.buffer_size_ = _buffer_size;
            r.bit_count_ = _bit_count;
            r.num1_ = _num1;

            std::memcpy(r.buffer_, data.data() + pos, sizeof(uint64_t) * (size_t)_buffer_size);
            pos += sizeof(uint64_t) * _buffer_size;

            return r;
        }

        /**
         * @brief Returns the NaiveBitVector instance loaded from a file stream \p ifs
         */
        static NaiveBitVector load_from_file(std::ifstream &ifs)
        {
            uint16_t _bit_count;
            uint16_t _num1;
            uint16_t _buffer_size;

            ifs.read(reinterpret_cast<char *>(&_bit_count), sizeof(uint16_t));
            ifs.read(reinterpret_cast<char *>(&_num1), sizeof(uint16_t));
            ifs.read(reinterpret_cast<char *>(&_buffer_size), sizeof(uint16_t));

            NaiveBitVector r(_buffer_size);
            r.bit_count_ = _bit_count;
            r.num1_ = _num1;
            r.buffer_size_ = _buffer_size;
            ifs.read(reinterpret_cast<char *>(r.buffer_), sizeof(uint64_t) * (size_t)_buffer_size);

            return r;
        }

        /**
         * @brief Returns the vector of NaiveBitVector instances loaded from a byte vector \p data at the position \p pos
         */
        static std::vector<NaiveBitVector> load_vector_from_bytes(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t size = 0;
            std::memcpy(&size, data.data() + pos, sizeof(uint64_t));
            pos += sizeof(uint64_t);

            std::vector<NaiveBitVector> output;
            output.resize(size);
            for (uint64_t i = 0; i < size; i++)
            {
                output[i] = NaiveBitVector::load_from_bytes(data, pos);
            }
            return output;
        }

        /**
         * @brief Returns the vector of NaiveBitVector instances loaded from a file stream \p ifs
         */
        static std::vector<NaiveBitVector> load_vector_from_file(std::ifstream &ifs)
        {
            uint64_t size = 0;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(uint64_t));

            std::vector<NaiveBitVector> output;
            output.resize(size);
            for (uint64_t i = 0; i < size; i++)
            {
                output[i] = NaiveBitVector::load_from_file(ifs);
            }

            return output;
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Update Operations
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Swap operation
         */
        void swap(NaiveBitVector &item)
        {
            std::swap(this->buffer_, item.buffer_);
            std::swap(this->num1_, item.num1_);
            std::swap(this->buffer_size_, item.buffer_size_);
            std::swap(this->bit_count_, item.bit_count_);
        }

        /**
         * @brief Remove all elements from the deque
         */
        void clear()
        {
            this->num1_ = 0;
            this->bit_count_ = 0;
            this->shrink_to_fit(0);
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
            if (size + 1 > MAX_BIT_LENGTH)
            {
                throw std::invalid_argument("Error: push_back()");
            }

            this->update_size_if_needed(size + 1);

            uint64_t block_index = (this->bit_count_) / 64;
            uint64_t bit_index = (this->bit_count_) % 64;
            this->buffer_[block_index] = stool::MSBByte::write_bit(this->buffer_[block_index], bit_index, value);
            this->bit_count_++;

            if (value)
            {
                this->num1_++;
            }
#if DEBUG
            assert(this->size() == prev_size + 1);
#endif
        }

        void push_back64(uint64_t value, uint8_t len = 64)
        {
            assert(len <= 64);
            if (len == 0)
                return;
            uint64_t size = this->size();
            if (size + len > MAX_BIT_LENGTH)
            {
                std::cout << "Error: push_back64() " << size << " " << len << " " << MAX_BIT_LENGTH << std::endl;
                throw std::invalid_argument("Error: push_back64()");
            }

            this->update_size_if_needed(size + len);

            uint64_t num1 = stool::MSBByte::popcount(value, len - 1);
            this->num1_ += num1;

            if (size == 0)
            {
                this->buffer_[0] = value;
            }
            else
            {
                uint64_t next_block_index = (this->bit_count_) / 64;
                uint8_t next_bit_index = (this->bit_count_) % 64;

                if (next_bit_index + len <= 64)
                {
                    this->buffer_[next_block_index] = stool::MSBByte::write_bits(this->buffer_[next_block_index], next_bit_index, len, value);
                }
                else
                {
                    uint64_t pref_len = 64 - next_bit_index;
                    uint64_t suf_len = len - pref_len;
                    this->buffer_[next_block_index] = stool::MSBByte::write_bits(this->buffer_[next_block_index], next_bit_index, pref_len, value);
                    this->buffer_[next_block_index + 1] = stool::MSBByte::write_bits(this->buffer_[next_block_index + 1], 0, suf_len, value << pref_len);
                }

                // stool::MSBByte::write_bits(this->buffer_, value, len, next_block_index, next_bit_index, this->buffer_size_);
            }
            this->bit_count_ += len;
        }
        template <typename T>
        void push_back64(const T &bits64_array, uint64_t bit_count, [[maybe_unused]] uint64_t array_size)
        {
            if (bit_count == 0)
                return;

            uint64_t size = this->size();
            if (size + bit_count > MAX_BIT_LENGTH)
            {
                throw std::invalid_argument("Error: push_back64()");
            }
            uint64_t old_size = this->size();

            this->shift_right(old_size, bit_count);

            this->replace_64bit_string_sequence(old_size, bits64_array, bit_count, array_size);
        }

        void push_front64(uint64_t value, uint8_t len = 64)
        {

            if (this->size() == 0)
            {
                this->push_back64(value, len);
            }
            else
            {
                this->insert_64bit_string(0, value, len);
            }
        }
        template <typename T>
        void push_front64(const T &bits64_array, uint64_t bit_count, [[maybe_unused]] uint64_t array_size)
        {
            if (bit_count == 0)
                return;

            uint64_t size = this->size();
            if (size + bit_count > MAX_BIT_LENGTH)
            {
                throw std::invalid_argument("Error: push_front64()");
            }

            this->shift_right(0, bit_count);
            this->replace_64bit_string_sequence(0, bits64_array, bit_count, array_size);
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
            if (size == 0)
            {
                this->push_back(value);
            }
            else
            {
                this->insert(0, value);
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
                this->bit_count_--;
            }
        }
        void pop_back(uint64_t len)
        {

            if (len == 0)
            {
                return;
            }
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
                    this->bit_count_ -= len;
                    this->update_size_if_needed(this->size());
                }
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
                this->erase(0, 1);
            }
        }

        void pop_front(uint64_t len)
        {
            if (len == 1)
            {
                this->pop_front();
            }
            else
            {
                this->erase(0, len);
            }
        }
        template <typename T>
        void pop_front(uint64_t len, T &output_bits64_array, [[maybe_unused]] uint64_t array_size)
        {
            if (len == 0)
            {
                return;
            }
            if (len > this->size())
            {
                throw std::invalid_argument("Error: pop_front()");
            }
            uint64_t block_size = (len - 1) / 64 + 1;
            for (uint64_t i = 0; i < block_size; i++)
            {
                output_bits64_array[i] = this->buffer_[i];
            }

            if (len == 1)
            {
                this->pop_front();
            }
            else
            {
                this->erase(0, len);
            }
        }

        void insert(size_t position, bool value)
        {
            uint64_t value64 = value ? (1ULL << 63) : 0;

            this->insert_64bit_string(position, value64, 1);
        }
        void insert_64bit_string(size_t position, uint64_t value, uint64_t len)
        {

            uint64_t size = this->size();
            if (size + len > MAX_BIT_LENGTH)
            {
                throw std::invalid_argument("Error: insert_64bit_string()");
            }

            if (position == size)
            {
                this->push_back64(value, len);
                assert(this->num1_ == this->rank1(0, this->size() - 1));
            }
            else if (position < size)
            {
                assert(this->num1_ == this->rank1(0, this->size() - 1));

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
        void insert_64bit_string(size_t position, const T &bits64_array, uint64_t bit_size, uint64_t array_size)
        {
            this->shift_right(position, bit_size);

            this->replace_64bit_string_sequence(position, bits64_array, bit_size, array_size);
        }

        void erase(size_t position)
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
            else if (position + len < size)
            {

                this->shift_left(position + len, len);
            }
        }
        void remove(size_t position)
        {
            this->erase(position, 1);
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
                        this->push_back64(0, 64);
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

                stool::MSBByte::shift_right(this->buffer_, position, len, this->buffer_size_);
                this->bit_count_ += len;

                assert(this->num1_ == this->rank1(0, this->size() - 1));

                /*
                uint64_t src_block_index = position / 64;
                uint64_t src_bit_index = position % 64;
                uint64_t dst_block_index = (position + len) / 64;
                uint64_t dst_bit_index = (position + len) % 64;

                stool::MSBByte::move_suffix_blocks_to_a_block_position<uint64_t *, TMP_BUFFER_SIZE>(this->buffer_, src_block_index, src_bit_index, dst_block_index, dst_bit_index, this->buffer_size_);
                */
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

                stool::MSBByte::shift_left(this->buffer_, position, len, this->buffer_size_);

                /*
                uint64_t dst_block_index = posL / 64;
                uint64_t dst_bit_index = posL % 64;
                uint64_t src_block_index = position / 64;
                uint64_t src_bit_index = position % 64;

                assert(dst_block_index <= src_block_index);

                stool::MSBByte::move_suffix_blocks_to_a_block_position<uint64_t *, TMP_BUFFER_SIZE>(this->buffer_, src_block_index, src_bit_index, dst_block_index, dst_bit_index, this->buffer_size_);
                */

                // stool::MSBByte::block_shift_left(this->buffer_, position, len, this->buffer_size_);

                this->num1_ -= removed_num1;
                this->bit_count_ -= len;

                this->update_size_if_needed(size - len);
            }
        }

        void replace(uint64_t position, bool value)
        {
            this->replace_64bit_string(position, value ? (1ULL << 63) : 0, 1);
        }
        void replace_64bit_string(uint64_t position, uint64_t value, uint64_t len)
        {
            assert(this->num1_ == this->rank1(0, this->size() - 1));
            if (position + len > this->size())
            {
                throw std::invalid_argument("Error: replace()");
            }
            if (len == 0)
            {
                return;
            }

            uint64_t block_index = position / 64;
            uint64_t bit_index = position % 64;

            uint64_t removed_bits = stool::MSBByte::access_64bits(this->buffer_, block_index, bit_index, this->buffer_size_);

            uint64_t added_num1 = stool::MSBByte::popcount(value, len - 1);
            uint64_t removed_num1 = stool::MSBByte::popcount(removed_bits, len - 1);
            this->num1_ += added_num1;
            this->num1_ -= removed_num1;

            stool::MSBByte::write_bits(this->buffer_, value, len, block_index, bit_index, this->buffer_size_);

            assert(this->num1_ == this->rank1(0, this->size() - 1));
        }

        template <typename T>
        void replace_64bit_string_sequence(uint64_t position, const T &bits64_array, uint64_t bit_size, uint64_t array_size)
        {
            if (bit_size == 0)
            {
                return;
            }

            uint64_t start_block_index = position / 64;
            uint64_t start_bit_index = position % 64;
            uint64_t end_block_index = (position + bit_size - 1) / 64;
            uint64_t end_bit_index = (position + bit_size - 1) % 64;

            uint64_t blockL_size = start_bit_index;
            uint64_t blockR_size = 64 - start_bit_index;

            if (start_block_index == end_block_index)
            {
                uint64_t old_block = this->buffer_[start_block_index];
                uint64_t removed_num = stool::Byte::popcount(old_block);
                uint64_t new_block = stool::MSBByte::write_bits(old_block, start_bit_index, bit_size, bits64_array[0]);
                uint64_t added_num = stool::Byte::popcount(new_block);
                this->num1_ += added_num;
                this->num1_ -= removed_num;
                this->buffer_[start_block_index] = new_block;
            }
            else
            {
                {
                    uint64_t old_block = this->buffer_[start_block_index];
                    uint64_t removed_num = stool::Byte::popcount(old_block);
                    uint64_t new_block = stool::MSBByte::write_suffix(old_block, blockR_size, bits64_array[0]);
                    uint64_t added_num = stool::Byte::popcount(new_block);
                    this->num1_ += added_num;
                    this->num1_ -= removed_num;
                    this->buffer_[start_block_index] = new_block;
                }

                if (blockL_size == 0)
                {
                    for (uint64_t i = start_block_index + 1; i < end_block_index; i++)
                    {
                        uint64_t old_block = this->buffer_[i];
                        uint64_t removed_num = stool::Byte::popcount(old_block);
                        uint64_t new_block = bits64_array[i - start_block_index];
                        uint64_t added_num = stool::Byte::popcount(new_block);
                        this->num1_ += added_num;
                        this->num1_ -= removed_num;
                        this->buffer_[i] = new_block;
                    }
                }
                else
                {
                    for (uint64_t i = start_block_index + 1; i < end_block_index; i++)
                    {
                        uint64_t old_block = this->buffer_[i];
                        uint64_t removed_num = stool::Byte::popcount(old_block);
                        uint64_t blockL = bits64_array[(i - 1) - start_block_index] << blockR_size;
                        uint64_t blockR = bits64_array[i - start_block_index] >> blockL_size;
                        uint64_t new_block = blockL | blockR;
                        uint64_t added_num = stool::Byte::popcount(new_block);
                        this->num1_ += added_num;
                        this->num1_ -= removed_num;
                        this->buffer_[i] = new_block;
                    }
                }

                {
                    uint64_t last_block_size = end_bit_index + 1;

                    /*
                    CircularBitPointer start_value_bp(array_size, 0, 0);
                    CircularBitPointer end_value_bp(array_size, 0, 0);
                    start_value_bp.add(bit_size - last_block_size);
                    end_value_bp.add(bit_size - 1);
                    uint64_t pattern = start_value_bp.read64(bits64_array);
                    */
                    uint64_t __block_index = (bit_size - last_block_size) / 64;
                    uint64_t __bit_index = (bit_size - last_block_size) % 64;
                    uint64_t pattern = stool::MSBByte::access_64bits(bits64_array, __block_index, __bit_index, array_size);
                    uint64_t old_block = this->buffer_[end_block_index];
                    uint64_t removed_num = stool::Byte::popcount(old_block);
                    assert(last_block_size <= 64 && last_block_size > 0);
                    uint64_t new_block = stool::MSBByte::write_prefix(old_block, last_block_size, pattern);
                    uint64_t added_num = stool::Byte::popcount(new_block);
                    this->num1_ += added_num;
                    this->num1_ -= removed_num;
                    this->buffer_[end_block_index] = new_block;
                }
            }
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

        /**
         * @brief Reduce buffer size to fit current content
         *
         * Resizes the buffer to the minimum size needed to hold current elements.
         */
        void shrink_to_fit(int64_t new_size)
        {
            new_size = std::max(new_size, (int64_t)0);

            int64_t current_size_index = this->get_current_buffer_size_index();
            int64_t appropriate_size_index = NaiveBitVector::get_appropriate_buffer_size_index(new_size);

            // uint64_t old_buffer_size = this->buffer_size_;
            uint64_t old_size = this->buffer_size_;

            if (appropriate_size_index + 1 < current_size_index || appropriate_size_index > current_size_index)
            {
                // this->reset_starting_position();
                uint64_t *tmp = this->buffer_;
                assert(tmp != nullptr);

                assert(appropriate_size_index < (int64_t)size_array.size());
                uint64_t new_size = size_array[appropriate_size_index];
                this->buffer_ = new uint64_t[new_size];
                this->buffer_size_ = new_size;

                // std::array<uint64_t, TMP_BUFFER_SIZE> tmp_array;
                uint64_t copy_size = std::min(old_size, new_size);

                if (copy_size > 0)
                {
                    std::memcpy(&this->buffer_[0], &tmp[0], copy_size * sizeof(uint64_t));
                }

                delete[] tmp;
            }

            // assert(this->size() == old_size);
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Static functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Calculate the serialized size of a deque
         *
         * @param item The deque to measure
         * @return uint64_t Size in bytes when serialized
         */
        static uint64_t get_byte_size(const NaiveBitVector &item)
        {
            uint64_t bytes = (sizeof(uint64_t)) + (item.buffer_size_ * sizeof(uint64_t));
            return bytes;
        }
        static uint64_t get_byte_size(const std::vector<NaiveBitVector> &items)
        {
            uint64_t size = sizeof(uint64_t);
            for (const auto &item : items)
            {
                size += get_byte_size(item);
            }
            return size;
        }
        static uint64_t get_appropriate_buffer_size_index(int64_t size)
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
        static uint64_t max_deque_size()
        {
            return (uint64_t)UINT16_MAX * 64;
        }

        //}@
    };
}
