#pragma once
#include "./lsb_byte.hpp"
#include "./msb_byte.hpp"
#include "./pext64.hpp"
#include <cassert>
#include <cstring>

namespace stool
{
    class IntegerSketch8
    {
        public:
        uint64_t sketch_value;
        uint64_t sketch_pos;
        uint8_t count;

        IntegerSketch8(uint64_t _sketch_value, uint64_t _sketch_pos, uint8_t _count){
            this->sketch_value = _sketch_value;
            this->sketch_pos = _sketch_pos;
            this->count = _count;
        }

        static IntegerSketch8 build(const std::vector<uint64_t> &values){
            if(values.size() > 8){
                throw std::runtime_error("values.size() is greater than 8");
            }
            for(uint64_t i = 1; i < values.size(); i++){
                if(values[i-1] > values[i]){
                    throw std::runtime_error("values[i-1] is greater than values[i]");
                }
            }

            uint64_t sketch_pos = 0;
            for(uint64_t i=1; i < values.size(); i++){
                uint64_t lcp_bits = values[i-1] ^ values[i];
                int64_t p = stool::Byte::number_of_leading_zero(lcp_bits);
                if(p != -1){
                    sketch_pos = sketch_pos | (1ULL << (63 - p));
                }
            }

            uint64_t sketch_value = 0;
            uint8_t prev_value = 0;

            for(uint64_t i = 0; i < values.size(); i++){
                uint8_t sub_sketch_value = stool::Pext64::pext64(values[i], sketch_pos);
                uint8_t diff = sub_sketch_value - prev_value;
                sketch_value = sketch_value | (diff << ((7 - i) * 8));
                prev_value = sub_sketch_value;
            }

            return IntegerSketch8(sketch_value, sketch_pos, values.size());


        }

    };

}
