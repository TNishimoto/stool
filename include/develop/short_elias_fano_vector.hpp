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
            this->clear();
        }
        ShortEliasFanoVector(const std::vector<uint64_t> &values)
        {
            this->build(values);
        }

        static uint64_t get_upper_value(uint64_t value, uint64_t bit_size, uint64_t upper_bit_size)
        {
            return value >> (bit_size - upper_bit_size);
        }
        static uint64_t get_lower_value(uint64_t value, uint64_t bit_size, uint64_t upper_bit_size)
        {
            uint64_t lower_bit_size = bit_size - upper_bit_size;
            uint64_t mask = UINT64_MAX >> (64 - lower_bit_size);
            return value & mask;
        }

        void clear(){
            this->sbv.clear();
            this->sbv.push_back(16, 0);
        }

        void swap(ShortEliasFanoVector &item){
            this->sbv.swap(item.sbv);
        }

        template <size_t N>
        void build(const std::array<uint64_t, N> &values, uint64_t size_of_array)
        {
            this->sbv.clear();
            uint64_t n = size_of_array;
            uint64_t size_bits = (n << 48);
            this->sbv.push_back(16, size_bits);

            uint64_t u = 0;
            for (uint64_t i = 0; i < n; i++)
            {
                if (values[i] > u)
                {
                    u = values[i];
                }
            }

            if (n > 0)
            {

                uint64_t bit_size = std::ceil(std::log2(u+2));
                uint64_t upper_bit_size = std::ceil(std::log2(n));

                uint64_t i = 0;
                uint64_t current_upper_value = 0;
                uint64_t current_upper_value_num = 0;

                if (n > 1)
                {

                    while (i < n)
                    {
                        uint64_t upper_value = get_upper_value(values[i], bit_size, upper_bit_size);
                        if (current_upper_value < upper_value)
                        {
                            for (uint64_t x = 1; x <= current_upper_value_num; x++)
                            {
                                this->sbv.push_back(true);
                            }
                            this->sbv.push_back(false);
                            current_upper_value++;
                            current_upper_value_num = 0;
                        }
                        else if (current_upper_value == upper_value)
                        {
                            current_upper_value_num++;
                            i++;
                        }
                        else
                        {
                            throw std::runtime_error("Error in ShortEliasFanoVector::build: The input sequence is not sorted");
                        }
                    }
                    if (current_upper_value_num > 0)
                    {
                        for (uint64_t x = 1; x <= current_upper_value_num; x++)
                        {
                            this->sbv.push_back(true);
                        }
                        current_upper_value++;
                        current_upper_value_num = 0;
                    }
                }

                uint64_t lower_bit_size = bit_size - upper_bit_size;
                for (uint64_t i = 0; i < n; i++)
                {
                    uint64_t lower_value = get_lower_value(values[i], bit_size, upper_bit_size);
                    uint64_t lower_value_bits = lower_value << (64 - lower_bit_size);

                    this->sbv.push_back(lower_bit_size, lower_value_bits);
 
                }
            }
        }
        size_t capacity() const{
            return this->sbv.capacity();
        }
        void reserve(size_t new_capacity){
            this->sbv.reserve(new_capacity);
        }
        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            return this->sbv.size_in_bytes(only_extra_bytes);
        }


        void print_color_bits() const
        {
            uint64_t starting_position_of_lower_value_bits = this->get_starting_position_of_lower_value_bits();
            std::string s = this->sbv.to_string();
            std::cout << "EF: " << std::flush;
            for (uint64_t i = 0; i < this->sbv.size(); i++)
            {
                if (i == 0)
                {
                    std::cout << "\033[41m";
                }
                else if (i == 16)
                {
                    std::cout << "\033[42m";
                }
                else if (i == starting_position_of_lower_value_bits)
                {
                    std::cout << "\033[43m";
                }
                std::cout << s[i] << std::flush;
            }
            std::cout << "\e[m" << std::endl;
        }

        void build(const std::vector<uint64_t> &values)
        {
            std::array<uint64_t, 4192> values_array;
            for (uint64_t i = 0; i < values.size(); i++)
            {
                values_array[i] = values[i];
            }
            this->build(values_array, values.size());
        }

        void erase(uint64_t nth)
        {
            std::array<uint64_t, 4192> values_array;
            uint64_t i = 0;
            for (auto it = this->begin(); it != this->end(); it++)
            {
                if (it.index != nth)
                {
                    values_array[i] = *it;
                    i++;
                }
            }


            this->build(values_array, i);

        }
        uint64_t insert(uint64_t value)
        {
            std::array<uint64_t, 4192> values_array;
            uint64_t i = 0;
            uint64_t nth = 0;

            // bool b = false;

            for (auto it = this->begin(); it != this->end(); it++)
            {
                uint64_t current_value = *it;
                bool is_insertion_position = value <= current_value && (i == 0 || value > values_array[i - 1]);
                if (is_insertion_position)
                {
                    values_array[i] = value;
                    nth = i;
                    i++;
                }
                values_array[i] = current_value;
                i++;
            }
            if (i == 0 || value > values_array[i - 1])
            {
                values_array[i] = value;
                nth = i;
                i++;
            }

            this->build(values_array, i);
            return nth;
        }

        uint64_t size() const
        {
            uint64_t m = this->sbv.copy_to(0, 16);

            return m >> 48;
        }
        std::vector<uint64_t> to_vector() const
        {
            std::vector<uint64_t> values;
            for (auto it = this->begin(); it != this->end(); it++)
            {
                values.push_back(*it);
            }
            return values;
        }

        uint64_t get_upepr_value_index(uint64_t i) const
        {
            uint64_t gap1 = this->sbv.rank1(15);
            uint64_t j = this->sbv.select1(gap1 + i);
            return j;
        }
        uint64_t get_upepr_value([[maybe_unused]] uint64_t i, uint64_t upper_value_index) const
        {
            uint64_t gap0 = this->sbv.rank0(15);
            uint64_t zero_count = this->sbv.rank0(upper_value_index) - gap0;
            return zero_count;
        }

        uint64_t get_upepr_value(uint64_t i) const
        {
            uint64_t idx = this->get_upepr_value_index(i);
            uint64_t x = this->get_upepr_value(i, idx);
            return x;
        }
        uint64_t get_starting_position_of_lower_value_bits() const
        {
            uint64_t size = this->size();
            uint64_t gap1 = this->sbv.rank1(15);

            if (size == 0)
            {
                return UINT64_MAX;
            }
            else if (size == 1)
            {
                return 16;
            }
            else
            {
                return this->sbv.select1(gap1 + size - 1) + 1;
            }
        }
        uint64_t get_lower_bit_size() const
        {
            uint64_t size = this->size();
            uint64_t starting_position = this->get_starting_position_of_lower_value_bits();
            uint64_t lower_bit_size = (this->sbv.size() - starting_position) / size;
            return lower_bit_size;
        }
        uint64_t get_lower_value(uint64_t i, uint64_t starting_position_of_lower_value_bits) const
        {
            uint64_t size = this->size();
            uint64_t lower_bit_size = (this->sbv.size() - starting_position_of_lower_value_bits) / size;
            uint64_t lower_value_bits = this->sbv.copy_to(starting_position_of_lower_value_bits + (i * lower_bit_size), lower_bit_size);

            uint64_t v = lower_value_bits >> (64 - lower_bit_size);

            return v;
        }

        uint64_t at(uint64_t i, uint16_t upper_value, uint16_t starting_position_of_lower_value_bits) const
        {
            uint64_t size = this->size();
            if (size == 1)
            {
                uint64_t lower_value = this->get_lower_value(i, starting_position_of_lower_value_bits);
                return lower_value;
            }
            else
            {
                uint64_t lower_bit_size = (this->sbv.size() - starting_position_of_lower_value_bits) / size;
                uint64_t lower_value = this->get_lower_value(i, starting_position_of_lower_value_bits);

                return (upper_value << lower_bit_size) | lower_value;
            }
        }

        uint64_t at(uint64_t i) const
        {

            uint64_t starting_position_of_lower_value_bits = this->get_starting_position_of_lower_value_bits();
            uint64_t upper_value = this->get_upepr_value(i);

            return this->at(i, upper_value, starting_position_of_lower_value_bits);
        }
        int64_t successor(uint64_t i) const{
            for(auto it = this->begin(); it != this->end(); it++){
                if(*it > i){
                    return it.index;
                }
            }
            return -1;
        }
        uint64_t unused_size_in_bytes() const
        {
            return this->sbv.unused_size_in_bytes();
        }        

        class iterator
        {

        public:
            const ShortEliasFanoVector *efs;
            uint16_t index = 0;
            uint16_t upper_value = 0;
            uint16_t upper_value_index = 0;
            uint16_t starting_position_of_lower_value_bits = 0;

            using iterator_category = std::forward_iterator_tag; ///< Iterator category
            using value_type = uint64_t;                         ///< Value type
            using difference_type = std::ptrdiff_t;              ///< Difference type

            iterator(const ShortEliasFanoVector *_efs, uint64_t _index) : efs(_efs), index(_index)
            {
                uint64_t size = this->efs->size();
                if (_index == size)
                {
                    this->index = UINT16_MAX;
                    this->upper_value = UINT16_MAX;
                    this->upper_value_index = UINT16_MAX;
                    this->starting_position_of_lower_value_bits = UINT16_MAX;
                }
                else
                {
                    this->index = _index;
                    this->upper_value_index = this->efs->get_upepr_value_index(_index);
                    this->upper_value = this->efs->get_upepr_value(_index, this->upper_value_index);
                    this->starting_position_of_lower_value_bits = this->efs->get_starting_position_of_lower_value_bits();
                }
            }

            uint64_t operator*() const
            {
                return this->efs->at(this->index, this->upper_value, this->starting_position_of_lower_value_bits);
            }

            iterator &operator++()
            {
                uint64_t size = this->efs->size();
                if (this->index < size)
                {
                    if ((uint64_t)(this->index + 1) == size)
                    {
                        this->index = UINT16_MAX;
                        this->upper_value = UINT16_MAX;
                        this->upper_value_index = UINT16_MAX;
                    }
                    else
                    {
                        uint64_t next_upper_value_index = this->efs->sbv.successor1(this->upper_value_index);

                        this->upper_value += next_upper_value_index - this->upper_value_index - 1;
                        this->upper_value_index = next_upper_value_index;

                        this->index++;
                        assert(this->upper_value == this->efs->get_upepr_value(this->index));
                    }
                }
                else
                {
                    throw std::runtime_error("iterator: index is out of range");
                }
                return *this;
            }

            iterator operator++(int)
            {
                iterator temp = *this;
                ++(*this);
                return temp;
            }

            bool operator==(const iterator &other) const { return index == other.index; }

            bool operator!=(const iterator &other) const { return index != other.index; }

            bool operator<(const iterator &other) const { return index < other.index; }

            bool operator>(const iterator &other) const { return index > other.index; }

            bool operator<=(const iterator &other) const { return index <= other.index; }
            bool operator>=(const iterator &other) const { return index >= other.index; }
        };

        iterator begin() const
        {
            return iterator(this, 0);
        }
        iterator end() const
        {
            return iterator(this, this->size());
        }
    };

} // namespace stool