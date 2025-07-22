#pragma once
#include "./elias_fano_vector_builder.hpp"

namespace stool
{

    /**
     * @brief Elias-Fano encoding implementation for storing a sequence of integers efficiently
     * 
     * This class implements the Elias-Fano encoding scheme, which provides space-efficient
     * storage for monotonically increasing sequences of integers. The encoding splits
     * each value into upper and lower bits, storing them separately for optimal compression.
     * 
     * The data structure supports O(1) random access and O(log n) rank queries.
     */
    class EliasFanoVector
    {
    public:
        /**
         * @brief Iterator class for EliasFanoVector providing random access iteration
         * 
         * This iterator implements the standard random access iterator interface,
         * allowing traversal of the encoded sequence with O(1) access time.
         * Note that decrement operations are not supported.
         */
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

            /**
             * @brief Constructs an iterator pointing to a specific position
             * @param _efs Pointer to the EliasFanoVector instance
             * @param _index Position in the sequence (0-based)
             */
            iterator(const EliasFanoVector *_efs, uint64_t _index) : index(_index), efs(_efs)
            {
            }

            /**
             * @brief Pre-increment operator
             * @return Reference to the incremented iterator
             */
            iterator &operator++()
            {
                this->index++;
                return *this;
            }
            
            /**
             * @brief Dereference operator
             * @return The value at the current position
             */
            uint64_t operator*()
            {
                return efs->access(this->index);
            }
            
            /**
             * @brief Inequality comparison
             * @param rhs Iterator to compare with
             * @return true if iterators point to different positions
             */
            bool operator!=(const iterator &rhs)
            {
                return (index != rhs.index);
            }
            
            /**
             * @brief Equality comparison
             * @param rhs Iterator to compare with
             * @return true if iterators point to the same position
             */
            bool operator==(const iterator &rhs)
            {
                return (index == rhs.index);
            }

            /**
             * @brief Compound assignment operator for addition
             * @param p Number of positions to advance
             * @return Reference to the modified iterator
             */
            iterator &operator+=(int64_t p)
            {
                this->index += p;
                return *this;
            }
            
            /**
             * @brief Addition operator
             * @param p Number of positions to advance
             * @return New iterator at the advanced position
             */
            iterator operator+(int64_t p)
            {
                iterator r(this->efs, this->index + p);
                return r;
            }

            /**
             * @brief Less-than comparison
             * @param rhs Iterator to compare with
             * @return true if this iterator points to an earlier position
             */
            bool operator<(const iterator &rhs)
            {
                return (index < rhs.index);
            }
            
            /**
             * @brief Greater-than comparison
             * @param rhs Iterator to compare with
             * @return true if this iterator points to a later position
             */
            bool operator>(const iterator &rhs)
            {
                return (index > rhs.index);
            }
            
            /**
             * @brief Less-than-or-equal comparison
             * @param rhs Iterator to compare with
             * @return true if this iterator points to the same or earlier position
             */
            bool operator<=(const iterator &rhs)
            {
                return (index <= rhs.index);
            }
            
            /**
             * @brief Greater-than-or-equal comparison
             * @param rhs Iterator to compare with
             * @return true if this iterator points to the same or later position
             */
            bool operator>=(const iterator &rhs)
            {
                return (index >= rhs.index);
            }
            
            /**
             * @brief Distance between two iterators
             * @param rhs Iterator to calculate distance from
             * @return Absolute distance between iterator positions
             */
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
            
            /**
             * @brief Pre-decrement operator (not supported)
             * @return Reference to the iterator (never reached due to exception)
             * @throws std::logic_error Always thrown as decrement is not supported
             */
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

        /**
         * @brief Splits a value into upper and lower bits according to the encoding scheme
         * @param value The value to split
         * @return Pair containing (upper_bits, lower_bits)
         */
        std::pair<uint64_t, uint64_t> get_upper_and_lower_bits(uint64_t value) const
        {
            uint64_t upper = value >> this->lower_bit_size;
            uint64_t upper_filter = 64 - this->lower_bit_size;
            uint64_t lower = (value << upper_filter) >> upper_filter;
            return std::pair<uint64_t, uint64_t>(upper, lower);
        }

        /**
         * @brief Reconstructs a value from its upper and lower bits
         * @param lower The lower bits
         * @param upper The upper bits
         * @return The reconstructed value
         */
        uint64_t recover(uint64_t lower, uint64_t upper) const
        {
            return (upper << lower_bit_size) | lower;
        }

    public:
        using value_type = uint64_t;
        using const_iterator = iterator;

        /**
         * @brief Default constructor
         */
        EliasFanoVector()
        {
        }

        /**
         * @brief Move constructor
         * @param obj The EliasFanoVector to move from
         */
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
        
        /**
         * @brief Swaps the contents of this vector with another
         * @param obj The EliasFanoVector to swap with
         */
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
        
        /**
         * @brief Builds the Elias-Fano vector from a builder object
         * @param builder The EliasFanoVectorBuilder containing the data
         * @throws int Thrown if the builder is not finished
         */
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
        
        /**
         * @brief Constructs the Elias-Fano vector from a sequence of integers
         * @tparam VEC The type of the input vector (defaults to std::vector<uint64_t>)
         * @param seq Pointer to the input sequence
         */
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
        
        /**
         * @brief Builds the Elias-Fano vector from a bit vector representation
         * @param seq Vector of booleans where true indicates presence of an element
         */
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

        }

        /**
         * @brief Accesses the element at the specified position
         * @param i Index of the element to access (0-based)
         * @return The value at position i
         * @pre i < size()
         */
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
        
        /**
         * @brief Computes the rank of a value (number of elements less than the value)
         * @param value The value to compute rank for
         * @return Number of elements in the sequence that are less than value
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

        /**
         * @brief Verifies that the encoded sequence matches the original input
         * @tparam VEC The type of the input vector (defaults to std::vector<uint64_t>)
         * @param seq The original input sequence to verify against
         * @throws std::logic_error If the encoded sequence doesn't match the input
         */
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
        
        /**
         * @brief Array-style access operator
         * @param i Index of the element to access
         * @return The value at position i
         */
        uint64_t operator[](uint64_t i) const
        {
            return this->access(i);
        }
        
        /**
         * @brief Converts the encoded sequence back to a standard vector
         * @return std::vector<uint64_t> containing all elements in order
         */
        std::vector<uint64_t> to_vector()
        {
            std::vector<uint64_t> r;
            for (uint64_t i = 0; i < _size; i++)
            {
                r.push_back(this->access(i));
            }

            return r;
        }
        
        /**
         * @brief Returns the number of elements in the sequence
         * @return The size of the sequence
         */
        uint64_t size() const
        {
            return this->_size;
        }

        /**
         * @brief Returns an iterator pointing to the first element
         * @return Iterator to the beginning of the sequence
         */
        iterator begin() const
        {
            auto p = iterator(this, 0);
            return p;
        }
        
        /**
         * @brief Returns an iterator pointing past the last element
         * @return Iterator to the end of the sequence
         */
        iterator end() const
        {
            auto p = iterator(this, _size);
            return p;
        }

        /**
         * @brief Estimates the memory usage of the data structure
         * @return Approximate memory usage in bytes
         */
        uint64_t get_using_memory() const
        {
            return sdsl::size_in_bytes(lower_bits) + (this->upper_bits.size() / 8) + 18;
        }

        /**
         * @brief Prints detailed information about the data structure for debugging
         * 
         * Outputs the size, bit sizes, maximum value, lower bits array,
         * upper bits bit vector, and the reconstructed values with their
         * upper/lower bit decomposition.
         */
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