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

    struct CircularBitPointer
    {
    public:
        uint16_t circular_buffer_size_;
        uint16_t block_index_;
        uint8_t bit_index_;

        CircularBitPointer(uint16_t circular_buffer_size, uint16_t block_index, uint8_t bit_index)
        {
            this->circular_buffer_size_ = circular_buffer_size;
            this->block_index_ = block_index;
            this->bit_index_ = bit_index;
        }
        uint64_t get_position() const
        {
            return this->block_index_ * 64 + this->bit_index_;
        }

        void add(int64_t x)
        {
            while (x > 0)
            {
                if (this->bit_index_ + x < 64)
                {
                    this->bit_index_ += x;
                    x = 0;
                }
                else
                {
                    uint64_t diff = 64 - this->bit_index_;
                    x -= diff;
                    this->bit_index_ = 0;
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
                if (x <= (int64_t)this->bit_index_)
                {
                    this->bit_index_ -= x;
                    x = 0;
                }
                else
                {
                    uint64_t diff = this->bit_index_ + 1;
                    x -= diff;

                    if (this->block_index_ != 0)
                    {
                        this->bit_index_ = 63;
                        this->block_index_--;
                    }
                    else
                    {
                        this->bit_index_ = 63;
                        this->block_index_ = this->circular_buffer_size_ - 1;
                    }
                }
            }
        }

        template <typename T>
        uint64_t read64(const T &bits) const
        {
            if (this->bit_index_ == 0)
            {
                assert(this->block_index_ < this->circular_buffer_size_);
                return bits[this->block_index_];
            }
            else if (this->block_index_ + 1 < this->circular_buffer_size_)
            {                
                // uint64_t Lsize = 64 - this->bit_index_;
                assert(this->block_index_ + 1 < this->circular_buffer_size_);

                uint64_t L = bits[block_index_] << this->bit_index_;
                uint64_t R = bits[block_index_ + 1] >> (64 - this->bit_index_);
                return L | R;
            }
            else
            {
                assert(this->block_index_  < this->circular_buffer_size_);

                // uint64_t Lsize = 64 - this->bit_index_;
                uint64_t L = bits[block_index_] << this->bit_index_;
                uint64_t R = bits[0] >> (64 - this->bit_index_);
                return L | R;
            }
        }

        template <typename T>
        void write64(T &bits, uint64_t value)
        {
            assert(this->block_index_  < this->circular_buffer_size_);

            if (this->bit_index_ == 0)
            {
                bits[this->block_index_] = value;
            }
            else
            {
                uint64_t Lvalue = value;
                uint64_t Rvalue = value << (64 - this->bit_index_);
                uint64_t npos = this->block_index_ + 1 < this->circular_buffer_size_ ? this->block_index_ + 1 : 0;
                bits[this->block_index_] = stool::MSBByte::write_bits(bits[this->block_index_], this->bit_index_, 64 - this->bit_index_, Lvalue);
                bits[npos] = stool::MSBByte::write_bits(bits[npos], 0, this->bit_index_, Rvalue);
            }
        }

        template <typename T>
        void write_bits(T &bits, uint64_t value, uint64_t len)
        {
            if (this->bit_index_ + len <= 64)
            {
                assert(this->block_index_  < this->circular_buffer_size_);

                bits[this->block_index_] = stool::MSBByte::write_bits(bits[this->block_index_], this->bit_index_, len, value);
            }
            else
            {
                uint64_t Lvalue = value;
                uint64_t Rvalue = value << (64 - this->bit_index_);

                uint64_t diff = 64 - this->bit_index_;
                assert(this->block_index_ < this->circular_buffer_size_);

                bits[this->block_index_] = stool::MSBByte::write_bits(bits[this->block_index_], this->bit_index_, diff, Lvalue);

                if (this->block_index_ + 1 < this->circular_buffer_size_)
                {
                    assert(this->block_index_ + 1 < this->circular_buffer_size_);

                    bits[this->block_index_ + 1] = stool::MSBByte::write_bits(bits[this->block_index_ + 1], 0, len - diff, Rvalue);
                }
                else
                {
                    assert(this->circular_buffer_size_ > 0);
                    bits[0] = stool::MSBByte::write_bits(bits[0], 0, len - diff, Rvalue);
                }
            }
        }
    };
}