#pragma once
#include "../../basic/byte.hpp"
#include "../../basic/lsb_byte.hpp"
#include "../../basic/packed_psum.hpp"
#include "../../debug/print.hpp"

namespace stool
{

    template<bool USE_PSUM = true>
    class NaiveFLCVector
    {
        inline static std::vector<int> size_array{1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 33, 40, 48, 58, 70, 84, 101, 122, 147, 177, 213, 256, 308, 370, 444, 533, 640, 768, 922, 1107, 1329, 1595, 1914, 2297, 2757, 3309, 3971, 4766};
        inline static const uint64_t MAX_SIZE = 4000;

    public:
        uint64_t *buffer_ = nullptr;
        uint64_t psum_;
        uint16_t size_;
        uint16_t buffer_size_;
        uint8_t code_type_;

        // INDEX_TYPE deque_size_;


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

            bool is_end() const {
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
        /**
         * @brief Calculate the total memory usage in bytes
         *
         * @return uint64_t Total memory usage including object overhead and buffer
         */
        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if (only_extra_bytes)
            {
                return sizeof(uint64_t) * this->buffer_size_;
            }
            else
            {
                return sizeof(uint64_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t) + (sizeof(uint64_t) * this->buffer_size_);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            uint64_t buffer_bytes = this->buffer_size_ * sizeof(uint64_t);
            uint64_t bitsize = 1ULL << this->code_type_;
            uint64_t item_bytes = (this->size_ * bitsize) / 8;
            return buffer_bytes - item_bytes;
        }

        /**
         * @brief Copy constructor
         *
         * @param other The NaiveBitVector to copy from
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
        NaiveFLCVector(uint64_t buffer_size) noexcept
        {
            this->buffer_size_ = buffer_size;
            this->code_type_ = 0;
            this->size_ = 0;
            this->psum_ = 0;
            this->buffer_ = new uint64_t[this->buffer_size_];
        }

        NaiveFLCVector(const std::vector<uint64_t> &bv) noexcept
        {
            this->initialize();
            if (bv.size() == 0)
            {
                this->initialize();
            }
            else
            {
                for (auto &value : bv)
                {
                    this->push_back(value);
                }
            }
        }

        /**
         * @brief Move constructor
         *
         * @param other The NaiveBitVector to move from
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
         * @brief Get the current buffer capacity
         *
         * @return size_t The number of elements the buffer can hold
         */
        size_t capacity() const
        {
            uint64_t code_length = 1ULL << this->code_type_;
            return (this->buffer_size_ * 64) / code_length;
        }

        /**
         * @brief Remove all elements from the deque
         */
        void clear()
        {
            this->size_ = 0;
            this->psum_ = 0;
            this->code_type_ = 0;
            this->shrink_to_fit(0, this->code_type_);
        }
        bool empty() const
        {
            return this->size_ == 0;
        }

        /**
         * @brief Default constructor
         *
         * Creates an empty deque with initial capacity of 2 elements.
         */
        NaiveFLCVector()
        {
            this->initialize();
        }

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

        /**
         * @brief Destructor
         *
         * Frees the allocated circular buffer memory.
         */
        ~NaiveFLCVector()
        {
            if (this->buffer_ != nullptr)
            {
                delete[] this->buffer_;
                this->buffer_ = nullptr;
            }
        }

        uint64_t head() const
        {
            return this->at(0);
        }
        uint64_t tail() const
        {
            return this->at(this->size() - 1);
        }

        /**
         * @brief Reduce buffer size to fit current content
         *
         * Resizes the buffer to the minimum size needed to hold current elements.
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

                    uint64_t old_value = stool::MSBByte::read_as_64bit_integer(tmp[old_block_index], old_bit_index, old_code_length);
                    old_value = old_value << (64 - new_code_length);
                    this->buffer_[new_block_index] = stool::MSBByte::write_bits(this->buffer_[new_block_index], new_bit_index, new_code_length, old_value);
                }
                delete[] tmp;

                this->code_type_ = new_code_type;
                this->buffer_size_ = new_size;
            }
        }


        /**
         * @brief Add an element to the end of the deque
         *
         * @param value The element to add
         * @throws std::invalid_argument If the deque would exceed maximum size
         */
        void push_back(uint64_t value)
        {
            uint64_t size = this->size();
            if (size + 1 > MAX_SIZE)
            {
                std::cout << "Error: push_back() " << size + 1 << " > " << MAX_SIZE << std::endl;
                throw std::invalid_argument("Error: push_back()");
            }

            uint8_t code_type_candidate = (uint8_t)stool::PackedPsum::get_code_type(value);
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
            if(v != value){
                std::cout << "push_back: " << v << " != " << value << std::endl;
                std::cout << "new_code_type: " << new_code_type << std::endl;
                std::cout << "code_candidate: " << code_type_candidate << std::endl;

            }
            assert(v == value);
            #endif

            assert(this->verify());
        }

        /**
         * @brief Add an element to the beginning of the deque
         *
         * @param value The element to add
         * @throws std::invalid_argument If the deque would exceed maximum size
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
        void push_back(const std::vector<uint64_t> &new_items)
        {
            for (uint64_t v : new_items)
            {
                this->push_back(v);
            }

            assert(this->verify());
            #ifdef DEBUG
            for(uint64_t i = 0; i < new_items.size(); i++){
                uint64_t v = this->at(this->size() + i - new_items.size());
                assert(v == new_items[i]);
            }
            #endif
        }
        void push_front(const std::vector<uint64_t> &new_items)
        {
            uint64_t max_value = 0;
            uint64_t x_sum = 0;
            uint64_t x_size = new_items.size();
            for (uint64_t v : new_items)
            {
                if (v > max_value)
                {
                    max_value = v;
                }
                x_sum += v;
            }
            uint8_t code_type_candidate = (uint8_t)stool::PackedPsum::get_code_type(max_value);
            uint64_t new_code_type = std::max(this->code_type_, code_type_candidate);
            assert(new_code_type <= 6);

            this->shift_right(0, x_size, new_code_type);
            uint64_t code_length = 1ULL << new_code_type;


            for (uint64_t i = 0; i < x_size; i++)
            {
                uint64_t pos = i << new_code_type;
                uint64_t block_index = pos / 64;
                uint64_t bit_index = pos % 64;
                uint64_t value = new_items[i] << (64 - code_length);
                assert(block_index < this->buffer_size_);
                this->buffer_[block_index] = stool::MSBByte::write_bits(this->buffer_[block_index], bit_index, code_length, value);
            }

            this->psum_ += x_sum;

            assert(this->verify());
            #ifdef DEBUG
            for(uint64_t i = 0; i < new_items.size(); i++){
                uint64_t v = this->at(i);
                assert(v == new_items[i]);
            }
            #endif
            
        }


        /**
         * @brief Remove the last element from the deque
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
         * @brief Remove the first element from the deque
         */
        uint64_t pop_front()
        {
            uint64_t value = this->remove(0);
            assert(this->verify());
            return value;
        }

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
                uint64_t value = stool::MSBByte::read_as_64bit_integer(this->buffer_[block_index], bit_index, code_length);
                r[i] = value;
            }
            this->remove(0, len);
            assert(this->verify());
            return r;
        }

        /**
         * @brief Get the current number of elements
         *
         * @return size_t Number of elements in the deque
         */
        size_t size() const
        {
            return this->size_;
        }

        /**
         * @brief Swap contents with another NaiveBitVector
         *
         * @param item The NaiveBitVector to swap with
         */
        void swap(NaiveFLCVector &item)
        {
            std::swap(this->buffer_, item.buffer_);
            std::swap(this->psum_, item.psum_);
            std::swap(this->size_, item.size_);
            std::swap(this->buffer_size_, item.buffer_size_);
            std::swap(this->code_type_, item.code_type_);
        }

        uint64_t psum() const
        {
            return this->psum_;
        }

        uint64_t psum(uint64_t i) const
        {
            uint64_t sum = stool::PackedPsum::psum(this->buffer_, i, (stool::PackedBitType)this->code_type_, this->buffer_size_);
            return sum;
        }
        uint64_t psum(uint64_t i, uint64_t j) const
        {
            uint64_t sum = stool::PackedPsum::psum(this->buffer_, i, j, (stool::PackedBitType)this->code_type_, this->buffer_size_);

            #if DEBUG
            uint64_t true_sum = 0;
            for(uint64_t k = i; k <= j; k++){
                true_sum += this->at(k);
            }
            if(true_sum != sum){
                std::cout << "psum(" << i << ", " << j << ") = " << sum << ", true_sum = " << true_sum << std::endl;
            }
            #endif 
            return sum;
        }

        uint64_t reverse_psum(uint64_t i) const
        {
            uint64_t size = this->size();
            if(size == 0){
                return 0;
            }else if(i+1 == size){
                assert(this->verify());
                assert(this->psum() == this->psum(size - i - 1, size - 1));
                return this->psum(size - i - 1, size - 1);

                //return this->psum();
            }
            else{
                return this->psum(size - i - 1, size - 1);
            }
        }
        int64_t search(uint64_t x) const noexcept
        {
            return stool::PackedPsum::search(this->buffer_, x, (stool::PackedBitType)this->code_type_, this->psum_, this->buffer_size_);
            /*
            uint64_t sum = 0;
            uint64_t i = 0;

            if (x > this->psum_)
            {
                return -1;
            }
            else
            {
                while (true)
                {
                    sum += this->at(i);
                    if (sum >= x)
                    {
                        return i;
                    }
                    else
                    {
                        i++;
                    }
                }
                return -1;
            }
            */
        }
        /**
         * @brief Const subscript operator for element access
         *
         * @param index Position of the element to access
         * @return const T& Const reference to the element
         */
        uint64_t operator[](size_t index) const
        {
            //PackedBitType code_type = (PackedBitType)this->code_type_;
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

        /**
         * @brief Get element at specified position
         *
         * @param i Index of the element
         * @return T Copy of the element at position i
         */
        uint64_t at(uint64_t i) const
        {
            assert(i < this->size());
            return (*this)[i];
        }

        void shift_right(uint64_t position, uint64_t len, uint64_t new_code_type)
        {
            uint64_t size = this->size();
            //uint64_t code_length = 1ULL << new_code_type;

            this->shrink_to_fit(size + len, new_code_type);
            this->size_ += len;

            uint64_t bit_position = position << new_code_type;
            uint64_t bit_length = len << new_code_type;

            stool::MSBByte::shift_right(this->buffer_, bit_position, bit_length, this->buffer_size_);
            assert(this->verify());


        }
        void shift_left(uint64_t position, uint64_t len)
        {
            uint64_t size = this->size();
            uint64_t removed_sum = this->psum(position - len, position - 1);
            uint64_t new_size = size - len;
            uint64_t bit_position = position << this->code_type_;
            uint64_t bit_length = len << this->code_type_;
            //uint64_t move_size = size - position;
            //uint64_t code_length = 1ULL << this->code_type_;

            stool::MSBByte::shift_left(this->buffer_, bit_position, bit_length, this->buffer_size_);
            this->shrink_to_fit(new_size, this->code_type_);
            this->psum_ -= removed_sum;
            this->size_ -= len;
            assert(this->verify());

        }

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
                uint8_t code_type_candidate = (uint8_t)stool::PackedPsum::get_code_type(value);
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
        void remove(uint64_t pos, uint64_t len)
        {
            if(pos + len == this->size()){
                this->pop_back(len);
            }else{
                this->shift_left(pos + len, len);
            }
            assert(this->verify());

        }

        void set_value(uint64_t position, uint64_t value)
        {
            assert(position < this->size());

            uint8_t code_type_candidate = (uint8_t)stool::PackedPsum::get_code_type(value);
            uint64_t new_code_type = std::max(this->code_type_, code_type_candidate);
            if (new_code_type != this->code_type_)
            {
                this->shrink_to_fit(this->size(), new_code_type);
            }
            uint64_t code_length = 1ULL << new_code_type;

            uint64_t new_pos = position << new_code_type;
            uint64_t block_index = new_pos / 64;
            uint8_t bit_index = new_pos % 64;

            uint64_t old_value = stool::MSBByte::read_as_64bit_integer(this->buffer_[block_index], bit_index, code_length);
            if (value > old_value)
            {
                this->psum_ += value - old_value;
            }
            else
            {
                this->psum_ -= old_value - value;
            }

            uint64_t write_value = value << (64 - code_length);
            this->buffer_[block_index] = stool::MSBByte::write_bits(this->buffer_[block_index], bit_index, code_length, write_value);


            assert(this->verify());

        }

        std::string get_buffer_bit_string() const
        {
            std::vector<uint64_t> bits;
            bits.resize(this->buffer_size_);
            for (uint64_t i = 0; i < this->buffer_size_; i++)
            {
                bits[i] = this->buffer_[i];
            }

            return stool::Byte::to_bit_string(bits);
        }
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
        std::deque<uint64_t> to_deque() const
        {
            std::deque<uint64_t> r;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                r.push_back(this->at(i));
            }
            return r;
        }

        std::vector<uint64_t> to_vector() const
        {
            std::vector<uint64_t> v;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                v.push_back(this->at(i));
            }
            return v;
        }

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
                std::cout << "Buffer: " << this->get_buffer_bit_string() << std::endl;
            }
            else
            {
                std::cout << "Buffer: nullptr" << std::endl;
            }
            std::cout << "Content: " << this->to_string() << std::endl;

            std::cout << "}" << std::endl;
        }

        /**
         * @brief Increments a value at a given index by a delta
         *
         * Adds the specified delta to the value at index i. If the new value
         * requires a different number of bits to encode, the value is removed
         * and reinserted to maintain proper encoding.
         *
         * @param i Index of the value to increment
         * @param delta Amount to add to the value (can be negative)
         */
        void increment(uint64_t i, int64_t delta)
        {
            uint64_t new_value = this->at(i) + delta;
            this->set_value(i, new_value);
            assert(this->verify());
        }

        /**
         * @brief Calculates the memory size in bytes of a VLCDeque instance
         *
         * @param item The VLCDeque instance to measure
         * @return Number of bytes used by the VLCDeque
         */
        static uint64_t get_byte_size(const NaiveFLCVector &item)
        {
            uint64_t bytes = sizeof(item.psum_) + sizeof(item.size_) + sizeof(item.buffer_size_) + sizeof(item.code_type_) + (sizeof(uint64_t) * item.buffer_size_);
            return bytes;
        }

        /**
         * @brief Calculates the total memory size in bytes of a vector of VLCDeque instances
         *
         * @param items Vector of VLCDeque instances to measure
         * @return Total number of bytes used by all VLCDeque instances
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

        /**
         * @brief Saves a VLCDeque instance to a file stream
         *
         * Serializes the VLCDeque by writing its members to the provided output file stream.
         *
         * @param item The VLCDeque instance to save
         * @param os The output file stream to write to
         */
        static void save(const NaiveFLCVector &item, std::ofstream &os)
        {
            os.write(reinterpret_cast<const char *>(&item.psum_), sizeof(item.psum_));
            os.write(reinterpret_cast<const char *>(&item.size_), sizeof(item.size_));
            os.write(reinterpret_cast<const char *>(&item.buffer_size_), sizeof(item.buffer_size_));
            os.write(reinterpret_cast<const char *>(&item.code_type_), sizeof(item.code_type_));
            os.write(reinterpret_cast<const char *>(item.buffer_), sizeof(uint64_t) * item.buffer_size_);
        }
        static void save(const NaiveFLCVector &item, std::vector<uint8_t> &output, uint64_t &pos)
        {
            uint64_t bytes = item.size_in_bytes();
            if(output.size() < pos + bytes){
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
         * @brief Saves a vector of VLCDeque instances to a byte array
         *
         * Serializes multiple VLCDeque instances by first writing the vector size
         * followed by each VLCDeque instance.
         *
         * @param items Vector of VLCDeque instances to save
         * @param output The output byte array to write to
         * @param pos Current position in the output array, updated after writing
         */
        static void save(const std::vector<NaiveFLCVector> &items, std::vector<uint8_t> &output, uint64_t &pos)
        {
            uint64_t size = items.size();
            std::memcpy(output.data() + pos, &size, sizeof(size));
            pos += sizeof(size);

            for (auto &it : items)
            {
                NaiveFLCVector::save(it, output, pos);
            }
        }

        /**
         * @brief Saves a vector of VLCDeque instances to a file stream
         *
         * Serializes multiple VLCDeque instances by first writing the vector size
         * followed by each VLCDeque instance.
         *
         * @param items Vector of VLCDeque instances to save
         * @param os The output file stream to write to
         */
        static void save(const std::vector<NaiveFLCVector> &items, std::ofstream &os)
        {
            uint64_t size = items.size();
            os.write(reinterpret_cast<const char *>(&size), sizeof(size));
            for (auto &it : items)
            {
                NaiveFLCVector::save(it, os);
            }
        }

        /**
         * @brief Loads a VLCDeque instance from a byte array
         *
         * Deserializes a VLCDeque by reading its members from the input byte array
         * at the specified position.
         *
         * @param data The input byte array to read from
         * @param pos Current position in the input array, updated after reading
         * @return The deserialized VLCDeque instance
         */
        static NaiveFLCVector load(const std::vector<uint8_t> &data, uint64_t &pos)
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
         * @brief Loads a VLCDeque instance from a file stream
         *
         * Deserializes a VLCDeque by reading its members from the provided input file stream.
         *
         * @param ifs The input file stream to read from
         * @return The deserialized VLCDeque instance
         */
        static NaiveFLCVector load(std::ifstream &ifs)
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
         * @brief Loads a vector of VLCDeque instances from a byte array
         *
         * Deserializes multiple VLCDeque instances by first reading the vector size,
         * then loading each VLCDeque instance sequentially from the input byte array.
         *
         * @param data The input byte array to read from
         * @param pos Current position in the input array, updated after reading
         * @return Vector containing the deserialized VLCDeque instances
         */
        static std::vector<NaiveFLCVector> load_vector(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t size;
            std::memcpy(&size, data.data() + pos, sizeof(size));
            pos += sizeof(size);

            std::vector<NaiveFLCVector> r;
            for (uint64_t i = 0; i < size; i++)
            {
                r.push_back(NaiveFLCVector::load(data, pos));
            }
            return r;
        }

        /**
         * @brief Loads a vector of VLCDeque instances from a file stream
         *
         * Deserializes multiple VLCDeque instances by first reading the vector size,
         * then loading each VLCDeque instance sequentially from the input file stream.
         *
         * @param ifs The input file stream to read from
         * @return Vector containing the deserialized VLCDeque instances
         */
        static std::vector<NaiveFLCVector> load_vector(std::ifstream &ifs)
        {
            uint64_t size = 0;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(size));

            std::vector<NaiveFLCVector> r;
            for (uint64_t i = 0; i < size; i++)
            {
                r.push_back(NaiveFLCVector::load(ifs));
            }
            return r;
        }
        template <typename VEC>
        void to_values(VEC &output_vec) const
        {
            output_vec.clear();
            output_vec.resize(this->size());
            for (uint64_t i = 0; i < this->size(); i++)
            {
                output_vec[i] = this->at(i);
            }
        }


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
        bool verify() const{
            uint64_t true_sum = 0;
            for(uint64_t i = 0; i < this->size(); i++){
                true_sum += this->at(i);
            }
            if(true_sum != this->psum()){
                std::cout << "psum = " << this->psum() << ", true_sum = " << true_sum << std::endl;
                throw std::runtime_error("Error: verify");
            }
            return true;
        }
        
    };
}
