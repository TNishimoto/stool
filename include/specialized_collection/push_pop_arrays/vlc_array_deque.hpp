#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include "./bit_array_deque.hpp"

namespace stool
{
    /**
     * @class VLCDeque
     * @brief A compressed double-ended queue (deque) supporting variable-length encoded integers.
     *
     * This data structure stores integers encoded in variable-length binary form
     * and supports push/pop operations on both ends as well as random access,
     * prefix sums, and search functionalities. Internally, it uses a bit-packed
     * representation for space efficiency.
     */
    template <uint64_t MAX_SEQUENCE_LENGTH = 8092>
    class VLCArrayDeque
    {
        inline static const uint64_t MAX_BIT_LENGTH = MAX_SEQUENCE_LENGTH * 8;

        BitArrayDeque<MAX_BIT_LENGTH> value_length_deque;
        BitArrayDeque<MAX_BIT_LENGTH> code_deque;
        uint64_t psum_ = 0;

        /**
         * @class VLCDequeIterator
         * @brief Bidirectional iterator for VLCDeque.
         *
         * Allows iteration over the decoded integer values stored in VLCDeque.
         */
        class VLCArrayDequeIterator
        {
            VLCArrayDeque *m_vlc_deque;
            uint64_t m_idx;
            CircularBitPointer m_bp;
            uint8_t m_code_len;

        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;

            VLCArrayDequeIterator(VLCArrayDeque *_vlc_deque, uint64_t _idx, CircularBitPointer _bp, uint8_t _code_len) : m_vlc_deque(_vlc_deque), m_idx(_idx), m_bp(_bp), m_code_len(_code_len) {}

            uint64_t operator*() const
            {
                return m_vlc_deque->at(this->m_bp, this->m_code_len);
            }
            bool is_end() const{
                uint64_t size = this->m_vlc_deque->size();
                return this->m_idx >= size;
            }

            VLCArrayDequeIterator &operator++()
            {
                if (this->is_end())
                {
                    throw std::invalid_argument("VLCArrayDequeIterator::operator++: The iterator is at the beginning of the deque");
                }


                // CircularBitPointer base_bp = this->m_vlc_deque->value_length_deque.get_circular_bit_pointer_at_head();
                // uint64_t dist = this->m_bp.get_distance(base_bp);

                if (this->m_idx + 1 < this->m_vlc_deque->size())
                {
                    this->m_idx++;

                    this->m_bp.add(this->m_code_len);

                    if(this->m_idx + 1 < this->m_vlc_deque->size()){
                        uint64_t value_length_code = ((this->m_vlc_deque->value_length_deque.read_64_bit_string(this->m_bp)) << 1) >> 1;
                        uint64_t value_length = 64 - get_code_length(value_length_code);
                        this->m_code_len = value_length;    
                    }else{
                        CircularBitPointer base_bp = this->m_vlc_deque->value_length_deque.get_circular_bit_pointer_at_head();
                        uint64_t dist = this->m_bp.get_distance(base_bp);
                        this->m_code_len = this->m_vlc_deque->value_length_deque.size() - dist;
                    }


    
                }
                else
                {
                    this->m_idx = this->m_vlc_deque->size();
                    this->m_code_len = UINT8_MAX;
                }
                return *this;
            }

            VLCArrayDequeIterator operator++(int)
            {
                VLCArrayDequeIterator temp = *this;
                ++(*this);
                return temp;
            }

            VLCArrayDequeIterator &operator--()
            {
                uint64_t size = this->m_vlc_deque->size();
                if(size == 0){
                    throw std::invalid_argument("VLCArrayDequeIterator::operator--: The deque is empty");
                }else if(this->m_idx == 0){
                    throw std::invalid_argument("VLCArrayDequeIterator::operator--: m_idx == 0");
                }else if(this->m_idx < size){

                    this->m_idx--;

                    CircularBitPointer copy_bp = this->m_bp;
                    CircularBitPointer base_bp = this->m_vlc_deque->value_length_deque.get_circular_bit_pointer_at_head();
                    copy_bp.subtract(1);
                    uint64_t dist_p = copy_bp.get_distance(base_bp) + 1;
                    dist_p = std::min(dist_p, 64ULL);

                    uint64_t bits = this->m_vlc_deque->value_length_deque.read_prev_64bit(copy_bp);
                    uint64_t gap = 64 - dist_p;

                    uint64_t code_len = stool::LSBByte::select1(bits, 0) - gap + 1;
                    this->m_code_len = code_len;
                    this->m_bp.subtract(code_len);                        

                }else{
                    this->m_idx--;

                    uint64_t bits = this->m_vlc_deque->value_length_deque.read_last_64bit();
                    uint64_t x_size = this->m_vlc_deque->value_length_deque.size();
                    x_size = std::min(x_size, 64ULL);
                    uint64_t gap = 64 - x_size;


                    uint64_t code_len = stool::LSBByte::select1(bits, 0) - gap + 1;
                    this->m_code_len = code_len;
                    this->m_bp = this->m_vlc_deque->value_length_deque.get_circular_bit_pointer_at_tail();
                    this->m_bp.subtract(code_len - 1);


                }

                return *this;
            }

            VLCArrayDequeIterator operator--(int)
            {

                VLCArrayDequeIterator temp = *this;
                --(*this);
                return temp;
            }

            bool operator==(const VLCArrayDequeIterator &other) const { return m_idx == other.m_idx; }
            bool operator!=(const VLCArrayDequeIterator &other) const { return m_idx != other.m_idx; }
            bool operator<(const VLCArrayDequeIterator &other) const { return m_idx < other.m_idx; }
            bool operator>(const VLCArrayDequeIterator &other) const { return m_idx > other.m_idx; }
            bool operator<=(const VLCArrayDequeIterator &other) const { return m_idx <= other.m_idx; }
            bool operator>=(const VLCArrayDequeIterator &other) const { return m_idx >= other.m_idx; }
        };

    public:
        VLCArrayDeque()
        {
            this->clear();
            assert(this->verify());
        }
        VLCArrayDeque &operator=(const VLCArrayDeque &) = delete;
        VLCArrayDeque(VLCArrayDeque &&) noexcept = default;
        VLCArrayDeque &operator=(VLCArrayDeque &&) noexcept = default;

        VLCArrayDeque(std::vector<uint64_t> &values)
        {
            this->clear();
            for (uint64_t v : values)
            {
                this->push_back(v);
            }
        }

        /**
         * @brief Returns the name of the data structure
         * @return The name of the data structure
         */
        static std::string name()
        {
            return "VLC Deque";
        }

        /**
         * @brief Returns the size of the data structure in bytes
         * @return The size of the data structure in bytes
         */
        uint64_t size_in_bytes(bool only_extra_bytes = false) const
        {
            if (only_extra_bytes)
            {
                return this->value_length_deque.size_in_bytes(true) + this->code_deque.size_in_bytes(true);
            }
            else
            {
                return sizeof(VLCArrayDeque) + this->value_length_deque.size_in_bytes(true) + this->code_deque.size_in_bytes(true);
            }
        }

        uint64_t unused_size_in_bytes() const
        {
            return this->value_length_deque.unused_size_in_bytes() + this->code_deque.unused_size_in_bytes();
        }

        /**
         * @brief Computes the prefix sum from index i to j
         *
         * Calculates the sum of all elements from position i to position j inclusive.
         * If i equals j, returns the value at position i.
         *
         * @param i Starting index
         * @param j Ending index
         * @return Sum of elements from index i to j
         */
        uint64_t reverse_psum(uint64_t i) const
        {

            uint64_t len = i + 1;
            uint64_t sum = 0;
            VLCArrayDequeIterator it = this->end();
            assert(len <= this->size());


            for (size_t x = 0; x < len; x++)
            {
                --it;                
                sum += *it;

             
                assert(*it == this->at(this->size() - x - 1));


            }


            return sum;
        }

        uint64_t snatch(uint64_t i, std::array<uint8_t, MAX_SEQUENCE_LENGTH> &output){


        }


        /**
         * @brief Computes the reverse prefix sum up to index i
         *
         * Calculates the sum of elements from the end of the deque up to position i.
         * For example, if i=0, returns the last element. If i=1, returns the sum of
         * the last two elements, and so on.
         *
         * @param i Number of elements from the end to sum
         * @return Sum of the last i+1 elements
         */

         uint64_t psum2(uint64_t i) const noexcept
         {
            std::array<uint8_t, MAX_SEQUENCE_LENGTH> output;
            this->value_length_deque.special_process16(i, output);
            

         }
 

         uint64_t psum(uint64_t i) const noexcept
        {
            assert(i < this->size());

            if (i + 1 == this->size())
            {
                return this->psum_;
            }
            else
            {
                uint64_t sum = 0;
                uint64_t counter = 0;
                for (uint64_t v : *this)
                {
                    sum += v;
                    if (counter == i)
                    {

                        return sum;
                    }
                    else
                    {
                        counter++;
                    }
                }

                return sum;
            }
        }
        

        /**
         * @brief Computes the sum of elements between two indices
         *
         * Calculates the sum of elements from index i to index j inclusive.
         * For example, if i=0 and j=2, returns the sum of the first three elements.
         *
         * @param i Starting index
         * @param j Ending index
         * @return Sum of elements from index i to j
         */
        uint64_t psum() const noexcept
        {
            return this->psum_;
        }

        /**
         * @brief Computes the sum of elements between two indices
         *
         * Calculates the sum of elements from index i to index j inclusive.
         * For example, if i=0 and j=2, returns the sum of the first three elements.
         *
         * @param i Starting index
         * @param j Ending index
         * @return Sum of elements from index i to j
         */
        uint64_t psum(uint64_t i, uint64_t j) const
        {
            if (i == j)
            {
                return this->at(i);
            }
            else
            {
                throw std::runtime_error("No implementation");
            }
        }

        /**
         * @brief Searches for the first position where the prefix sum exceeds a given value
         *
         * Performs a linear search through the elements to find the first index i where
         * the sum of elements from 0 to i is greater than or equal to the target value x.
         * Returns -1 if no such position exists.
         *
         * @param x The target value to search for
         * @return The first index i where prefix sum exceeds x, or -1 if not found
         */

        int64_t search(uint64_t x) const noexcept
        {
            uint64_t sum = 0;
            uint64_t counter = 0;
            for (uint64_t v : *this)
            {
                sum += v;
                if (sum >= x)
                {
                    return counter;
                }
                else
                {
                    counter++;
                }
            }

            return -1;
        }

        /**
         * @brief Converts the deque contents to a string representation
         *
         * Creates a string representation of the deque in the format "[x1, x2, ..., xn]",
         * where x1 through xn are the values stored in the deque.
         *
         * @return String representation of the deque contents
         */
        std::string to_string() const
        {
            std::string s;
            s.push_back('[');

            uint64_t counter = 0;
            for (uint64_t v : *this)
            {
                s += std::to_string(v);
                if (counter + 1 < this->size())
                {
                    s += ", ";
                }
                counter++;
            }
            s.push_back(']');

            return s;
        }

        /**
         * @brief Converts the deque contents to a vector of values
         *
         * Creates a vector containing all the values stored in the deque,
         * preserving their order.
         *
         * @return Vector containing the deque values
         */
        std::vector<uint64_t> to_value_vector() const
        {
            return this->to_vector();
        }

        /**
         * @brief Converts the deque contents to a vector of values
         *
         * Creates a vector containing all the values stored in the deque,
         * preserving their order.
         *
         * @return Vector containing the deque values
         */
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
         * @brief Pushes multiple values to the front of the deque
         *
         * Adds multiple values from a vector to the front of the deque in reverse order,
         * so that the first element of the vector becomes the first element of the deque.
         *
         * @param new_items Vector containing the values to add
         */
        void push_front(std::vector<uint64_t> &new_items)
        {
            for (auto i = new_items.rbegin(), e = new_items.rend(); i != e; ++i)
            {
                this->push_front(*i);
            }
        }

        /**
         * @brief Pushes multiple values to the back of the deque
         *
         * Adds multiple values from a vector to the back of the deque in order,
         * so that the last element of the vector becomes the last element of the deque.
         *
         * @param new_items Vector containing the values to add
         */
        void push_back(std::vector<uint64_t> &new_items)
        {
            for (uint64_t v : new_items)
            {
                this->push_back(v);
            }
        }

        /**
         * @brief Pops multiple values from the front of the deque
         *
         * Removes and returns a specified number of values from the front of the deque.
         * The values are returned in a vector in the same order they appeared in the deque.
         *
         * @param len Number of values to remove from the front
         * @return Vector containing the removed values in order
         */
        std::vector<uint64_t> pop_front(uint64_t len)
        {
            std::vector<uint64_t> r;
            r.resize(len, UINT64_MAX);
            for (uint64_t i = 0; i < len; i++)
            {
                r[i] = this->pop_front();
            }
            return r;
        }

        /**
         * @brief Pops multiple values from the back of the deque
         *
         * Removes and returns a specified number of values from the back of the deque.
         * The values are returned in a vector in the same order they appeared in the deque
         * (last value popped is first in returned vector).
         *
         * @param len Number of values to remove from the back
         * @return Vector containing the removed values in order
         */
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

        /**
         * @brief Returns an iterator to the first element of the deque
         *
         * @return Iterator to the first element
         */
        VLCArrayDequeIterator begin() const
        {
            uint64_t size = this->size();
            if (size == 0)
            {
                return this->end();
            }
            else
            {
                CircularBitPointer bp = this->value_length_deque.get_circular_bit_pointer_at_head();
                uint64_t p = 0;
                uint64_t q = size > 1 ? this->value_length_deque.select1(1) : this->value_length_deque.size();
                uint64_t code_len = q - p;
                return VLCArrayDequeIterator(const_cast<VLCArrayDeque *>(this), 0, bp, code_len);
            }
        }

        /**
         * @brief Returns an iterator to the end of the deque
         *
         * @return Iterator to the end of the deque
         */
        VLCArrayDequeIterator end() const
        {
            uint64_t size = this->size();
            CircularBitPointer bp = this->value_length_deque.get_circular_bit_pointer_at_head();
            return VLCArrayDequeIterator(const_cast<VLCArrayDeque *>(this), size, bp, UINT8_MAX);
        }

        /**
         * @brief Swaps the contents of this deque with another VLCDeque
         *
         * This member function exchanges the contents of the current deque with those
         * of the given deque. All elements are swapped and the deques' sizes are exchanged.
         * The operation is efficient as it only swaps internal pointers and data members.
         *
         * @param item Another VLCDeque whose contents will be swapped with this one
         */
        void swap(VLCArrayDeque &item)
        {
            std::swap(this->psum_, item.psum_);
            this->value_length_deque.swap(item.value_length_deque);
            this->code_deque.swap(item.code_deque);
        }

        /*
        static uint64_t access_value(uint64_t code1, uint64_t code2, uint8_t pos, uint8_t len)
        {
            if (len > 0)
            {
                uint64_t end_pos = pos + len - 1;
                if (end_pos <= 63)
                {
                    uint64_t right_gap = 63 - end_pos;
                    uint64_t left_gap = pos;
                    return (code1 << left_gap) >> (left_gap + right_gap);
                }
                else if (pos <= 63 && end_pos <= 127)
                {
                    uint64_t left_code = (code1 << pos) >> pos;
                    uint64_t right_code = code2 >> (127 - end_pos);
                    return (left_code << (end_pos - 63)) | right_code;
                }
                else if (pos > 63 && end_pos <= 127)
                {
                    uint64_t pos1 = pos - 64;
                    uint64_t end_pos1 = end_pos - 64;
                    uint64_t right_gap = 63 - end_pos1;
                    uint64_t left_gap = pos1;
                    return (code2 << left_gap) >> (left_gap + right_gap);
                }
                else
                {
                    throw std::invalid_argument("Error: VLCDeque::access_value");
                }
            }
            else
            {
                return 0;
            }
        }
        */

        /*
        static std::pair<uint64_t, uint64_t> renove_range(uint64_t code1, uint64_t code2, uint8_t pos, uint8_t len)
        {

            if (len == 0)
            {
                return std::pair<uint64_t, uint64_t>(code1, code2);
            }

            uint64_t end_pos = pos + len - 1;
            if (end_pos < 63)
            {
                uint64_t tmp_code1 = stool::Byte::zero_pad_tail(code1, 64 - pos);
                // uint64_t tmp_code1_len = pos;
                uint64_t suf = (stool::Byte::zero_pad_head(code1, end_pos + 1)) << len;
                // uint64_t suf_len = 63 - end_pos;
                uint64_t pref_len = len;
                uint64_t pref = (code2 >> (64 - pref_len));
                uint64_t new_code1 = tmp_code1 | suf | pref;
                uint64_t new_code2 = code2 << len;
                return std::pair<uint64_t, uint64_t>(new_code1, new_code2);
            }
            else
            {
                uint64_t tmp_code1 = stool::Byte::zero_pad_tail(code1, 64 - pos);
                uint64_t pref_len = end_pos - 63;
                uint64_t suf_len = len - pref_len;
                uint64_t tmp_code2 = stool::Byte::zero_pad_head(code2, pref_len);
                uint64_t tmp_code2_len = 64 - pref_len;
                uint64_t xsuf = tmp_code2 >> (tmp_code2_len - suf_len);
                uint64_t ysuf = tmp_code2 << (64 - (tmp_code2_len - suf_len));

                uint64_t new_code1 = tmp_code1 | xsuf;
                uint64_t new_code2 = ysuf;
                return std::pair<uint64_t, uint64_t>(new_code1, new_code2);
            }
        }
        */

        /*
        static std::pair<uint64_t, uint64_t> insert_zero_bits(uint64_t code, uint8_t pos, uint8_t len)
        {
            if (len == 0)
            {
                return std::pair<uint64_t, uint64_t>(code, 0);
            }

            uint64_t rLen = 64 - pos;
            uint64_t sgap = std::min(rLen, (uint64_t)len);

            uint64_t _left = stool::Byte::zero_pad_tail(code, 64 - pos);
            uint64_t _right_tmp = code << pos;
            uint64_t _right = pos + sgap < 64 ? (_right_tmp >> (pos + sgap)) : 0;

            uint64_t val1 = _left | _right;

            uint64_t end_pos = pos + len - 1;
            uint64_t val2 = 0;
            if (end_pos < 64)
            {
                uint64_t result_suf_length = 63 - end_pos;
                val2 = _right_tmp << result_suf_length;
            }
            else
            {
                uint64_t result_pref_length = end_pos - 63;
                val2 = result_pref_length < 64 ? _right_tmp >> result_pref_length : 0;
            }

            return std::pair<uint64_t, uint64_t>(val1, val2);
        }
        */

        /*
       static std::pair<uint64_t, uint64_t> write(uint64_t code1, uint64_t code2, uint8_t pos, uint64_t value)
       {
           uint64_t len = stool::LSBByte::get_code_length(value);

           if (len > 0)
           {
               uint64_t end_pos = pos + len - 1;
               if (end_pos <= 63)
               {
                   uint64_t code1_with_padding_zero = stool::LSBByte::zero_pad(code1, pos, len);
                   uint64_t new_code1 = code1_with_padding_zero | (value << (63 - end_pos));
                   return std::pair<uint64_t, uint64_t>(new_code1, code2);
               }
               else if (pos <= 63 && end_pos <= 127)
               {
                   uint64_t left_len = 64 - pos;
                   uint64_t right_len = end_pos - 63;
                   uint64_t code1_with_padding_zero = stool::LSBByte::zero_pad(code1, pos, left_len);
                   uint64_t code2_with_padding_zero = stool::LSBByte::zero_pad(code2, 0, right_len);

                   uint64_t value1 = value >> right_len;
                   uint64_t value2 = value << (64 - right_len);
                   // uint64_t left_code = (code1 << pos) >> pos;
                   // uint64_t right_code = code2 >> (127 - end_pos);
                   uint64_t new_code1 = code1_with_padding_zero | value1;
                   uint64_t new_code2 = code2_with_padding_zero | value2;

                   return std::pair<uint64_t, uint64_t>(new_code1, new_code2);
               }
               else if (pos > 63 && end_pos <= 127)
               {
                   uint64_t code2_with_padding_zero = stool::LSBByte::zero_pad(code2, end_pos - 64, len);
                   uint64_t new_code2 = code2_with_padding_zero | (value << (127 - end_pos));
                   return std::pair<uint64_t, uint64_t>(code1, new_code2);
               }
               else
               {
                   throw std::invalid_argument("Error: VLCDeque::access_value");
               }
           }
           else
           {
               uint64_t code1_with_padding_zero = stool::LSBByte::zero_pad(code1, pos, 1);
               return std::pair<uint64_t, uint64_t>(code1_with_padding_zero, code2);
           }
       }
           */

        /**
         * @brief Returns the number of elements in the deque
         *
         * Gets the current size of the variable-length code deque by returning
         * the number of values stored in the internal value_length_deque.
         *
         * @return The number of elements in the deque
         */
        size_t size() const
        {
            return this->value_length_deque.psum();
        }

        static uint8_t get_code_length(uint64_t code)
        {
            if (code > 0)
            {

                return 64 - __builtin_clzll(code);
            }
            else
            {
                return 1;
            }
        }

        /**
         * @brief Adds a value to the end of the deque
         *
         * Pushes a new value to the back of the variable-length code deque.
         * The value is encoded using variable-length encoding and stored efficiently
         * in the internal code_deque. The length of the value is also recorded
         * in the value_length_deque.
         *
         * @param v The value to append to the deque
         */
        void push_back(uint64_t v)
        {

            uint64_t value_length = get_code_length(v);
            uint64_t length_code = 1ULL << 63;
            uint64_t value_code = v << (64 - value_length);

            this->code_deque.push_back64(value_code, value_length);
            this->value_length_deque.push_back64(length_code, value_length);
            this->psum_ += v;

            assert(this->verify());
        }

        /**
         * @brief Returns the value at the back of the deque
         *
         * Retrieves the last value stored in the variable-length code deque
         * without removing it. The value is decoded from the internal code_deque
         * using the recorded length information.
         *
         * @return The last value in the deque
         */
        void push_front(uint64_t v)
        {
            uint64_t value_length = get_code_length(v);
            uint64_t length_code = 1ULL << 63;
            uint64_t value_code = v << (64 - value_length);

            this->code_deque.push_front64(value_code, value_length);
            this->value_length_deque.push_front64(length_code, value_length);
            this->psum_ += v;

            assert(this->verify());
        }

        /**
         * @brief Returns the value at the front of the deque
         *
         * Retrieves the first value stored in the variable-length code deque
         * without removing it. The value is decoded from the internal code_deque
         * using the recorded length information.
         *
         * @return The first value in the deque
         */
        uint64_t head() const
        {
            if (this->size() > 0)
            {
                return this->at(0);
            }
            else
            {
                throw std::invalid_argument("VLCArrayDeque::head: The deque is empty");
            }
        }

        /**
         * @brief Returns the value at the back of the deque
         *
         * Retrieves the last value stored in the variable-length code deque
         * without removing it. The value is decoded from the internal code_deque
         * using the recorded length information.
         *
         * @return The last value in the deque
         */
        uint64_t tail() const
        {
            if (this->size() > 0)
            {

                uint64_t p = this->value_length_deque.rev_select1(0);
                uint64_t q = this->value_length_deque.size();
                uint64_t code_len = q - p;

                CircularBitPointer bp = this->value_length_deque.get_circular_bit_pointer_at_head();
                bp.add(p);
                uint64_t v = this->code_deque.read_64_bit_string(bp);
                uint64_t value = v >> (64 - code_len);
                return value;
            }
            else
            {
                throw std::invalid_argument("VLCArrayDeque::head: The deque is empty");
            }
        }

        /**
         * @brief Removes and returns the last element from the deque
         *
         * Removes the last value from the variable-length code deque and returns it.
         * This operation updates the internal code_deque and value_length_deque structures
         * to maintain proper encoding of the remaining values. The last_gap is adjusted
         * to account for the removed value's bits.
         *
         * The function handles two cases:
         * 1. When the value fits in the current last code word with the gap
         * 2. When the value spans across two code words
         *
         * If the last_gap becomes 0, a new empty code word is added.
         *
         * @return The last value that was removed from the deque
         */
        uint64_t pop_back()
        {
            if (this->size() == 0)
            {
                throw std::invalid_argument("VLCArrayDeque::pop_back: The deque is empty");
            }
            uint64_t p = this->value_length_deque.rev_select1(0);
            uint64_t q = this->value_length_deque.size();
            uint64_t code_len = q - p;

            CircularBitPointer bp = this->value_length_deque.get_circular_bit_pointer_at_head();
            bp.add(p);
            uint64_t v = this->code_deque.read_64_bit_string(bp);
            uint64_t value = v >> (64 - code_len);

            this->code_deque.pop_back(code_len);
            this->value_length_deque.pop_back(code_len);
            this->psum_ -= value;
            assert(this->verify());
            return value;
        }

        /**
         * @brief Removes and returns the first value from the variable-length code deque
         *
         * This operation updates the internal code_deque and value_length_deque structures
         * to maintain proper encoding of the remaining values. The first_gap is adjusted
         * to account for the removed value's bits.
         *
         * The function handles two cases:
         * 1. When the value fits in the current first code word with the gap
         * 2. When the value spans across two code words
         *
         * If the first_gap becomes 0, a new empty code word is added at the front.
         *
         * @return The first value that was removed from the deque
         */
        uint64_t pop_front()
        {
            if (this->size() == 0)
            {
                throw std::invalid_argument("VLCArrayDeque::pop_back: The deque is empty");
            }
            assert(this->value_length_deque.select1(0) == 0);
            uint64_t p = 0;
            uint64_t q = this->size() > 1 ? this->value_length_deque.select1(1) : this->value_length_deque.size();
            uint64_t code_len = q - p;

            CircularBitPointer bp = this->value_length_deque.get_circular_bit_pointer_at_head();
            bp.add(p);
            uint64_t v = this->code_deque.read_64_bit_string(bp);
            uint64_t value = v >> (64 - code_len);

            assert(this->code_deque.size() >= code_len);
            assert(this->value_length_deque.size() >= code_len);

            this->code_deque.pop_front(code_len);
            this->value_length_deque.pop_front(code_len);
            this->psum_ -= value;
            return value;
        }

        /**
         * @brief Accesses the value at a given index in the deque
         *
         * Retrieves the i-th value by first calculating its position in the code deque
         * and then decoding it using the stored value length.
         *
         * @param i The index of the value to access
         * @return The value at index i
         */
        uint64_t at(uint64_t i) const
        {
            return this->at_pair(i).first;
        }
        std::pair<uint64_t, uint64_t> at_pair(uint64_t i) const
        {
            if (i >= this->size())
            {
                throw std::invalid_argument("VLCArrayDeque::at: The index is out of range");
            }
            uint64_t p = this->value_length_deque.select1(i);
            uint64_t q = i + 1 < this->size() ? this->value_length_deque.select1(i + 1) : this->value_length_deque.size();
            uint64_t code_len = q - p;
            CircularBitPointer bp = this->value_length_deque.get_circular_bit_pointer_at_head();
            bp.add(p);
            uint64_t value = this->at(bp, code_len);

            return std::pair<uint64_t, uint64_t>(value, p);
        }
        uint64_t at(const CircularBitPointer &bp, uint8_t code_len) const
        {
            uint64_t v = this->code_deque.read_64_bit_string(bp);
            uint64_t value = v >> (64 - code_len);

            return value;
        }


        /**
         * @brief Checks if the deque is empty
         *
         * @return true if the deque contains no values, false otherwise
         */
        bool empty() const
        {
            return this->value_length_deque.empty();
        }
        /**
         * @brief Shrinks the internal containers to fit their contents
         *
         * Reduces memory usage by shrinking the value length and code deques
         * to exactly fit their current contents.
         */
        void shrink_to_fit()
        {
            this->value_length_deque.shrink_to_fit();
            this->code_deque.shrink_to_fit();
        }

        /**
         * @brief Clears all contents and resets the deque to initial state
         *
         * Removes all elements and resets the deque to its initial empty state
         * with two empty code words and default gap values.
         */
        void clear()
        {
            this->value_length_deque.clear();
            this->code_deque.clear();
            this->psum_ = 0;
        }

        /**
         * @brief Converts the contents to a standard deque
         *
         * Creates and returns a new std::deque containing all values
         * from this variable-length coded deque.
         *
         * @return std::deque<uint64_t> containing all values
         */
        std::deque<uint64_t> to_deque() const
        {
            std::deque<uint64_t> r;
            for (uint64_t v : *this)
            {
                r.push_back(v);
            }
            return r;
        }

        /**
         * @brief Converts the contents to a vector
         *
         * Creates and returns a new std::vector containing all values
         * from this variable-length coded deque.
         *
         * @return std::vector<uint64_t> containing all values
         */
        std::vector<uint64_t> to_vector() const
        {

            std::vector<uint64_t> r;
            for (uint64_t v : *this)
            {
                r.push_back(v);
            }

            return r;
        }

        /**
         * @brief Inserts a value at the specified position
         *
         * Shifts elements right to make space and inserts the new value
         * at the given position using variable-length encoding.
         *
         * @param pos Position at which to insert
         * @param value Value to insert
         */
        void insert(uint64_t pos, uint64_t value)
        {
            uint64_t size = this->size();

            if (pos > size)
            {
                throw std::invalid_argument("VLCArrayDeque::insert: The position is out of range");
            }

            if (pos == size)
            {
                this->push_back(value);
            }
            else if (pos == 0)
            {
                this->push_front(value);
            }
            else
            {
                uint64_t value_len = get_code_length(value);
                uint64_t p = this->value_length_deque.select1(pos);

                uint64_t value_length_code = 1ULL << 63;
                uint64_t value_code = value << (64 - value_len);

                this->value_length_deque.insert_64bit_string(p, value_length_code, value_len);
                this->code_deque.insert_64bit_string(p, value_code, value_len);
                this->psum_ += value;
            }

            assert(this->verify());
        }

        /**
         * @brief Removes the value at the specified position
         *
         * Removes the value at the given position and shifts remaining elements left.
         * Throws if position is out of range.
         *
         * @param pos Position of value to remove
         * @throws std::invalid_argument if pos is out of range
         */
        void remove(uint64_t pos)
        {

            uint64_t size = this->size();

            if (pos >= size)
            {
                throw std::invalid_argument("VLCArrayDeque::insert: The position is out of range");
            }

            if (pos + 1 == size)
            {
                this->pop_back();
            }
            else if (pos == 0)
            {
                this->pop_front();
            }
            else
            {
                uint64_t value = this->at(pos);
                uint64_t value_len = get_code_length(value);
                uint64_t p = this->value_length_deque.select1(pos);

                // uint64_t value_length_code = 1ULL << 63;
                // uint64_t value_code = value << (64 - value_len);

                this->value_length_deque.erase(p, value_len);
                this->code_deque.erase(p, value_len);
                this->psum_ -= value;
            }

            assert(this->verify());
        }

        /**
         * @brief Sets a new value at the specified position
         *
         * Replaces the value at the given position by removing the old value
         * and inserting the new one.
         *
         * @param pos Position to set
         * @param value New value to set
         */
        void set_value(uint64_t pos, uint64_t value)
        {
            std::pair<uint64_t, uint64_t> pair = this->at_pair(pos);
            this->set_value(pos, value, pair.first, pair.second);
        }
        void set_value(uint64_t pos, uint64_t new_value, uint64_t old_value, uint64_t code_pos)
        {
            uint64_t old_code_len = get_code_length(old_value);
            uint64_t new_code_len = get_code_length(new_value);

            if (old_code_len == new_code_len)
            {
                uint64_t code_value = new_value << (64 - new_code_len);
                this->code_deque.replace_64bit_string(code_pos, code_value, new_code_len);
                this->psum_ += new_value;
                this->psum_ -= old_value;
            }
            else
            {
                this->remove(pos);
                this->insert(pos, new_value);
            }
        }

        /**
         * @brief Verifies the integrity of the variable-length coded deque
         *
         * Checks that the unused bits at the beginning and end of the code deque
         * are properly zeroed out. The first_gap bits at the start of the first code word
         * and the last_gap bits at the end of the last code word should be zero.
         *
         * @return true if the deque passes verification
         * @throws std::logic_error if verification fails due to non-zero bits in unused areas
         */
        bool verify() const
        {
            /*
            uint64_t left = this->code_deque[0] >> (64 - this->first_gap);
            uint64_t right = this->code_deque[this->code_deque.size() - 1] << (64 - this->last_gap);

            if (left > 0)
            {
                throw std::logic_error("Error: VLCDeque::verify(1)");
            }
            if (right > 0)
            {
                throw std::logic_error("Error: VLCDeque::verify(2)");
            }
            */
            return true;
        }

        /**
         * @brief Prints the internal state of the VLCDeque for debugging
         *
         * Outputs the following information to stdout:
         * - Value length deque contents
         * - Code deque contents in binary format
         * - First and last gap values
         * - Actual stored values
         */
        void print() const
        {
            std::cout << "============================" << std::endl;
            std::cout << "Length Code: \t" << this->value_length_deque.to_string() << std::endl;
            std::cout << "Value Code: \t" << this->code_deque.to_string() << std::endl;
            std::cout << "values: \t" << this->to_string() << std::endl;
            std::cout << "============================" << std::endl;
        }
        void print_info() const
        {
            this->print();
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
            std::pair<uint64_t, uint64_t> pair = this->at_pair(i);
            uint64_t new_value = pair.first + delta;            
            this->set_value(i, new_value, pair.first, pair.second);
        }

        /**
         * @brief Calculates the memory size in bytes of a VLCDeque instance
         *
         * @param item The VLCDeque instance to measure
         * @return Number of bytes used by the VLCDeque
         */
        static uint64_t get_byte_size(const VLCArrayDeque &item)
        {
            uint64_t bytes = sizeof(item.value_length_deque) + sizeof(item.code_deque);
            return bytes;
        }

        /**
         * @brief Calculates the total memory size in bytes of a vector of VLCDeque instances
         *
         * @param items Vector of VLCDeque instances to measure
         * @return Total number of bytes used by all VLCDeque instances
         */
        static uint64_t get_byte_size(const std::vector<VLCArrayDeque> &items)
        {
            uint64_t bytes = sizeof(uint64_t);
            for (auto &it : items)
            {
                bytes += VLCArrayDeque::get_byte_size(it);
            }
            return bytes;
        }
        /**
         * @brief Saves a VLCDeque instance to a byte array
         *
         * Serializes the VLCDeque by writing its members to the output byte array
         * at the specified position.
         *
         * @param item The VLCDeque instance to save
         * @param output The output byte array to write to
         * @param pos Current position in the output array, updated after writing
         */
        static void save(const VLCArrayDeque &item, std::vector<uint8_t> &output, uint64_t &pos)
        {
            std::memcpy(output.data() + pos, &item.psum_, sizeof(item.psum_));
            pos += sizeof(item.psum_);
            item.value_length_deque.save(output, pos);
            item.code_deque.save(output, pos);
        }

        /**
         * @brief Saves a VLCDeque instance to a file stream
         *
         * Serializes the VLCDeque by writing its members to the provided output file stream.
         *
         * @param item The VLCDeque instance to save
         * @param os The output file stream to write to
         */
        static void save(const VLCArrayDeque &item, std::ofstream &os)
        {
            os.write(reinterpret_cast<const char *>(&item.psum_), sizeof(item.psum_));
            BitArrayDeque<MAX_BIT_LENGTH>::save(item.value_length_deque, os);
            BitArrayDeque<MAX_BIT_LENGTH>::save(item.code_deque, os);
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
        static void save(const std::vector<VLCArrayDeque> &items, std::vector<uint8_t> &output, uint64_t &pos)
        {
            uint64_t size = items.size();
            std::memcpy(output.data() + pos, &size, sizeof(size));
            pos += sizeof(size);

            for (auto &it : items)
            {
                VLCArrayDeque::save(it, output, pos);
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
        static void save(const std::vector<VLCArrayDeque> &items, std::ofstream &os)
        {
            uint64_t size = items.size();
            os.write(reinterpret_cast<const char *>(&size), sizeof(size));
            for (auto &it : items)
            {
                VLCArrayDeque::save(it, os);
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
        static VLCArrayDeque load(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            VLCArrayDeque r;
            BitArrayDeque<MAX_BIT_LENGTH> tmp1 = BitArrayDeque<MAX_BIT_LENGTH>::load(data, pos);
            BitArrayDeque<MAX_BIT_LENGTH> tmp2 = BitArrayDeque<MAX_BIT_LENGTH>::load(data, pos);
            r.value_length_deque.swap(tmp1);
            r.code_deque.swap(tmp2);

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
        static VLCArrayDeque load(std::ifstream &ifs)
        {
            VLCArrayDeque r;
            BitArrayDeque<MAX_BIT_LENGTH> tmp1 = BitArrayDeque<MAX_BIT_LENGTH>::load(ifs);
            BitArrayDeque<MAX_BIT_LENGTH> tmp2 = BitArrayDeque<MAX_BIT_LENGTH>::load(ifs);

            r.value_length_deque.swap(tmp1);
            r.code_deque.swap(tmp2);
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
        static std::vector<VLCArrayDeque> load_vector(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t size;
            std::memcpy(&size, data.data() + pos, sizeof(size));
            pos += sizeof(size);

            std::vector<VLCArrayDeque> r;
            for (uint64_t i = 0; i < size; i++)
            {
                r.push_back(VLCArrayDeque::load(data, pos));
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
        static std::vector<VLCArrayDeque> load_vector(std::ifstream &ifs)
        {
            uint64_t size = 0;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(size));

            std::vector<VLCArrayDeque> r;
            for (uint64_t i = 0; i < size; i++)
            {
                r.push_back(VLCArrayDeque::load(ifs));
            }
            return r;
        }
    };

}