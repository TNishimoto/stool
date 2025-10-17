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

    template <uint64_t SIZE = 1024>
    class NaiveArrayForFasterPsum
    {
    public:
    protected:
        std::array<uint64_t, SIZE> circular_buffer_;
        uint64_t deque_size_ = 0;

    public:

    uint64_t size_in_bytes() const
        {
            return sizeof(NaiveArrayForFasterPsum);
        }

        NaiveArrayForFasterPsum(const std::vector<uint64_t> &items)
        {
            this->initialize(items);
        }
        void initialize(const std::vector<uint64_t> &items){
            if(items.size() != 0){
                this->circular_buffer_[0] = items[0];
            }
            for(uint64_t i = 1; i < items.size(); i++){
                this->circular_buffer_[i] = this->circular_buffer_[i-1] + items[i];
            }
            for(uint64_t i = items.size(); i < SIZE; i++){
                this->circular_buffer_[i] = UINT64_MAX;
            }
            this->deque_size_ = items.size();
        }

        size_t capacity() const
        {
            return SIZE;
        }

        /**
         * @brief Clear all elements from the deque
         */
        void clear()
        {
            this->deque_size_ = 0; 
            for(uint64_t i = 0; i < SIZE; i++){
                this->circular_buffer_[i] = UINT64_MAX;
            }

        }

        /**
         * @brief Default constructor
         *
         * Initializes an empty deque with default capacity
         */
        NaiveArrayForFasterPsum()
        {
            this->clear();
        }

        /**
         * @brief Destructor
         *
         * Frees the allocated memory
         */
        ~NaiveArrayForFasterPsum()
        {
            this->clear();
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

        uint64_t max_size() const
        {
            return SIZE;
        }

        static NaiveArrayForFasterPsum build(const std::vector<uint64_t> &items){
            NaiveArrayForFasterPsum deque(items);
            return deque;
        }

        /**
         * @brief Add an element to the back of the deque
         *
         * @param value The value to add
         */
        void push_back(uint64_t value)
        {

            if (this->deque_size_ >= SIZE)
            {
                throw std::out_of_range("push_back, Size out of range");
            }

            if(this->deque_size_ == 0){
                this->circular_buffer_[0] = value;
            }else{
                this->circular_buffer_[this->deque_size_] = this->circular_buffer_[this->deque_size_ - 1] + value;
            }
            this->deque_size_++;

            assert(this->verify());
        }

        /**
         * @brief Add an element to the front of the deque
         *
         * @param value The value to add
         */
        void push_front(uint64_t value)
        {
            this->insert(0, value);
        }

        /**
         * @brief Remove the last element from the deque
         */
        void pop_back()
        {
            if (this->deque_size_ == 0)
            {
                throw std::out_of_range("pop_back, Size out of range");
            }
            this->circular_buffer_[this->deque_size_-1] = UINT64_MAX;            
            this->deque_size_--;
            assert(this->verify());
        }

        /**
         * @brief Remove the first element from the deque
         */
        void pop_front()
        {
            if (this->deque_size_ == 0)
            {
                throw std::out_of_range("pop_front, Size out of range");
            }

            uint64_t first_value = this->circular_buffer_[0];
            for(uint64_t i = 0; i < this->deque_size_ - 1; i++){
                this->circular_buffer_[i] = this->circular_buffer_[i+1] - first_value;
            }
            this->circular_buffer_[this->deque_size_ - 1] = UINT64_MAX;
            this->deque_size_--;
            assert(this->verify());

        }
        uint64_t size() const
        {
            return this->deque_size_;
        }

        /**
         * @brief Insert an element at a specific position
         *
         * @param position The position to insert at
         * @param value The value to insert
         */
        void insert(uint64_t position, uint64_t value)
        {
            uint64_t size = this->size();

            if (size  >= SIZE)
            {
                throw std::out_of_range("insert, Size out of range");
            }

            if (position > this->deque_size_ + 1)
            {
                throw std::out_of_range("Position out of range");
            }

            for(int64_t i = this->deque_size_ ; i >= (int64_t)(position + 1); i--){
                this->circular_buffer_[i] = this->circular_buffer_[i-1] + value;
            }

            this->circular_buffer_[position] = value + (position == 0 ? 0 : this->circular_buffer_[position - 1]);
            this->deque_size_++;
            assert(this->verify());

        }
        void remove(uint64_t position)
        {
            this->erase(position);
        }


        /**
         * @brief Erase an element at a specific position
         *
         * @param position The position of the element to erase
         */
        void erase(uint64_t position)
        {
            if(position >= this->deque_size_){
                throw std::out_of_range("erase, Position out of range");
            }
            uint64_t removed_value = this->at(position);
            for(uint64_t i = position; i < this->deque_size_ - 1; i++){
                this->circular_buffer_[i] = this->circular_buffer_[i+1] - removed_value;
            }
            this->circular_buffer_[this->deque_size_ - 1] = UINT64_MAX;
            this->deque_size_--;
            assert(this->verify());
        }


        uint64_t value_capacity() const
        {
            return (UINT64_MAX - 1);
        }

        /**
         * @brief Print debug information about the deque
         */
        void print_info() const
        {
            std::cout << "deque_size = " << this->deque_size_ << std::endl;
            std::cout << "circular_buffer = ";
            for(uint64_t i = 0; i < this->deque_size_; i++){
                std::cout << this->circular_buffer_[i] << " ";
            }
            std::cout << std::endl;
        }
        std::string to_string() const
        {
            std::string r = "[";
            for(uint64_t i = 0; i < this->deque_size_; i++){
                r += std::to_string(this->circular_buffer_[i]);
                if(i < this->deque_size_ - 1){
                    r += ", ";
                }
            }
            r += "]";
            return r;
        }

        /**
         * @brief Swap contents with another IntegerDeque
         *
         * @param item The other deque to swap with
         */
        void swap(NaiveArrayForFasterPsum &item)
        {
            std::swap(this->circular_buffer_, item.circular_buffer_);
            //std::swap(this->circular_sum_buffer_, item.circular_sum_buffer_);
            std::swap(this->deque_size_, item.deque_size_);
            assert(this->verify());

        }

        /**
         * @brief Access element by index (const version)
         *
         * @param index The index of the element
         * @return T The value at the specified index
         */
        uint64_t operator[](uint64_t index) const
        {
            if(index == 0){
                return this->circular_buffer_[0];
            }else{
                return this->circular_buffer_[index] - this->circular_buffer_[index - 1];
            }
        }

        /**
         * @brief Set a value at a specific index
         *
         * @param index The index where to set the value
         * @param value The value to set
         */
        void set_value(uint64_t index, uint64_t value)
        {
            uint64_t old_value = this->at(index);
            for(uint64_t i = index; i < this->deque_size_; i++){
                this->circular_buffer_[i] = (this->circular_buffer_[i] - old_value) + value;
            }
            assert(this->verify());

        }

        /**
         * @brief Access element by index with bounds checking
         *
         * @param i The index of the element
         * @return T The value at the specified index
         */
        uint64_t at(uint64_t index) const
        {
            if(index == 0){
                return this->circular_buffer_[0];
            }else{
                return this->circular_buffer_[index] - this->circular_buffer_[index - 1];
            }
        }

        uint64_t psum(uint64_t i) const
        {
            return this->circular_buffer_[i];
        }
        int64_t search(uint64_t value) const
        {
            uint64_t _sum = 0;
            return this->search(value, _sum);

        }

        int64_t search(uint64_t value, uint64_t &sum) const
        {
            uint64_t _psum = this->psum();
            if(value > _psum){
                return -1;
            }else{
                std::array<uint8_t, SIZE> bits;
                for(uint64_t i = 0; i < SIZE; i++)
                {
                    bits[i] = this->circular_buffer_[i] < value;
                }
                int64_t p = std::accumulate(bits.begin(), bits.end(), 0);
                sum = p == 0 ? 0 : this->circular_buffer_[p-1];
                return p;
    
            }
        }
        uint64_t psum() const
        {
            if(this->deque_size_ == 0){
                return 0;
            }else{
                return this->circular_buffer_[this->deque_size_ - 1];
            }
        }
        bool verify() const{
            for(uint64_t i = 1; i < this->size(); i++){
                if(this->circular_buffer_[i] < this->circular_buffer_[i-1]){

                    std::cout << std::endl;

                    std::cout << "verify error: " << this->circular_buffer_[i] << " < " << this->circular_buffer_[i-1] << std::endl;

                    for(uint64_t j = 0; j < this->size(); j++){
                        std::cout << this->circular_buffer_[j] << " ";
                    }
                    std::cout << std::endl;

                    return false;
                }
            }

            return true;
        }

        void increment(uint64_t pos, int64_t delta)
        {
            uint64_t value = this->at(pos);
            this->set_value(pos, value + delta);
        }

        void decrement(uint64_t pos, int64_t delta)
        {
            uint64_t value = this->at(pos);
            this->set_value(pos, value - delta);
        }

        std::vector<uint64_t> to_vector() const
        {
            std::vector<uint64_t> r;
            if(this->deque_size_ > 0){
                r.push_back(this->circular_buffer_[0]);
            }
            for(uint64_t i = 1; i < this->deque_size_; i++){
                r.push_back(this->circular_buffer_[i] - this->circular_buffer_[i-1]);
            }
            return r;

        }
        uint64_t reverse_psum([[maybe_unused]] uint64_t i) const
        {
            throw std::runtime_error("reverse_psum is not supported for NaiveArrayForFasterPsum");
        }


        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if(only_extra_bytes){
                return 0;
            }else{
                return sizeof(NaiveArrayForFasterPsum<SIZE>);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return (SIZE - this->size()) * sizeof(uint64_t);
        }
    };
}
