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
    class EytzingerLayoutForPsum
    {
    public:
    protected:
        std::array<uint64_t, (SIZE * 2) - 1> eytzinger_layout_;
        uint64_t deque_size_ = 0;

        static constexpr unsigned log2_pow2(uint64_t x) {
            return __builtin_ctzll(x); // x は 2^k なので、末尾の0ビット数が k
        }

    public:
        uint64_t size_in_bytes() const
        {
            return sizeof(EytzingerLayoutForPsum);
        }
        inline static uint64_t DUMMY_VALUE = UINT64_MAX;

        inline static uint64_t HEIGHT = log2_pow2(SIZE) + 1;

        EytzingerLayoutForPsum(const std::vector<uint64_t> &items)
        {
            this->clear();
            this->initialize(items);
        }
        /**
         * @brief Default constructor
         *
         * Initializes an empty deque with default capacity
         */
        EytzingerLayoutForPsum()
        {
            this->clear();
        }
        /**
         * @brief Destructor
         *
         * Frees the allocated memory
         */
        ~EytzingerLayoutForPsum()
        {
            this->clear();
        }

        /**
         * @brief Get the current capacity of the deque
         *
         * @return size_t The number of elements that can be stored
         */
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
            assert(this->verify());
        }
        void initialize(const std::vector<uint64_t> &items)
        {
            std::array<uint64_t, SIZE> tmp;
            uint64_t _size = items.size();
            for(uint64_t i = 0; i < _size;i++){
                tmp[i] = items[i];
            }
            for(uint64_t i = _size; i < SIZE;i++){
                tmp[i] = DUMMY_VALUE;
            }


            std::array<uint64_t, (SIZE*2) - 1> tmp_array = rebuild_layout(tmp);
            this->eytzinger_layout_ = tmp_array;
            this->deque_size_ = items.size();


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

        static EytzingerLayoutForPsum build(const std::vector<uint64_t> &items)
        {
            EytzingerLayoutForPsum deque(items);
            return deque;
        }
        static std::array<uint64_t, (SIZE * 2) - 1> rebuild_layout(std::array<uint64_t, SIZE> &items)
        {
            std::array<uint64_t, (SIZE * 2) - 1> layout;

            uint64_t first_leaf_position = get_first_level_node_position_on_layout(HEIGHT - 1);
            for(uint64_t i = 0; i < SIZE; i++){
                layout[first_leaf_position + i] = items[i];
            }
            for(int64_t h = (int64_t)HEIGHT - 2; h >= 0; h--){
                uint64_t first_node_position = get_first_level_node_position_on_layout(h);
                uint64_t node_count = count_level_nodes(h);

                for(uint64_t i = 0; i < node_count;i++){
                    uint64_t node_position = first_node_position + i;
                    uint64_t left_child_position = ((node_position + 1) * 2) - 1;
                    uint64_t right_child_position = ((node_position + 1) * 2);
                    uint64_t sum = (layout[left_child_position] == DUMMY_VALUE ? 0 : layout[left_child_position]) + (layout[right_child_position] == DUMMY_VALUE ? 0 : layout[right_child_position]); 
                    layout[node_position] = sum;
                }
            }
            return layout;
        }

        
        static uint64_t get_leaf_position_on_layout(uint64_t leaf_index){
            return ((1ULL << (HEIGHT - 1) ) - 1) + leaf_index;
        }
        static uint64_t get_internal_node_position_on_layout(uint64_t leaf_index, uint64_t h){
            return ((1ULL << h) - 1) + (leaf_index >> (HEIGHT - h - 1));
        }

        static uint64_t get_first_level_node_position_on_layout(uint64_t h){
            return (1ULL << h) - 1;
        }
        static uint64_t count_level_nodes(uint64_t h){
            return (1ULL << (h+1)) - (1ULL << h);
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


            uint64_t leaf_index_on_layout = this->get_leaf_position_on_layout(this->deque_size_);
            this->eytzinger_layout_[leaf_index_on_layout] = value;
            
            for(int64_t h = HEIGHT - 2; h >= 0; h--){
                uint64_t internal_node_pos = this->get_internal_node_position_on_layout(this->deque_size_, h);
                this->eytzinger_layout_[internal_node_pos] += value;
            }
            this->deque_size_++;


            

            assert(this->verify());
        }

        template <typename ARRAY_TYPE = std::vector<uint64_t>>
        void push_back_many(ARRAY_TYPE values)
        {
            uint64_t size = values.size();
            for (uint64_t i = 0; i < size; i++)
            {
                this->push_back(values[i]);
            }
        }

        /**
         * @brief Add an element to the front of the deque
         *
         * @param value The value to add
         */
        void push_front(uint64_t value)
        {
            uint64_t first_leaf_position = get_first_level_node_position_on_layout(HEIGHT - 1);
            std::array<uint64_t, SIZE> tmp;
            tmp[0] = value;
            
            for(uint64_t i = 1; i < SIZE;i++){
                tmp[i] = this->eytzinger_layout_[first_leaf_position + i - 1];
            }

            std::array<uint64_t, (SIZE * 2) - 1> new_layout = rebuild_layout(tmp);
            this->eytzinger_layout_ = new_layout;
            this->deque_size_++;            
        }

        template <typename ARRAY_TYPE = std::vector<uint64_t>>
        void push_front_many(ARRAY_TYPE values)
        {
            uint64_t first_leaf_position = get_first_level_node_position_on_layout(HEIGHT - 1);
            std::array<uint64_t, SIZE> tmp;
            uint64_t new_value_count = values.size();
            for(uint64_t i = 0; i < new_value_count;i++){
                tmp[i] = values[i];
            }
            
            for(uint64_t i = new_value_count; i < SIZE;i++){
                tmp[i] = this->eytzinger_layout_[first_leaf_position + i - new_value_count];
            }

            std::array<uint64_t, (SIZE * 2) - 1> new_layout = rebuild_layout(tmp);
            this->eytzinger_layout_ = new_layout;
            this->deque_size_+= new_value_count;     
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

            uint64_t leaf_index_on_layout = this->get_leaf_position_on_layout(this->deque_size_-1);
            uint64_t removed_value = this->eytzinger_layout_[leaf_index_on_layout];
            this->eytzinger_layout_[leaf_index_on_layout] = DUMMY_VALUE;
            
            for(int64_t h = HEIGHT - 2; h >= 0; h--){
                uint64_t internal_node_pos = this->get_internal_node_position_on_layout(this->deque_size_-1, h);
                this->eytzinger_layout_[internal_node_pos] -= removed_value;
            }
            this->deque_size_--;            

            assert(this->verify());

        }

        void pop_back_many(uint64_t len)
        {
            for (uint64_t i = 0; i < len; i++)
            {
                this->pop_back();
            }
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

            uint64_t first_leaf_position = get_first_level_node_position_on_layout(HEIGHT - 1);
            std::array<uint64_t, SIZE> tmp;
            tmp[SIZE-1] = DUMMY_VALUE;
            
            for(uint64_t i = 1; i < SIZE;i++){
                tmp[i-1] = this->eytzinger_layout_[first_leaf_position + i];
            }

            std::array<uint64_t, (SIZE * 2) - 1> new_layout = rebuild_layout(tmp);
            this->eytzinger_layout_ = new_layout;
            this->deque_size_--;            

            assert(this->verify());
        }

        void pop_front_many(uint64_t len)
        {
            if (this->deque_size_ < len)
            {
                throw std::out_of_range("pop_front, Size out of range");
            }

            uint64_t first_leaf_position = get_first_level_node_position_on_layout(HEIGHT - 1);
            std::array<uint64_t, SIZE> tmp;
            for(uint64_t i = 0; i < len;i++){
                tmp[SIZE - i - 1] = DUMMY_VALUE;
            }
            
            for(uint64_t i = len; i < SIZE;i++){
                tmp[i-len] = this->eytzinger_layout_[first_leaf_position + i];
            }

            std::array<uint64_t, (SIZE * 2) - 1> new_layout = rebuild_layout(tmp);
            this->eytzinger_layout_ = new_layout;
            this->deque_size_-= len;            

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

            if (size >= SIZE)
            {
                throw std::out_of_range("insert, Size out of range");
            }

            if (position > this->deque_size_ + 1)
            {
                throw std::out_of_range("Position out of range");
            }

            if (position == 0)
            {
                this->push_front(value);
                assert(this->at(position) == value);
            }
            else if (position == size)
            {
                this->push_back(value);
                assert(this->at(position) == value);
            }
            else
            {
                uint64_t first_leaf_position = get_first_level_node_position_on_layout(HEIGHT - 1);
                std::array<uint64_t, SIZE> tmp;
                for(uint64_t x = 0; x < position;x++){
                    tmp[x] = this->eytzinger_layout_[first_leaf_position + x];
                }
                tmp[position] = value;
                for(uint64_t x = position; x < SIZE - 1;x++){
                    tmp[x+1] = this->eytzinger_layout_[first_leaf_position + x];
                }


                std::array<uint64_t, (SIZE * 2) - 1> new_layout = rebuild_layout(tmp);
                this->eytzinger_layout_ = new_layout;
                this->deque_size_++;        
            }
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

                uint64_t first_leaf_position = get_first_level_node_position_on_layout(HEIGHT - 1);
                std::array<uint64_t, SIZE> tmp;
                for(uint64_t x = 0; x < position;x++){
                    tmp[x] = this->eytzinger_layout_[first_leaf_position + x];
                }
                for(uint64_t x = position+1; x < SIZE;x++){
                    tmp[x-1] = this->eytzinger_layout_[first_leaf_position + x];
                }
                tmp[SIZE-1] = DUMMY_VALUE;

                std::array<uint64_t, (SIZE * 2) - 1> new_layout = rebuild_layout(tmp);
                this->eytzinger_layout_ = new_layout;
                this->deque_size_--;        
            }
            assert(this->verify());
        }

        uint64_t value_capacity() const
        {
            return UINT64_MAX;
        }

        /**
         * @brief Print debug information about the deque
         */
        void print_info() const
        {
            std::cout << "deque_size = " << this->deque_size_ << std::endl;
            stool::DebugPrinter::print_integers(this->eytzinger_layout_, "EYTZINGER_LAYOUT");

            std::vector<std::string> layout_strings;
            for(uint64_t i = 0; i < HEIGHT;i++){
                layout_strings.push_back("");
            }
            for(uint64_t i = 0; i< SIZE;i++){
                uint64_t p = this->at(i);

                //uint64_t first_leaf_position = get_first_level_node_position_on_layout(HEIGHT - 1);
                if(p != DUMMY_VALUE){
                    std::string s = std::to_string(p);
                    layout_strings[HEIGHT-1] += s;    
                }else{
                    layout_strings[HEIGHT-1] += "_";    
                }
                layout_strings[HEIGHT-1] += " | ";
            }
            
            for(int64_t h = HEIGHT - 2; h >= 0; h--){
                uint64_t count_node_count = count_level_nodes(h);

                std::queue<uint64_t> position_queue;
                for(uint64_t x = 0; x < layout_strings[h+1].size();x++){
                    if(layout_strings[h+1][x] == '|'){
                        position_queue.push(x);
                    }
                }
                
                for(uint64_t i = 0; i < count_node_count;i++){
                    
                    uint64_t node_position = get_first_level_node_position_on_layout(h) + i;
                    std::string s = std::to_string(this->eytzinger_layout_[node_position]);
                    layout_strings[h] += s;

                    position_queue.pop();
                    uint64_t next_stop_position = position_queue.front();
                    
                    while(layout_strings[h].size() < next_stop_position){
                        layout_strings[h] += " ";
                    }
                    position_queue.pop();
                    
                    
                    layout_strings[h] += "| ";
                }
                
            }
            
            for(uint64_t h = 0; h < HEIGHT;h++){
                std::cout << layout_strings[h] << std::endl;
            }
            std::cout << std::endl;
        }
        std::string to_string() const
        {
            std::string r = "[";
            for (uint64_t i = 0; i < this->deque_size_; i++)
            {
                r += std::to_string(this->at(i));
                if (i < this->deque_size_ - 1)
                {
                    r += ", ";
                }
            }
            r += "]";
            return r;
        }

        /**
         * @brief Reserve space for a specific capacity and byte size
         *
         * @param capacity_bit_size The bit size for the capacity
         * @param byte_size The byte size for storing values
         */
        /*
        void reserve(size_t capacity_bit_size, uint64_t byte_size)
        {
            this->shrink_to_fit(capacity_bit_size, byte_size);
        }
        */

        /**
         * @brief Swap contents with another IntegerDeque
         *
         * @param item The other deque to swap with
         */
        void swap(EytzingerLayoutForPsum &item)
        {            
            std::swap(this->eytzinger_layout_, item.eytzinger_layout_);
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
            return this->at(index);
        }

        /**
         * @brief Set a value at a specific index
         *
         * @param index The index where to set the value
         * @param value The value to set
         */
        void set_value(uint64_t index, uint64_t value)
        {
            uint64_t leaf_index_on_layout = this->get_leaf_position_on_layout(index);
            uint64_t old_value = this->eytzinger_layout_[leaf_index_on_layout];
            this->eytzinger_layout_[leaf_index_on_layout] = value;
            
            for(int64_t h = HEIGHT - 2; h >= 0; h--){
                uint64_t internal_node_pos = this->get_internal_node_position_on_layout(index, h);
                this->eytzinger_layout_[internal_node_pos] = this->eytzinger_layout_[internal_node_pos] - old_value + value;
            }
        }

        /**
         * @brief Access element by index with bounds checking
         *
         * @param i The index of the element
         * @return T The value at the specified index
         */
        uint64_t at(uint64_t index) const
        {
            uint64_t leaf_index_on_layout = this->get_leaf_position_on_layout(index);            
            return this->eytzinger_layout_[leaf_index_on_layout];
        }

        uint64_t psum(uint64_t i) const
        {
            uint64_t sum = 0;
            uint64_t leaf_index_on_layout = this->get_leaf_position_on_layout(i);


            for(int64_t h = 0; h < (int64_t)HEIGHT; h++){
                uint64_t internal_node_pos = this->get_internal_node_position_on_layout(i, h);
                uint64_t fst_pos = this->get_first_level_node_position_on_layout(h);
                sum += ((internal_node_pos - fst_pos) & 1ULL) ? this->eytzinger_layout_[internal_node_pos-1] : 0;
            }
            return sum + this->eytzinger_layout_[leaf_index_on_layout];
        }
        int64_t search(uint64_t value) const
        {
            uint64_t sum = 0;
            return this->search(value, sum);
        }

        int64_t search(uint64_t value, uint64_t &sum) const
        {
            assert(this->verify());

            uint64_t total = this->psum();
            uint64_t size = this->size();

            if (value > total || size == 0)
            {
                return -1;
            }
            else
            {

                sum = 0;
                uint64_t pos = 0;
                uint64_t tmp_value = value;
                for(int64_t h = 0; h < (int64_t)(HEIGHT - 1); h++){
                    uint64_t left_child_pos = ((pos + 1) * 2) - 1;
                    uint64_t left_value = this->eytzinger_layout_[left_child_pos];
                    if(left_value >= tmp_value){
                        pos = left_child_pos;
                    }else{
                        pos = left_child_pos+1;
                        sum += left_value;
                        tmp_value -= left_value;
                    }
                }
                return pos - get_first_level_node_position_on_layout(HEIGHT - 1);
            }
        }
        uint64_t psum() const
        {
            return this->eytzinger_layout_[0];
        }
        bool verify() const
        {
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
            for (uint64_t i = 0; i < this->size(); i++)
            {
                r.push_back(this->at(i));
            }
            return r;
        }
        uint64_t reverse_psum([[maybe_unused]] uint64_t i) const
        {
            throw std::runtime_error("reverse_psum is not supported for NaiveIntegerArray");
        }

        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if (only_extra_bytes)
            {
                return 0;
            }
            else
            {
                return sizeof(EytzingerLayoutForPsum<SIZE>);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return ((SIZE - this->size()) * sizeof(uint64_t)) * 2;
        }
    };
}
