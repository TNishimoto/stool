#pragma once
#include "./short_elias_fano_vector.hpp"

namespace stool
{

    class ShortIntegerVector
        {
            ShortEliasFanoVector ef;
            static constexpr uint64_t MAX_SEQUENCE_LENGTH = 300;

        public:
            ShortIntegerVector()
            {
            }
            ShortIntegerVector(std::vector<uint64_t> &_items)
            {
                std::array<uint64_t, 4096> array;
                uint64_t x = 0;
                uint64_t sum = 0;
                for(auto it: _items){
                    array[x++] = it + sum;
                    sum += it;
                }
                this->ef.build(array, x);

            }


            uint64_t size() const
            {
                return this->ef.size();
            }
            uint64_t size_in_bytes(bool only_extra_bytes = false) const
            {
                return this->ef.size_in_bytes(only_extra_bytes);
            }

            uint64_t at(uint64_t pos) const
            {
                if(pos > 0){
                    return this->ef.at(pos) - this->ef.at(pos-1);
                }else{
                    return this->ef.at(0);
                }
            }
            void print() const
            {
            }

            void clear()
            {
                this->ef.clear();
            }
            void swap(ShortIntegerVector &item)
            {
                this->ef.swap(item.ef);
            }
            static std::string name()
            {
                return "ShortIntegerVector";
            }
            uint64_t psum(uint64_t i) const noexcept
            {
                return this->ef.at(i);
            }
            uint64_t psum() const noexcept
            {
                if (this->size() == 0)
                {
                    return 0;
                }
                else
                {
                    return this->ef.at(this->size() - 1);
                }
            }
            int64_t search(uint64_t x) const noexcept
            {
                if(x == 0){
                    return this->size() > 0 ? 0 : -1;
                }else{
                    return this->ef.successor(x-1);
                }
            }

            uint64_t unused_size_in_bytes() const
            {
                return this->ef.unused_size_in_bytes();
            }        
            std::vector<uint64_t> to_value_vector() const
            {
                std::vector<uint64_t> r;
                uint64_t current_value = 0;
                for(auto it: this->ef){
                    r.push_back(it - current_value);
                    current_value = it;
                }
                return r;
            }

            std::string to_string() const
            {
                std::vector<uint64_t> r = this->to_value_vector();
                return stool::DebugPrinter::to_integer_string(r);
            }

            template<typename VEC>
            void to_values(VEC &output_vec) const{
                output_vec.clear();
                output_vec.resize(this->size());

                uint64_t current_value = 0;
                for(auto it: this->ef){
                    output_vec.push_back(it - current_value);
                    current_value = it;
                }
            }



            void insert(uint64_t pos, uint64_t value)
            {
                assert(this->size() < MAX_SEQUENCE_LENGTH);

                std::array<uint64_t, 4096> array;
                if(this->size() == 0){
                    array[0] = value;
                    this->ef.build(array, 1);
                }else{
                    std::array<uint8_t, 4096> upper_array;
                    std::array<uint64_t, 4096> lower_array;
                    uint64_t lower_bit_size = this->ef.get_lower_bit_size();
                    uint64_t size = this->size();
    
                    this->ef.decode_upper_bits(upper_array);
                    this->ef.decode_lower_bits(lower_array);
    
    
                    for(uint64_t i = 0; i < pos; i++){
                        array[i] = lower_array[i] | (upper_array[i] << lower_bit_size);

                        assert(array[i] == this->ef.at(i));
                    }
    
                    if(pos == 0){
                        array[pos] = value;
                    }else{
                        array[pos] = value + array[pos-1];
                    }
    
                    for(uint64_t i = pos; i < size; i++){
                        array[i+1] = ((lower_array[i] | (upper_array[i] << lower_bit_size))) + value;


                        assert(array[i+1] == this->ef.at(i) + value);

                    }
                    this->ef.build(array, size+1);

                }


                /*

                if(pos < this->size()){
                    uint64_t x = 0;
                    uint64_t current_value = 0;
                    for(auto it: this->ef){
                        if(x < pos){
                            array[x++] = it;
                            current_value = it;
                        }else if(x == pos){
                            array[x++] = value + current_value;
                            array[x++] = it + value;
                            current_value = it;
                        }else{
                            array[x++] = it + value;
                            current_value = it;
                        }
                    }
                    this->ef.build(array, x);
                }else{
                    uint64_t new_value = this->psum() + value;
                    this->ef.insert(new_value);
                }
                */
            }
            void remove(uint64_t pos)
            {
                std::array<uint64_t, 4096> array;

                uint64_t x = 0;
                uint64_t current_value = 0;
                uint64_t removed_value = UINT64_MAX;
                uint64_t p = 0;
                for(auto it: this->ef){
                    if(p < pos){
                        array[x++] = it;
                        current_value = it;
                    }else if(p == pos){
                        removed_value = it - current_value;
                        current_value = it;
                    }else{
                        array[x++] = it - removed_value;
                        current_value = it;
                    }
                    p++;
                }    
                this->ef.build(array, x);
            }
            void push_front(std::vector<uint64_t> &new_items)
            {
                assert(this->size() < MAX_SEQUENCE_LENGTH);

                std::array<uint64_t, 4096> array;

                uint64_t x = 0;
                uint64_t sum = 0;
                for(auto it: new_items){
                    array[x++] = it + sum;
                    sum += it;
                }    
                for(auto it: this->ef){
                    array[x++] = it + sum;
                } 

                this->ef.build(array, x);
            }
            void push_front(uint64_t new_item)
            {
                assert(this->size() < MAX_SEQUENCE_LENGTH);

                std::array<uint64_t, 4096> array;

                uint64_t x = 0;
                uint64_t sum = 0;
                array[x++] = new_item;
                sum += new_item;

                for(auto it: this->ef){
                    array[x++] = it + sum;
                } 

                this->ef.build(array, x);
            }

            void push_back(std::vector<uint64_t> &new_items)
            {
                assert(this->size() < MAX_SEQUENCE_LENGTH);

                std::array<uint64_t, 4096> array;
                uint64_t last_value = 0;

                uint64_t x = 0;
                for(auto it: this->ef){
                    array[x++] = it;
                    last_value = it;
                } 

                for (uint64_t v : new_items)
                {
                    array[x++] = v + last_value;
                    last_value = v + last_value;
                }

                this->ef.build(array, x);
            }
            void push_back(uint64_t value)
            {
                assert(this->size() < MAX_SEQUENCE_LENGTH);

                std::array<uint64_t, 4096> array;
                uint64_t last_value = 0;

                uint64_t x = 0;
                for(auto it: this->ef){
                    array[x++] = it;
                    last_value = it;
                } 
                array[x++] = value + last_value;
                this->ef.build(array, x);
            }
            std::vector<uint64_t> pop_front(uint64_t len)
            {

                std::vector<uint64_t> tmp1;
                tmp1.resize(len);

                std::array<uint64_t, 4096> array;

                uint64_t x = 0;
                uint64_t idx = 0;
                uint64_t removed_value_sum = 0;
                uint64_t current_value = 0;
                for(auto it: this->ef){
                    if(idx < len){
                        removed_value_sum = it;
                        tmp1[idx] = it -current_value;
                        current_value = it;
                    }else{
                        array[x++] = it - removed_value_sum;                        
                    }
                    idx++;
                } 
                this->ef.build(array, x);
                return tmp1;
            }
            std::vector<uint64_t> pop_back(uint64_t len)
            {

                std::vector<uint64_t> tmp1;
                tmp1.resize(len);

                std::array<uint64_t, 4096> array;
                uint64_t x = 0;
                uint64_t idx = 0;
                uint64_t current_value = 0;
                uint64_t xlen = this->size() - len;
                uint64_t tmp_idx = 0;

                for(auto it: this->ef){
                    if(idx < xlen){
                        array[x++] = it;                        
                        current_value = it;
                    }else{
                        tmp1[tmp_idx++] = it - current_value;
                        current_value = it;                        
                    }
                    idx++;
                }
                this->ef.build(array, x);
                assert(tmp_idx == len);


                return tmp1;
            }

            uint64_t reverse_psum(uint64_t i) const
            {
                uint64_t size = this->size();
                if(i + 1 == size){
                    return this->psum();
                }else{
                    return this->psum() - this->psum(size - i - 2);
                }
            }

            uint64_t psum(uint64_t i, uint64_t j) const
            {                
                if (i == j)
                {
                    return this->at(i);
                }
                else
                {
                    if(i > 0){
                        return this->psum(j) - this->psum(i-1);
                    }else{
                        return this->psum(j);
                    }
                }
            }

            void increment(uint64_t i, int64_t delta)
            {
                std::array<uint64_t, 4096> array;
                uint64_t x = 0;
                
                for(auto it: this->ef){
                    if(x < i){
                        array[x++] = it;                        
                    }else{
                        array[x++] = it + delta;                        
                    }
                }
                this->ef.build(array, x);
            }

            /*

            std::vector<uint64_t>::const_iterator begin() const {
                return this->items.begin();
            }

            std::vector<uint64_t>::const_iterator end() const {
                return this->items.end();
            }
            

            int64_t rank(uint64_t i, uint64_t c) const {
                return stool::StringFunctions::rank_query(this->items, i, c);
            }
            int64_t rank0(uint64_t i) const {
                return this->rank(0, i);
            }
            int64_t rank1(uint64_t i) const {
                return this->rank(1, i);
            }

            int64_t select(uint64_t i, uint64_t c) const {
                return stool::StringFunctions::select_query(this->items, i, c);
            }
            int64_t select0(uint64_t i) const {
                return this->select(0, i);
            }
            int64_t select1(uint64_t i) const {
                return this->select(1, i);
            }
            */


        };

} // namespace stool