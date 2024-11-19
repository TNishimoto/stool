#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include "../byte.hpp"
#include "../print.hpp"

namespace stool
{
    template <typename T, typename INDEX_TYPE = uint16_t>
    class SimpleDeque
    {
        using BUFFER_POS = INDEX_TYPE;
        using DEQUE_POS = INDEX_TYPE;
        T *circular_buffer_ = nullptr;
        INDEX_TYPE circular_buffer_size_;
        INDEX_TYPE starting_position_;
        INDEX_TYPE deque_size_;

    public:
        static uint64_t max_deque_size()
        {
            uint64_t b = stool::Byte::get_code_length(std::numeric_limits<INDEX_TYPE>::max());
            return (1 << (b - 1)) - 1;
        }
        uint64_t size_in_bytes() const
        {
            //return (sizeof(T) * this->circular_buffer_size_);

            return sizeof(SimpleDeque) + (sizeof(T) * this->circular_buffer_size_);
            //return sizeof(this->circular_buffer_size_) + sizeof(this->starting_position_) + sizeof(this->deque_size_) + sizeof(this->circular_buffer_) + (sizeof(T) * this->circular_buffer_size_);
        }

        SimpleDeque(const SimpleDeque &other) noexcept{
            this->circular_buffer_size_= other.circular_buffer_size_;
            this->starting_position_ = other.starting_position_;
            this->deque_size_ = other.deque_size_;
            this->circular_buffer_ = new T[this->circular_buffer_size_];
            for(uint64_t i = 0; i < this->circular_buffer_size_;i++){
                this->circular_buffer_[i] = other.circular_buffer_[i];
            }

        }


        // Move constructor
        SimpleDeque(SimpleDeque &&other) noexcept
            : circular_buffer_(other.circular_buffer_),
              circular_buffer_size_(other.circular_buffer_size_),
              starting_position_(other.starting_position_),
              deque_size_(other.deque_size_)
        {
            // Reset the source object
            other.circular_buffer_ = nullptr;
            other.circular_buffer_size_ = 0;
            other.starting_position_ = 0;
            other.deque_size_ = 0;
        }

        size_t capacity() const
        {
            return this->circular_buffer_size_;
        }
        void clear()
        {
            SimpleDeque tmp;
            this->swap(tmp);
        }

        class SimpleDequeIterator
        {

        public:
            SimpleDeque *_m_deq;
            INDEX_TYPE _m_idx;

            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T *;
            using reference = T &;
            SimpleDequeIterator(SimpleDeque *_deque, INDEX_TYPE _idx) : _m_deq(_deque), _m_idx(_idx) {}

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

            SimpleDequeIterator &operator++()
            {
                ++this->_m_idx;
                return *this;
            }

            SimpleDequeIterator operator++(int)
            {
                SimpleDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            SimpleDequeIterator &operator--()
            {
                --this->_m_idx;
                return *this;
            }

            SimpleDequeIterator operator--(int)
            {
                SimpleDequeIterator temp = *this;
                --(*this);
                return temp;
            }
            // ランダムアクセス演算子（i + n）
            SimpleDequeIterator operator+(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return SimpleDequeIterator(this->_m_deq, sum);
            }
            SimpleDequeIterator &operator+=(difference_type n)
            {
                this->_m_idx += n;
                return *this;
            }
            // ランダムアクセス演算子（i - n）
            SimpleDequeIterator operator-(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx - (int16_t)n;
                return SimpleDequeIterator(this->_m_deq, sum);
            }
            SimpleDequeIterator& operator-=(difference_type n) {
        this->_m_idx -= n;
        return *this;
    }

            // イテレータ同士の差（i - j）
            difference_type operator-(const SimpleDequeIterator &other) const
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
            bool operator==(const SimpleDequeIterator &other) const { return _m_idx == other._m_idx; }
            bool operator!=(const SimpleDequeIterator &other) const { return _m_idx != other._m_idx; }
            bool operator<(const SimpleDequeIterator &other) const { return this->_m_idx < other._m_idx; }
            bool operator>(const SimpleDequeIterator &other) const { return this->_m_idx > other._m_idx; }
            bool operator<=(const SimpleDequeIterator &other) const { return this->_m_idx <= other._m_idx; }
            bool operator>=(const SimpleDequeIterator &other) const { return this->_m_idx >= other._m_idx; }
        };
        SimpleDeque()
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
        }
        ~SimpleDeque()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
        }
        SimpleDequeIterator begin() const
        {
            return SimpleDequeIterator(const_cast<SimpleDeque<T, INDEX_TYPE> *>(this), 0);
        }
        SimpleDequeIterator end() const
        {
            return SimpleDequeIterator(const_cast<SimpleDeque<T, INDEX_TYPE> *>(this), this->deque_size_);
        }
        /*
        bool check_size() const
        {
            bool b1 = _deque_size + 1 >= this->circular_buffer_size_;
            bool b2 = _deque_size * 3 < this->circular_buffer_size_;
            returb b1 || b2;
        }
        */
        void update_size_if_needed()
        {
            uint64_t deque_bit = stool::Byte::get_code_length(this->deque_size_ + 1);
            uint64_t buffer_bit = stool::Byte::get_code_length(this->circular_buffer_size_);

            // uint64_t max = 1 << deque_bit;
            assert(deque_bit <= buffer_bit);

            if (deque_bit == buffer_bit)
            {
                this->reserve(buffer_bit);
            }
            else if (deque_bit + 3 < buffer_bit)
            {
                this->reserve(buffer_bit - 2);
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

        // 要素を末尾に追加
        void push_back(const T &value)
        {
            if (this->size() >= SimpleDeque<T, INDEX_TYPE>::max_deque_size())
            {
                throw std::invalid_argument("Error: push_back()");
            }
            this->update_size_if_needed();

            uint64_t pos = this->starting_position_ + this->size();
            uint64_t mask = this->circular_buffer_size_ - 1;
            this->circular_buffer_[pos & mask] = value;

            this->deque_size_++;
        }
        void push_front(const T &value)
        {
            if (this->size() >= SimpleDeque<T, INDEX_TYPE>::max_deque_size())
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
        void insert(const SimpleDequeIterator &position, const T &value)
        {
            uint64_t pos = position._m_idx;
            this->insert(pos, value);
        }
        void erase(const SimpleDequeIterator &position)
        {
            uint64_t pos = position._m_idx;
            this->erase(pos);
        }

        void insert(size_t position, const T &value)
        {
            if (this->size() >= SimpleDeque<T, INDEX_TYPE>::max_deque_size())
            {
                throw std::invalid_argument("Error: push_back()");
            }

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
                this->update_size_if_needed();

                // 後ろの要素を1つずつシフトして空間を作る
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
                for (int64_t i = position + 1; i < this->deque_size_; ++i)
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
        void shrink_to_fit(uint64_t capacity_bit_size)
        {
            uint64_t size = 1 << capacity_bit_size;

            if (size > SimpleDeque<T, INDEX_TYPE>::max_deque_size())
            {
                std::cout << "@@@" << capacity_bit_size << "/" << SimpleDeque<T, INDEX_TYPE>::max_deque_size() << "/" << size << "/" << this->deque_size_ << std::endl;
                throw std::invalid_argument("shrink_to_fit");
            }
            else if (size > this->deque_size_)
            {

                T *new_data = new T[size];
                uint64_t i = 0;
                for (SimpleDequeIterator it = this->begin(); it != this->end(); ++it)
                {
                    new_data[i++] = *it;
                }

                if (this->circular_buffer_ != nullptr)
                {
                    delete[] this->circular_buffer_;
                    this->circular_buffer_ = nullptr;
                }

                this->circular_buffer_ = new_data;
                this->starting_position_ = 0;
                this->circular_buffer_size_ = size;
            }
        }
        std::deque<T> to_deque() const
        {

            std::deque<T> r;

            for (SimpleDequeIterator it = this->begin(); it != this->end(); ++it)
            {
                r.push_back(*it);
            }
            assert(r.size() == this->size());

            return r;
        }
        void print_info() const
        {
            std::cout << "SimpleDeque: " << (uint64_t)this->circular_buffer_size_ << ", " << (uint64_t)this->deque_size_ << ", " << (uint64_t)this->starting_position_ << std::endl;
        }
        void reserve(size_t capacity_bit_size)
        {
            this->shrink_to_fit(capacity_bit_size);
        }
        void swap(SimpleDeque &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            std::swap(this->circular_buffer_size_, item.circular_buffer_size_);
            std::swap(this->starting_position_, item.starting_position_);
            std::swap(this->deque_size_, item.deque_size_);
        }
        // operator[]: 要素をインデックスでアクセスする
        const T &operator[](size_t index) const
        {
            assert(index < this->size());
            uint64_t pos = this->starting_position_ + index;
            uint64_t mask = this->circular_buffer_size_ - 1;
            return this->circular_buffer_[pos & mask];
        }

        T at(uint64_t i) const
        {
            return (*this)[i];
        }
        T &operator[](size_t index)
        {
            assert(index < this->size());
            uint64_t pos = this->starting_position_ + index;
            uint64_t mask = this->circular_buffer_size_ - 1;
            return this->circular_buffer_[pos & mask];
        }
    };
    /*
    template <typename T>
    using SimpleDeque8 = SimpleDeque<T, uint8_t>;
    */

    template <typename T>
    using SimpleDeque16 = SimpleDeque<T, uint16_t>;

    template <typename T>
    using SimpleDeque32 = SimpleDeque<T, uint32_t>;

    template <typename T>
    using SimpleDeque64 = SimpleDeque<T, uint64_t>;

}
