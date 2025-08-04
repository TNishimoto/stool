#pragma once
#include <iostream>
#include <cassert>
#include <numeric> // std::gcd in C++17

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#define USE_NEON 1
#else
#define USE_NEON 0
#endif

namespace stool
{
    class SIMDFunctions
    {
    public:
    static uint64_t sum_16_8bits_with_no_overflow(uint8_t *buffer, uint64_t pos, [[maybe_unused]] uint64_t buffer_size)
    {
        uint64_t sum = 0;
        assert(pos + 16 < buffer_size);

        #if USE_NEON
        uint8x16_t acc = vld1q_u8(&buffer[pos]);
        sum = vaddvq_u8(acc);

        #else
        for (uint64_t x = 0; x < 16; x++)
        {
            sum += buffer[pos + x];
        }
        #endif
        return sum;
    }

    static uint64_t sum_16_8bits_with_overflow(uint8_t *buffer, uint64_t pos, [[maybe_unused]] uint64_t buffer_size)
    {
        uint64_t sum = 0;
        assert(pos + 16 < buffer_size);

        #if USE_NEON
        uint16x8_t acc = vdupq_n_u16(0);
        uint8x16_t vec8 = vld1q_u8(&buffer[pos]);     // 8要素ロード
        uint16x8_t lo = vmovl_u8(vget_low_u8(vec8)); // 下位4つを32bitに昇格
        uint16x8_t hi = vmovl_u8(vget_high_u8(vec8));
        acc = vaddq_u16(acc, lo);
        acc = vaddq_u16(acc, hi);
        sum = vaddvq_u16(acc);


        #else
        for (uint64_t x = 0; x < 16; x++)
        {
            sum += buffer[pos + x];
        }
        #endif
        return sum;
    }

    static uint64_t sum_4_32bits_with_no_overflow(uint32_t *buffer, uint64_t pos, [[maybe_unused]] uint64_t buffer_size)
    {
        uint64_t sum = 0;
        assert(pos + 4 < buffer_size);

        #if USE_NEON
        uint32x4_t acc = vld1q_u32(&buffer[pos]);
        sum = vaddvq_u32(acc);

        #else
        for (uint64_t x = 0; x < 4; x++)
        {
            sum += buffer[pos + x];
        }
        #endif
        return sum;
    }
    static uint64_t sum_4_32bits_with_overflow(uint32_t *buffer, uint64_t pos, [[maybe_unused]] uint64_t buffer_size)
    {
        uint64_t sum = 0;
        assert(pos + 4 < buffer_size);

        #if USE_NEON
        uint64x2_t acc = vdupq_n_u64(0);
        uint32x4_t vec32 = vld1q_u32(&buffer[pos]);     // 8要素ロード
        uint64x2_t lo = vmovl_u32(vget_low_u32(vec32)); // 下位4つを32bitに昇格
        uint64x2_t hi = vmovl_u32(vget_high_u32(vec32));
        acc = vaddq_u64(acc, lo);
        acc = vaddq_u64(acc, hi);
        sum = vaddvq_u64(acc);

        #else
        for (uint64_t x = 0; x < 4; x++)
        {
            sum += buffer[pos + x];
        }
        #endif
        return sum;
    }
    




    static uint64_t sum_8_16bits_with_no_overflow(uint16_t *buffer, uint64_t pos, [[maybe_unused]] uint64_t buffer_size)
        {

            uint64_t sum = 0;
            assert(pos + 8 < buffer_size);

#if USE_NEON
            uint16x8_t acc = vld1q_u16(&buffer[pos]);
            sum = vaddvq_u16(acc);

#else
            for (uint64_t x = 0; x < 8; x++)
            {
                sum += buffer[pos + x];
            }
#endif
            return sum;

#if DEBUG
            uint64_t _sum = 0;
            for (uint64_t j = 0; j < 8; j++)
            {
                _sum += buffer[pos + j];
            }
            if (_sum != sum)
            {
                throw std::runtime_error("sum_8_16bits, Error: _sum != psum");
            }
#endif
            return sum;
        }

        static uint64_t sum_8_16bits_with_overflow(uint16_t *buffer, uint64_t pos, [[maybe_unused]] uint64_t buffer_size)
        {
            uint64_t sum = 0;
            assert(pos + 8 < buffer_size);

#if USE_NEON
            uint32x4_t acc = vdupq_n_u32(0);
            uint16x8_t vec16 = vld1q_u16(&buffer[pos]);     // 8要素ロード
            uint32x4_t lo = vmovl_u16(vget_low_u16(vec16)); // 下位4つを32bitに昇格
            uint32x4_t hi = vmovl_u16(vget_high_u16(vec16));
            acc = vaddq_u32(acc, lo);
            acc = vaddq_u32(acc, hi);
            sum = vaddvq_u32(acc);

#else
            for (uint64_t x = 0; x < 8; x++)
            {
                sum += buffer[pos + x];
            }
#endif
            return sum;

#if DEBUG
            uint64_t _sum = 0;
            for (uint64_t j = 0; j < 8; j++)
            {
                _sum += buffer[pos + j];
            }
            if (_sum != sum)
            {
                throw std::runtime_error("sum_8_16bits, Error: _sum != psum");
            }
#endif
            return sum;
        }

        static int64_t cyclic_search_16(uint16_t *buffer, uint64_t starting_position, uint64_t buffer_size, uint64_t element_count, bool overflow_flag, uint64_t value, uint64_t &sum)
        {
            uint64_t mask16 = buffer_size - 1;
            uint64_t j = 0;

            if (overflow_flag)
            {
                while (j + 8 < element_count)
                {
                    uint64_t v = stool::SIMDFunctions::sum_8_16bits_with_overflow(buffer, (starting_position + j) & mask16, buffer_size);
                    if (value <= sum + v)
                    {
                        break;
                    }
                    j += 8;
                    sum += v;
                }
            }
            else
            {
                while (j + 8 < element_count)
                {
                    uint64_t v = stool::SIMDFunctions::sum_8_16bits_with_no_overflow(buffer, (starting_position + j) & mask16, buffer_size);
                    if (value <= sum + v)
                    {
                        break;
                    }
                    j += 8;
                    sum += v;
                }
            }
            uint8_t width = std::min(8ULL, (uint64_t)element_count - j);

            for (uint16_t x = 0; x < width; x++)
            {
                uint16_t v = buffer[(starting_position + j) & mask16];
                if (value <= sum + v)
                {
                    return j;
                }
                j++;
                sum += v;
            }
            return -1;
        }
        static int64_t cyclic_search_32(uint32_t *buffer, uint64_t starting_position, uint64_t buffer_size, uint64_t element_count, bool overflow_flag, uint64_t value, uint64_t &sum)
        {
            uint64_t mask = buffer_size - 1;
            uint64_t j = 0;

            if (overflow_flag)
            {
                while (j + 8 < element_count)
                {
                    uint64_t v = stool::SIMDFunctions::sum_4_32bits_with_overflow(buffer, (starting_position + j) & mask, buffer_size);
                    if (value <= sum + v)
                    {
                        break;
                    }
                    j += 4;
                    sum += v;
                }
            }
            else
            {
                while (j + 8 < element_count)
                {
                    uint64_t v = stool::SIMDFunctions::sum_4_32bits_with_no_overflow(buffer, (starting_position + j) & mask, buffer_size);
                    if (value <= sum + v)
                    {
                        break;
                    }
                    j += 4;
                    sum += v;
                }
            }
            uint8_t width = std::min(4ULL, (uint64_t)element_count - j);

            for (uint16_t x = 0; x < width; x++)
            {
                uint32_t v = buffer[(starting_position + j) & mask];
                if (value <= sum + v)
                {
                    return j;
                }
                j++;
                sum += v;
            }
            return -1;
        }

        static int64_t cyclic_search_8(uint8_t *buffer, uint64_t starting_position, uint64_t buffer_size, uint64_t element_count, bool overflow_flag, uint64_t value, uint64_t &sum)
        {
            uint64_t mask = buffer_size - 1;
            uint64_t j = 0;

            if (overflow_flag)
            {
                while (j + 16 < element_count)
                {
                    uint64_t v = stool::SIMDFunctions::sum_16_8bits_with_overflow(buffer, (starting_position + j) & mask, buffer_size);
                    if (value <= sum + v)
                    {
                        break;
                    }
                    j += 8;
                    sum += v;
                }
            }
            else{
                while (j + 16 < element_count)
                {
                    uint64_t v = stool::SIMDFunctions::sum_16_8bits_with_no_overflow(buffer, (starting_position + j) & mask, buffer_size);
                    if (value <= sum + v)
                    {
                        break;
                    }
                    j += 8;
                    sum += v;
                }
            }
            uint8_t width = std::min(16ULL, (uint64_t)element_count - j);

            for (uint16_t x = 0; x < width; x++)
            {
                uint8_t v = buffer[(starting_position + j) & mask];
                if (value <= sum + v)
                {
                    return j;
                }
                j++;
                sum += v;
            }
            return -1;
        }

    };
}