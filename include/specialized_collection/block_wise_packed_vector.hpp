#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>
#include "./bwpv_block.hpp"

namespace stool
{

    template <std::size_t BLOCK_SIZE = 128>
    class BlockPackedVector
    {
    public:
        using block_type = BWPVBlock<BLOCK_SIZE>;
        using value_type = typename block_type::value_type;
        using size_type = typename block_type::size_type;

        static_assert(BLOCK_SIZE > 0, "BLOCK_SIZE must be greater than zero");
        static_assert(block_type::item_count == BLOCK_SIZE);

    private:
        size_type size_ = 0;
        std::vector<block_type> blocks_;

        std::pair<size_type, size_type>
        locate(size_type i) const
        {
            if (i >= size_)
            {
                throw std::out_of_range(
                    "BlockPackedVector index out of range");
            }

            return {i / BLOCK_SIZE,i % BLOCK_SIZE};
        }

    public:
        class Reference
        {
        private:
            BlockPackedVector *owner_;
            size_type index_;

        public:
            Reference(
                BlockPackedVector *owner,
                size_type index) noexcept
                : owner_(owner),
                  index_(index)
            {
            }

            Reference &operator=(value_type x)
            {
                owner_->set(index_, x);
                return *this;
            }

            Reference &operator=(const Reference &other)
            {
                return *this =
                           static_cast<value_type>(other);
            }

            operator value_type() const
            {
                return owner_->get(index_);
            }
        };

        explicit BlockPackedVector(
            size_type n = 0,
            value_type initial_value = 0)
            : size_(n)
        {
            if (n == 0)
            {
                return;
            }

            const size_type num_blocks =
                (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

            blocks_.resize(num_blocks);

            std::vector<value_type> tmp(
                BLOCK_SIZE,
                initial_value);

            for (size_type b = 0; b < num_blocks; ++b)
            {
                const size_type begin =
                    b * BLOCK_SIZE;

                const size_type length =
                    std::min(
                        BLOCK_SIZE,
                        n - begin);

                blocks_[b].assign(
                    tmp.data(),
                    length);
            }
        }

        explicit BlockPackedVector(
            const std::vector<value_type> &values)
            : size_(values.size())
        {
            if (values.empty())
            {
                return;
            }

            const size_type num_blocks =
                (size_ + BLOCK_SIZE - 1) / BLOCK_SIZE;

            blocks_.resize(num_blocks);

            for (size_type b = 0; b < num_blocks; ++b)
            {
                const size_type begin =
                    b * BLOCK_SIZE;

                const size_type length =
                    std::min(
                        BLOCK_SIZE,
                        size_ - begin);

                blocks_[b].assign(
                    values.data() + begin,
                    length);
            }
        }

        size_type size() const noexcept
        {
            return size_;
        }

        bool empty() const noexcept
        {
            return size_ == 0;
        }

        static constexpr size_type block_size() noexcept
        {
            return BLOCK_SIZE;
        }

        size_type num_blocks() const noexcept
        {
            return blocks_.size();
        }

        value_type get(size_type i) const
        {
            const auto location = locate(i);

            return blocks_[location.first]
                .get(location.second);
        }

        void set(size_type i, value_type x)
        {
            const auto location = locate(i);

            blocks_[location.first]
                .set(location.second, x);
        }

        value_type operator[](size_type i) const
        {
            return get(i);
        }

        Reference operator[](size_type i)
        {
            if (i >= size_)
            {
                throw std::out_of_range(
                    "BlockPackedVector index out of range");
            }

            return Reference(this, i);
        }

        /**
         * @brief Appends a value to the end of the vector.
         *
         * @param value Value to append.
         */
        void push_back(value_type value)
        {
            const size_type offset = size_ % BLOCK_SIZE;
            if (offset == 0)
            {
                block_type new_block;
                new_block.assign(&value, 1);
                blocks_.push_back(std::move(new_block));
            }
            else
            {
                blocks_.back().set(offset, value);
            }
            ++size_;
        }

        /**
         * @brief Removes the last value from the vector.
         *
         * @throws std::out_of_range If the vector is empty.
         */
        void pop_back()
        {
            if (empty())
            {
                throw std::out_of_range(
                    "BlockPackedVector::pop_back on empty vector");
            }

            const size_type last_index = size_ - 1;
            const size_type offset = last_index % BLOCK_SIZE;
            if (offset == 0)
            {
                blocks_.pop_back();
            }
            else
            {
                blocks_.back().set(offset, 0);
            }
            --size_;
        }

        unsigned block_width(size_type block) const
        {
            if (block >= blocks_.size())
            {
                throw std::out_of_range(
                    "Block index out of range");
            }

            return blocks_[block].item_bit_width;
        }

        size_type packed_bytes() const noexcept
        {
            size_type total = 0;

            for (const auto &block : blocks_)
            {
                total +=
                    block.words.size() * sizeof(std::uint64_t);
            }

            return total;
        }

        size_type size_in_bytes() const noexcept
        {
            size_type total =
                sizeof(BlockPackedVector) + blocks_.capacity() * sizeof(block_type);

            for (const auto &block : blocks_)
            {
                total +=
                    block.words.capacity() * sizeof(std::uint64_t);
            }

            return total;
        }

        double average_block_capacity() const noexcept{
            size_type total = 0;
            for (const auto &block : blocks_)
            {
                total +=
                    block.words.capacity();
            }
            return static_cast<double>(total) / blocks_.size();
        }
    };

}
