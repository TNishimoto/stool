#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include "../basic/byte.hpp"
#include "../debug/print.hpp"
#include "./simple_deque.hpp"

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
    class VLCDeque
    {
        SimpleDeque16<uint8_t> value_length_deque;
        SimpleDeque16<uint64_t> code_deque;
        uint8_t first_gap = 0;
        uint8_t last_gap = 0;

        /**
         * @class VLCDequeIterator
         * @brief Bidirectional iterator for VLCDeque.
         *
         * Allows iteration over the decoded integer values stored in VLCDeque.
         */
        class VLCDequeIterator
        {
            VLCDeque *m_vlc_deque;
            uint64_t m_idx;
            uint64_t m_code_pos1;
            uint8_t m_code_pos2;

        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;

            VLCDequeIterator(VLCDeque *_vlc_deque, uint64_t _idx, uint64_t _code_pos1, uint8_t _code_pos2) : m_vlc_deque(_vlc_deque), m_idx(_idx), m_code_pos1(_code_pos1), m_code_pos2(_code_pos2) {}

            uint64_t operator*() const { return m_vlc_deque->at(m_code_pos1, m_code_pos2, m_vlc_deque->value_length_deque[m_idx]); }

            
            VLCDequeIterator &operator++()
            {
                uint64_t len = m_vlc_deque->value_length_deque[m_idx];
                if (m_code_pos2 + len < 64)
                {
                    m_code_pos2 += len;
                }
                else
                {
                    m_code_pos1++;
                    m_code_pos2 = ((uint16_t)m_code_pos2 + (uint16_t)len) - 64;
                }
                m_idx++;
                return *this;
            }

            VLCDequeIterator operator++(int)
            {
                VLCDequeIterator temp = *this;
                ++(*this);
                return temp;
            }

            VLCDequeIterator &operator--()
            {

                uint64_t len = m_vlc_deque->value_length_deque[m_idx - 1];

                if (m_code_pos2 >= len)
                {
                    m_code_pos2 -= len;
                }
                else
                {
                    assert(m_code_pos1 > 0);
                    m_code_pos1--;
                    m_code_pos2 = 64 - (len - m_code_pos2);
                }
                m_idx--;

                return *this;
            }

            VLCDequeIterator operator--(int)
            {

                VLCDequeIterator temp = *this;
                --(*this);
                return temp;
            }

            bool operator==(const VLCDequeIterator &other) const { return m_idx == other.m_idx; }
            bool operator!=(const VLCDequeIterator &other) const { return m_idx != other.m_idx; }
            bool operator<(const VLCDequeIterator &other) const { return m_idx < other.m_idx; }
            bool operator>(const VLCDequeIterator &other) const { return m_idx > other.m_idx; }
            bool operator<=(const VLCDequeIterator &other) const { return m_idx <= other.m_idx; }
            bool operator>=(const VLCDequeIterator &other) const { return m_idx >= other.m_idx; }
        };

    public:
        VLCDeque()
        {
            this->clear();
            assert(this->verify());
        }
        VLCDeque &operator=(const VLCDeque &) = delete;
        VLCDeque(VLCDeque &&) noexcept = default;
        VLCDeque &operator=(VLCDeque &&) noexcept = default;

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
        uint64_t size_in_bytes(bool only_extra_bytes) const
        {
            if(only_extra_bytes){
                return this->value_length_deque.size_in_bytes(true) + this->code_deque.size_in_bytes(true);
            }else{
                return sizeof(VLCDeque) + this->value_length_deque.size_in_bytes(true) + this->code_deque.size_in_bytes(true);
            }
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
            VLCDequeIterator it = this->end();
            for (size_t x = 0; x < len; x++)
            {
                --it;
                sum += *it;
            }
            return sum;
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
        uint64_t psum(uint64_t i) const noexcept
        {
            assert(i < this->size());

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
            if (this->size() == 0)
            {
                return 0;
            }
            else
            {
                return this->psum(this->size() - 1);
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
        VLCDequeIterator begin() const
        {
            if (this->first_gap < 64)
            {
                return VLCDequeIterator(const_cast<VLCDeque *>(this), 0, 0, this->first_gap);
            }
            else
            {
                return VLCDequeIterator(const_cast<VLCDeque *>(this), 0, 1, 0);
            }
        }

        /**
         * @brief Returns an iterator to the end of the deque
         * 
         * @return Iterator to the end of the deque
         */
        VLCDequeIterator end() const
        {
            // uint64_t code_pos = 64 * this->code_deque.size() - 1 - (uint64_t)this->last_gap;
            if (this->last_gap < 64)
            {
                return VLCDequeIterator(const_cast<VLCDeque *>(this), this->value_length_deque.size(), this->code_deque.size() - 1, 64 - this->last_gap);
            }
            else
            {
                return VLCDequeIterator(const_cast<VLCDeque *>(this), this->value_length_deque.size(), this->code_deque.size() - 1, 0);
            }
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
        void swap(VLCDeque &item)
        {
            this->value_length_deque.swap(item.value_length_deque);
            this->code_deque.swap(item.code_deque);
            std::swap(this->first_gap, item.first_gap);
            std::swap(this->last_gap, item.last_gap);
        }

        /**
         * @brief Removes a range of bits from a pair of 64-bit codes
         * 
         * This static utility function removes a specified range of bits from a pair of consecutive
         * 64-bit codes. The range can span across both codes. The function handles different cases:
         * - When the range is entirely within the first code
         * - When the range spans both codes
         * - When the range is entirely within the second code
         * 
         * @param code1 The first 64-bit code
         * @param code2 The second 64-bit code 
         * @param pos Starting position of the range to remove (0-based)
         * @param len Length of the range to remove in bits
         * @return A pair containing the modified codes after removing the specified range
         */
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

        /**
         * @brief Accesses a value from a pair of 64-bit codes
         * 
         * This static utility function extracts a value from a specified bit range spanning
         * a pair of consecutive 64-bit codes. The range can be located:
         * - Entirely within the first code
         * - Across both codes
         * - Entirely within the second code
         * 
         * @param code1 The first 64-bit code
         * @param code2 The second 64-bit code
         * @param pos Starting position of the range to access (0-based)
         * @param len Length of the range to access in bits
         * @return The extracted value from the specified bit range
         * @throws std::invalid_argument if the specified range is invalid
         */
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

        /**
         * @brief Removes a range of bits from a pair of 64-bit codes
         * 
         * This static utility function removes a specified range of bits from a pair of consecutive
         * 64-bit codes. The range can be located:
         * - Entirely within the first code
         * - Across both codes
         * - Entirely within the second code
         * 
         * After removal, the remaining bits are shifted to fill the gap.
         * 
         * @param code1 The first 64-bit code
         * @param code2 The second 64-bit code  
         * @param pos Starting position of the range to remove (0-based)
         * @param len Length of the range to remove in bits
         * @return A pair containing the modified codes after bit removal
         * @throws std::invalid_argument if the specified range is invalid
         */
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
        /**
         * @brief Inserts zero bits into a 64-bit code at a specified position
         * 
         * This static utility function inserts a specified number of zero bits into a 64-bit code
         * at a given position. The original bits after the insertion point are shifted right to
         * make room for the inserted zeros. If the shifted bits exceed 64 bits, they are stored
         * in a second code.
         * 
         * @param code The original 64-bit code
         * @param pos Starting position where zeros should be inserted (0-based)
         * @param len Number of zero bits to insert
         * @return A pair containing the modified code(s) after zero bit insertion:
         *         - first: The primary code with inserted zeros
         *         - second: Overflow bits if any, otherwise 0
         */
        static std::pair<uint64_t, uint64_t> write(uint64_t code1, uint64_t code2, uint8_t pos, uint64_t value)
        {
            uint64_t len = stool::Byte::get_code_length(value);

            if (len > 0)
            {
                uint64_t end_pos = pos + len - 1;
                if (end_pos <= 63)
                {
                    uint64_t code1_with_padding_zero = stool::Byte::zero_pad(code1, pos, len);
                    uint64_t new_code1 = code1_with_padding_zero | (value << (63 - end_pos));
                    return std::pair<uint64_t, uint64_t>(new_code1, code2);
                }
                else if (pos <= 63 && end_pos <= 127)
                {
                    uint64_t left_len = 64 - pos;
                    uint64_t right_len = end_pos - 63;
                    uint64_t code1_with_padding_zero = stool::Byte::zero_pad(code1, pos, left_len);
                    uint64_t code2_with_padding_zero = stool::Byte::zero_pad(code2, 0, right_len);

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
                    uint64_t code2_with_padding_zero = stool::Byte::zero_pad(code2, end_pos - 64, len);
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
                uint64_t code1_with_padding_zero = stool::Byte::zero_pad(code1, pos, 1);
                return std::pair<uint64_t, uint64_t>(code1_with_padding_zero, code2);
            }
        }

        

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
            return this->value_length_deque.size();
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

            uint64_t value_length = stool::Byte::get_code_length(v);
            this->value_length_deque.push_back(value_length);
            if (v > 0)
            {
                uint64_t last_code = this->code_deque[this->code_deque.size() - 1];

                if (value_length <= this->last_gap)
                {
                    this->code_deque[this->code_deque.size() - 1] = last_code | (v << (this->last_gap - value_length));
                    this->last_gap -= value_length;
                }
                else
                {
                    uint64_t len1 = value_length - this->last_gap;
                    uint64_t new_last_gap = 64 - len1;

                    uint64_t new_left_code = last_code | (v >> len1);
                    uint64_t new_right_code = v << new_last_gap;

                    this->code_deque[this->code_deque.size() - 1] = new_left_code;
                    this->code_deque.push_back(new_right_code);
                    assert(this->code_deque[this->code_deque.size() - 2] == new_left_code);

                    this->last_gap = new_last_gap;
                }

                if (this->last_gap == 0)
                {
                    this->code_deque.push_back(0);
                    this->last_gap = 64;
                }
            }

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
            uint64_t value_length = stool::Byte::get_code_length(v);
            this->value_length_deque.push_front(value_length);
            if (v > 0)
            {
                uint64_t fst_code = this->code_deque[0];

                if (value_length <= this->first_gap)
                {
                    this->code_deque[0] = fst_code | (v << (64 - this->first_gap));
                    this->first_gap -= value_length;
                }
                else
                {
                    uint64_t new_right_code = fst_code | (v << (64 - this->first_gap));
                    uint64_t new_left_code = v >> this->first_gap;

                    this->code_deque[0] = new_right_code;
                    this->code_deque.push_front(new_left_code);
                    this->first_gap = 64 - (value_length - this->first_gap);
                }

                if (this->first_gap == 0)
                {
                    this->code_deque.push_front(0);
                    this->first_gap = 64;
                }
            }
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
            if (this->first_gap < 64)
            {
                return this->at(0, this->first_gap, this->value_length_deque[0]);
            }
            else
            {
                return this->at(1, 0, this->value_length_deque[0]);
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
            assert(this->size() > 0);
            VLCDequeIterator it = this->end();
            --it;

            return *it;
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
            uint64_t _tail = this->tail();

            uint64_t value_len = this->value_length_deque[this->value_length_deque.size() - 1];
            uint64_t last_code = this->code_deque[this->code_deque.size() - 1];
            this->value_length_deque.pop_back();

            if (value_len > 0)
            {
                if (value_len + this->last_gap < 64)
                {
                    this->code_deque[this->code_deque.size() - 1] = stool::Byte::zero_pad_tail(last_code, value_len + this->last_gap);
                    this->last_gap += value_len;
                }
                else
                {
                    this->code_deque.pop_back();
                    last_code = this->code_deque[this->code_deque.size() - 1];
                    uint64_t len1 = value_len - (64 - this->last_gap);
                    this->code_deque[this->code_deque.size() - 1] = stool::Byte::zero_pad_tail(last_code, len1);
                    this->last_gap = (value_len + this->last_gap) - 64;
                }
            }

            if (this->last_gap == 0)
            {
                this->code_deque.push_back(0);
                this->last_gap = 64;
            }

            assert(this->verify());

            return _tail;
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

            uint64_t _head = this->head();
            uint64_t value_len = this->value_length_deque[0];
            uint64_t fst_code = this->code_deque[0];
            this->value_length_deque.pop_front();

            if (value_len > 0)
            {
                if (value_len + this->first_gap < 64)
                {
                    this->code_deque[0] = stool::Byte::zero_pad_head(fst_code, value_len + this->first_gap);
                    this->first_gap += value_len;
                }
                else
                {
                    this->code_deque.pop_front();
                    fst_code = this->code_deque[0];
                    uint64_t len1 = value_len - (64 - this->first_gap);
                    this->code_deque[0] = stool::Byte::zero_pad_head(fst_code, len1);
                    this->first_gap = ((uint64_t)value_len + (uint64_t)this->first_gap) - 64;
                }
            }

            if (this->first_gap == 0)
            {
                this->code_deque.push_front(0);
                this->first_gap = 64;
            }
            assert(this->verify());

            return _head;
        }

        /**
         * @brief Gets the starting position of a code in the deque
         * 
         * Calculates the position of the i-th value in terms of code word index and bit offset.
         * Takes into account the first_gap and accumulated value lengths.
         * 
         * @param i The index of the value
         * @return A pair containing:
         *         - first: The index of the code word in code_deque
         *         - second: The bit offset within that code word
         */
        std::pair<uint64_t, uint8_t> get_code_starting_position(uint64_t i) const
        {
            uint64_t pos = this->first_gap;
            uint64_t x = 0;
            for (uint8_t c : this->value_length_deque)
            {
                if (x == i)
                    break;
                pos += c;
                ++x;
            }
            return std::pair<uint64_t, uint8_t>(pos / 64, pos % 64);
        }

        /**
         * @brief Accesses a value at a specific position in the code deque
         * 
         * Retrieves a value given its code word position, bit offset, and length.
         * Can span across two code words if needed.
         * 
         * @param code_deque_pos1 Index of the first code word
         * @param code_deque_pos2 Bit offset within the first code word
         * @param value_length Length of the value in bits
         * @return The decoded value at the specified position
         */
        uint64_t at(uint64_t code_deque_pos1, uint8_t code_deque_pos2, uint8_t value_length) const
        {
            assert(code_deque_pos1 < this->code_deque.size());
            uint64_t code1 = this->code_deque[code_deque_pos1];
            uint64_t code2 = code_deque_pos1 + 1 < this->code_deque.size() ? this->code_deque[code_deque_pos1 + 1] : 0;
            return VLCDeque::access_value(code1, code2, code_deque_pos2, value_length);
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
            std::pair<uint64_t, uint8_t> code_pos = this->get_code_starting_position(i);
            uint8_t value_length = this->value_length_deque[i];
            return this->at(code_pos.first, code_pos.second, value_length);
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
        void shrink_to_front()
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
            this->code_deque.push_back(0);
            this->code_deque.push_back(0);
            this->first_gap = 64;
            this->last_gap = 64;
            this->code_deque.shrink_to_fit();
            this->value_length_deque.shrink_to_fit();
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
            uint64_t value_len = stool::Byte::get_code_length(value);

            this->shift_right(pos, value_len);
            std::pair<uint64_t, uint8_t> code_pos = this->get_code_starting_position(pos);
            this->set_value(pos, code_pos.first, code_pos.second, value);

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
            if (pos < this->size())
            {
                this->shift_left(pos);
            }
            else
            {
                throw std::invalid_argument("Error: VLCDeque::remove");
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
            this->remove(pos);
            this->insert(pos, value);
        }

        /**
         * @brief Sets a value at a specific code position
         * 
         * Sets a value at the given position in the code deque, handling the bit-level
         * operations needed for variable-length encoding. The new value must have
         * the same length as the existing value.
         * 
         * @param pos Logical position in the deque
         * @param code_pos1 Index of first code word
         * @param code_pos2 Bit offset in first code word
         * @param value New value to set
         * @throws std::invalid_argument if value length doesn't match existing value length
         */
        void set_value(uint64_t pos, uint64_t code_pos1, uint8_t code_pos2, uint64_t value)
        {

            if (stool::Byte::get_code_length(value) != this->value_length_deque[pos])
            {
                throw std::invalid_argument("set_value");
            }
            assert(stool::Byte::get_code_length(value) == this->value_length_deque[pos]);
            if (value == 0)
            {
                return;
            }
            if (code_pos1 + 1 < this->code_deque.size())
            {
                uint64_t code1 = this->code_deque[code_pos1];
                uint64_t code2 = this->code_deque[code_pos1 + 1];
                std::pair<uint64_t, uint64_t> new_code = VLCDeque::write(code1, code2, code_pos2, value);
                this->code_deque[code_pos1] = new_code.first;
                this->code_deque[code_pos1 + 1] = new_code.second;
            }
            else
            {
                uint64_t code1 = this->code_deque[code_pos1];
                uint64_t code2 = 0;

                std::pair<uint64_t, uint64_t> new_code = VLCDeque::write(code1, code2, code_pos2, value);

                this->code_deque[code_pos1] = new_code.first;
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
            return true;
        }

        /**
         * @brief Shifts values to the left starting from a given position
         * 
         * Removes the value at the specified position and shifts all subsequent values
         * to the left by one position. This operation involves bit-level manipulation
         * to maintain proper variable-length encoding of the values.
         */
        void shift_left(uint64_t pos)
        {
            if (pos > 0 && pos < this->size())
            {
                uint64_t value_len = this->value_length_deque[pos];
                if (value_len > 0)
                {
                    std::pair<uint64_t, uint8_t> code_pos = this->get_code_starting_position(pos);
                    if (code_pos.first + 1 < this->code_deque.size())
                    {
                        std::pair<uint64_t, uint64_t> rmv_result = VLCDeque::renove_range(this->code_deque[code_pos.first], this->code_deque[code_pos.first + 1], code_pos.second, value_len);
                        this->code_deque[code_pos.first] = rmv_result.first;
                        this->code_deque[code_pos.first + 1] = rmv_result.second;
                    }
                    else
                    {
                        std::pair<uint64_t, uint64_t> rmv_result = VLCDeque::renove_range(this->code_deque[code_pos.first], 0, code_pos.second, value_len);
                        this->code_deque[code_pos.first] = rmv_result.first;
                    }
                    for (size_t i = code_pos.first + 2; i < this->code_deque.size(); i++)
                    {
                        uint64_t left = this->code_deque[i] >> (64 - value_len);
                        uint64_t right = value_len < 64 ? (this->code_deque[i] << value_len) : 0;
                        this->code_deque[i - 1] = this->code_deque[i - 1] | left;
                        this->code_deque[i] = right;
                    }

                    this->value_length_deque.erase(this->value_length_deque.begin() + pos);

                    if (this->last_gap + value_len <= 64)
                    {
                        this->last_gap += value_len;
                    }
                    else
                    {
                        if (this->size() > 0)
                        {
                            this->code_deque.pop_back();
                            this->last_gap = ((uint64_t)this->last_gap + (uint64_t)value_len) - 64;
                        }
                        else
                        {
                            this->clear();
                        }
                    }
                }
                else
                {

                    this->value_length_deque.erase(this->value_length_deque.begin() + pos);
                }
            }
            else if (pos == 0)
            {
                this->pop_front();
            }
            else if (pos == this->size())
            {
                this->pop_back();
            }
            else
            {
                throw std::invalid_argument("Error: VLCDeque::shift_left()");
            }
        }

        /**
         * @brief Shifts values to the right starting from a given position
         * 
         * Inserts a new value at the specified position by shifting all subsequent values
         * to the right. This operation involves bit-level manipulation to maintain proper
         * variable-length encoding of the values.
         */
        void shift_right(uint64_t pos, uint64_t value_len)
        {
            if (pos > 0 && pos < this->size())
            {
                if (value_len > 0)
                {
                    int64_t epos = this->code_deque.size() - 1;
                    this->code_deque.push_back(0);
                    std::pair<uint64_t, uint8_t> code_pos = this->get_code_starting_position(pos);

                    for (int64_t i = epos; i > (int64_t)code_pos.first; --i)
                    {
                        std::pair<uint64_t, uint64_t> ins_result = VLCDeque::insert_zero_bits(this->code_deque[i], 0, value_len);
                        this->code_deque[i + 1] = this->code_deque[i + 1] | ins_result.second;
                        this->code_deque[i] = ins_result.first;
                    }

                    // this->code_deque[i+1] = this->code_deque[i+1] | ((this->code_deque[i] << (64 - value_len)) >> (64 - value_len));

                    std::pair<uint64_t, uint64_t> ins_result = VLCDeque::insert_zero_bits(this->code_deque[code_pos.first], code_pos.second, value_len);

                    this->code_deque[code_pos.first] = ins_result.first;
                    this->code_deque[code_pos.first + 1] = ins_result.second | this->code_deque[code_pos.first + 1];

                    this->value_length_deque.insert(this->value_length_deque.begin() + pos, value_len);

                    if (value_len < this->last_gap)
                    {
                        this->code_deque.pop_back();
                        this->last_gap -= value_len;
                    }
                    else if (value_len == this->last_gap)
                    {
                        this->last_gap = 64;
                    }
                    else
                    {
                        this->last_gap = 64 - (value_len - this->last_gap);
                    }
                }
                else
                {
                    this->value_length_deque.insert(this->value_length_deque.begin() + pos, 0);
                }
            }
            else if (pos == 0)
            {
                if (value_len > 0)
                {
                    this->push_front(1 << (value_len - 1));
                }
                else
                {
                    this->push_front(0);
                }
            }
            else if (pos == this->size())
            {
                if (value_len > 0)
                {
                    this->push_back(1 << (value_len - 1));
                }
                else
                {
                    this->push_back(0);
                }
            }
            else
            {
                throw std::invalid_argument("Error: VLCDeque::shift_right()");
            }
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
            stool::Printer::print("Value Length", this->value_length_deque.to_deque());
            std::cout << "Code: [" << std::flush;
            for (uint64_t i = 0; i < this->code_deque.size(); i++)
            {
                std::bitset<64> bs(this->code_deque[i]);
                std::cout << bs;
                if (i + 1 < this->code_deque.size())
                {
                    std::cout << ", ";
                }
            }
            std::cout << "]" << std::endl;
            std::cout << "First Gap: " << (int)this->first_gap << std::endl;
            std::cout << "Last Gap: " << (int)this->last_gap << std::endl;
            stool::Printer::print("Values", this->to_vector());
            std::cout << "============================" << std::endl;
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
            std::pair<uint64_t, uint8_t> code_pos = this->get_code_starting_position(i);

            uint64_t x = this->at(code_pos.first, code_pos.second, this->value_length_deque[i]) + delta;
            uint64_t code_length = stool::Byte::get_code_length(x);
            if (code_length != this->value_length_deque[i])
            {
                this->remove(i);
                this->insert(i, x);
            }
            else
            {
                this->set_value(i, code_pos.first, code_pos.second, x);
            }
            assert(this->at(i) == x);
        }

        /**
         * @brief Calculates the memory size in bytes of a VLCDeque instance
         * 
         * @param item The VLCDeque instance to measure
         * @return Number of bytes used by the VLCDeque
         */
        static uint64_t get_byte_size(const VLCDeque &item)
        {
            uint64_t bytes = sizeof(item.first_gap) + sizeof(item.last_gap) + SimpleDeque16<uint8_t>::get_byte_size(item.value_length_deque) + SimpleDeque16<uint64_t>::get_byte_size(item.code_deque);
            return bytes;
        }

        /**
         * @brief Calculates the total memory size in bytes of a vector of VLCDeque instances
         * 
         * @param items Vector of VLCDeque instances to measure
         * @return Total number of bytes used by all VLCDeque instances
         */
        static uint64_t get_byte_size(const std::vector<VLCDeque> &items)
        {
            uint64_t bytes = sizeof(uint64_t);
            for (auto &it : items)
            {
                bytes += VLCDeque::get_byte_size(it);
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
        static void save(const VLCDeque &item, std::vector<uint8_t> &output, uint64_t &pos)
        {

            std::memcpy(output.data() + pos, &item.first_gap, sizeof(item.first_gap));
            pos += sizeof(item.first_gap);
            std::memcpy(output.data() + pos, &item.last_gap, sizeof(item.last_gap));
            pos += sizeof(item.last_gap);
            SimpleDeque16<uint8_t>::save(item.value_length_deque, output, pos);
            SimpleDeque16<uint64_t>::save(item.code_deque, output, pos);
        }

        /**
         * @brief Saves a VLCDeque instance to a file stream
         * 
         * Serializes the VLCDeque by writing its members to the provided output file stream.
         * 
         * @param item The VLCDeque instance to save
         * @param os The output file stream to write to
         */
        static void save(const VLCDeque &item, std::ofstream &os)
        {
            os.write(reinterpret_cast<const char *>(&item.first_gap), sizeof(item.first_gap));
            os.write(reinterpret_cast<const char *>(&item.last_gap), sizeof(item.last_gap));
            SimpleDeque16<uint8_t>::save(item.value_length_deque, os);
            SimpleDeque16<uint64_t>::save(item.code_deque, os);
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
        static void save(const std::vector<VLCDeque> &items, std::vector<uint8_t> &output, uint64_t &pos)
        {
            uint64_t size = items.size();
            std::memcpy(output.data() + pos, &size, sizeof(size));
            pos += sizeof(size);

            for (auto &it : items)
            {
                VLCDeque::save(it, output, pos);
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
        static void save(const std::vector<VLCDeque> &items, std::ofstream &os)
        {
            uint64_t size = items.size();
            os.write(reinterpret_cast<const char *>(&size), sizeof(size));
            for (auto &it : items)
            {
                VLCDeque::save(it, os);
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
        static VLCDeque load(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            VLCDeque r;
            std::memcpy(&r.first_gap, data.data() + pos, sizeof(r.first_gap));
            pos += sizeof(r.first_gap);
            std::memcpy(&r.last_gap, data.data() + pos, sizeof(r.last_gap));
            pos += sizeof(r.last_gap);
            SimpleDeque16<uint8_t> tmp1 = SimpleDeque16<uint8_t>::load(data, pos);
            SimpleDeque16<uint64_t> tmp2 = SimpleDeque16<uint64_t>::load(data, pos);
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
        static VLCDeque load(std::ifstream &ifs)
        {
            VLCDeque r;
            ifs.read(reinterpret_cast<char *>(&r.first_gap), sizeof(r.first_gap));
            ifs.read(reinterpret_cast<char *>(&r.last_gap), sizeof(r.last_gap));
            SimpleDeque16<uint8_t> tmp1 = SimpleDeque16<uint8_t>::load(ifs);
            SimpleDeque16<uint64_t> tmp2 = SimpleDeque16<uint64_t>::load(ifs);

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
        static std::vector<VLCDeque> load_vector(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t size;
            std::memcpy(&size, data.data() + pos, sizeof(size));
            pos += sizeof(size);

            std::vector<VLCDeque> r;
            for (uint64_t i = 0; i < size; i++)
            {
                r.push_back(VLCDeque::load(data, pos));
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
        static std::vector<VLCDeque> load_vector(std::ifstream &ifs)
        {
            uint64_t size = 0;
            ifs.read(reinterpret_cast<char *>(&size), sizeof(size));

            std::vector<VLCDeque> r;
            for (uint64_t i = 0; i < size; i++)
            {
                r.push_back(VLCDeque::load(ifs));
            }
            return r;
        }
    };

}