#pragma once
#include <fstream>
#include <iostream>
#include <cassert>
#include <cstring>
#include <stdexcept>

#include "../../basic/byte.hpp"
#include "../../basic/lsb_byte.hpp"
#include "../../basic/packed_psum.hpp"
#include "../../basic/packed_search.hpp"
#include "../../debug/debug_printer.hpp"

namespace stool
{
    /*!
     * @brief A naive vector implementation using fixed-length codes for non-negative integer sequences \p S[0..n-1] [in progress]
     * @note The non-negative integer sequence \p S[0..n-1] is stored in the 64-bit integers buffer B[0..m-1]. Each integer of \p S is encoded as a fixed-length code of bit length \p 2^{x} for an integer \p x in { 1, 2, 4, 8, 16, 32, 64  }.
     * @tparam USE_PSUM Boolean parameter to enable/disable prefix sum maintenance
     */
    template <bool USE_PSUM = true>
    class NaiveFLCVector
    {
    private:
        inline static std::vector<int> size_array{1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 33, 40, 48, 58, 70, 84, 101, 122, 147, 177, 213, 256, 308, 370, 444, 533, 640, 768, 922, 1107, 1329, 1595, 1914, 2297, 2757, 3309, 3971, 4766};
        inline static const uint64_t MAX_SIZE = 4000;

        uint64_t *buffer_ = nullptr; // Buffer B[0..m-1]
        uint64_t psum_;              // The sum of the elements in integer sequence S[0..n-1]
        uint16_t size_;              // |S|
        uint16_t buffer_size_;       // |B|
        uint8_t code_type_;          // The bit length \p x of each integer in \p S, where \p x in { 1, 2, 4, 8, 16, 32, 64 }.

    public:
        // INDEX_TYPE deque_size_;

        /*!
         * @brief Integer sequence vector iterator
         */
        class NaiveFLCVectorIterator
        {

        public:
            NaiveFLCVector *_m_deq;
            uint64_t _m_idx;

            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;

            /**
             * @brief Construct an iterator
             *
             * @param _deque Pointer to the deque
             * @param _idx Index position
             */
            NaiveFLCVectorIterator(NaiveFLCVector *_deque, uint64_t _idx) : _m_deq(_deque), _m_idx(_idx) {}

            bool is_end() const
            {
                return this->_m_idx >= this->_m_deq->size();
            }

            /**
             * @brief Dereference operator
             *
             * @return T The element at the current position
             */
            uint64_t operator*() const
            {
                return (*_m_deq)[this->_m_idx];
            }

            /*
            uint64_t get_deque_position() const
            {
                return this->_m_deq->get_deque_position(this->_m_idx);
            }
            */

            /**
             * @brief Pre-increment operator
             *
             * @return SimpleDequeIterator& Reference to the incremented iterator
             */
            NaiveFLCVectorIterator &operator++()
            {
                ++this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-increment operator
             *
             * @return SimpleDequeIterator Copy of the iterator before increment
             */
            NaiveFLCVectorIterator operator++(int)
            {
                NaiveFLCVectorIterator temp = *this;

                ++(*this);
                return temp;
            }

            /**
             * @brief Pre-decrement operator
             *
             * @return SimpleDequeIterator& Reference to the decremented iterator
             */
            NaiveFLCVectorIterator &operator--()
            {
                --this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-decrement operator
             *
             * @return SimpleDequeIterator Copy of the iterator before decrement
             */
            NaiveFLCVectorIterator operator--(int)
            {
                NaiveFLCVectorIterator temp = *this;
                --(*this);
                return temp;
            }

            /**
             * @brief Addition operator for random access
             *
             * @param n Number of positions to advance
             * @return SimpleDequeIterator Iterator at the new position
             */
            NaiveFLCVectorIterator operator+(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return NaiveFLCVectorIterator(this->_m_deq, sum);
            }

            /**
             * @brief Compound addition assignment
             *
             * @param n Number of positions to advance
             * @return SimpleDequeIterator& Reference to this iterator
             */
            NaiveFLCVectorIterator &operator+=(difference_type n)
            {
                this->_m_idx += n;
                return *this;
            }

            /**
             * @brief Subtraction operator for random access
             *
             * @param n Number of positions to retreat
             * @return SimpleDequeIterator Iterator at the new position
             */
            NaiveFLCVectorIterator operator-(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx - (int16_t)n;
                return NaiveFLCVectorIterator(this->_m_deq, sum);
            }

            /**
             * @brief Compound subtraction assignment
             *
             * @param n Number of positions to retreat
             * @return SimpleDequeIterator& Reference to this iterator
             */
            NaiveFLCVectorIterator &operator-=(difference_type n)
            {
                this->_m_idx -= n;
                return *this;
            }

            /**
             * @brief Difference between two iterators
             *
             * @param other The other iterator
             * @return difference_type Number of positions between iterators
             */
            difference_type operator-(const NaiveFLCVectorIterator &other) const
            {
                return (int16_t)this->_m_idx - (int16_t)other._m_idx;
            }

            /**
             * @brief Subscript operator for random access
             *
             * @param n Offset from current position
             * @return T& Reference to the element at offset n
             */
            uint64_t &operator[](difference_type n)
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }

            /**
             * @brief Const subscript operator for random access
             *
             * @param n Offset from current position
             * @return const T& Const reference to the element at offset n
             */
            const uint64_t &operator[](difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }

            /**
             * @brief Equality comparison
             */
            bool operator==(const NaiveFLCVectorIterator &other) const { return _m_idx == other._m_idx; }

            /**
             * @brief Inequality comparison
             */
            bool operator!=(const NaiveFLCVectorIterator &other) const { return _m_idx != other._m_idx; }

            /**
             * @brief Less than comparison
             */
            bool operator<(const NaiveFLCVectorIterator &other) const { return this->_m_idx < other._m_idx; }

            /**
             * @brief Greater than comparison
             */
            bool operator>(const NaiveFLCVectorIterator &other) const { return this->_m_idx > other._m_idx; }

            /**
             * @brief Less than or equal comparison
             */
            bool operator<=(const NaiveFLCVectorIterator &other) const { return this->_m_idx <= other._m_idx; }

            /**
             * @brief Greater than or equal comparison
             */
            bool operator>=(const NaiveFLCVectorIterator &other) const { return this->_m_idx >= other._m_idx; }
        };

    public:
        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Constructors and Destructor
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Default constructor with |S| = 0 and |B| = 2
         */
        NaiveFLCVector()
        {
            this->initialize();
        }
        /**
         * @brief Constructor with |S| = 0 and |B| = buffer_size
         */
        NaiveFLCVector(uint64_t buffer_size) noexcept
        {
            this->buffer_size_ = buffer_size;
            this->code_type_ = 0;
            this->size_ = 0;
            this->psum_ = 0;
            this->buffer_ = new uint64_t[this->buffer_size_];
        }

        /**
         * @brief Constructor with S = _S
         */
        NaiveFLCVector(const std::vector<uint64_t> &_S) noexcept
        {
            this->initialize();
            if (_S.size() == 0)
            {
                this->initialize();
            }
            else
            {
                for (auto &value : _S)
                {
                    this->push_back(value);
                }
            }
        }

        /**
         * @brief Copy constructor
         */
        NaiveFLCVector(const NaiveFLCVector &other) noexcept
        {
            this->buffer_size_ = other.buffer_size_;
            this->code_type_ = other.code_type_;
            this->size_ = other.size_;
            this->psum_ = other.psum_;
            this->buffer_ = new uint64_t[this->buffer_size_];

            std::memcpy(this->buffer_, other.buffer_, this->buffer_size_ * sizeof(uint64_t));
        }

        /**
         * @brief Move constructor
         */
        NaiveFLCVector(NaiveFLCVector &&other) noexcept
            : buffer_(other.buffer_),
              psum_(other.psum_),
              size_(other.size_),
              buffer_size_(other.buffer_size_),
              code_type_(other.code_type_)
        {
            // Reset the source object
            other.buffer_ = nullptr;
            other.buffer_size_ = 0;
            other.code_type_ = 0;
            other.size_ = 0;
            other.psum_ = 0;
        }

        /**
         * @brief Destructor
         */
        ~NaiveFLCVector()
        {
            if (this->buffer_ != nullptr)
            {
                delete[] this->buffer_;
                this->buffer_ = nullptr;
            }
        }

        /**
         * @brief Initialize this instance as an empty vector
         *
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
            this->size_ = 0;
            this->code_type_ = 0;
            this->psum_ = 0;
            this->buffer_size_ = 2;
        }

        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Begin and end iterators
        ////////////////////////////////////////////////////////////////////////////////
        //@{
        /**
         * @brief Get iterator to the first element
         *
         * @return SimpleDequeIterator Iterator pointing to the beginning
         */
        NaiveFLCVectorIterator begin() const
        {
            return NaiveFLCVectorIterator(const_cast<NaiveFLCVector *>(this), 0);
        }

        /**
         * @brief Get iterator past the last element
         *
         * @return SimpleDequeIterator Iterator pointing past the end
         */
        NaiveFLCVectorIterator end() const
        {
            return NaiveFLCVectorIterator(const_cast<NaiveFLCVector *>(this), this->size());
        }

        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Operators
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Move assignment operator
         *
         * @param other The NaiveBitVector to move from
         * @return NaiveBitVector& Reference to this object
         */
        NaiveFLCVector &operator=(NaiveFLCVector &&other) noexcept
        {
            if (this != &other)
            {
                this->buffer_ = other.buffer_;
                this->buffer_size_ = other.buffer_size_;
                this->code_type_ = other.code_type_;
                this->size_ = other.size_;
                this->psum_ = other.psum_;

                other.buffer_ = nullptr;
                other.buffer_size_ = 0;
                other.code_type_ = 0;
                other.size_ = 0;
                other.psum_ = 0;
            }
            return *this;
        }
        /**
         * @brief Const subscript operator for element access
         *
         * @param index Position of the element to access
         * @return const T& Const reference to the element
         */
        uint64_t operator[](size_t index) const
        {
            // PackedBitType code_type = (PackedBitType)this->code_type_;
            uint8_t code_length = 1ULL << this->code_type_;
            uint64_t pos = index << this->code_type_;
            uint64_t block_index = pos / 64;
            uint64_t bit_index = pos % 64;
            uint64_t block = this->buffer_[block_index];
            uint64_t end_bit_index = bit_index + code_length - 1;
            uint64_t mask = UINT64_MAX >> (64 - code_length);

            uint64_t value = block >> (63 - end_bit_index);
            value = value & mask;
            return value;
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Lightweight functions for accessing to properties of this class
        ////////////////////////////////////////////////////////////////////////////////
        //@{
        /**
         * @brief Return the number of lements in S (i.e., |S|)
         */
        size_t size() const
        {
            return this->size_;
        }

        /**
         * @brief Check if the integer sequence is empty (i.e., |S| == 0)
         */
        bool empty() const
        {
            return this->size_ == 0;
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
                return sizeof(uint64_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t) + (sizeof(uint64_t) * this->buffer_size_);
            }
        }

        /**
         * @brief Returns the size of the unused memory in bytes (i.e., ((|B| * 8) - (|S| * W / 8), where W is the bitsize of elements in S)
         */
        uint64_t unused_size_in_bytes() const
        {
            uint64_t buffer_bytes = this->buffer_size_ * sizeof(uint64_t);
            uint64_t bitsize = 1ULL << this->code_type_;
            uint64_t item_bytes = (this->size_ * bitsize) / 8;
            return buffer_bytes - item_bytes;
        }

        /**
         * @brief Return the maximum number of bits that can be stored without resizing the buffer (i.e., |S| * 64 / W, where W is the bitsize of elements in S)
         */
        size_t capacity() const
        {
            uint64_t code_length = 1ULL << this->code_type_;
            return (this->buffer_size_ * 64) / code_length;
        }

        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Main queries (Access, search, and psum operations)
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Returns the first element \p S[0] of \p S
         */
        uint64_t head() const
        {
            return this->at(0);
        }

        /**
         * @brief Returns the last element \p S[n-1] of \p S
         */
        uint64_t tail() const
        {
            return this->at(this->size() - 1);
        }

        /**
         * @brief Returns the (i+1)-th element \p S[i] of \p S
         */
        uint64_t at(uint64_t i) const
        {
            assert(i < this->size());
            return (*this)[i];
        }

        /**
         * @brief Returns the sum of the elements in \p S[0..n-1] (i.e., \p psum(n-1))
         * @note \p O(1) time
         */
        uint64_t psum() const
        {
            return this->psum_;
        }

        /**
         * @brief Returns the sum of integers in \p S[0..i]
         * @note \p O(i) time
         */
        uint64_t psum(uint64_t i) const
        {
            uint64_t sum = stool::PackedPSum::psum(this->buffer_, i, (stool::PackedPSum::PackedBitType)this->code_type_, this->buffer_size_);
            return sum;
        }

        /**
         * @brief Returns the sum of integers in \p S[i..j]
         * @note \p O(j-i) time
         */
        uint64_t psum(uint64_t i, uint64_t j) const
        {
            uint64_t sum = stool::PackedPSum::psum(this->buffer_, i, j, (stool::PackedPSum::PackedBitType)this->code_type_, this->buffer_size_);
            return sum;
        }

        /**
         * @brief Returns the sum of integers in \p S[(n-1)-i..n-1]
         * @note \p O(n) time
         */
        uint64_t reverse_psum(uint64_t i) const
        {
            uint64_t size = this->size();
            if (size == 0)
            {
                return 0;
            }
            else if (i + 1 == size)
            {
                assert(this->verify());
                assert(this->psum() == this->psum(size - i - 1, size - 1));
                return this->psum(size - i - 1, size - 1);

                // return this->psum();
            }
            else
            {
                return this->psum(size - i - 1, size - 1);
            }
        }

        /**
         * @brief Returns the first position \p p such that psum(p) >= x if such a position exists, otherwise returns -1
         * @note \p O(p) time
         */
        int64_t search(uint64_t x) const noexcept
        {
            return stool::PackedSearch::search(this->buffer_, x, (stool::PackedSearch::PackedBitType)this->code_type_, this->psum_, this->buffer_size_);
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Convertion functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /*!
         * @brief Returns \p S as a string
         */
        std::string to_string() const
        {
            std::string s;
            s += "[";
            for (uint64_t i = 0; i < this->size(); i++)
            {
                s += std::to_string(this->at(i));
                if (i < this->size() - 1)
                {
                    s += ", ";
                }
            }
            s += "]";
            return s;
        }

        /*!
         * @brief Returns \p S as a deque
         */
        std::deque<uint64_t> to_deque() const
        {
            std::deque<uint64_t> r;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                r.push_back(this->at(i));
            }
            return r;
        }

        /*!
         * @brief Returns \p S as a vector
         */
        std::vector<uint64_t> to_vector() const
        {
            std::vector<uint64_t> v;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                v.push_back(this->at(i));
            }
            return v;
        }

        /*!
         * @brief Clears a given sequence \p output_vec (e.g., std::vector<uint64_t>) and fills it with the elements of \p S
         */
        template <typename VEC = std::vector<uint64_t>>
        void to_values(VEC &output_vec) const
        {
            output_vec.clear();
            output_vec.resize(this->size());
            for (uint64_t i = 0; i < this->size(); i++)
            {
                output_vec[i] = this->at(i);
            }
        }

        /*!
         * @brief Returns the 64-bit integers \p B as a binary string
         */
        std::string convert_buffer_to_binary_string() const
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
            std::cout << "NaiveFLCVector = {" << std::endl;
            std::cout << "size = " << this->size() << std::endl;
            std::cout << "capacity = " << this->capacity() << std::endl;
            std::cout << "buffer_size = " << this->buffer_size_ << std::endl;
            std::cout << "code_type = " << (int)this->code_type_ << std::endl;
            std::cout << "psum = " << this->psum_ << std::endl;

            if (this->buffer_ != nullptr)
            {
                std::cout << "Buffer: " << this->convert_buffer_to_binary_string() << std::endl;
            }
            else
            {
                std::cout << "Buffer: nullptr" << std::endl;
            }
            std::cout << "Content: " << this->to_string() << std::endl;

            std::cout << "}" << std::endl;
        }

        /**
         * @brief Verifies this instance
         * @note this function is used to debug this instance
         */
        bool verify() const
        {
            uint64_t true_sum = 0;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                true_sum += this->at(i);
            }
            if (true_sum != this->psum())
            {
                std::cout << "psum = " << this->psum() << ", true_sum = " << true_sum << std::endl;
                throw std::runtime_error("Error: verify");
            }
            return true;
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Update Operations
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Set a given value \p v at a given position \p i in \p S
         * @note \p O(|S|) time
         */
        void set_value(uint64_t position_i, uint64_t value_v)
        {
            assert(position_i < this->size());

            uint8_t code_type_candidate = (uint8_t)stool::PackedPSum::get_code_type(value_v);
            uint64_t new_code_type = std::max(this->code_type_, code_type_candidate);
            if (new_code_type != this->code_type_)
            {
                this->shrink_to_fit(this->size(), new_code_type);
            }
            uint64_t code_length = 1ULL << new_code_type;

            uint64_t new_pos = position_i << new_code_type;
            uint64_t block_index = new_pos / 64;
            uint8_t bit_index = new_pos % 64;

            uint64_t old_value = NaiveFLCVector::read_as_64bit_integer(this->buffer_[block_index], bit_index, code_length);
            if (value_v > old_value)
            {
                this->psum_ += value_v - old_value;
            }
            else
            {
                this->psum_ -= old_value - value_v;
            }

            uint64_t write_value = value_v << (64 - code_length);
            this->buffer_[block_index] = stool::MSBByte::write_bits(this->buffer_[block_index], bit_index, code_length, write_value);

            assert(this->verify());
        }

        /**
         * @brief Set the value \p \p S[i+delta] at a given position \p i in \p S
         * @note \p O(|S|) time
         */
        void increment(uint64_t i, int64_t delta)
        {
            uint64_t new_value = this->at(i) + delta;
            this->set_value(i, new_value);
            assert(this->verify());
        }

        /**
         * @brief Swap operation
         */
        void swap(NaiveFLCVector &item)
        {
            std::swap(this->buffer_, item.buffer_);
            std::swap(this->psum_, item.psum_);
            std::swap(this->size_, item.size_);
            std::swap(this->buffer_size_, item.buffer_size_);
            std::swap(this->code_type_, item.code_type_);
        }
        /**
         * @brief Remove all elements in \p S and appropriately resize the buffer \p B
         */
        void clear()
        {
            this->size_ = 0;
            this->psum_ = 0;
            this->code_type_ = 0;
            this->shrink_to_fit(0, this->code_type_);
        }

        /**
         * @brief Reduce buffer size to fit current content
         */
        void shrink_to_fit(int64_t new_element_count, uint8_t new_code_type)
        {
            assert(new_code_type <= 6);
            uint8_t new_code_length = 1ULL << new_code_type;

            int64_t current_size_index = this->get_current_buffer_size_index();
            int64_t appropriate_size_index = NaiveFLCVector::get_appropriate_buffer_size_index2(new_element_count, new_code_type);

            if (this->code_type_ == new_code_type)
            {

                if (appropriate_size_index + 1 < current_size_index || appropriate_size_index > current_size_index)
                {
                    // this->reset_starting_position();
                    uint64_t old_size = this->buffer_size_;
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
            }
            else
            {

                uint64_t *tmp = this->buffer_;
                assert(tmp != nullptr);
                assert(appropriate_size_index < (int64_t)size_array.size());
                uint64_t new_size = size_array[appropriate_size_index];
                this->buffer_ = new uint64_t[new_size];
                uint64_t old_code_length = 1ULL << this->code_type_;
                for (uint64_t i = 0; i < this->size_; i++)
                {
                    uint64_t old_pos = i << this->code_type_;
                    uint64_t new_pos = i << new_code_type;
                    uint64_t old_block_index = old_pos / 64;
                    uint64_t new_block_index = new_pos / 64;
                    uint64_t old_bit_index = old_pos % 64;
                    uint64_t new_bit_index = new_pos % 64;

                    uint64_t old_value = NaiveFLCVector::read_as_64bit_integer(tmp[old_block_index], old_bit_index, old_code_length);
                    old_value = old_value << (64 - new_code_length);
                    this->buffer_[new_block_index] = stool::MSBByte::write_bits(this->buffer_[new_block_index], new_bit_index, new_code_length, old_value);
                }
                delete[] tmp;

                this->code_type_ = new_code_type;
                this->buffer_size_ = new_size;
            }
        }

        /**
         * @brief Add a given integer to the end of \p S
         * @note \p O(|S|) time
         */
        void push_back(uint64_t value)
        {
            uint64_t size = this->size();
            if (size + 1 > MAX_SIZE)
            {
                std::cout << "Error: push_back() " << size + 1 << " > " << MAX_SIZE << std::endl;
                throw std::invalid_argument("Error: push_back()");
            }

            uint8_t code_type_candidate = (uint8_t)stool::PackedPSum::get_code_type(value);
            uint64_t new_code_type = std::max(this->code_type_, code_type_candidate);
            assert(new_code_type <= 6);

            this->shrink_to_fit(size + 1, new_code_type);
            uint64_t code_length = 1ULL << new_code_type;
            uint64_t pos = this->size_ << new_code_type;

            uint64_t block_index = pos / 64;
            uint64_t bit_index = pos % 64;

            uint64_t write_value = value << (64 - code_length);
            this->buffer_[block_index] = stool::MSBByte::write_bits(this->buffer_[block_index], bit_index, code_length, write_value);

            this->size_++;
            this->psum_ += value;

#ifdef DEBUG
            uint64_t v = this->at(this->size() - 1);
            if (v != value)
            {
                std::cout << "push_back: " << v << " != " << value << std::endl;
                std::cout << "new_code_type: " << new_code_type << std::endl;
                std::cout << "code_candidate: " << code_type_candidate << std::endl;
            }
            assert(v == value);
#endif

            assert(this->verify());
        }
        /**
         * @brief Add a given sequence \p Q[0..k-1] to the end of \p S[0..n-1] (i.e., \p S = S[0..n-1]Q[0..k-1])
         * @note \p O(|S| + |Q|) time
         */
        void push_back(const std::vector<uint64_t> &new_items_Q)
        {
            for (uint64_t v : new_items_Q)
            {
                this->push_back(v);
            }

            assert(this->verify());
#ifdef DEBUG
            for (uint64_t i = 0; i < new_items.size(); i++)
            {
                uint64_t v = this->at(this->size() + i - new_items.size());
                assert(v == new_items[i]);
            }
#endif
        }

        /**
         * @brief Add a given value to the beginning of \p S
         * @note \p O(|S|) time
         */
        void push_front(uint64_t value)
        {
            this->insert(0, value);
#ifdef DEBUG
            uint64_t v = this->at(0);
            assert(v == value);
#endif

            assert(this->verify());
        }

        /**
         * @brief Add a given sequence \p Q[0..k-1] to the beginning of \p S[0..n-1] (i.e., \p S = Q[0..k-1]S[0..n-1])
         * @note \p O(|S| + |Q|) time
         */
        void push_front(const std::vector<uint64_t> &new_items_Q)
        {
            uint64_t max_value = 0;
            uint64_t x_sum = 0;
            uint64_t x_size = new_items_Q.size();
            for (uint64_t v : new_items_Q)
            {
                if (v > max_value)
                {
                    max_value = v;
                }
                x_sum += v;
            }
            uint8_t code_type_candidate = (uint8_t)stool::PackedPSum::get_code_type(max_value);
            uint64_t new_code_type = std::max(this->code_type_, code_type_candidate);
            assert(new_code_type <= 6);

            this->shift_right(0, x_size, new_code_type);
            uint64_t code_length = 1ULL << new_code_type;

            for (uint64_t i = 0; i < x_size; i++)
            {
                uint64_t pos = i << new_code_type;
                uint64_t block_index = pos / 64;
                uint64_t bit_index = pos % 64;
                uint64_t value = new_items_Q[i] << (64 - code_length);
                assert(block_index < this->buffer_size_);
                this->buffer_[block_index] = stool::MSBByte::write_bits(this->buffer_[block_index], bit_index, code_length, value);
            }

            this->psum_ += x_sum;

            assert(this->verify());
#ifdef DEBUG
            for (uint64_t i = 0; i < new_items.size(); i++)
            {
                uint64_t v = this->at(i);
                assert(v == new_items[i]);
            }
#endif
        }

        /**
         * @brief Remove the last element from S and return it
         * @note \p O(|S|) time
         */
        uint64_t pop_back()
        {
            uint64_t size = this->size();
            if (size == 0)
            {
                throw std::invalid_argument("Error: pop_back()");
            }
            else if (size == 1)
            {
                uint64_t value = this->at(0);
                this->clear();
                return value;
            }
            else
            {
                uint64_t value = this->at(size - 1);
                this->shrink_to_fit(size - 1, this->code_type_);

                this->psum_ -= value;
                this->size_--;

                assert(this->verify());
                return value;
            }
        }
        /**
         * @brief Remove the last \p len elements from S and return them as a vector
         * @note \p O(|S|) time
         */
        std::vector<uint64_t> pop_back(uint64_t len)
        {
            std::vector<uint64_t> r;
            r.resize(len, UINT64_MAX);
            for (uint64_t i = 0; i < len; i++)
            {
                uint64_t v = this->pop_back();
                r[len - i - 1] = v;
            }
            assert(this->verify());

            return r;
        }
        /**
         * @brief Remove the first element from \p S and return it
         * @note \p O(|S|) time
         */
        uint64_t pop_front()
        {
            uint64_t value = this->remove(0);
            assert(this->verify());
            return value;
        }

        /**
         * @brief Remove the first \p len elements from \p S and return them as a vector
         * @note \p O(|S|) time
         */
        std::vector<uint64_t> pop_front(uint64_t len)
        {
            std::vector<uint64_t> r;
            r.resize(len, UINT64_MAX);
            uint64_t code_length = 1ULL << this->code_type_;
            for (uint64_t i = 0; i < len; i++)
            {
                uint64_t pos = i << this->code_type_;
                uint64_t block_index = pos / 64;
                uint64_t bit_index = pos % 64;
                uint64_t value = NaiveFLCVector::read_as_64bit_integer(this->buffer_[block_index], bit_index, code_length);
                r[i] = value;
            }
            this->remove(0, len);
            assert(this->verify());
            return r;
        }

        /**
         * @brief Insert a given integer \p value into \p S as the \p (pos+1)-th element
         * @note \p O(|S|) time
         */
        void insert(uint64_t pos, uint64_t value)
        {
            uint64_t size = this->size();

            if (pos > size)
            {
                throw std::invalid_argument("NaiveFLCVector::insert: The position is out of range");
            }
            else if (size == MAX_SIZE)
            {
                throw std::invalid_argument("NaiveFLCVector::insert: The size is too large");
            }

            if (pos == size)
            {
                this->push_back(value);
            }
            else
            {
                uint8_t code_type_candidate = (uint8_t)stool::PackedPSum::get_code_type(value);
                uint64_t new_code_type = std::max(this->code_type_, code_type_candidate);
                assert(new_code_type <= 6);
                uint64_t code_length = 1ULL << new_code_type;
                this->shift_right(pos, 1, new_code_type);

                uint64_t new_pos = pos << new_code_type;
                uint64_t new_block_index = new_pos / 64;
                uint64_t new_bit_index = new_pos % 64;
                uint64_t write_value = value << (64 - code_length);
                this->buffer_[new_block_index] = stool::MSBByte::write_bits(this->buffer_[new_block_index], new_bit_index, code_length, write_value);
                this->psum_ += value;
            }
            assert(this->verify());
        }
        /**
         * @brief Remove the element at the position \p pos from \p S and return it
         * @note \p O(|S|) time
         */
        uint64_t remove(uint64_t pos)
        {

            uint64_t size = this->size();

            if (pos >= size)
            {
                throw std::invalid_argument("NaiveFLCVector::remove: The position is out of range");
            }

            if (pos + 1 == size)
            {
                return this->pop_back();
            }
            else
            {
                uint64_t value = this->at(pos);
                this->shift_left(pos + 1, 1);
                return value;
            }
            assert(this->verify());
        }
        /**
         * @brief Remove the \p len elements starting from the position \p pos from \p S
         * @note \p O(|S|) time
         */
        void remove(uint64_t pos, uint64_t len)
        {
            if (pos + len == this->size())
            {
                this->pop_back(len);
            }
            else
            {
                this->shift_left(pos + len, len);
            }
            assert(this->verify());
        }

        /**
         * @brief Change \p S[0..n-1] to \p S[0..p-1]0^{len}S[p..n-1]  
         * @note \p O(|S|) time
         */
        void shift_right(uint64_t position_p, uint64_t len, uint64_t new_code_type)
        {
            uint64_t size = this->size();
            // uint64_t code_length = 1ULL << new_code_type;

            this->shrink_to_fit(size + len, new_code_type);
            this->size_ += len;

            uint64_t bit_position = position_p << new_code_type;
            uint64_t bit_length = len << new_code_type;

            stool::MSBByte::shift_right(this->buffer_, bit_position, bit_length, this->buffer_size_);
            assert(this->verify());
        }

        /**
         * @brief Change \p S[0..n-1] to \p S[0..p-len-1]S[p..n-1]  
         * @note \p O(|S|) time
         */
        void shift_left(uint64_t position_p, uint64_t len)
        {
            uint64_t size = this->size();
            uint64_t removed_sum = this->psum(position_p - len, position_p - 1);
            uint64_t new_size = size - len;
            uint64_t bit_position = position_p << this->code_type_;
            uint64_t bit_length = len << this->code_type_;
            // uint64_t move_size = size - position;
            // uint64_t code_length = 1ULL << this->code_type_;

            stool::MSBByte::shift_left(this->buffer_, bit_position, bit_length, this->buffer_size_);
            this->shrink_to_fit(new_size, this->code_type_);
            this->psum_ -= removed_sum;
            this->size_ -= len;
            assert(this->verify());
        }
        
        //}@

    private:
        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Private functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{
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
        //}@

    public:
        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Load, save, and builder functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Construct an instance such that \p S = \p values
         */
        static NaiveFLCVector build(const std::vector<uint64_t> &values)
        {
            NaiveFLCVector flc_vector(values);
            return flc_vector;
        }

        /**
         * @brief Save the given instance \p item to a byte vector \p output at the position \p pos
         */
        static void store_to_bytes(const NaiveFLCVector &item, std::vector<uint8_t> &output, uint64_t &pos)
        {
            uint64_t bytes = item.size_in_bytes();
            if (output.size() < pos + bytes)
            {
                output.resize(pos + bytes);
            }

            std::memcpy(output.data() + pos, &item.psum_, sizeof(item.psum_));
            pos += sizeof(item.psum_);
            std::memcpy(output.data() + pos, &item.size_, sizeof(item.size_));
            pos += sizeof(item.size_);
            std::memcpy(output.data() + pos, &item.buffer_size_, sizeof(item.buffer_size_));
            pos += sizeof(item.buffer_size_);
            std::memcpy(output.data() + pos, &item.code_type_, sizeof(item.code_type_));
            pos += sizeof(item.code_type_);
            std::memcpy(output.data() + pos, item.buffer_, sizeof(uint64_t) * item.buffer_size_);
            pos += sizeof(uint64_t) * item.buffer_size_;
        }

        /**
         * @brief Save the given vector of NaiveFLCVector instances \p items to a byte vector \p output at the position \p pos
         */
        static void store_to_bytes(const std::vector<NaiveFLCVector> &items, std::vector<uint8_t> &output, uint64_t &pos)
        {
            uint64_t size = items.size();
            std::memcpy(output.data() + pos, &size, sizeof(size));
            pos += sizeof(size);

            for (auto &it : items)
            {
                NaiveFLCVector::store_to_bytes(it, output, pos);
            }
        }

        /**
         * @brief Save the given instance \p item to a file stream \p os
         */
        static void store_to_file(const NaiveFLCVector &item, std::ofstream &os)
        {
            os.write(reinterpret_cast<const char *>(&item.psum_), sizeof(item.psum_));
            os.write(reinterpret_cast<const char *>(&item.size_), sizeof(item.size_));
            os.write(reinterpret_cast<const char *>(&item.buffer_size_), sizeof(item.buffer_size_));
            os.write(reinterpret_cast<const char *>(&item.code_type_), sizeof(item.code_type_));
            os.write(reinterpret_cast<const char *>(item.buffer_), sizeof(uint64_t) * item.buffer_size_);
        }
        /**
         * @brief Save the given vector of NaiveFLCVector instances \p items to a file stream \p os
         */
        static void store_to_file(const std::vector<NaiveFLCVector> &items, std::ofstream &os)
        {
            uint64_t size = items.size();
            os.write(reinterpret_cast<const char *>(&size), sizeof(size));
            for (auto &it : items)
            {
                NaiveFLCVector::store_to_file(it, os);
            }
        }


        /**
         * @brief Returns the NaiveFLCVector instance loaded from a byte vector \p data at the position \p pos
         */
        static NaiveFLCVector load_from_bytes(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t _psum;
            uint16_t _size;
            uint16_t _buffer_size;
            uint8_t _code_type;

            std::memcpy(&_psum, data.data() + pos, sizeof(_psum));
            pos += sizeof(_psum);
            std::memcpy(&_size, data.data() + pos, sizeof(_size));
            pos += sizeof(_size);
            std::memcpy(&_buffer_size, data.data() + pos, sizeof(_buffer_size));
            pos += sizeof(_buffer_size);
            std::memcpy(&_code_type, data.data() + pos, sizeof(_code_type));
            pos += sizeof(_code_type);

            NaiveFLCVector r(_buffer_size);
            r.psum_ = _psum;
            r.size_ = _size;
            r.buffer_size_ = _buffer_size;
            r.code_type_ = _code_type;

            std::memcpy(r.buffer_, data.data() + pos, sizeof(uint64_t) * r.buffer_size_);
            pos += sizeof(uint64_t) * r.buffer_size_;

            return r;
        }

        /**
         * @brief Returns the NaiveFLCVector instance loaded from a file stream \p ifs
         */
        static NaiveFLCVector load_from_file(std::ifstream &ifs)
        {
            uint64_t _psum;
            uint16_t _size;
            uint16_t _buffer_size;
            uint8_t _code_type;
            ifs.read(reinterpret_cast<char *>(&_psum), sizeof(_psum));
            ifs.read(reinterpret_cast<char *>(&_size), sizeof(_size));
            ifs.read(reinterpret_cast<char *>(&_buffer_size), sizeof(_buffer_size));
            ifs.read(reinterpret_cast<char *>(&_code_type), sizeof(_code_type));

            NaiveFLCVector r(_buffer_size);
            r.psum_ = _psum;
            r.size_ = _size;
            r.buffer_size_ = _buffer_size;
            r.code_type_ = _code_type;
            ifs.read(reinterpret_cast<char *>(r.buffer_), sizeof(uint64_t) * (size_t)_buffer_size);

            return r;
        }
        /**
         * @brief Returns the vector of NaiveFLCVector instances loaded from a byte vector \p data at the position \p pos
         */

        static std::vector<NaiveFLCVector> load_vector_from_bytes(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t size;
            std::memcpy(&size, data.data() + pos, sizeof(size));
            pos += sizeof(size);

            std::vector<NaiveFLCVector> r;
            for (uint64_t i = 0; i < size; i++)
            {
                r.push_back(NaiveFLCVector::load_from_bytes(data, pos));
            }
            return r;
        }


        /**
         * @brief Returns the vector of NaiveFLCVector instances loaded from a file stream \p ifs
         */
        static std::vector<NaiveFLCVector> load_vector_from_file(std::ifstream &ifs)
        {
            uint64_t size = 0;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(size));

            std::vector<NaiveFLCVector> r;
            for (uint64_t i = 0; i < size; i++)
            {
                r.push_back(NaiveFLCVector::load_from_file(ifs));
            }
            return r;
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Other static functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Returns the total memory usage in bytes of a given instance
         */
        static uint64_t get_byte_size(const NaiveFLCVector &item)
        {
            uint64_t bytes = sizeof(item.psum_) + sizeof(item.size_) + sizeof(item.buffer_size_) + sizeof(item.code_type_) + (sizeof(uint64_t) * item.buffer_size_);
            return bytes;
        }

        /**
         * @brief Returns the total memory usage in bytes of a vector of NaiveFLCVector instances \p items
         */
        static uint64_t get_byte_size(const std::vector<NaiveFLCVector> &items)
        {
            uint64_t bytes = sizeof(uint64_t);
            for (auto &it : items)
            {
                bytes += NaiveFLCVector::get_byte_size(it);
            }
            return bytes;
        }

        //}@

    private:
        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Private static functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{
        static uint64_t read_as_64bit_integer(uint64_t B, uint8_t i, uint8_t len)
        {
            uint64_t end_bit_index = i + len - 1;
            uint64_t mask = UINT64_MAX >> (64 - len);

            uint64_t value = B >> (63 - end_bit_index);
            value = value & mask;

            return value;
        }
        static uint64_t get_appropriate_buffer_size_index2(int64_t num_elements, uint8_t code_type)
        {
            uint64_t total_code_size = num_elements << code_type;

            for (uint64_t i = 0; i < size_array.size(); i++)
            {
                uint64_t xsize = size_array[i] * 64;
                if (xsize > total_code_size)
                {
                    return i;
                }
            }
            throw std::runtime_error("size is too large");
        }
        //}@
    };
}
