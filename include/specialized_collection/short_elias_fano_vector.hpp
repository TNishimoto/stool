#pragma once
#include "./short_bit_vector.hpp"

namespace stool
{

    class ShortEliasFanoVector
    {

    private:
        stool::ShortBitVector sbv; 

    public:
        ShortEliasFanoVector()
        {
            
        }
        ShortEliasFanoVector(const std::vector<uint64_t> &values)
        {
            this->build(values);
        }

        static uint64_t get_upper_value(uint64_t value, uint64_t bit_size, uint64_t upper_bit_size){
            return value >> (bit_size - upper_bit_size);
        }
        static uint64_t get_lower_value(uint64_t value, uint64_t bit_size, uint64_t upper_bit_size){
            uint64_t lower_bit_size = bit_size - upper_bit_size;
            uint64_t mask = UINT64_MAX >> (64-lower_bit_size);
            return value & mask;
        }

        void build(const std::vector<uint64_t> &values)
        {
            this->sbv.clear();
            uint64_t n = values.size();
            uint64_t size_bits = (n << 48);
            this->sbv.push_back(16, size_bits);

            uint64_t u = 0;
            if(n > 0){
                u = *std::max_element(values.begin(), values.end());

                uint64_t bit_size = std::ceil(std::log2(u));
                uint64_t upper_bit_size = std::ceil(std::log2(n));

                uint64_t i = 0;
                uint64_t current_upper_value = 0;
                uint64_t current_upper_value_num = 0;

                while(i < n){
                    uint64_t upper_value = get_upper_value(values[i], bit_size, upper_bit_size);
                    if(current_upper_value < upper_value){
                        for(uint64_t x = 1; x <= current_upper_value_num;x++){
                            this->sbv.push_back(true);
                        }
                        this->sbv.push_back(false);
                        current_upper_value++;
                        current_upper_value_num = 0;
                    }else if(current_upper_value == upper_value){
                        current_upper_value_num++;
                        i++;
                    }else{
                        throw std::runtime_error("Invalid value");
                    }
                }
                if(current_upper_value_num > 0){
                    for(uint64_t x = 1; x <= current_upper_value_num;x++){
                        this->sbv.push_back(true);
                    }
                    this->sbv.push_back(false);
                    current_upper_value++;
                    current_upper_value_num = 0;
                }

                uint64_t lower_bit_size = bit_size - upper_bit_size;
                for(uint64_t i = 0; i < n;i++){
                    uint64_t lower_value = get_lower_value(values[i], bit_size, upper_bit_size);
                    uint64_t lower_value_bits = lower_value << (64 - lower_bit_size);
                    this->sbv.push_back(lower_bit_size, lower_value_bits);
                }
            }
        }

        uint64_t size() const{
            uint64_t m = this->sbv.copy_to(0, 16);
            return m >> 48;
        }

        uint64_t get_upepr_value(uint64_t i) const{
            uint64_t gap0 = this->sbv.rank0(15);
            uint64_t gap1 = this->sbv.rank1(15);

            uint64_t j = this->sbv.select1(gap1 + 1 + i);
            uint64_t zero_count = this->sbv.rank0(j) - gap0;
            return zero_count;
        }
        uint64_t get_starting_position_of_lower_value_bits() const {
            uint64_t size = this->size();
            uint64_t gap1 = this->sbv.rank1(15);

            return this->sbv.select1(gap1 + size) + 1;
        }
        uint64_t get_upper_bit_size() const{
            uint64_t size = this->size();
            uint64_t starting_position = this->get_starting_position_of_lower_value_bits();
            uint64_t lower_bit_size = (this->sbv.size() - starting_position) / size;
            return lower_bit_size;
        }
        uint64_t get_lower_value(uint64_t i, uint64_t starting_position_of_lower_value_bits) const{
            uint64_t size = this->size();
            uint64_t lower_bit_size = (this->sbv.size() - starting_position_of_lower_value_bits) / size;
            uint64_t lower_value_bits = this->sbv.copy_to(starting_position_of_lower_value_bits + (i * lower_bit_size), lower_bit_size);
            return lower_value_bits >> (64 - lower_bit_size);
        }
        uint64_t at(uint64_t i) const{
            uint64_t size = this->size();
            uint64_t starting_position_of_lower_value_bits = this->get_starting_position_of_lower_value_bits();
            uint64_t lower_bit_size = (this->sbv.size() - starting_position_of_lower_value_bits) / size;

            uint64_t upper_value = this->get_upepr_value(i);
            uint64_t lower_value = this->get_lower_value(i, starting_position_of_lower_value_bits);
            return (upper_value << lower_bit_size) | lower_value;
        }



    };

} // namespace stool