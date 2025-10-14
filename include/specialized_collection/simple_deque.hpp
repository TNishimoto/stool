#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include <cassert>
#include "../basic/byte.hpp"
#include "../basic/lsb_byte.hpp"
#include "../debug/debug_printer.hpp"

namespace stool
{
    
    /**
     * @brief A simple circular buffer-based deque implementation [Unchecked AI's Comment] 
     * 
     * @tparam T The type of elements stored in the deque
     * @tparam INDEX_TYPE The type used for indexing (default: uint16_t)
     * 
     * This class provides a memory-efficient deque implementation using a circular buffer.
     * It supports O(1) push/pop operations at both ends and random access to elements.
     * The buffer size is automatically managed to maintain optimal memory usage.
     */
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
        /**
         * @brief Get the maximum possible deque size for the given index type
         * 
         * @return uint64_t The maximum number of elements that can be stored
         */
        static uint64_t max_deque_size()
        {
            uint64_t b = stool::LSBByte::get_code_length(std::numeric_limits<INDEX_TYPE>::max());
            return (1 << (b - 1)) - 1;
        }

        
        /**
         * @brief Calculate the total memory usage in bytes
         * 
         * @return uint64_t Total memory usage including object overhead and buffer
         */
        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if(only_extra_bytes){
                return sizeof(T) * this->circular_buffer_size_;
            }else{
                return sizeof(SimpleDeque) + (sizeof(T) * this->circular_buffer_size_);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return (this->capacity() - this->size()) * sizeof(T);
        }


        /**
         * @brief Copy constructor
         * 
         * @param other The SimpleDeque to copy from
         */
        SimpleDeque(const SimpleDeque &other) noexcept
        {
            this->circular_buffer_size_ = other.circular_buffer_size_;
            this->starting_position_ = other.starting_position_;
            this->deque_size_ = other.deque_size_;
            this->circular_buffer_ = new T[this->circular_buffer_size_];
            for (uint64_t i = 0; i < this->circular_buffer_size_; i++)
            {
                this->circular_buffer_[i] = other.circular_buffer_[i];
            }
        }

        /**
         * @brief Move constructor
         * 
         * @param other The SimpleDeque to move from
         */
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
        
        /**
         * @brief Move assignment operator
         * 
         * @param other The SimpleDeque to move from
         * @return SimpleDeque& Reference to this object
         */
        SimpleDeque &operator=(SimpleDeque &&other) noexcept
        {
            if (this != &other)
            {
                this->circular_buffer_ = other.circular_buffer_;
                this->circular_buffer_size_ = other.circular_buffer_size_;
                this->starting_position_ = other.starting_position_;
                this->deque_size_ = other.deque_size_;


                other.circular_buffer_ = nullptr;
                other.circular_buffer_size_ = 0;
                other.starting_position_ = 0;
                other.deque_size_ = 0;
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
            return this->circular_buffer_size_;
        }
        
        /**
         * @brief Remove all elements from the deque
         */
        void clear()
        {
            SimpleDeque tmp;
            this->swap(tmp);
        }

        /**
         * @brief Iterator class for SimpleDeque
         * 
         * Provides random access iterator functionality for traversing the deque elements.
         */
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
            
            /**
             * @brief Construct an iterator
             * 
             * @param _deque Pointer to the deque
             * @param _idx Index position
             */
            SimpleDequeIterator(SimpleDeque *_deque, INDEX_TYPE _idx) : _m_deq(_deque), _m_idx(_idx) {}

            /**
             * @brief Dereference operator
             * 
             * @return T The element at the current position
             */
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

            /**
             * @brief Pre-increment operator
             * 
             * @return SimpleDequeIterator& Reference to the incremented iterator
             */
            SimpleDequeIterator &operator++()
            {
                ++this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-increment operator
             * 
             * @return SimpleDequeIterator Copy of the iterator before increment
             */
            SimpleDequeIterator operator++(int)
            {
                SimpleDequeIterator temp = *this;

                ++(*this);
                return temp;
            }

            /**
             * @brief Pre-decrement operator
             * 
             * @return SimpleDequeIterator& Reference to the decremented iterator
             */
            SimpleDequeIterator &operator--()
            {
                --this->_m_idx;
                return *this;
            }

            /**
             * @brief Post-decrement operator
             * 
             * @return SimpleDequeIterator Copy of the iterator before decrement
             */
            SimpleDequeIterator operator--(int)
            {
                SimpleDequeIterator temp = *this;
                --(*this);
                return temp;
            }
            
            /**
             * @brief Addition operator for random access
             * 
             * @param n Number of positions to advance
             * @return SimpleDequeIterator Iterator at the new position
             */
            SimpleDequeIterator operator+(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return SimpleDequeIterator(this->_m_deq, sum);
            }
            
            /**
             * @brief Compound addition assignment
             * 
             * @param n Number of positions to advance
             * @return SimpleDequeIterator& Reference to this iterator
             */
            SimpleDequeIterator &operator+=(difference_type n)
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
            SimpleDequeIterator operator-(difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx - (int16_t)n;
                return SimpleDequeIterator(this->_m_deq, sum);
            }
            
            /**
             * @brief Compound subtraction assignment
             * 
             * @param n Number of positions to retreat
             * @return SimpleDequeIterator& Reference to this iterator
             */
            SimpleDequeIterator &operator-=(difference_type n)
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
            difference_type operator-(const SimpleDequeIterator &other) const
            {
                return (int16_t)this->_m_idx - (int16_t)other._m_idx;
            }

            /**
             * @brief Subscript operator for random access
             * 
             * @param n Offset from current position
             * @return T& Reference to the element at offset n
             */
            T &operator[](difference_type n)
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
            const T &operator[](difference_type n) const
            {
                int16_t sum = (int16_t)this->_m_idx + (int16_t)n;
                return (*this->_m_deq)[sum];
            }
            
            /**
             * @brief Equality comparison
             */
            bool operator==(const SimpleDequeIterator &other) const { return _m_idx == other._m_idx; }
            
            /**
             * @brief Inequality comparison
             */
            bool operator!=(const SimpleDequeIterator &other) const { return _m_idx != other._m_idx; }
            
            /**
             * @brief Less than comparison
             */
            bool operator<(const SimpleDequeIterator &other) const { return this->_m_idx < other._m_idx; }
            
            /**
             * @brief Greater than comparison
             */
            bool operator>(const SimpleDequeIterator &other) const { return this->_m_idx > other._m_idx; }
            
            /**
             * @brief Less than or equal comparison
             */
            bool operator<=(const SimpleDequeIterator &other) const { return this->_m_idx <= other._m_idx; }
            
            /**
             * @brief Greater than or equal comparison
             */
            bool operator>=(const SimpleDequeIterator &other) const { return this->_m_idx >= other._m_idx; }
        };
        
        /**
         * @brief Default constructor
         * 
         * Creates an empty deque with initial capacity of 2 elements.
         */
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
        
        /**
         * @brief Constructor with specified buffer size
         * 
         * @param _circular_buffer_size Initial capacity of the circular buffer
         */
        SimpleDeque(uint64_t _circular_buffer_size)
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
            this->circular_buffer_ = new T[_circular_buffer_size];
            this->starting_position_ = 0;
            this->circular_buffer_size_ = _circular_buffer_size;
            this->deque_size_ = 0;
        }

        /**
         * @brief Destructor
         * 
         * Frees the allocated circular buffer memory.
         */
        ~SimpleDeque()
        {
            if (this->circular_buffer_ != nullptr)
            {
                delete[] this->circular_buffer_;
                this->circular_buffer_ = nullptr;
            }
        }
        
        /**
         * @brief Get iterator to the first element
         * 
         * @return SimpleDequeIterator Iterator pointing to the beginning
         */
        SimpleDequeIterator begin() const
        {
            return SimpleDequeIterator(const_cast<SimpleDeque<T, INDEX_TYPE> *>(this), 0);
        }
        
        /**
         * @brief Get iterator past the last element
         * 
         * @return SimpleDequeIterator Iterator pointing past the end
         */
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
        
        /**
         * @brief Update buffer size if needed based on current usage
         * 
         * Automatically resizes the buffer to maintain optimal memory efficiency.
         */
        void update_size_if_needed()
        {
            uint64_t deque_bit = stool::LSBByte::get_code_length(this->deque_size_ + 1);
            uint64_t buffer_bit = stool::LSBByte::get_code_length(this->circular_buffer_size_);

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
        
        /**
         * @brief Check if the deque is empty
         * 
         * @return bool True if the deque contains no elements
         */
        bool empty() const
        {
            return this->deque_size_ == 0;
        }
        
        /**
         * @brief Reduce buffer size to fit current content
         * 
         * Resizes the buffer to the minimum size needed to hold current elements.
         */
        void shrink_to_fit()
        {
            uint64_t bit_size = stool::LSBByte::get_code_length(this->deque_size_ + 1);
            this->shrink_to_fit(bit_size);
        }

        /**
         * @brief Add an element to the end of the deque
         * 
         * @param value The element to add
         * @throws std::invalid_argument If the deque would exceed maximum size
         */
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
        
        /**
         * @brief Add an element to the beginning of the deque
         * 
         * @param value The element to add
         * @throws std::invalid_argument If the deque would exceed maximum size
         */
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
        
        /**
         * @brief Remove the last element from the deque
         */
        void pop_back()
        {
            this->deque_size_--;
            this->update_size_if_needed();
        }
        
        /**
         * @brief Remove the first element from the deque
         */
        void pop_front()
        {
            uint64_t pos = this->starting_position_ + 1;
            uint64_t mask = this->circular_buffer_size_ - 1;
            this->starting_position_ = pos & mask;
            this->deque_size_--;
            this->update_size_if_needed();
        }
        
        /**
         * @brief Insert an element at the specified iterator position
         * 
         * @param position Iterator pointing to the insertion position
         * @param value The element to insert
         */
        void insert(const SimpleDequeIterator &position, const T &value)
        {
            uint64_t pos = position._m_idx;
            this->insert(pos, value);
        }
        
        /**
         * @brief Remove element at the specified iterator position
         * 
         * @param position Iterator pointing to the element to remove
         */
        void erase(const SimpleDequeIterator &position)
        {
            uint64_t pos = position._m_idx;
            this->erase(pos);
        }

        /**
         * @brief Insert an element at the specified position
         * 
         * @param position Index where to insert the element
         * @param value The element to insert
         * @throws std::invalid_argument If the deque would exceed maximum size
         * @throws std::out_of_range If position is out of range
         */
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
        
        /**
         * @brief Remove element at the specified position
         * 
         * @param position Index of the element to remove
         */
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

        /**
         * @brief Get the current number of elements
         * 
         * @return size_t Number of elements in the deque
         */
        size_t size() const
        {
            return this->deque_size_;
        }
        
        /**
         * @brief Resize buffer to specified bit size
         * 
         * @param capacity_bit_size The bit size for the new capacity (2^capacity_bit_size)
         * @throws std::invalid_argument If the new size would exceed maximum allowed size
         */
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
        
        /**
         * @brief Convert to std::deque
         * 
         * @return std::deque<T> A standard deque containing the same elements
         */
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
        
        /**
         * @brief Print debug information about the deque
         */
        void print_info() const
        {
            std::cout << "SimpleDeque: " << (uint64_t)this->circular_buffer_size_ << ", " << (uint64_t)this->deque_size_ << ", " << (uint64_t)this->starting_position_ << std::endl;
        }
        
        /**
         * @brief Reserve buffer space with specified bit size
         * 
         * @param capacity_bit_size The bit size for the new capacity (2^capacity_bit_size)
         */
        void reserve(size_t capacity_bit_size)
        {
            this->shrink_to_fit(capacity_bit_size);
        }
        
        /**
         * @brief Swap contents with another SimpleDeque
         * 
         * @param item The SimpleDeque to swap with
         */
        void swap(SimpleDeque &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            std::swap(this->circular_buffer_size_, item.circular_buffer_size_);
            std::swap(this->starting_position_, item.starting_position_);
            std::swap(this->deque_size_, item.deque_size_);
        }
        
        /**
         * @brief Const subscript operator for element access
         * 
         * @param index Position of the element to access
         * @return const T& Const reference to the element
         */
        const T &operator[](size_t index) const
        {
            assert(index < this->size());
            uint64_t pos = this->starting_position_ + index;
            uint64_t mask = this->circular_buffer_size_ - 1;
            return this->circular_buffer_[pos & mask];
        }

        /**
         * @brief Get element at specified position
         * 
         * @param i Index of the element
         * @return T Copy of the element at position i
         */
        T at(uint64_t i) const
        {
            return (*this)[i];
        }
        
        /**
         * @brief Subscript operator for element access
         * 
         * @param index Position of the element to access
         * @return T& Reference to the element
         */
        T &operator[](size_t index)
        {
            assert(index < this->size());
            uint64_t pos = this->starting_position_ + index;
            uint64_t mask = this->circular_buffer_size_ - 1;
            return this->circular_buffer_[pos & mask];
        }

        /**
         * @brief Save deque to a byte vector
         * 
         * @param item The deque to save
         * @param output Vector to store the serialized data
         * @param pos Current position in the output vector (will be updated)
         */
        static void save(const SimpleDeque<T, INDEX_TYPE> &item, std::vector<uint8_t> &output, uint64_t &pos)
        {

            std::memcpy(output.data() + pos, &item.circular_buffer_size_, sizeof(item.circular_buffer_size_));
            pos += sizeof(item.circular_buffer_size_);
            std::memcpy(output.data() + pos, &item.starting_position_, sizeof(item.starting_position_));
            pos += sizeof(item.starting_position_);
            std::memcpy(output.data() + pos, &item.deque_size_, sizeof(item.deque_size_));
            pos += sizeof(item.deque_size_);
            std::memcpy(output.data() + pos, item.circular_buffer_, item.circular_buffer_size_ * sizeof(T));
            pos += item.circular_buffer_size_ * sizeof(T);
        }
        
        /**
         * @brief Save deque to a file stream
         * 
         * @param item The deque to save
         * @param os Output file stream
         */
        static void save(const SimpleDeque<T, INDEX_TYPE> &item, std::ofstream &os)
        {
            // uint64_t bytes = sizeof(item.circular_buffer_size_) + sizeof(item.starting_position_) + sizeof(item.deque_size_) + (item.circular_buffer_size_ * sizeof(T));

            os.write(reinterpret_cast<const char *>(&item.circular_buffer_size_), sizeof(item.circular_buffer_size_));
            os.write(reinterpret_cast<const char *>(&item.starting_position_), sizeof(item.starting_position_));
            os.write(reinterpret_cast<const char *>(&item.deque_size_), sizeof(item.deque_size_));
            os.write(reinterpret_cast<const char *>(item.circular_buffer_), item.circular_buffer_size_ * sizeof(T));
        }

        /**
         * @brief Load deque from a byte vector
         * 
         * @param data Vector containing the serialized data
         * @param pos Current position in the data vector (will be updated)
         * @return SimpleDeque<T, INDEX_TYPE> The loaded deque
         */
        static SimpleDeque<T, INDEX_TYPE> load(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            INDEX_TYPE _circular_buffer_size;
            INDEX_TYPE _starting_position;
            INDEX_TYPE _deque_size;

            std::memcpy(&_circular_buffer_size, data.data() + pos, sizeof(INDEX_TYPE));
            pos += sizeof(INDEX_TYPE);
            std::memcpy(&_starting_position, data.data() + pos, sizeof(INDEX_TYPE));
            pos += sizeof(INDEX_TYPE);
            std::memcpy(&_deque_size, data.data() + pos, sizeof(INDEX_TYPE));
            pos += sizeof(INDEX_TYPE);

            SimpleDeque<T, INDEX_TYPE> r(_circular_buffer_size);
            r.starting_position_ = _starting_position;
            r.deque_size_ = _deque_size;

            std::memcpy(r.circular_buffer_, data.data() + pos, sizeof(T) * _circular_buffer_size);
            pos += sizeof(T) * _circular_buffer_size;

            return r;
        }
        
        /**
         * @brief Load deque from a file stream
         * 
         * @param ifs Input file stream
         * @return SimpleDeque<T, INDEX_TYPE> The loaded deque
         */
        static SimpleDeque<T, INDEX_TYPE> load(std::ifstream &ifs)
        {
            INDEX_TYPE _circular_buffer_size;
            INDEX_TYPE _starting_position;
            INDEX_TYPE _deque_size;

            ifs.read(reinterpret_cast<char *>(&_circular_buffer_size), sizeof(INDEX_TYPE));
            ifs.read(reinterpret_cast<char *>(&_starting_position), sizeof(INDEX_TYPE));
            ifs.read(reinterpret_cast<char *>(&_deque_size), sizeof(INDEX_TYPE));

            SimpleDeque<T, INDEX_TYPE> r(_circular_buffer_size);
            r.starting_position_ = _starting_position;
            r.deque_size_ = _deque_size;
            ifs.read(reinterpret_cast<char *>(r.circular_buffer_), sizeof(T) * _circular_buffer_size);

            return r;
        }
        
        /**
         * @brief Calculate the serialized size of a deque
         * 
         * @param item The deque to measure
         * @return uint64_t Size in bytes when serialized
         */
        static uint64_t get_byte_size(const SimpleDeque<T, INDEX_TYPE> &item)
        {
            uint64_t bytes = (sizeof(INDEX_TYPE) * 3) + (item.circular_buffer_size_ * sizeof(T));
            return bytes;
        }

        /*
        T *circular_buffer_ = nullptr;
        INDEX_TYPE circular_buffer_size_;
        INDEX_TYPE starting_position_;
        INDEX_TYPE deque_size_;
        */
    };
    /*
    template <typename T>
    using SimpleDeque8 = SimpleDeque<T, uint8_t>;
    */

    /**
     * @brief SimpleDeque with 16-bit indexing
     */
    template <typename T>
    using SimpleDeque16 = SimpleDeque<T, uint16_t>;

    /**
     * @brief SimpleDeque with 32-bit indexing
     */
    template <typename T>
    using SimpleDeque32 = SimpleDeque<T, uint32_t>;

    /**
     * @brief SimpleDeque with 64-bit indexing
     */
    template <typename T>
    using SimpleDeque64 = SimpleDeque<T, uint64_t>;

}
