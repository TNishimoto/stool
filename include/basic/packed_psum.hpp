#pragma once
#include "./lsb_byte.hpp"
#include <cassert>
#include <cstring>

namespace stool
{
    enum class PackedBitType
    {
        BIT_1 = 0,
        BIT_2 = 1,
        BIT_4 = 2,
        BIT_8 = 3,
        BIT_16 = 4,
        BIT_32 = 5,
        BIT_64 = 6
    };

    class PackedPsum
    {
    public:
        static PackedBitType get_code_type(uint64_t value)
        {
            if (value <= 2)
            {
                return PackedBitType::BIT_1;
            }
            else if (value <= 4)
            {
                return PackedBitType::BIT_2;
            }
            else if (value <= 16)
            {
                return PackedBitType::BIT_4;
            }
            else if (value <= UINT8_MAX)
            {
                return PackedBitType::BIT_8;
            }
            else if (value <= UINT16_MAX)
            {
                return PackedBitType::BIT_16;
            }
            else if (value <= UINT32_MAX)
            {
                return PackedBitType::BIT_32;
            }
            else
            {
                return PackedBitType::BIT_64;
            }
        }

        static uint64_t sum32x2bits(uint64_t bits)
        {
            uint64_t lsb = bits & 0x5555555555555555ULL;

            uint64_t msb = (bits >> 1) & 0x5555555555555555ULL;

            uint64_t lsb_sum = stool::Byte::count_bits(lsb);
            uint64_t msb_sum = stool::Byte::count_bits(msb);

            return lsb_sum + (2 * msb_sum);
        }
        static inline uint64_t sum16x4bits(uint64_t x)
        {
            const uint64_t mask = 0x0F0F0F0F0F0F0F0FULL;
            uint64_t lo = x & mask;
            uint64_t hi = (x >> 4) & mask;
            uint64_t bytes = lo + hi;

            return (bytes * 0x0101010101010101ULL) >> 56;
        }
        static inline uint32_t sum8x8bits(uint64_t x)
        {
            x = (x & 0x00FF00FF00FF00FFULL) + ((x >> 8) & 0x00FF00FF00FF00FFULL);
            x = (x & 0x0000FFFF0000FFFFULL) + ((x >> 16) & 0x0000FFFF0000FFFFULL);
            x = (x & 0x00000000FFFFFFFFULL) + (x >> 32);
            return static_cast<uint32_t>(x); // or (uint32_t)(x & 0x7FF)
        }

        static inline uint32_t sum4x16bits(uint64_t x)
        {
            x = (x & 0x0000FFFF0000FFFFULL) + ((x >> 16) & 0x0000FFFF0000FFFFULL);
            x = (x & 0x00000000FFFFFFFFULL) + (x >> 32);
            return static_cast<uint32_t>(x);
        }
        static inline uint64_t sum2x32bits(uint64_t x)
        {
            uint64_t L = x >> 32;
            uint64_t R = x & (UINT64_MAX >> 32);
            return static_cast<uint64_t>(L + R);
        }

        static uint64_t psum64x1bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = i / 64;
            uint64_t bit_index = i % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += stool::Byte::count_bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += stool::Byte::count_bits(last_block);
            return sum;
        }
        static uint64_t psum64x1bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = i / 64;
            uint64_t start_bit_index = i % 64;
            uint64_t end_block_index = j / 64;
            uint64_t end_bit_index = j % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);


            if(start_block_index < end_block_index){
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += stool::Byte::count_bits(modified_start_block);

                for (uint64_t k = start_block_index+1; k < end_block_index; k++)
                {
                    sum += stool::Byte::count_bits(bits[k]);
                }    

                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);           
                sum += stool::Byte::count_bits(modified_last_block);    
            }else{
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += stool::Byte::count_bits(modified_last_block);
            }
            return sum;
        }


        static uint64_t psum32x2bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 2) / 64;
            uint64_t bit_index = ((i * 2) + 1) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);
            
            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum32x2bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum32x2bits(last_block);
            return sum;
        }
        static uint64_t psum32x2bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i*2) / 64;
            uint64_t start_bit_index = (i*2) % 64;
            uint64_t end_block_index = (j*2) / 64;
            uint64_t end_bit_index = ((j*2) + 1) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);


            if(start_block_index < end_block_index){
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum32x2bits(modified_start_block);
    
                for (uint64_t k = start_block_index+1; k < end_block_index; k++)
                {
                    sum += sum32x2bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);           
                sum += sum32x2bits(modified_last_block);    
            }else{
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum32x2bits(modified_last_block);
            }


            return sum;
        }


        static uint64_t psum16x4bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 4) / 64;
            uint64_t bit_index = ((i * 4) + 3) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum16x4bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum16x4bits(last_block);
            return sum;
        }

        static uint64_t psum16x4bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i*4) / 64;
            uint64_t start_bit_index = (i*4) % 64;
            uint64_t end_block_index = (j*4) / 64;
            uint64_t end_bit_index = ((j*4) + 3) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if(start_block_index < end_block_index){
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum16x4bits(modified_start_block);
    
                for (uint64_t k = start_block_index+1; k < end_block_index; k++)
                {
                    sum += sum16x4bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);           
                sum += sum16x4bits(modified_last_block);    
            }else{
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum16x4bits(modified_last_block);
            }


            return sum;
        }


        static uint64_t psum8x8bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 8) / 64;
            uint64_t bit_index = ((i * 8) + 7) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum8x8bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum8x8bits(last_block);
            return sum;
        }

        static uint64_t psum8x8bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i*8) / 64;
            uint64_t start_bit_index = (i*8) % 64;
            uint64_t end_block_index = (j*8) / 64;
            uint64_t end_bit_index = ((j*8) + 7) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if(start_block_index < end_block_index){
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum8x8bits(modified_start_block);
    
                for (uint64_t k = start_block_index+1; k < end_block_index; k++)
                {
                    sum += sum8x8bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);           
                sum += sum8x8bits(modified_last_block);    
            }else{
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum8x8bits(modified_last_block);
            }

            return sum;
        }


        static uint64_t psum4x16bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 16) / 64;
            uint64_t bit_index = ((i * 16) + 15) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum4x16bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum4x16bits(last_block);
            return sum;
        }

        static uint64_t psum4x16bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i*16) / 64;
            uint64_t start_bit_index = (i*16) % 64;
            uint64_t end_block_index = (j*16) / 64;
            uint64_t end_bit_index = ((j*16) + 15) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);


            if(start_block_index < end_block_index){
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum4x16bits(modified_start_block);
    
                for (uint64_t k = start_block_index+1; k < end_block_index; k++)
                {
                    sum += sum4x16bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);           
                sum += sum4x16bits(modified_last_block);    
            }else{
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum4x16bits(modified_last_block);
            }

            return sum;
        }


        static uint64_t psum2x32bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t block_index = (i * 32) / 64;
            uint64_t bit_index = ((i * 32) + 31) % 64;
            uint64_t sum = 0;
            assert(block_index < array_size);

            for (uint64_t j = 0; j < block_index; j++)
            {
                sum += sum2x32bits(bits[j]);
            }
            uint64_t last_block = bits[block_index] >> (63 - bit_index);
            sum += sum2x32bits(last_block);
            return sum;
        }
        static uint64_t psum2x32bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i*32) / 64;
            uint64_t start_bit_index = (i*32) % 64;
            uint64_t end_block_index = (j*32) / 64;
            uint64_t end_bit_index = ((j*32) + 31) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if(start_block_index < end_block_index){
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum2x32bits(modified_start_block);
    
                for (uint64_t k = start_block_index+1; k < end_block_index; k++)
                {
                    sum += sum2x32bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);           
                sum += sum2x32bits(modified_last_block);    
            }else{
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum2x32bits(modified_last_block);
            }

            return sum;
        }


        static uint64_t psum1x64bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            for (uint64_t j = 0; j <= i; j++)
            {
                sum += bits[j];
            }
            return sum;
        }
        static uint64_t psum1x64bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            for (uint64_t k = i; k <= j; k++)
            {
                sum += bits[k];
            }
            return sum;
        }


        static uint64_t psum(uint64_t *bits, uint64_t i, PackedBitType bit_type, [[maybe_unused]] uint64_t array_size)
        {
            switch (bit_type)
            {
            case PackedBitType::BIT_1:
                return psum64x1bits(bits, i, array_size);
            case PackedBitType::BIT_2:
                return psum32x2bits(bits, i, array_size);
            case PackedBitType::BIT_4:
                return psum16x4bits(bits, i, array_size);
            case PackedBitType::BIT_8:
                return psum8x8bits(bits, i, array_size);
            case PackedBitType::BIT_16:
                return psum4x16bits(bits, i, array_size);
            case PackedBitType::BIT_32:
                return psum2x32bits(bits, i, array_size);
            case PackedBitType::BIT_64:
                return psum1x64bits(bits, i, array_size);
            default:
                break;
            }
            throw std::invalid_argument("Invalid bit type");
        }
        static uint64_t psum(uint64_t *bits, uint64_t i, uint64_t j, PackedBitType bit_type, [[maybe_unused]] uint64_t array_size)
        {
            switch (bit_type)
            {
            case PackedBitType::BIT_1:
                return psum64x1bits(bits, i, j, array_size);
            case PackedBitType::BIT_2:
                return psum32x2bits(bits, i, j, array_size);
            case PackedBitType::BIT_4:
                return psum16x4bits(bits, i, j, array_size);
            case PackedBitType::BIT_8:
                return psum8x8bits(bits, i, j, array_size);
            case PackedBitType::BIT_16:
                return psum4x16bits(bits, i, j, array_size);
            case PackedBitType::BIT_32:
                return psum2x32bits(bits, i, j, array_size);
            case PackedBitType::BIT_64:
                return psum1x64bits(bits, i, j, array_size);
            default:
                break;
            }
            throw std::invalid_argument("Invalid bit type");
        }

    };
}
