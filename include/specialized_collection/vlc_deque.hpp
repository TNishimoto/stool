#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include "../basic/byte.hpp"
#include "../debug/print.hpp"
#include "./simple_deque.hpp"

namespace stool
{
    class VLCDeque
    {
        SimpleDeque16<uint8_t> value_length_deque;
        SimpleDeque16<uint64_t> code_deque;
        uint8_t first_gap = 0;
        uint8_t last_gap = 0;

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

        static std::string name()
        {
            return "VLC Deque";
        }
        uint64_t size_in_bytes() const
        {
            // return (sizeof(VLCDeque) + this->value_length_deque.size_in_bytes() + this->code_deque.size_in_bytes());

            return (sizeof(VLCDeque) + this->value_length_deque.size_in_bytes() + this->code_deque.size_in_bytes()) - (sizeof(this->code_deque) + sizeof(this->value_length_deque));
            // return sizeof(this->first_gap) + sizeof(this->last_gap) + sizeof(this->value_length_deque) + sizeof(this->code_deque) + (sizeof(uint8_t) * this->value_length_deque.size()) + (sizeof(uint64_t) * this->code_deque.size());
        }
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
        std::vector<uint64_t> to_value_vector() const
        {
            return this->to_vector();
        }
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

        void push_front(std::vector<uint64_t> &new_items)
        {
            for (auto i = new_items.rbegin(), e = new_items.rend(); i != e; ++i)
            {
                this->push_front(*i);
            }
        }
        void push_back(std::vector<uint64_t> &new_items)
        {
            for (uint64_t v : new_items)
            {
                this->push_back(v);
            }
        }
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
        void swap(VLCDeque &item)
        {
            this->value_length_deque.swap(item.value_length_deque);
            this->code_deque.swap(item.code_deque);
            std::swap(this->first_gap, item.first_gap);
            std::swap(this->last_gap, item.last_gap);
        }

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
        /*
        static uint8_t get_code_length(uint64_t code)
        {
            if (code > 0)
            {

                return 64 - __builtin_clzll(code);
            }
            else
            {
                return 0;
            }
        }
        */

        size_t size() const
        {
            return this->value_length_deque.size();
        }
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
        uint64_t tail() const
        {
            assert(this->size() > 0);
            VLCDequeIterator it = this->end();
            --it;

            return *it;
        }

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

        uint64_t at(uint64_t code_deque_pos1, uint8_t code_deque_pos2, uint8_t value_length) const
        {
            assert(code_deque_pos1 < this->code_deque.size());
            uint64_t code1 = this->code_deque[code_deque_pos1];
            uint64_t code2 = code_deque_pos1 + 1 < this->code_deque.size() ? this->code_deque[code_deque_pos1 + 1] : 0;
            return VLCDeque::access_value(code1, code2, code_deque_pos2, value_length);
        }

        uint64_t at(uint64_t i) const
        {
            std::pair<uint64_t, uint8_t> code_pos = this->get_code_starting_position(i);
            uint8_t value_length = this->value_length_deque[i];
            return this->at(code_pos.first, code_pos.second, value_length);
        }
        bool empty() const
        {
            return this->value_length_deque.empty();
        }
        void shrink_to_front()
        {
            this->value_length_deque.shrink_to_fit();
            this->code_deque.shrink_to_fit();
        }
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
        std::deque<uint64_t> to_deque() const
        {
            std::deque<uint64_t> r;
            for (uint64_t v : *this)
            {
                r.push_back(v);
            }
            return r;
        }
        std::vector<uint64_t> to_vector() const
        {

            std::vector<uint64_t> r;
            for (uint64_t v : *this)
            {
                r.push_back(v);
            }

            return r;
        }
        void insert(uint64_t pos, uint64_t value)
        {
            uint64_t value_len = stool::Byte::get_code_length(value);

            this->shift_right(pos, value_len);
            std::pair<uint64_t, uint8_t> code_pos = this->get_code_starting_position(pos);
            this->set_value(pos, code_pos.first, code_pos.second, value);

            assert(this->verify());
        }
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

        void set_value(uint64_t pos, uint64_t value)
        {
            this->remove(pos);
            this->insert(pos, value);
        }

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

                    /*
                    uint64_t rLen = 64 - code_pos.second;
                    uint64_t sgap = std::min(rLen, value_len);
                    uint64_t p1 = (this->code_deque[code_pos.first + 1] << (64 - sgap)) >> (64 - sgap);
                    this->code_deque[code_pos.first + 1] = this->code_deque[code_pos.first + 1] | (p1 << (64 - value_len));

                    uint64_t _left = (this->code_deque[code_pos.first] >> (64 - code_pos.second)) << (64 - code_pos.second);
                    uint64_t _right_tmp = this->code_deque[code_pos.first] << code_pos.second;
                    uint64_t _right = _right_tmp >> (code_pos.second + sgap);

                    std::cout << "Write(" << code_pos.first << "): " << std::bitset<64>(this->code_deque[code_pos.first]) << " -> " << std::bitset<64>(_left | _right) << "/" << std::bitset<64>(_left) << "/" << std::bitset<64>(_right) << std::endl;
                    this->code_deque[code_pos.first] = _left | _right;
                    */
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

        static uint64_t get_byte_size(const VLCDeque &item)
        {
            uint64_t bytes = sizeof(item.first_gap) + sizeof(item.last_gap) + SimpleDeque16<uint8_t>::get_byte_size(item.value_length_deque) + SimpleDeque16<uint64_t>::get_byte_size(item.code_deque);
            return bytes;
        }
        static uint64_t get_byte_size(const std::vector<VLCDeque> &items)
        {
            uint64_t bytes = sizeof(uint64_t);
            for (auto &it : items)
            {
                bytes += VLCDeque::get_byte_size(it);
            }
            return bytes;
        }

        static void save(const VLCDeque &item, std::vector<uint8_t> &output, uint64_t &pos)
        {

            std::memcpy(output.data() + pos, &item.first_gap, sizeof(item.first_gap));
            pos += sizeof(item.first_gap);
            std::memcpy(output.data() + pos, &item.last_gap, sizeof(item.last_gap));
            pos += sizeof(item.last_gap);
            SimpleDeque16<uint8_t>::save(item.value_length_deque, output, pos);
            SimpleDeque16<uint64_t>::save(item.code_deque, output, pos);
        }
        static void save(const VLCDeque &item, std::ofstream &os)
        {
            os.write(reinterpret_cast<const char *>(&item.first_gap), sizeof(item.first_gap));
            os.write(reinterpret_cast<const char *>(&item.last_gap), sizeof(item.last_gap));
            SimpleDeque16<uint8_t>::save(item.value_length_deque, os);
            SimpleDeque16<uint64_t>::save(item.code_deque, os);
        }

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
        static void save(const std::vector<VLCDeque> &items, std::ofstream &os)
        {
            uint64_t size = items.size();
            os.write(reinterpret_cast<const char *>(&size), sizeof(size));
            for (auto &it : items)
            {
                VLCDeque::save(it, os);
            }
        }

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