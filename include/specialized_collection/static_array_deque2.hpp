#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include <cassert>
#include "../basic/byte.hpp"
#include "../debug/print.hpp"
#include "../basic/simd.hpp"
#include "static_array_deque.hpp"

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
    template <uint64_t SIZE>
    class StaticArrayDeque<SIZE, true> : public StaticArrayDeque<SIZE, false>
    {
    public:
        using StaticArrayDeque<SIZE, false>::StaticArrayDeque;
        static constexpr uint64_t BUFFER_SIZE = SIZE * 8;
        using ByteType = typename StaticArrayDeque<SIZE, false>::ByteType;
        using BASECLASS = StaticArrayDeque<SIZE, false>;
        using ElementIndex = typename BASECLASS::ElementIndex;

        std::array<uint8_t, BUFFER_SIZE> circular_sum_buffer_;
        ElementIndex sum_starting_index = 0;

        StaticArrayDeque(const std::vector<uint64_t> &items)
        {
            if constexpr (!BASECLASS::is_power_of_two)
            {
                throw std::invalid_argument("SIZE must be a power of two");
            }
            this->clear();
            for (uint64_t i = 0; i < items.size(); i++)
            {
                this->push_back(items[i]);
            }
        }

        void update_sum_buffer()
        {
            uint64_t sumR = 0;
            uint64_t sumL = 0;

            ByteType type = (ByteType)this->value_byte_type_;
            uint64_t half_index = this->deque_size_ / 2;
            switch (type)
            {
            case ByteType::U8:
            {
                for (uint64_t i = half_index; i < this->deque_size_; i++)
                {
                    uint64_t pos = BASECLASS::translate_index8(this->starting_position_, i);
                    sumR += ((uint8_t *)&this->circular_buffer_)[pos];
                    ((uint8_t *)&this->circular_sum_buffer_)[pos] = sumR;
                }
                for (int64_t i = half_index - 1; i >= 0; i--)
                {
                    uint64_t pos = BASECLASS::translate_index8(this->starting_position_, i);
                    sumL += ((uint8_t *)&this->circular_buffer_)[pos];
                    ((uint8_t *)&this->circular_sum_buffer_)[pos] = sumL;
                }
                break;
            }
            case ByteType::U16:
            {
                for (uint64_t i = half_index; i < this->deque_size_; i++)
                {
                    uint64_t pos = BASECLASS::translate_index16(this->starting_position_, i);
                    sumR += ((uint16_t *)&this->circular_buffer_)[pos];
                    ((uint16_t *)&this->circular_sum_buffer_)[pos] = sumR;
                }
                for (int64_t i = half_index - 1; i >= 0; i--)
                {
                    uint64_t pos = BASECLASS::translate_index16(this->starting_position_, i);
                    sumL += ((uint16_t *)&this->circular_buffer_)[pos];
                    ((uint16_t *)&this->circular_sum_buffer_)[pos] = sumL;
                }
                break;
            }
            case ByteType::U32:
            {
                for (uint64_t i = half_index; i < this->deque_size_; i++)
                {
                    uint64_t pos = BASECLASS::translate_index32(this->starting_position_, i);
                    sumR += ((uint32_t *)&this->circular_buffer_)[pos];
                    ((uint32_t *)&this->circular_sum_buffer_)[pos] = sumR;
                }
                for (int64_t i = half_index - 1; i >= 0; i--)
                {
                    uint64_t pos = BASECLASS::translate_index32(this->starting_position_, i);
                    sumL += ((uint32_t *)&this->circular_buffer_)[pos];
                    ((uint32_t *)&this->circular_sum_buffer_)[pos] = sumL;
                }
                break;
            }
            case ByteType::U64:
            {
                for (uint64_t i = half_index; i < this->deque_size_; i++)
                {
                    uint64_t pos = BASECLASS::translate_index64(this->starting_position_, i);
                    sumR += ((uint64_t *)&this->circular_buffer_)[pos];
                    ((uint64_t *)&this->circular_sum_buffer_)[pos] = sumR;
                }
                for (int64_t i = half_index - 1; i >= 0; i--)
                {
                    uint64_t pos = BASECLASS::translate_index64(this->starting_position_, i);
                    sumL += ((uint64_t *)&this->circular_buffer_)[pos];
                    ((uint64_t *)&this->circular_sum_buffer_)[pos] = sumL;
                }
                break;
            }
            default:
                break;
            }
            this->sum_starting_index = half_index;
        }

        void clear()
        {
            this->deque_size_ = 0;
            this->starting_position_ = 0;
            this->value_byte_type_ = 1;
            this->sum_starting_index = 0;
        }

        void relocate_buffer(uint8_t new_byte_type, bool update_sum_buffer = true)
        {
            bool b = false;

            if (this->starting_position_ != 0)
            {
                BASECLASS::reset_starting_position_of_array_deque(this->circular_buffer_, this->starting_position_, this->deque_size_, (ByteType)this->value_byte_type_);
                b = true;
                this->starting_position_ = 0;
            }

            if (this->value_byte_type_ != new_byte_type)
            {
                BASECLASS::change_byte_type_of_array_elements(this->circular_buffer_, this->deque_size_, (ByteType)this->value_byte_type_, (ByteType)new_byte_type);
                b = true;
                this->value_byte_type_ = new_byte_type;
            }

            if (b && update_sum_buffer)
            {

                this->update_sum_buffer();
            }
        }
        uint64_t psum() const
        {
            uint64_t size = this->deque_size_;
            if (size == 0)
            {
                return 0;
            }
            return this->psum(size - 1);
        }
        uint64_t psum(ElementIndex i) const
        {
            ByteType v = (ByteType)this->value_byte_type_;
            uint64_t sum = 0;

            switch (v)
            {
            case ByteType::U8:
            {
                uint64_t pos0 = BASECLASS::translate_index8(this->starting_position_, 0);
                uint64_t psumL = this->sum_starting_index == 0 ? 0 : ((uint8_t *)&this->circular_sum_buffer_)[pos0];

                if (i < this->sum_starting_index)
                {
                    uint64_t u = 0;
                    if (i + 1 < this->sum_starting_index)
                    {
                        uint64_t posU = BASECLASS::translate_index8(this->starting_position_, i + 1);
                        u = ((uint8_t *)&this->circular_sum_buffer_)[posU];
                    }
                    sum = psumL - u;
                }
                else
                {
                    uint64_t pos = BASECLASS::translate_index8(this->starting_position_, i);
                    uint64_t psumR = ((uint8_t *)&this->circular_sum_buffer_)[pos];
                    sum = psumL + psumR;
                }
                break;
            }
            case ByteType::U16:
            {
                uint64_t pos0 = BASECLASS::translate_index16(this->starting_position_, 0);
                uint64_t psumL = this->sum_starting_index == 0 ? 0 : ((uint16_t *)&this->circular_sum_buffer_)[pos0];

                if (i < this->sum_starting_index)
                {
                    uint64_t u = 0;
                    if (i + 1 < this->sum_starting_index)
                    {
                        uint64_t posU = BASECLASS::translate_index16(this->starting_position_, i + 1);
                        u = ((uint16_t *)&this->circular_sum_buffer_)[posU];
                    }
                    sum = psumL - u;
                }
                else
                {
                    uint64_t pos = BASECLASS::translate_index16(this->starting_position_, i);
                    uint64_t psumR = ((uint16_t *)&this->circular_sum_buffer_)[pos];
                    sum = psumL + psumR;
                }
                break;
            }
            case ByteType::U32:
            {
                uint64_t pos0 = BASECLASS::translate_index32(this->starting_position_, 0);
                uint64_t psumL = this->sum_starting_index == 0 ? 0 : ((uint32_t *)&this->circular_sum_buffer_)[pos0];

                if (i < this->sum_starting_index)
                {
                    uint64_t u = 0;
                    if (i + 1 < this->sum_starting_index)
                    {
                        uint64_t posU = BASECLASS::translate_index32(this->starting_position_, i + 1);
                        u = ((uint32_t *)&this->circular_sum_buffer_)[posU];
                    }
                    sum = psumL - u;
                }
                else
                {
                    uint64_t pos = BASECLASS::translate_index32(this->starting_position_, i);
                    uint64_t psumR = ((uint32_t *)&this->circular_sum_buffer_)[pos];
                    sum = psumL + psumR;
                }
                break;
            }
            case ByteType::U64:
            {
                uint64_t pos0 = BASECLASS::translate_index64(this->starting_position_, 0);
                uint64_t psumL = this->sum_starting_index == 0 ? 0 : ((uint64_t *)&this->circular_sum_buffer_)[pos0];

                if (i < this->sum_starting_index)
                {
                    uint64_t u = 0;
                    if (i + 1 < this->sum_starting_index)
                    {
                        uint64_t posU = BASECLASS::translate_index64(this->starting_position_, i + 1);
                        u = ((uint64_t *)&this->circular_sum_buffer_)[posU];
                    }
                    sum = psumL - u;
                }
                else
                {
                    uint64_t pos = BASECLASS::translate_index64(this->starting_position_, i);
                    uint64_t psumR = ((uint64_t *)&this->circular_sum_buffer_)[pos];
                    sum = psumL + psumR;
                }
                break;
            }
            default:
                break;
            }
            return sum;
        }

        uint64_t value_capacity() const
        {
            return UINT64_MAX - this->psum();
        }

        /**
         * @brief Set a value at a specific index
         *
         * @param index The index where to set the value
         * @param value The value to set
         */
        void set_value(ElementIndex index, uint64_t value)
        {
            uint64_t at_value = this->at(index);
            if (at_value == value)
            {
                return;
            }
            if (value > INT64_MAX)
            {
                throw std::out_of_range("static_array_deque::set_value, Value out of range(1)");
            }
            int64_t diff = value - at_value;

            uint64_t current_sum = this->psum();
            uint64_t value_capacity = UINT64_MAX - current_sum;
            if (diff > 0 && ((uint64_t)diff) > value_capacity)
            {
                throw std::out_of_range("static_array_deque::set_value, Value out of range(2)");
            }

            uint64_t psum = this->psum();
            uint64_t new_byte_type = std::max((uint8_t)BASECLASS::get_byte_type(psum + diff), this->value_byte_type_);
            if (new_byte_type > this->value_byte_type_)
            {
                this->relocate_buffer(new_byte_type);
            }

            ByteType type = (ByteType)this->value_byte_type_;
            BASECLASS::write_value(this->circular_buffer_, this->starting_position_, index, value, type);

            if (index < this->sum_starting_index)
            {
                switch (type)
                {
                case ByteType::U8:
                {
                    for (uint64_t i = 0; i <= index; i++)
                    {
                        uint64_t pos = BASECLASS::translate_index8(this->starting_position_, i);
                        ((uint8_t *)&this->circular_sum_buffer_)[pos] += diff;
                    }
                    break;
                }
                case ByteType::U16:
                {
                    for (uint64_t i = 0; i <= index; i++)
                    {
                        uint64_t pos = BASECLASS::translate_index16(this->starting_position_, i);
                        ((uint16_t *)&this->circular_sum_buffer_)[pos] += diff;
                    }
                    break;
                }
                case ByteType::U32:
                {
                    for (uint64_t i = 0; i <= index; i++)
                    {
                        uint64_t pos = BASECLASS::translate_index32(this->starting_position_, i);
                        ((uint32_t *)&this->circular_sum_buffer_)[pos] += diff;
                    }
                    break;
                }
                case ByteType::U64:
                {
                    for (uint64_t i = 0; i <= index; i++)
                    {
                        uint64_t pos = BASECLASS::translate_index64(this->starting_position_, i);
                        ((uint64_t *)&this->circular_sum_buffer_)[pos] += diff;
                    }
                    break;
                }
                default:
                    break;
                }
            }
            else
            {
                switch (type)
                {
                case ByteType::U8:
                {
                    for (uint64_t i = index; i < this->deque_size_; i++)
                    {
                        uint64_t pos = BASECLASS::translate_index8(this->starting_position_, i);
                        ((uint8_t *)&this->circular_sum_buffer_)[pos] += diff;
                    }
                    break;
                }
                case ByteType::U16:
                {
                    for (uint64_t i = index; i < this->deque_size_; i++)
                    {
                        uint64_t pos = BASECLASS::translate_index16(this->starting_position_, i);
                        ((uint16_t *)&this->circular_sum_buffer_)[pos] += diff;
                    }
                    break;
                }
                case ByteType::U32:
                {
                    for (uint64_t i = index; i < this->deque_size_; i++)
                    {
                        uint64_t pos = BASECLASS::translate_index32(this->starting_position_, i);
                        ((uint32_t *)&this->circular_sum_buffer_)[pos] += diff;
                    }
                    break;
                }
                case ByteType::U64:
                {
                    for (uint64_t i = index; i < this->deque_size_; i++)
                    {
                        uint64_t pos = BASECLASS::translate_index64(this->starting_position_, i);
                        ((uint64_t *)&this->circular_sum_buffer_)[pos] += diff;
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }

        /**
         * @brief Add an element to the back of the deque
         *
         * @param value The value to add
         */
        void push_back(uint64_t value)
        {
            uint64_t current_sum = this->psum();
            uint64_t value_capacity = UINT64_MAX - current_sum;
            if (value > value_capacity)
            {
                throw std::out_of_range("static_array_deque::push_back, Value out of range");
            }
            if (value > INT64_MAX)
            {
                throw std::out_of_range("static_array_deque::push_back, Value out of range(VALUE)");
            }

            assert(this->sum_starting_index < this->deque_size_ || this->deque_size_ == 0);

            if constexpr (!BASECLASS::is_power_of_two)
            {
                throw std::invalid_argument("SIZE must be a power of two");
            }

            if (this->deque_size_ == SIZE)
            {
                throw std::out_of_range("static_array_deque::push_back, Size out of range");
            }

            uint64_t psum = this->psum();
            uint64_t new_byte_type = std::max((uint8_t)BASECLASS::get_byte_type(psum + value), this->value_byte_type_);

            if (new_byte_type > this->value_byte_type_)
            {
                this->relocate_buffer(new_byte_type);
            }

            uint64_t pos = this->size();
            ByteType type = (ByteType)this->value_byte_type_;
            uint64_t psumR = this->deque_size_ == 0 ? 0 : BASECLASS::read_value(this->circular_sum_buffer_, this->starting_position_, pos - 1, type);

            this->deque_size_++;

            BASECLASS::write_value(this->circular_buffer_, this->starting_position_, pos, value, type);
            BASECLASS::write_value(this->circular_sum_buffer_, this->starting_position_, pos, psumR + value, type);
            assert(this->sum_starting_index < this->deque_size_ || this->deque_size_ == 0);
        }

        /**
         * @brief Remove the last element from the deque
         */
        void pop_back()
        {
            if (this->deque_size_ == 0)
            {
                throw std::out_of_range("static_array_deque::pop_back, Size out of range");
            }
            this->deque_size_--;
            if (this->deque_size_ == 0)
            {
                this->clear();
            }
            else if (this->sum_starting_index == this->deque_size_)
            {
                this->update_sum_buffer();
            }
            assert(this->sum_starting_index < this->deque_size_ || this->deque_size_ == 0);
        }
        /**
         * @brief Add an element to the front of the deque
         *
         * @param value The value to add
         */
        void push_front(uint64_t value)
        {
            uint64_t current_sum = this->psum();
            uint64_t value_capacity = UINT64_MAX - current_sum;
            if (value > value_capacity)
            {
                throw std::out_of_range("static_array_deque::push_back, Value out of range");
            }

            if constexpr (!BASECLASS::is_power_of_two)
            {
                throw std::invalid_argument("SIZE must be a power of two");
            }

            if (this->deque_size_ == SIZE)
            {
                throw std::out_of_range("static_array_deque::push_front, Size out of range");
            }

            uint64_t psum = this->psum();
            uint64_t new_byte_type = std::max((uint8_t)BASECLASS::get_byte_type(psum + value), this->value_byte_type_);
            if (new_byte_type > this->value_byte_type_)
            {
                this->relocate_buffer(new_byte_type);
            }

            uint64_t value_byte_size = BASECLASS::get_byte_size2(this->value_byte_type_);
            ByteType type = (ByteType)this->value_byte_type_;
            uint64_t psumL = this->sum_starting_index == 0 ? 0 : BASECLASS::read_value(this->circular_sum_buffer_, this->starting_position_, 0, type);

            bool empty_flag = this->deque_size_ == 0;

            if (this->starting_position_ >= value_byte_size)
            {
                this->starting_position_ -= value_byte_size;
                this->deque_size_++;
                BASECLASS::write_value(this->circular_buffer_, this->starting_position_, 0, value, type);
            }
            else if (this->starting_position_ == 0)
            {
                this->starting_position_ = BUFFER_SIZE - value_byte_size;
                this->deque_size_++;
                BASECLASS::write_value(this->circular_buffer_, this->starting_position_, 0, value, type);
            }
            else
            {
                throw std::invalid_argument("push_front");
            }

            if (empty_flag)
            {
                this->sum_starting_index = 0;
                BASECLASS::write_value(this->circular_sum_buffer_, this->starting_position_, 0, value, type);
            }
            else
            {
                this->sum_starting_index++;
                BASECLASS::write_value(this->circular_sum_buffer_, this->starting_position_, 0, psumL + value, type);
            }

            assert(this->sum_starting_index < this->deque_size_ || this->deque_size_ == 0);
        }

        /**
         * @brief Remove the first element from the deque
         */
        void pop_front()
        {
            if constexpr (!BASECLASS::is_power_of_two)
            {
                throw std::invalid_argument("SIZE must be a power of two");
            }
            if (this->deque_size_ == 0)
            {
                throw std::out_of_range("static_array_deque::pop_front, Size out of range");
            }

            uint64_t value_byte_size = BASECLASS::get_byte_size2(this->value_byte_type_);

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

            if (this->deque_size_ == 0)
            {
                this->clear();
            }
            else if (this->sum_starting_index == 0)
            {
                this->update_sum_buffer();
            }
            else
            {
                this->sum_starting_index--;
            }
            assert(this->sum_starting_index < this->deque_size_ || this->deque_size_ == 0);
        }

        void increment(uint64_t pos, int64_t delta)
        {
            this->set_value(pos, this->at(pos) + delta);
        }

        void decrement(uint64_t pos, int64_t delta)
        {
            this->set_value(pos, this->at(pos) - delta);
        }
        /**
         * @brief Insert an element at a specific iterator position
         *
         * @param position Iterator pointing to the insertion position
         * @param value The value to insert
         */
        void insert(const typename BASECLASS::StaticArrayDequeIterator &position, const uint64_t &value)
        {
            uint64_t pos = position._m_idx;
            this->insert(pos, value);
        }

        void insert(ElementIndex position, uint64_t value)
        {
            uint64_t current_sum = this->psum();
            uint64_t value_capacity = UINT64_MAX - current_sum;
            if (value > value_capacity)
            {
                throw std::out_of_range("static_array_deque::insert, Value out of range");
            }

            uint64_t size = this->size();

            if (size >= SIZE)
            {
                throw std::out_of_range("static_array_deque::insert,Size out of range");
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

                uint64_t psum = this->psum();
                uint64_t new_byte_type = std::max((uint8_t)BASECLASS::get_byte_type(psum + value), this->value_byte_type_);
                if (new_byte_type > this->value_byte_type_)
                {
                    this->relocate_buffer(new_byte_type, false);
                }
                else
                {
                    this->relocate_buffer(this->value_byte_type_, false);
                }

                uint64_t value_byte_size = BASECLASS::get_byte_size2(this->value_byte_type_);

                this->deque_size_++;
                uint64_t deque_byte_size = this->deque_size_ << (this->value_byte_type_ - 1);
                uint64_t src_pos = position << (this->value_byte_type_ - 1);
                uint64_t dst_pos = src_pos + value_byte_size;
                uint64_t move_size = deque_byte_size - dst_pos;

                memmove(&this->circular_buffer_[dst_pos], &this->circular_buffer_[src_pos], move_size);

                BASECLASS::write_value(this->circular_buffer_, this->starting_position_, position, value, (ByteType)this->value_byte_type_);
                this->update_sum_buffer();
            }

            assert(this->at(position) == value);
        }

        /**
         * @brief Erase an element at a specific iterator position
         *
         * @param position Iterator pointing to the element to erase
         */
        void erase(const typename BASECLASS::StaticArrayDequeIterator &position)
        {
            uint64_t pos = position._m_idx;
            this->erase(pos);
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

                uint64_t psum = this->psum();
                uint64_t value = this->at(position);
                uint64_t new_byte_type = std::max((uint8_t)BASECLASS::get_byte_type(psum - value), this->value_byte_type_);
                if (new_byte_type > this->value_byte_type_)
                {
                    this->relocate_buffer(new_byte_type, false);
                }
                else
                {
                    this->relocate_buffer(this->value_byte_type_, false);
                }

                uint64_t value_byte_size = BASECLASS::get_byte_size2(this->value_byte_type_);
                uint64_t deque_byte_size = this->deque_size_ << (this->value_byte_type_ - 1);
                uint64_t dst_pos = position << (this->value_byte_type_ - 1);
                uint64_t src_pos = dst_pos + value_byte_size;
                uint64_t move_size = deque_byte_size - src_pos;

                memmove(&this->circular_buffer_[dst_pos], &this->circular_buffer_[src_pos], move_size);
                this->deque_size_--;

                this->update_sum_buffer();
            }
        }

        template <typename F>
        static int64_t find_lower_bound(int64_t l, int64_t r, F f, uint64_t p)
        {
            while (l < r)
            {
                int64_t m = (l + r) / 2;
                if (f(m) >= p)
                {
                    r = m;
                }
                else
                {
                    l = m + 1;
                }
            }
            return (f(l) >= p) ? l : -1;
        }


        int64_t search(uint64_t value, uint64_t &sum) const
        {
            uint64_t psum = this->psum();
            if (this->deque_size_ == 0)
            {
                return -1;
            }
            else if (value == 0)
            {
                return 0;
            }
            else if (value > psum)
            {
                sum = psum;
                return -1;
            }
            else
            {
                uint64_t psumL = this->sum_starting_index == 0 ? 0 : BASECLASS::read_value(this->circular_sum_buffer_, this->starting_position_, 0, (ByteType)this->value_byte_type_);
                if (psumL >= value)
                {
                    auto psumLFunc = [&](int64_t i)
                    {
                        uint64_t value1 = BASECLASS::read_value(this->circular_sum_buffer_, this->starting_position_, i, (ByteType)this->value_byte_type_);
                        uint64_t value2 = BASECLASS::read_value(this->circular_buffer_, this->starting_position_, i, (ByteType)this->value_byte_type_);
                        uint64_t psum = psumL - value1 + value2;
                        return psum;
                    };

                    int64_t result = find_lower_bound(0, this->sum_starting_index, psumLFunc, value);
                    assert(result != -1);
                    if (result != 0)
                    {
                        sum = psumLFunc(result - 1);
                    }
                    return result;
                }
                else
                {
                    auto psumRFunc = [&](int64_t i)
                    {
                        uint64_t value1 = BASECLASS::read_value(this->circular_sum_buffer_, this->starting_position_, i, (ByteType)this->value_byte_type_);
                        uint64_t psum = psumL + value1;
                        return psum;
                    };
                    int64_t result = find_lower_bound(this->sum_starting_index, this->deque_size_, psumRFunc, value);
                    assert(result != -1);
                    if (result > (int64_t)this->sum_starting_index)
                    {
                        sum = psumRFunc(result - 1);
                    }
                    else
                    {
                        sum = psumL;
                    }
                    return result;
                }
            }
        }

        int64_t search(uint64_t value) const
        {
            uint64_t sum = 0;
            return this->search(value, sum);
        }

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

            std::deque<uint64_t> sum_buffer_values;
            for (uint64_t i = 0; i < this->deque_size_; i++)
            {
                uint64_t value = BASECLASS::read_value(this->circular_sum_buffer_, this->starting_position_, i, (ByteType)this->value_byte_type_);
                sum_buffer_values.push_back(value);
            }

            std::deque<uint64_t> deque_values = this->to_deque();
            stool::DebugPrinter::print_integers(deque_values, "Deque");
            // std::cout << "Buffer: " << buffer_str << std::endl;
            std::cout << "Buffer size: " << (int64_t)SIZE << std::endl;
            std::cout << "Starting position: " << (int64_t)this->starting_position_ << std::endl;
            std::cout << "Deque size: " << (int64_t)this->deque_size_ << std::endl;
            std::cout << "Value byte type: " << (int64_t)this->value_byte_type_ << std::endl;
            std::cout << "Sum starting index: " << (int64_t)this->sum_starting_index << std::endl;
            stool::DebugPrinter::print_integers(sum_buffer_values, "SUM");
            std::cout << "==============================" << std::endl;
        }

        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if(only_extra_bytes){
                return 0;
            }else{
                return sizeof(StaticArrayDeque<SIZE, true>);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            uint64_t value_byte_size = get_byte_size2(this->value_byte_type_);
            uint64_t size = this->size();
            uint64_t used_size = size * value_byte_size;
            return (BUFFER_SIZE - used_size) * 2;
        }
    };
}