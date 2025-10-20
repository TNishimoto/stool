#pragma once
#include "./lsb_byte.hpp"
#include "./msb_byte.hpp"
#include "./pext64.hpp"
#include "./packed_search.hpp"
#include "../debug/debug_printer.hpp"
#include <cassert>
#include <cstring>

namespace stool
{
    class IntegerSketch8
    {
    public:
        uint64_t sketch_diff;
        uint64_t sketch_pos;
        uint8_t count;

        IntegerSketch8()
        {
            this->sketch_diff = 0;
            this->sketch_pos = 0;
            this->count = 0;
        }

        IntegerSketch8(uint64_t _sketch_diff, uint64_t _sketch_pos, uint8_t _count)
        {
            this->sketch_diff = _sketch_diff;
            this->sketch_pos = _sketch_pos;
            this->count = _count;
        }
        int64_t successor(uint64_t v, const std::vector<uint64_t> &sorted_values) const
        {
            if(this->count == 0) return -1;
            uint8_t v_sketch_value = stool::Pext64::pext64(v, this->sketch_pos);
            uint8_t p_idx = geq_successor_on_sketch_values(v_sketch_value);
            uint64_t found_value = p_idx < this->count ? sorted_values[p_idx] : UINT64_MAX;

            if (p_idx < this->count && v == found_value)
            {
                return p_idx;
            }
            else
            {
                uint64_t lcp_among_values = this->get_lcp_among_values(v, p_idx, sorted_values);
                assert(lcp_among_values < 64);
                assert(lcp_among_values == IntegerSketch8::get_naive_lcp_among_values(v, sorted_values));

                uint64_t mask = UINT64_MAX << (63 - lcp_among_values);
                uint64_t mask2 = UINT64_MAX >> (lcp_among_values + 1);

                uint64_t e = 0;
                if (lcp_among_values == 63)
                {
                    e = v;
                }
                else
                {
                    bool b = MSBByte::get_bit(v, lcp_among_values);
                    if (b)
                    {
                        e = (v & mask) | mask2;
                    }
                    else
                    {
                        e = v & mask;
                    }
                }

                uint8_t e_sketch_value = stool::Pext64::pext64(e, this->sketch_pos);
                assert(e_sketch_value < UINT8_MAX);

                uint8_t q_idx = geq_successor_on_sketch_values(e_sketch_value);
                uint64_t q = q_idx < this->count ? sorted_values[q_idx] : UINT64_MAX;

                if (v <= q)
                {
                    return q_idx < this->count ? q_idx : -1;
                }
                else
                {
                    uint8_t proper_successor = proper_successor_on_sketch_values(q_idx);
                    return proper_successor < this->count ? proper_successor : -1;
                }
            }

        }


        void print(const std::vector<uint64_t> &sorted_values) const
        {
            std::cout << "POS    : " << stool::Byte::to_bit_string(this->sketch_pos, true) << std::endl;
            uint64_t sum = 0;
            for (uint64_t i = 0; i < this->count; i++)
            {
                uint8_t i_sketch_diff = this->sketch_diff >> ((7 - i) * 8);
                sum += i_sketch_diff;
                std::cout << stool::LSBByte::to_bit_string(sum, 8) << " ";
                std::cout << stool::Byte::to_bit_string(sorted_values[i], true) << " ";
                std::cout << sorted_values[i] << " ";
                std::cout << "S: " << sum << std::endl;
            }
        }

        static IntegerSketch8 build(const std::vector<uint64_t> &values)
        {
            if (values.size() > 8)
            {
                throw std::runtime_error("values.size() is greater than 8");
            }
            for (uint64_t i = 1; i < values.size(); i++)
            {
                if (values[i - 1] > values[i])
                {
                    throw std::runtime_error("values[i-1] is greater than values[i]");
                }
            }

            uint64_t sketch_pos = 0;
            for (uint64_t i = 1; i < values.size(); i++)
            {
                uint64_t lcp_value = stool::IntegerSketch8::lcp(values[i - 1], values[i]);
                if (lcp_value != 64)
                {
                    sketch_pos = sketch_pos | (1ULL << (63 - lcp_value));
                }
            }

            uint64_t sketch_diff = 0;
            uint8_t prev_value = 0;

            for (uint64_t i = 0; i < values.size(); i++)
            {
                uint8_t sub_sketch_value = stool::Pext64::pext64(values[i], sketch_pos);
                uint8_t diff = sub_sketch_value - prev_value;
                sketch_diff = sketch_diff | (((uint64_t)diff) << ((7 - i) * 8));
                prev_value = sub_sketch_value;
            }

            return IntegerSketch8(sketch_diff, sketch_pos, values.size());
        }
    private:

        static uint8_t lcp(uint64_t left, uint64_t right)
        {
            if (left == right)
            {
                return 64;
            }
            else
            {
                uint64_t lcp_bits = left ^ right;
                int64_t p = stool::Byte::number_of_leading_zero(lcp_bits);
                return p;
            }
        }
        static uint8_t get_naive_lcp_among_values(uint64_t v, const std::vector<uint64_t> &values)
        {
            uint8_t max_lcp = 0;
            for (uint64_t i = 0; i < values.size(); i++)
            {
                max_lcp = std::max(max_lcp, IntegerSketch8::lcp(v, values[i]));
            }
            return max_lcp;
        }
        uint8_t get_lcp_among_values(uint64_t v, uint8_t v_sketch_geq_successor_index, const std::vector<uint64_t> &sorted_values) const
        {
            assert(this->count > 0);
            if (v_sketch_geq_successor_index == this->count)
            {
                return IntegerSketch8::lcp(sorted_values[this->count - 1], v);
            }
            else
            {
                uint64_t p_value = sorted_values[v_sketch_geq_successor_index];
                if (v < p_value)
                {
                    uint64_t left_lcp = v_sketch_geq_successor_index == 0 ? 0 : IntegerSketch8::lcp(sorted_values[v_sketch_geq_successor_index - 1], v);
                    uint64_t right_lcp = IntegerSketch8::lcp(sorted_values[v_sketch_geq_successor_index], v);
                    return std::max(left_lcp, right_lcp);
                }
                else if (v > p_value)
                {
                    uint64_t left_lcp = IntegerSketch8::lcp(sorted_values[v_sketch_geq_successor_index], v);
                    uint64_t right_lcp = v_sketch_geq_successor_index + 1 < this->count ? IntegerSketch8::lcp(sorted_values[v_sketch_geq_successor_index + 1], v) : 0;
                    return std::max(left_lcp, right_lcp);
                }
                else
                {
                    return 64;
                }
            }
        }

        uint8_t geq_successor_on_sketch_values(uint64_t v) const
        {
            int64_t p = stool::PackedSearch::find_prefix_ge_y_nib8x8_branchless(this->sketch_diff, v);
            if (p < this->count)
            {
                return p;
            }
            else
            {
                return this->count;
            }
        }
        std::vector<uint64_t> get_sketch_values() const
        {
            std::vector<uint64_t> sketch_values;
            uint64_t sum = 0;
            for (uint64_t i = 0; i < this->count; i++)
            {
                uint8_t i_sketch_diff = this->sketch_diff >> ((7 - i) * 8);
                sum += i_sketch_diff;
                sketch_values.push_back(sum);
            }

            return sketch_values;
        }
        uint8_t proper_successor_on_sketch_values(uint64_t idx) const{
            assert(idx < this->count);
            if(idx == 7){
                return 8;
            }else{
                uint64_t modified_sketch_diff = (this->sketch_diff << ((idx+1) * 8)) | (0b1ULL << (idx * 8));
                uint8_t successor = stool::PackedSearch::find_prefix_ge_y_nib8x8_branchless(modified_sketch_diff, 1);
                return successor + idx + 1;

                
            }

        }

        
    };

}
