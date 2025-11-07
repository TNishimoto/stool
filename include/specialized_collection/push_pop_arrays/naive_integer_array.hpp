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
     * @brief A naive unsigned 64-bit integer vector \p S[0..n-1] stored in std::array<uint64_t, SIZE> \p B[0..SIZE-1]
     * @tparam SIZE The size of the array
     * \ingroup CollectionClasses
     */
    template <uint64_t SIZE = 1024>
    class NaiveIntegerArray
    {
    public:
    protected:
        std::array<uint64_t, SIZE> buffer_; // Buffer B[0..SIZE-1]
        uint64_t size_ = 0;                 // |S|
        uint64_t psum_ = 0;                 // The sum of the elements in integer sequence S[0..n-1]

    public:
        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Constructors and Destructor
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Default constructor
         */
        NaiveIntegerArray()
        {
            this->clear();
        }

        /**
         * @brief Constructor with S = S_
         */
        NaiveIntegerArray(const std::vector<uint64_t> &S_)
        {
            this->clear();

            for (uint64_t i = 0; i < S_.size(); i++)
            {
                this->push_back(S_[i]);
            }
        }

        /**
         * @brief Destructor
         */
        ~NaiveIntegerArray()
        {
            this->clear();
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Operators
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Return S[i]
         */
        uint64_t operator[](uint64_t i) const
        {
            return this->at(i);
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Lightweight functions for accessing to properties of this class
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Returns |B| (i.e., SIZE)
         */
        size_t capacity() const
        {
            return SIZE;
        }

        /**
         * @brief Check if |S| == 0
         */
        bool empty() const
        {
            return this->size_ == 0;
        }

        /**
         * @brief Returns |S|
         */
        uint64_t size() const
        {
            return this->size_;
        }

        /**
         * @brief Returns the total memory usage in bytes
         * @param only_dynamic_memory If true, only the size of the dynamic memory is returned
         */
        uint64_t size_in_bytes(bool only_dynamic_memory = false) const
        {
            if (only_dynamic_memory)
            {
                return 0;
            }
            else
            {
                return sizeof(NaiveIntegerArray<SIZE>);
            }
        }

        /**
         * @brief Returns the size of the unused memory in bytes
         */
        uint64_t unused_size_in_bytes() const
        {
            return (SIZE - this->size()) * sizeof(uint64_t);
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Main queries (Access, search, and psum operations)
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Return S[i]
         */
        uint64_t at(uint64_t i) const
        {
            return this->buffer_[i];
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
         * @brief Returns the sum of the first (i+1) elements in \p S[0..n-1]
         * @note \p O(i) time
         */
        uint64_t psum(uint64_t i) const
        {
            return this->psum(0, i);
        }

        /**
         * @brief Returns the sum of integers in \p S[i..j]
         * @note \p O(j-i) time
         */
        uint64_t psum(uint64_t i, uint64_t j) const
        {
            uint64_t sum = 0;
            uint64_t size = this->size();
            if (i <= j && j < size)
            {
                for (uint64_t x = i; x <= j; x++)
                {
                    sum += this->buffer_[x];
                }
            }
            else
            {
                throw std::out_of_range("psum, Index out of range");
            }
            return sum;
        }


        /**
         * @brief Returns the sum of integers in \p S[(n-1)-i..n-1]
         * @note \p O(n) time
         */
        uint64_t reverse_psum([[maybe_unused]] uint64_t i) const
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
        int64_t search(uint64_t x) const
        {
            uint64_t sum = 0;
            return this->search(x, sum);
        }

        /**
         * @brief Returns the first position \p p such that psum(p) >= x if such a position exists, otherwise returns -1
         * @param sum This variable is changed to the sum of the first \p elements in \p S[0..n-1] by this function
         * @note \p O(p) time
         */
        int64_t search(uint64_t x, uint64_t &sum) const
        {
            assert(this->verify());

            uint64_t size = this->size();
            uint64_t i = 0;

            if (x > this->psum_ || size == 0)
            {
                return -1;
            }
            else
            {

                sum = 0;
                uint64_t v = this->buffer_[i];

                while (sum + v < x)
                {
                    i++;
                    sum += v;
                    v = this->buffer_[i];
                    assert(i < size);
                }

                return i;
            }
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Update Operations
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Set a given value \p v at a given position \p i in \p S
         * @note \p O(1) time
         */

        void set_value(uint64_t index, uint64_t value)
        {
            uint64_t old_value = this->buffer_[index];
            this->psum_ -= old_value;
            this->psum_ += value;
            this->buffer_[index] = value;
            assert(this->verify());
        }

        /**
         * @brief Set the value \p \p S[i+delta] at a given position \p i in \p S
         * @note \p O(1) time
         */
        void increment(uint64_t pos, int64_t delta)
        {
            uint64_t value = this->buffer_[pos];
            this->set_value(pos, value + delta);
        }

        /**
         * @brief Set the value \p \p S[i-delta] at a given position \p i in \p S
         * @note \p O(1) time
         */
        void decrement(uint64_t pos, int64_t delta)
        {
            uint64_t value = this->buffer_[pos];
            this->set_value(pos, value - delta);
        }

        /**
         * @brief Swap operation
         */
        void swap(NaiveIntegerArray &item)
        {
            std::swap(this->buffer_, item.buffer_);
            // std::swap(this->circular_sum_buffer_, item.circular_sum_buffer_);
            std::swap(this->psum_, item.psum_);
            std::swap(this->size_, item.size_);
            assert(this->verify());
        }

        /**
         * @brief Clear all elements from \p S
         */
        void clear()
        {
            this->size_ = 0;
            this->psum_ = 0;
            assert(this->verify());
        }

        /**
         * @brief Add a given integer to the end of \p S
         * @note \p O(1) time
         */
        void push_back(uint64_t value)
        {

            if (this->size_ >= SIZE)
            {
                throw std::out_of_range("push_back, Size out of range");
            }

            this->buffer_[this->size_] = value;
            this->size_++;
            this->psum_ += value;
            // this->update_sum_buffer();
            assert(this->verify());
        }

        /**
         * @brief Add a given sequence \p Q[0..k-1] to the end of \p S[0..n-1] (i.e., \p S = S[0..n-1]Q[0..k-1])
         * @note \p O(|Q|) time
         */
        template <typename ARRAY_TYPE = std::vector<uint64_t>>
        void push_back_many(ARRAY_TYPE new_items_Q)
        {
            uint64_t size = new_items_Q.size();
            for (uint64_t i = 0; i < size; i++)
            {
                this->push_back(new_items_Q[i]);
            }
        }


        /**
         * @brief Add a given value to the beginning of \p S
         * @note \p O(|S|) time
         */
        void push_front(uint64_t value)
        {

            if (this->size_ >= SIZE)
            {
                throw std::out_of_range("push_front, Size out of range");
            }

            uint64_t src = 0;
            uint64_t dst = 1;
            std::memmove(&this->buffer_[dst], &this->buffer_[src], this->size_ * sizeof(uint64_t));
            this->buffer_[src] = value;
            this->psum_ += value;
            this->size_++;
            assert(this->verify());
        }

        /**
         * @brief Add a given sequence \p Q[0..k-1] to the beginning of \p S[0..n-1] (i.e., \p S = Q[0..k-1]S[0..n-1])
         * @note \p O(|S| + |Q|) time
         */
        template <typename ARRAY_TYPE = std::vector<uint64_t>>
        void push_front_many(ARRAY_TYPE new_items_Q)
        {
            int64_t size = new_items_Q.size();
            for (int64_t i = size - 1; i >= 0; i--)
            {
                this->push_front(new_items_Q[i]);
            }
        }

        /**
         * @brief Remove the last element from \p S
         * @note \p O(1) time
         */
        void pop_back()
        {
            if (this->size_ == 0)
            {
                throw std::out_of_range("pop_back, Size out of range");
            }
            uint64_t value = this->buffer_[this->size_ - 1];
            this->size_--;
            this->psum_ -= value;
            assert(this->verify());
        }

        /**
         * @brief Remove the last \p len elements from \p S
         * @note \p O(len) time
         */
        void pop_back_many(uint64_t len)
        {
            for (uint64_t i = 0; i < len; i++)
            {
                this->pop_back();
            }
        }

        /**
         * @brief Remove the first element from \p S
         * @note \p O(|S|) time
         */
        void pop_front()
        {
            if (this->size_ == 0)
            {
                throw std::out_of_range("pop_front, Size out of range");
            }

            if (this->size_ > 1)
            {
                uint64_t value = this->buffer_[0];
                this->psum_ -= value;

                uint64_t src = 1;
                uint64_t dst = 0;
                std::memmove(&this->buffer_[dst], &this->buffer_[src], (this->size_ - 1) * sizeof(uint64_t));
                this->size_--;
            }
            else
            {
                this->clear();
            }
            assert(this->verify());
        }

        /**
         * @brief Remove the first \p len elements from \p S
         * @note \p O(|S|) time
         */
        void pop_front_many(uint64_t len)
        {
            for (uint64_t i = 0; i < len; i++)
            {
                this->pop_front();
            }
        }

        /**
         * @brief Insert a given integer \p value into \p S as the \p (pos+1)-th element
         * @note \p O(|S|) time
         */
        void insert(uint64_t pos, uint64_t value)
        {
            uint64_t size = this->size();

            if (size >= SIZE)
            {
                throw std::out_of_range("insert, Size out of range");
            }

            if (pos > this->size_ + 1)
            {
                throw std::out_of_range("Position out of range");
            }

            if (pos == 0)
            {
                this->push_front(value);
                assert(this->at(pos) == value);
            }
            else if (pos == size)
            {
                this->push_back(value);
                assert(this->at(pos) == value);
            }
            else
            {

                uint64_t dst_pos = pos + 1;
                uint64_t src_pos = pos;
                uint64_t move_size = this->size_ - pos;

                memmove(&this->buffer_[dst_pos], &this->buffer_[src_pos], move_size * sizeof(uint64_t));

                this->buffer_[src_pos] = value;
                this->psum_ += value;
                this->size_++;
                assert(this->at(pos) == value);
            }
            assert(this->verify());
        }

        /**
         * @brief Remove the element at the position \p pos from \p S and return it
         * @note \p O(|S|) time
         */
        void remove(uint64_t pos)
        {
            this->erase(pos);
        }

        /**
         * @brief Remove the element at the position \p pos from \p S and return it
         * @note \p O(|S|) time
         */
        void erase(uint64_t pos)
        {
            if (pos == 0)
            {
                this->pop_front();
            }
            else if (pos == this->size() - 1)
            {
                this->pop_back();
            }
            else
            {

                uint64_t dst_pos = pos;
                uint64_t src_pos = pos + 1;
                uint64_t value = this->buffer_[dst_pos];
                uint64_t move_size = this->size_ - pos;

                memmove(&this->buffer_[dst_pos], &this->buffer_[src_pos], move_size * sizeof(uint64_t));

                this->psum_ -= value;
                this->size_--;
            }
            assert(this->verify());
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
            std::string r = "[";
            for (uint64_t i = 0; i < this->size_; i++)
            {
                r += std::to_string(this->buffer_[i]);
                if (i < this->size_ - 1)
                {
                    r += ", ";
                }
            }
            r += "]";
            return r;
        }

        /*!
         * @brief Returns \p S as a vector
         */
        std::vector<uint64_t> to_vector() const
        {
            std::vector<uint64_t> r;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                r.push_back(this->buffer_[i]);
            }
            return r;
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
            std::cout << "psum = " << this->psum_ << std::endl;
            std::cout << "deque_size = " << this->size_ << std::endl;
            std::cout << "circular_buffer = ";
            for (uint64_t i = 0; i < this->size_; i++)
            {
                std::cout << this->buffer_[i] << " ";
            }
            std::cout << std::endl;
        }

        /**
         * @brief Verifies this instance
         * @note this function is used to debug this instance
         */
        bool verify() const
        {
            uint64_t sum = 0;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                sum += this->buffer_[i];
            }
            if (sum != this->psum_)
            {
                std::cout << "sum: " << sum << " != psum: " << this->psum_ << std::endl;
                throw std::invalid_argument("verify, psum error");
            }

            return true;
        }
        //}@

        ////////////////////////////////////////////////////////////////////////////////
        ///   @name Load, save, and builder functions
        ////////////////////////////////////////////////////////////////////////////////
        //@{

        /**
         * @brief Construct an instance such that \p S = \p S_
         */
        static NaiveIntegerArray build(const std::vector<uint64_t> &S_)
        {
            NaiveIntegerArray deque(S_);
            return deque;
        }
        //}@
    };
}
