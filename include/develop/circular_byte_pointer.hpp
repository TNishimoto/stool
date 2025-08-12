#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include <cassert>
#include "../basic/byte.hpp"
#include "../basic/lsb_byte.hpp"
#include "../basic/msb_byte.hpp"

#include "../debug/print.hpp"

namespace stool
{

    struct CircularBytePointer
    {
    public:
        uint16_t circular_buffer_size_;
        uint16_t block_index_;
        uint8_t byte_index_;

        CircularBytePointer(uint16_t circular_buffer_size, uint16_t block_index, uint8_t byte_index)
        {
            this->circular_buffer_size_ = circular_buffer_size;
            this->block_index_ = block_index;
            this->byte_index_ = byte_index;
        }
        uint64_t get_position() const
        {
            return this->block_index_ * 8 + this->byte_index_;
        }

        void add(int64_t x)
        {
            while (x > 0)
            {
                if (this->byte_index_ + x < 8)
                {
                    this->byte_index_ += x;
                    x = 0;
                }
                else
                {
                    uint64_t diff = 8 - this->byte_index_;
                    x -= diff;
                    this->byte_index_ = 0;
                    this->block_index_++;
                    if (this->block_index_ == this->circular_buffer_size_)
                    {
                        this->block_index_ = 0;
                    }
                }
            }
            assert(x == 0);
        }
        void subtract(int64_t x)
        {
            while (x > 0)
            {
                if (x <= (int64_t)this->byte_index_)
                {
                    this->byte_index_ -= x;
                    x = 0;
                }
                else
                {
                    uint64_t diff = this->byte_index_ + 1;
                    x -= diff;

                    if (this->block_index_ != 0)
                    {
                        this->byte_index_ = 7;
                        this->block_index_--;
                    }
                    else
                    {
                        this->byte_index_ = 7;
                        this->block_index_ = this->circular_buffer_size_ - 1;
                    }
                }
            }
        }

        template <typename T>
        uint64_t read64(const T &bits) const
        {
            return bits[this->block_index_] << (this->byte_index_ * 8);
        }

        template <typename T>
        void write64(T &bits, uint64_t value)
        {
            assert(this->block_index_  < this->circular_buffer_size_);

            uint64_t mask = UINT64_MAX << (8 * this->byte_index_);

            bits[this->block_index_] = (bits[this->block_index_] & mask) | (value >> (8 * this->byte_index_));
        }
    };
}