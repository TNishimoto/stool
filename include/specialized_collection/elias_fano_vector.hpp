#pragma once
#include "./elias_fano_vector_builder.hpp"

namespace stool
{

    class EliasFanoVector
    {
    public:
        class iterator
        {
        private:
            uint64_t index = 0;
            const EliasFanoVector *efs;

        public:
            using difference_type = uint64_t;
            using value_type = uint64_t;
            using pointer = uint64_t *;
            using reference = uint64_t &;
            using iterator_category = std::random_access_iterator_tag;
            //using value_type = uint64_t;

            iterator(const EliasFanoVector *_efs, uint64_t _index) : index(_index), efs(_efs)
            {
            }

            iterator &operator++()
            {
                this->index++;
                return *this;
            }
            uint64_t operator*()
            {
                return efs->access(this->index);
            }
            bool operator!=(const iterator &rhs)
            {
                return (index != rhs.index);
            }
            bool operator==(const iterator &rhs)
            {
                return (index == rhs.index);
            }

            iterator &operator+=(int64_t p)
            {
                this->index += p;
                return *this;
            }
            iterator operator+(int64_t p)
            {
                iterator r(this->efs, this->index + p);
                return r;
            }

            bool operator<(const iterator &rhs)
            {
                return (index < rhs.index);
            }
            bool operator>(const iterator &rhs)
            {
                return (index > rhs.index);
            }
            bool operator<=(const iterator &rhs)
            {
                return (index <= rhs.index);
            }
            bool operator>=(const iterator &rhs)
            {
                return (index >= rhs.index);
            }
            uint64_t operator-(const iterator &rhs)
            {
                if (this->index < rhs.index)
                {
                    return rhs.index - this->index;
                }
                else
                {
                    return this->index - rhs.index;
                }
            }
            iterator &operator--()
            {
                throw std::logic_error("Error(EliasFanoVector:--): This function is not supported.");
                this->index--;                
                return *this;
            }

        };

    private:
        uint64_t _size = 0;
        //stool::ValueArray lower_bits;
        sdsl::int_vector<> lower_bits;
        sdsl::bit_vector upper_bits;
        sdsl::bit_vector::select_1_type upper_selecter;
        sdsl::bit_vector::select_0_type upper_0selecter;

        uint8_t upper_bit_size;
        uint8_t lower_bit_size;
        uint64_t max_value = 0;


        std::pair<uint64_t, uint64_t> get_upper_and_lower_bits(uint64_t value) const
        {
            uint64_t upper = value >> this->lower_bit_size;
            uint64_t upper_filter = 64 - this->lower_bit_size;
            uint64_t lower = (value << upper_filter) >> upper_filter;
            return std::pair<uint64_t, uint64_t>(upper, lower);
        }

        uint64_t recover(uint64_t lower, uint64_t upper) const
        {
            return (upper << lower_bit_size) | lower;
        }

    public:
        using value_type = uint64_t;
        using const_iterator = iterator;

        EliasFanoVector()
        {
        }

        EliasFanoVector(EliasFanoVector &&obj) : lower_bits(std::move(obj.lower_bits))
        {

            this->_size = obj._size;
            this->upper_bit_size = obj.upper_bit_size;
            this->lower_bit_size = obj.lower_bit_size;
            this->max_value = obj.max_value;
            this->upper_bits.swap(obj.upper_bits);
            this->upper_selecter.set_vector(&this->upper_bits);
            this->upper_selecter.swap(obj.upper_selecter);

            this->upper_0selecter.set_vector(&this->upper_bits);
            this->upper_0selecter.swap(obj.upper_0selecter);
        }
        void swap(EliasFanoVector &obj)
        {
            std::swap(this->_size, obj._size);
            std::swap(this->upper_bit_size, obj.upper_bit_size);
            std::swap(this->lower_bit_size, obj.lower_bit_size);
            std::swap(this->max_value, obj.max_value);
            this->lower_bits.swap(obj.lower_bits);
            this->upper_bits.swap(obj.upper_bits);

            
            this->upper_selecter.set_vector(&this->upper_bits);
            this->upper_selecter.swap(obj.upper_selecter);
            

            
            this->upper_0selecter.set_vector(&this->upper_bits);
            this->upper_0selecter.swap(obj.upper_0selecter);

            
        }
        void build_from_builder(EliasFanoVectorBuilder &builder)
        {
            if (!builder.finished)
            {
                std::cout << "ERROR" << std::endl;
                throw -1;
            }
            this->upper_bit_size = builder.upper_bit_size;
            this->lower_bit_size = builder.lower_bit_size;
            this->_size = builder._size;
            this->max_value = builder.max_value;

            sdsl::bit_vector b(builder.upper_bits.size(), 0);
            for (uint64_t i = 0; i < builder.upper_bits.size(); i++)
            {
                b[i] = builder.upper_bits[i] ? 1 : 0;
            }

#if DEBUG
            uint64_t one_count = 0;
            for (uint64_t i = 0; i < builder.upper_bits.size(); i++)
            {
                one_count += builder.upper_bits[i];
            }
            assert(one_count == _size);
#endif

            upper_bits.swap(b);

            sdsl::bit_vector::select_1_type b_sel(&upper_bits);
            upper_selecter.set_vector(&upper_bits);
            upper_selecter.swap(b_sel);

            sdsl::bit_vector::select_0_type b0_sel(&upper_bits);
            upper_0selecter.set_vector(&upper_bits);
            upper_0selecter.swap(b0_sel);

            this->lower_bits.swap(builder.lower_bits);
            /*
#if DEBUG
            uint64_t k = 0;
            for (uint64_t x = 0; x < this->_size; x++)
            {
                k += this->upper_selecter(x + 1);
            }
#endif
*/
        }
        template <typename VEC = std::vector<uint64_t>>
        void construct(VEC *seq)
        {
            uint64_t _max_value = 0;
            for (auto it : *seq)
            {
                if (_max_value < it)
                    _max_value = it;
            }

            EliasFanoVectorBuilder builder;
            builder.initialize(_max_value, seq->size());

            for (auto it : *seq)
            {
                builder.push(it);
            }
            builder.finish();
            this->build_from_builder(builder);

        }
        void build_from_bit_vector(const std::vector<bool> &seq)
        {
            uint64_t _max_value = 0;
            uint64_t _element_num = 0;
            for (auto it : seq)
            {
                if (it)
                {
                    _element_num++;
                }
                else
                {
                    _max_value++;
                }
            }
            EliasFanoVectorBuilder builder;
            builder.initialize(_max_value, _element_num);
            for (auto it : seq)
            {
                builder.push_bit(it);
            }
            builder.finish();
            this->build_from_builder(builder);

            /*
            uint64_t zero_counter = 0;
            uint64_t max_value = 0;
            uint64_t item_count = 0;
            for (size_t i = 0; i < seq.size(); i++)
            {
                if (seq[i])
                {
                    uint64_t value = zero_counter;
                    item_count++;
                    if (max_value < value)
                        max_value = value;
                }
                else
                {
                    zero_counter++;
                }
            }
            _size = item_count;
            this->upper_bit_size = std::log2(item_count) + 1;
            this->lower_bit_size = (std::log2(max_value) + 1) - upper_bit_size;

            this->build_upper_data_structure_from_bit_vector(seq);
            this->build_lower_data_structure_from_bit_vector(seq, item_count);
            */
        }

        uint64_t access(uint64_t i) const
        {
            assert(i < this->_size);

            uint64_t upper = (upper_selecter(i + 1) - i);

            if (lower_bit_size > 0)
            {
                uint64_t lower = lower_bits[i];

                return (upper << lower_bit_size) | lower;
            }
            else
            {
                return upper;
            }
        }
        /*
        Return the number of 1 in T[0..value-1]
        */
        uint64_t rank(uint64_t value) const
        {
            auto min_value = this->access(0);
            if (value > min_value)
            {
                std::pair<uint64_t, uint64_t> ul = get_upper_and_lower_bits(value);

                uint64_t l = ul.first == 0 ? 0 : upper_0selecter(ul.first) - (ul.first - 1);
                uint64_t lpos = l == 0 ? 0 : (l - 1);
                uint64_t r = upper_0selecter(ul.first + 1) - (ul.first);
                uint64_t rpos = r == 0 ? 0 : (r - 1);

                if (rpos - lpos < 16)
                {
                    for (uint64_t i = lpos; i <= rpos; i++)
                    {
                        if (this->access(i) >= value)
                        {
                            return i;
                        }
                    }
                    return rpos + 1;
                }
                else
                {
                    auto beg_poiner = this->begin() + lpos;
                    auto end_pointer = this->begin() + (rpos + 1);
                    stool::EliasFanoVector::iterator p2 = std::lower_bound(beg_poiner, end_pointer, value);
                    //stool:EliasFanoVector::iterator p2;

                    uint64_t pos2 = std::distance(this->begin(), p2);

                    return pos2;
                }
            }
            else
            {
                return 0;
            }
        }

        template <typename VEC = std::vector<uint64_t>>
        void check(const VEC &seq)
        {
            for (uint64_t i = 0; i < seq.size(); i++)
            {
                uint64_t p = this->access(i);
                if (seq[i] != p)
                {
                    throw std::logic_error("Something error!");
                }
            }
            std::cout << "OK!" << std::endl;
        }
        uint64_t operator[](uint64_t i) const
        {
            return this->access(i);
        }
        std::vector<uint64_t> to_vector()
        {
            std::vector<uint64_t> r;
            for (uint64_t i = 0; i < _size; i++)
            {
                r.push_back(this->access(i));
            }

            return r;
        }
        uint64_t size() const
        {
            return this->_size;
        }

        iterator begin() const
        {
            auto p = iterator(this, 0);
            return p;
        }
        iterator end() const
        {
            auto p = iterator(this, _size);
            return p;
        }

        uint64_t get_using_memory() const
        {
            return sdsl::size_in_bytes(lower_bits) + (this->upper_bits.size() / 8) + 18;
        }

        void print() const
        {
            std::cout << "@(" << _size << ", " << (uint64_t)upper_bit_size << ", " << (uint64_t)lower_bit_size << ", " << max_value << ")" << std::endl; 


            std::cout << "Lower bits: ";
            for (uint64_t i = 0; i < this->lower_bits.size(); i++)
            {
                std::cout << this->lower_bits[i] << ", ";
            }
            std::cout << std::endl;


            uint64_t upper_size = upper_bits.size();
            std::cout << "Upper bits: ";
            for (uint64_t i = 0; i < upper_size; i++)
            {
                std::cout << (upper_bits[i] ? "1" : "0");
            }
            std::cout << std::endl;
            for (uint64_t i = 0; i < this->size(); i++)
            {
                uint64_t p = this->access(i);
                std::pair<uint64_t, uint64_t> ul = get_upper_and_lower_bits(p);
                std::cout << "[" << p << ", " << ul.first << ", " << ul.second << "]";
            }
            std::cout << std::endl;
        }
    };

} // namespace stool