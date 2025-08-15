#pragma once
#include "../../basic/byte.hpp"
#include "../../basic/lsb_byte.hpp"
#include "../../basic/msb_byte.hpp"
#include "../../debug/print.hpp"

namespace stool
{

    class NaiveFLCVector
    {
        inline static std::vector<int> size_array{1, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 33, 40, 48, 58, 70, 84, 101, 122, 147, 177, 213, 256, 308, 370, 444, 533, 640, 768, 922, 1107, 1329, 1595, 1914, 2297, 2757, 3309, 3971, 4766};
        inline static const uint64_t MAX_SIZE = 4000;

    public:
        uint64_t *buffer_ = nullptr;
        uint64_t psum_;
        uint16_t size_;
        uint16_t buffer_size_;
        uint8_t code_length_;

        // INDEX_TYPE deque_size_;

        static uint64_t get_appropriate_buffer_size_index(int64_t num_elements, uint64_t code_length)
        {
            uint64_t total_code_size = num_elements * code_length;

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
        static uint8_t get_code_length(uint64_t value)
        {
            if (value <= 2)
            {
                return 1;
            }
            else if (value <= 4)
            {
                return 2;
            }
            else if (value <= 16)
            {
                return 4;
            }
            else if (value <= UINT8_MAX)
            {
                return 8;
            }
            else if (value <= UINT16_MAX)
            {
                return 16;
            }
            else if (value <= UINT32_MAX)
            {
                return 32;
            }
            else
            {
                return 64;
            }
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
                return sizeof(NaiveFLCVector) + (sizeof(uint64_t) * this->buffer_size_);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return (this->buffer_size_ - this->size_) * sizeof(uint64_t);
        }

        /**
         * @brief Copy constructor
         *
         * @param other The NaiveBitVector to copy from
         */
        NaiveFLCVector(const NaiveFLCVector &other) noexcept
        {
            this->buffer_size_ = other.buffer_size_;
            this->code_length_ = other.code_length_;
            this->size_ = other.size_;
            this->psum_ = other.psum_;
            this->buffer_ = new uint64_t[this->buffer_size_];

            std::memcpy(this->buffer_, other.buffer_, this->buffer_size_ * sizeof(uint64_t));
        }
        NaiveFLCVector(uint64_t buffer_size) noexcept
        {
            this->buffer_size_ = buffer_size;
            this->code_length_ = 1;
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
              code_length_(other.code_length_)
        {
            // Reset the source object
            other.buffer_ = nullptr;
            other.buffer_size_ = 0;
            other.code_length_ = 0;
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
                this->code_length_ = other.code_length_;
                this->size_ = other.size_;
                this->psum_ = other.psum_;

                other.buffer_ = nullptr;
                other.buffer_size_ = 0;
                other.code_length_ = 0;
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
            if (this->code_length_ == 0)
            {
                return this->buffer_size_ * 64;
            }
            else
            {
                return (this->buffer_size_ * 64) / this->code_length_;
            }
        }

        /**
         * @brief Remove all elements from the deque
         */
        void clear()
        {
            this->size_ = 0;
            this->psum_ = 0;
            this->code_length_ = 1;
            this->shrink_to_fit(0, 1);
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
            this->code_length_ = 0;
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
        void shrink_to_fit(int64_t new_element_count, uint8_t new_code_length)
        {
            assert(new_code_length == 1 || new_code_length == 2 || new_code_length == 4 || new_code_length == 8 || new_code_length == 16 || new_code_length == 32 || new_code_length == 64);

            int64_t current_size_index = this->get_current_buffer_size_index();
            int64_t appropriate_size_index = NaiveFLCVector::get_appropriate_buffer_size_index(new_element_count, new_code_length);

            if (this->code_length_ == new_code_length)
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
                uint64_t old_code_length = this->code_length_;

                for (uint64_t i = 0; i < this->size_; i++)
                {
                    uint64_t old_block_index = (i * old_code_length) / 64;
                    uint64_t new_block_index = (i * new_code_length) / 64;
                    uint64_t old_bit_index = (i * old_code_length) % 64;
                    uint64_t new_bit_index = (i * new_code_length) % 64;

                    uint64_t old_value = stool::MSBByte::read_as_64bit_integer(tmp[old_block_index], old_bit_index, old_code_length);
                    old_value = old_value << (64 - new_code_length);
                    this->buffer_[new_block_index] = stool::MSBByte::write_bits(this->buffer_[new_block_index], new_bit_index, new_code_length, old_value);
                }
                delete[] tmp;

                this->code_length_ = new_code_length;
                this->buffer_size_ = new_size;
            }
        }

        uint64_t get_max_value() const
        {
            uint64_t max_value = 0;
            for (uint64_t i = 0; i < this->size_; i++)
            {
                uint64_t value = this->at(i);
                if (value > max_value)
                {
                    max_value = value;
                }
            }
            return max_value;
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

            uint8_t code_length_candidate = NaiveFLCVector::get_code_length(value);
            uint64_t new_code_length = std::max(this->code_length_, code_length_candidate);
            assert(new_code_length > 0);

            this->shrink_to_fit(size + 1, new_code_length);

            uint64_t block_index = (this->size_ * this->code_length_) / 64;
            uint64_t bit_index = (this->size_ * this->code_length_) % 64;

            uint64_t write_value = value << (64 - this->code_length_);
            this->buffer_[block_index] = stool::MSBByte::write_bits(this->buffer_[block_index], bit_index, this->code_length_, write_value);

            this->size_++;
            this->psum_ += value;
        }

        /**
         * @brief Add an element to the beginning of the deque
         *
         * @param value The element to add
         * @throws std::invalid_argument If the deque would exceed maximum size
         */
        void push_front(uint64_t value)
        {

            uint64_t size = this->size();
            if (size == 0)
            {
                this->push_back(value);
            }
            else
            {

                this->insert(0, value);
            }
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
                this->shrink_to_fit(size - 1, this->code_length_);

                this->psum_ -= value;
                this->size_--;
                return value;
            }
        }

        /**
         * @brief Remove the first element from the deque
         */
        uint64_t pop_front()
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
                return this->remove(0);
            }
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

            return r;
        }
        std::vector<uint64_t> pop_front(uint64_t len)
        {
            std::vector<uint64_t> r;
            r.resize(len, UINT64_MAX);
            for (uint64_t i = 0; i < len; i++)
            {
                uint64_t block_index = (i * this->code_length_) / 64;
                uint64_t bit_index = (i * this->code_length_) % 64;
                uint64_t value = stool::MSBByte::read_as_64bit_integer(this->buffer_[block_index], bit_index, this->code_length_);
                r[i] = value;
            }
            this->remove(0, len);
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
            std::swap(this->buffer_size_, item.buffer_size_);
            std::swap(this->code_length_, item.code_length_);
        }

        uint64_t psum() const
        {
            return this->psum_;
        }

        uint64_t psum(uint64_t i) const
        {
            uint64_t sum = 0;
            for (uint64_t x = 0; x <= i; x++)
            {
                sum += this->at(x);
            }
            return sum;
        }
        uint64_t psum(uint64_t i, uint64_t j) const
        {
            assert(i <= j);
            assert(j < this->size());
            uint64_t sum = 0;
            for (uint64_t x = i; x <= j; x++)
            {
                assert(x < this->size());
                sum += this->at(x);
            }
            return sum;
        }

        uint64_t reverse_psum(uint64_t i) const
        {

            uint64_t sum = 0;
            uint64_t size = this->size();

            for (uint64_t x = 0; x <= i; x++)
            {
                uint64_t block_index = ((size - x - 1) * this->code_length_) / 64;
                uint64_t bit_index = ((size - x - 1) * this->code_length_) % 64;
                uint64_t value = stool::MSBByte::read_as_64bit_integer(this->buffer_[block_index], bit_index, this->code_length_);
                sum += value;
            }

            return sum;
        }
        int64_t search(uint64_t x) const noexcept
        {

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
        }
        /**
         * @brief Const subscript operator for element access
         *
         * @param index Position of the element to access
         * @return const T& Const reference to the element
         */
        uint64_t operator[](size_t index) const
        {
            uint64_t block_index = (index * this->code_length_) / 64;
            uint64_t bit_index = (index * this->code_length_) % 64;
            return stool::MSBByte::read_as_64bit_integer(this->buffer_[block_index], bit_index, this->code_length_);
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

        void shift_right(uint64_t position, uint64_t len, uint64_t new_code_length)
        {
            uint64_t size = this->size();

            if (size == 0)
            {
                for (uint64_t i = 0; i < len; i++)
                {
                    this->push_back(0);
                }
            }
            else
            {
                this->shrink_to_fit(size + len, new_code_length);
                uint64_t move_size = size - position;
                uint64_t new_size = size + len;
                for (uint64_t i = 0; i < move_size; i++)
                {
                    uint64_t new_block_index = ((new_size - i - 1) * this->code_length_) / 64;
                    uint64_t new_bit_index = ((new_size - i - 1) * this->code_length_) % 64;
                    uint64_t old_block_index = ((size - i - 1) * this->code_length_) / 64;
                    uint64_t old_bit_index = ((size - i - 1) * this->code_length_) % 64;

                    uint64_t value = stool::MSBByte::read_64bit_string(this->buffer_[old_block_index], old_bit_index, this->code_length_);
                    this->buffer_[new_block_index] = stool::MSBByte::write_bits(this->buffer_[new_block_index], new_bit_index, this->code_length_, value);
                }
                for (uint64_t i = 0; i < len; i++)
                {
                    uint64_t new_block_index = ((position + i) * this->code_length_) / 64;
                    uint64_t new_bit_index = ((position + i) * this->code_length_) % 64;
                    this->buffer_[new_block_index] = stool::MSBByte::write_bits(this->buffer_[new_block_index], new_bit_index, this->code_length_, 0);
                }

                this->size_ += len;
            }
        }
        void shift_left(uint64_t position, uint64_t len)
        {
            uint64_t size = this->size();
            uint64_t removed_sum = this->psum(position - len, position - 1);
            uint64_t new_size = size - len;
            uint64_t move_size = size - position;

            for (uint64_t i = 0; i < move_size; i++)
            {
                uint64_t new_block_index = ((position - len + i) * this->code_length_) / 64;
                uint64_t new_bit_index = ((position - len + i) * this->code_length_) % 64;
                uint64_t old_block_index = ((position + i) * this->code_length_) / 64;
                uint64_t old_bit_index = ((position + i) * this->code_length_) % 64;

                uint64_t value = stool::MSBByte::read_64bit_string(this->buffer_[old_block_index], old_bit_index, this->code_length_);
                this->buffer_[new_block_index] = stool::MSBByte::write_bits(this->buffer_[new_block_index], new_bit_index, this->code_length_, value);
            }
            this->shrink_to_fit(new_size, this->code_length_);
            this->psum_ -= removed_sum;
            this->size_ -= len;
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

                uint8_t code_length_candidate = NaiveFLCVector::get_code_length(value);
                uint64_t new_code_length = std::max(this->code_length_, code_length_candidate);
                assert(new_code_length > 0);
                this->shift_right(pos, 1, new_code_length);

                uint64_t new_block_index = ((pos) * this->code_length_) / 64;
                uint64_t new_bit_index = ((pos) * this->code_length_) % 64;
                uint64_t write_value = value << (64 - this->code_length_);
                this->buffer_[new_block_index] = stool::MSBByte::write_bits(this->buffer_[new_block_index], new_bit_index, this->code_length_, write_value);
                this->psum_ += value;
            }
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
        }
        void remove(uint64_t pos, uint64_t len)
        {
            if(pos + len == this->size()){
                this->pop_back(len);
            }else{
                this->shift_left(pos + len, len);
            }
        
        }

        void set_value(uint64_t pos, uint64_t value)
        {
            assert(pos < this->size());

            uint8_t code_length_candidate = NaiveFLCVector::get_code_length(value);
            uint64_t new_code_length = std::max(this->code_length_, code_length_candidate);
            if (new_code_length != this->code_length_)
            {
                this->shrink_to_fit(this->size(), new_code_length);
            }

            uint64_t block_index = (pos * this->code_length_) / 64;
            uint8_t bit_index = (pos * this->code_length_) % 64;

            uint64_t old_value = stool::MSBByte::read_as_64bit_integer(this->buffer_[block_index], bit_index, this->code_length_);
            if (value > old_value)
            {
                this->psum_ += old_value - value;
            }
            else
            {
                this->psum_ += value - old_value;
            }

            uint64_t write_value = value << (64 - this->code_length_);
            this->buffer_[block_index] = stool::MSBByte::write_bits(this->buffer_[block_index], bit_index, this->code_length_, write_value);
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
            std::cout << "code_length = " << (int)this->code_length_ << std::endl;
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
        }

        /**
         * @brief Calculates the memory size in bytes of a VLCDeque instance
         *
         * @param item The VLCDeque instance to measure
         * @return Number of bytes used by the VLCDeque
         */
        static uint64_t get_byte_size(const NaiveFLCVector &item)
        {
            uint64_t bytes = sizeof(item.psum_) + sizeof(item.size_) + sizeof(item.buffer_size_) + sizeof(item.code_length_) + (sizeof(uint64_t) * item.buffer_size_);
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
            os.write(reinterpret_cast<const char *>(&item.code_length_), sizeof(item.code_length_));
            os.write(reinterpret_cast<const char *>(item.buffer_), sizeof(uint64_t) * item.buffer_size_);
        }
        static void save(const NaiveFLCVector &item, std::vector<uint8_t> &output, uint64_t &pos)
        {
            std::memcpy(output.data() + pos, &item.psum_, sizeof(item.psum_));
            pos += sizeof(item.psum_);
            std::memcpy(output.data() + pos, &item.size_, sizeof(item.size_));
            pos += sizeof(item.size_);
            std::memcpy(output.data() + pos, &item.buffer_size_, sizeof(item.buffer_size_));
            pos += sizeof(item.buffer_size_);
            std::memcpy(output.data() + pos, &item.code_length_, sizeof(item.code_length_));
            pos += sizeof(item.code_length_);
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
            NaiveFLCVector r;
            std::memcpy(&r.psum_, data.data() + pos, sizeof(r.psum_));
            pos += sizeof(r.psum_);
            std::memcpy(&r.size_, data.data() + pos, sizeof(r.size_));
            pos += sizeof(r.size_);
            std::memcpy(&r.buffer_size_, data.data() + pos, sizeof(r.buffer_size_));
            pos += sizeof(r.buffer_size_);
            std::memcpy(&r.code_length_, data.data() + pos, sizeof(r.code_length_));
            pos += sizeof(r.code_length_);
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
            uint16_t _psum;
            uint16_t _size;
            uint16_t _buffer_size;
            uint8_t _code_length;
            ifs.read(reinterpret_cast<char *>(&_psum), sizeof(uint16_t));
            ifs.read(reinterpret_cast<char *>(&_size), sizeof(uint16_t));
            ifs.read(reinterpret_cast<char *>(&_buffer_size), sizeof(uint16_t));
            ifs.read(reinterpret_cast<char *>(&_code_length), sizeof(uint8_t));

            NaiveFLCVector r(_buffer_size);
            r.psum_ = _psum;
            r.size_ = _size;
            r.buffer_size_ = _buffer_size;
            r.code_length_ = _code_length;
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
    };
}
