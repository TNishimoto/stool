#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include "../byte.hpp"
#include "../print.hpp"

namespace stool
{
    template <typename INDEX_TYPE = uint16_t>
    class IntegerDeque
    {
        using T = uint64_t;
        using BUFFER_POS = INDEX_TYPE;
        using DEQUE_POS = INDEX_TYPE;
        uint64_t *circular_buffer_ = nullptr;
        INDEX_TYPE circular_buffer_size_;
        INDEX_TYPE starting_position_;
        INDEX_TYPE deque_size_;
        uint8_t value_byte_size_;

    public:
        static uint64_t max_deque_size()
        {
            uint64_t b = stool::Byte::get_code_length(std::numeric_limits<INDEX_TYPE>::max());
            return (1 << (b - 1)) - 1;
        }
        static uint64_t access_value(uint64_t code, uint8_t pos, uint8_t byte_size)
        {
            uint64_t left_size = (8 * byte_size) * pos;
            uint64_t right_size = 64 - (left_size + (8 * byte_size));
            return (code << left_size) >> (left_size + right_size);
        }
        static uint64_t set_code(uint64_t code, uint8_t pos, uint8_t byte_size, uint64_t value)
        {
            uint64_t left_size = (8 * byte_size) * pos;
            uint64_t right_size = 64 - (left_size + (8 * byte_size));

            uint64_t left_code = code >> (right_size + (8 * byte_size));
            uint64_t right_code = (code << (left_size + (8 * byte_size))) >> (left_size + (8 * byte_size));

            return (left_code << (right_size + (8 * byte_size))) | (value << right_size) | right_code;
        }

        uint64_t size_in_bytes() const
        {
            return sizeof(this->circular_buffer_size_) + sizeof(this->starting_position_) + sizeof(this->deque_size_) + sizeof(this->circular_buffer_) + (sizeof(T) * this->circular_buffer_size_);
        }

        // Move constructor
        IntegerDeque(IntegerDeque &&other) noexcept
            : circular_buffer_(other.circular_buffer_),
              circular_buffer_size_(other.circular_buffer_size_),
              starting_position_(other.starting_position_),
              deque_size_(other.deque_size_),
              value_byte_size_(other.value_byte_size_)
        {
            // Reset the source object
            other.circular_buffer_ = nullptr;
            other.circular_buffer_size_ = 0;
            other.starting_position_ = 0;
            other.deque_size_ = 0;
            other.value_byte_size_ = 0;
        }
        uint64_t get_starting_position_1() const
        {
            return this->starting_position_ / (8 / this->value_byte_size_);
        }
        uint64_t get_starting_position_2() const
        {
            return this->starting_position_ % (8 / this->value_byte_size_);
        }

        size_t capacity() const
        {
            return this->circular_buffer_size_;
        }
        void clear()
        {
            IntegerDeque tmp;
            this->swap(tmp);
        }

        class IntegerDequeIterator
        {

        public:
            IntegerDeque *_m_deq;
            INDEX_TYPE _m_idx;

            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T *;
            using reference = T &;
            IntegerDequeIterator(IntegerDeque *_deque, INDEX_TYPE _idx) : _m_deq(_deque), _m_idx(_idx) {}

            T operator*() const
            {
                return (*_m_deq)[this->_m_idx];
            }

            /*
            uint64_t get_deque_position() const
            {
                return this->_m_deq->get_deque_position(this->_m_idx);
            }
            */

            IntegerDequeIterator &operator++()
            {
                ++this->_m_idx;
                return *this;
            }

            IntegerDequeIterator operator++(int)
            {
                IntegerDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            IntegerDequeIterator &operator--()
            {
                --this->_m_idx;
                return *this;
            }

            IntegerDequeIterator operator--(int)
            {
                IntegerDequeIterator temp = *this;
                --(*this);
                return temp;
            }
            // ランダムアクセス演算子（i + n）
            IntegerDequeIterator operator+(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return IntegerDequeIterator(this->_m_deq, sum);
            }
            IntegerDequeIterator &operator+=(difference_type n)
            {
                this->_m_idx += n;
                return *this;
            }
            // ランダムアクセス演算子（i - n）
            IntegerDequeIterator operator-(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx - (int16_t)n;
                return IntegerDequeIterator(this->_m_deq, sum);
            }
            IntegerDequeIterator &operator-=(difference_type n)
            {
                this->_m_idx -= n;
                return *this;
            }

            // イテレータ同士の差（i - j）
            difference_type operator-(const IntegerDequeIterator &other) const
            {
                return (int16_t)this->_m_idx - (int16_t)other._m_idx;
            }

            // 非const版：書き込みと読み込み可能
            T &operator[](difference_type n)
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }

            // 要素アクセス演算子（i[n]）
            const T &operator[](difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }
            bool operator==(const IntegerDequeIterator &other) const { return _m_idx == other._m_idx; }
            bool operator!=(const IntegerDequeIterator &other) const { return _m_idx != other._m_idx; }
            bool operator<(const IntegerDequeIterator &other) const { return this->_m_idx < other._m_idx; }
            bool operator>(const IntegerDequeIterator &other) const { return this->_m_idx > other._m_idx; }
            bool operator<=(const IntegerDequeIterator &other) const { return this->_m_idx <= other._m_idx; }
            bool operator>=(const IntegerDequeIterator &other) const { return this->_m_idx >= other._m_idx; }
        };
        IntegerDeque()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
            this->circular_buffer_ = new T[2];
            this->circular_buffer_[0] = 0;
            this->circular_buffer_[1] = 0;

            this->starting_position_ = 0;
            this->circular_buffer_size_ = 2;
            this->deque_size_ = 0;
            this->value_byte_size_ = 1;
        }
        ~IntegerDeque()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
        }
        IntegerDequeIterator begin() const
        {
            return IntegerDequeIterator(const_cast<IntegerDeque<INDEX_TYPE> *>(this), 0);
        }
        IntegerDequeIterator end() const
        {
            return IntegerDequeIterator(const_cast<IntegerDeque<INDEX_TYPE> *>(this), this->deque_size_);
        }
        /*
        bool check_size() const
        {
            bool b1 = _deque_size + 1 >= this->circular_buffer_size_;
            bool b2 = _deque_size * 3 < this->circular_buffer_size_;
            returb b1 || b2;
        }
        */

        uint64_t get_buffer_bit() const
        {
            return stool::Byte::get_code_length(this->circular_buffer_size_ * (8 / this->value_byte_size_));
        }
        void update_size_if_needed()
        {
            uint64_t deque_bit = stool::Byte::get_code_length(this->deque_size_ + 1);
            uint64_t buffer_bit = this->get_buffer_bit();

            // uint64_t max = 1 << deque_bit;
            assert(deque_bit <= buffer_bit);

            if (deque_bit == buffer_bit)
            {
                this->reserve(buffer_bit, this->value_byte_size_);
            }
            else if (deque_bit + 3 < buffer_bit)
            {
                this->reserve(buffer_bit - 2, this->value_byte_size_);
            }

            /*
            bool b1 = _deque_size + 1 >= this->circular_buffer_size_;
            uint64_t xsize = std::max((uint64_t)this->deque_size_, (uint64_t)1) * 2;
            bool b2 = xsize * 3 < this->circular_buffer_size_;

            if (b1)
            {
                uint64_t new_size = std::max((uint64_t)this->circular_buffer_size_, (uint64_t)1) * 2;
                this->reserve(std::min(new_size, (uint64_t)std::numeric_limits<INDEX_TYPE>::max()));
            }
            else if (b2)
            {
                this->reserve(std::min(xsize * 2, (uint64_t)std::numeric_limits<INDEX_TYPE>::max()));
            }
            */
        }
        bool empty() const
        {
            return this->deque_size_ == 0;
        }
        void shrink_to_fit()
        {
            uint64_t bit_size = stool::Byte::get_code_length(this->deque_size_ + 1);
            this->shrink_to_fit(bit_size);
        }
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
        static uint64_t get_byte_size(uint64_t value)
        {
            uint64_t new_byte_size = 0;
            uint64_t vsize = stool::Byte::get_code_length(value);
            if (vsize <= 8)
            {
                new_byte_size = 1;
            }
            else if (vsize <= 16)
            {
                new_byte_size = 2;
            }
            else if (vsize <= 32)
            {
                new_byte_size = 4;
            }
            else
            {
                new_byte_size = 8;
            }
            return new_byte_size;
        }

        // 要素を末尾に追加
        void push_back(const T &value)
        {
            // std::cout << "PUSH: " << value << std::endl;
            uint64_t vsize = stool::Byte::get_code_length(value);
            if (vsize > this->value_byte_size_ * 8)
            {
                uint64_t new_byte_size = get_byte_size(value);
                this->reserve(this->get_buffer_bit(), new_byte_size);
            }

            if (this->size() >= IntegerDeque<INDEX_TYPE>::max_deque_size())
            {
                throw std::invalid_argument("Error: push_back()");
            }
            this->update_size_if_needed();
            this->set_value(this->size(), value);
            this->deque_size_++;
        }
        void push_front(const T &value)
        {
            if (this->size() >= IntegerDeque<INDEX_TYPE>::max_deque_size())
            {
                throw std::invalid_argument("Error: push_front()");
            }

            this->update_size_if_needed();

            uint64_t pos = this->starting_position_ - 1;
            uint64_t mask = this->circular_buffer_size_ - 1;

            this->circular_buffer_[pos & mask] = value;
            this->starting_position_ = pos & mask;

            this->deque_size_++;
            assert(this->at(0) == value);
        }
        void pop_back()
        {
            // std::cout << "POP" << std::endl;
            this->deque_size_--;
            this->update_size_if_needed();
        }
        void pop_front()
        {
            uint64_t pos = this->starting_position_ + 1;
            uint64_t mask = this->circular_buffer_size_ - 1;
            this->starting_position_ = pos & mask;
            this->deque_size_--;
            this->update_size_if_needed();
        }
        void insert(const IntegerDequeIterator &position, const T &value)
        {
            uint64_t pos = position._m_idx;
            this->insert(pos, value);
        }
        void erase(const IntegerDequeIterator &position)
        {
            uint64_t pos = position._m_idx;
            this->erase(pos);
        }
        //std::pair<uint64_t, uint64_t> get_point()

        void insert(size_t position, const T &value)
        {
            /*
            if (this->size() >= IntegerDeque<INDEX_TYPE>::max_deque_size())
            {
                throw std::invalid_argument("Error: push_back()");
            }
            */

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
                uint64_t vsize = stool::Byte::get_code_length(value);
                if (vsize > this->value_byte_size_ * 8)
                {
                    uint64_t new_byte_size = get_byte_size(value);
                    this->reserve(this->get_buffer_bit(), new_byte_size);
                }

                this->update_size_if_needed();


                for (size_t i = this->deque_size_; i > position; --i)
                {
                    uint64_t pos = this->starting_position_ + i;
                    if (pos >= this->circular_buffer_size_)
                    {
                        pos -= this->circular_buffer_size_;
                    }
                    this->circular_buffer_[pos] = pos > 0 ? this->circular_buffer_[pos - 1] : this->circular_buffer_[this->circular_buffer_size_ - 1];
                }

                uint64_t wpos = this->starting_position_ + position;
                if (wpos >= this->circular_buffer_size_)
                {
                    wpos -= this->circular_buffer_size_;
                }
                this->circular_buffer_[wpos] = value;

                this->deque_size_++;
            }

            assert(this->at(position) == value);
        }
        void erase(size_t position)
        {
            if (position > 0)
            {
                for (int64_t i = position + 1; i < (int64_t)this->deque_size_; ++i)
                {
                    uint64_t pos = this->starting_position_ + i;
                    if (pos >= this->circular_buffer_size_)
                    {
                        pos -= this->circular_buffer_size_;
                    }
                    uint64_t pos2 = pos > 0 ? pos - 1 : this->circular_buffer_size_ - 1;
                    this->circular_buffer_[pos2] = this->circular_buffer_[pos];
                }
                this->deque_size_--;
                this->update_size_if_needed();
            }
            else
            {
                this->pop_front();
            }
        }

        // 現在の要素数を返す
        size_t size() const
        {
            return this->deque_size_;
        }
        void shrink_to_fit(uint64_t capacity_bit_size, uint8_t byte_size)
        {
            uint64_t size = 1 << capacity_bit_size;
            uint64_t tsize = size / (8 / byte_size);

            if (tsize > IntegerDeque<INDEX_TYPE>::max_deque_size())
            {
                assert(tsize > IntegerDeque<INDEX_TYPE>::max_deque_size());

                throw std::invalid_argument("shrink_to_fit");
            }
            else if (size > this->deque_size_ || this->value_byte_size_ != byte_size)
            {
                std::cout << "SHRINK/" << tsize << "/" << (uint64_t)byte_size << std::endl;

                T *new_data = new T[tsize];
                uint64_t i = 0;
                uint64_t shift = 64 - (byte_size * 8);
                for (uint64_t i = 0; i < tsize; i++)
                {
                    new_data[i] = 0;
                }
                for (IntegerDequeIterator it = this->begin(); it != this->end(); ++it)
                {
                    new_data[i] = new_data[i] | (*it << shift);
                    if (shift == 0)
                    {
                        i++;
                        shift = 64 - (byte_size * 8);
                    }
                    else
                    {
                        shift -= byte_size * 8;
                    }
                }

                if (this->circular_buffer_ != nullptr)
                {
                    delete[] this->circular_buffer_;
                    this->circular_buffer_ = nullptr;
                }

                this->circular_buffer_ = new_data;
                this->starting_position_ = 0;
                this->circular_buffer_size_ = tsize;
                this->value_byte_size_ = byte_size;
            }
        }
        std::deque<T> to_deque() const
        {

            std::deque<T> r;

            for (IntegerDequeIterator it = this->begin(); it != this->end(); ++it)
            {
                r.push_back(*it);
            }
            assert(r.size() == this->size());

            return r;
        }
        void print_info() const
        {
            for (uint64_t t = 0; t < this->circular_buffer_size_; t++)
            {
                std::bitset<64> p(this->circular_buffer_[t]);
                std::cout << p << std::flush;
                std::cout << " " << std::flush;
            }
            std::cout << std::endl;
            std::cout << "IntegerDeque: " << (uint64_t)this->circular_buffer_size_ << ", " << (uint64_t)this->deque_size_ << ", " << (uint64_t)this->starting_position_ << ", " << this->value_byte_size_ << std::endl;
        }
        void reserve(size_t capacity_bit_size, uint64_t byte_size)
        {
            this->shrink_to_fit(capacity_bit_size, byte_size);
        }
        void swap(IntegerDeque &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            std::swap(this->circular_buffer_size_, item.circular_buffer_size_);
            std::swap(this->starting_position_, item.starting_position_);
            std::swap(this->deque_size_, item.deque_size_);
        }
        // operator[]: 要素をインデックスでアクセスする
        
        T operator[](size_t index) const
        {
            assert(index < this->size());
            uint64_t pos = this->starting_position_ + index;
            uint64_t pos1 = pos / (8 / this->value_byte_size_);
            uint64_t pos2 = pos % (8 / this->value_byte_size_);
            if (pos1 >= this->circular_buffer_size_)
            {
                pos1 -= this->circular_buffer_size_;
            }
            uint64_t result = access_value(this->circular_buffer_[pos1], pos2, this->value_byte_size_);
            // std::cout << "Access: " << index << "/" << result << std::endl;
            return result;
        }
    
        void set_value(int64_t index, uint64_t value)
        {
            uint64_t pos = this->starting_position_ + index;
            uint64_t pos1 = pos / (8 / this->value_byte_size_);
            uint64_t pos2 = pos % (8 / this->value_byte_size_);
            if (pos1 >= this->circular_buffer_size_)
            {
                pos1 -= this->circular_buffer_size_;
            }
            uint64_t new_code = set_code(this->circular_buffer_[pos1], pos2, this->value_byte_size_, value);
            this->circular_buffer_[pos1] = new_code;
        }

        T at(uint64_t i) const
        {
            return (*this)[i];
        }
        /*
        T &operator[](size_t index)
        {
            assert(index < this->size());
            uint64_t pos = this->starting_position_ + index;
            uint64_t mask = this->circular_buffer_size_ - 1;
            return this->circular_buffer_[pos & mask];
        }
        */
        std::vector<uint64_t> to_vector() const
        {
            std::vector<uint64_t> tmp(this->size());
            std::copy(this->begin(), this->end(), tmp.begin());
            return tmp;
        }
    };
    /*
    template <typename T>
    using IntegerDeque8 = IntegerDeque<uint8_t>;
    */

    template <typename T>
    using IntegerDeque16 = IntegerDeque<uint16_t>;

    template <typename T>
    using IntegerDeque32 = IntegerDeque<uint32_t>;

    template <typename T>
    using IntegerDeque64 = IntegerDeque<uint64_t>;

}
