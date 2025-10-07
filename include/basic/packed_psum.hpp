#pragma once
#include "./lsb_byte.hpp"
#include "./msb_byte.hpp"
#include <cassert>
#include <cstring>

namespace stool
{

    /*!
     * @brief A class for managing packed partial sums with variable bit widths
     * 
     * This class provides functionality to store and process sequences of integers using bit packing techniques.
     * It supports different bit widths (1,2,4,8,16,32,64) to efficiently store values based on their magnitude.
     * The class includes methods for:
     * - Determining optimal bit width for values
     * - Packing/unpacking values
     * - Computing partial sums
     * - Searching within packed sequences
     */
    class PackedPSum
    {
    public:
        /**
         * @brief Enumeration of supported bit widths for packed storage
         * 
         * Defines the available bit widths for packing integer values.
         * Each type corresponds to a specific number of bits used per value.
         */
        enum class PackedBitType
        {
            BIT_1 = 0,   ///< 1-bit per value (values 0-1)
            BIT_2 = 1,   ///< 2-bits per value (values 0-3)
            BIT_4 = 2,   ///< 4-bits per value (values 0-15)
            BIT_8 = 3,   ///< 8-bits per value (values 0-255)
            BIT_16 = 4,  ///< 16-bits per value (values 0-65535)
            BIT_32 = 5,  ///< 32-bits per value (values 0-4294967295)
            BIT_64 = 6   ///< 64-bits per value (full uint64_t range)
        };

        /**
         * @brief Determines the optimal bit width for storing a given value
         * 
         * Analyzes the input value and returns the minimum bit width required
         * to store it efficiently. This helps optimize storage by using the
         * smallest possible representation.
         * 
         * @param value The value to analyze
         * @return PackedBitType The optimal bit width for the value
         * 
         * @note Values are mapped as follows:
         *       - 0-1: BIT_1
         *       - 2-3: BIT_2
         *       - 4-15: BIT_4
         *       - 16-255: BIT_8
         *       - 256-65535: BIT_16
         *       - 65536-4294967295: BIT_32
         *       - Above 4294967295: BIT_64
         */
        static PackedBitType get_code_type(uint64_t value)
        {
            if (value <= 1)
            {
                return PackedBitType::BIT_1;
            }
            else if (value <= 3)
            {
                return PackedBitType::BIT_2;
            }
            else if (value <= 15)
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

        /**
         * @brief Computes the sum of 32 2-bit values packed in a 64-bit word
         * 
         * Efficiently calculates the sum of thirty-two 2-bit values stored
         * in a single 64-bit integer using bit manipulation techniques.
         * 
         * @param bits 64-bit word containing 32 packed 2-bit values
         * @return uint64_t Sum of all 32 values
         * 
         * @note Each 2-bit value can range from 0-3, so maximum sum is 96
         */
        static uint64_t sum32x2bits(uint64_t bits)
        {
            uint64_t lsb = bits & 0x5555555555555555ULL;

            uint64_t msb = (bits >> 1) & 0x5555555555555555ULL;

            uint64_t lsb_sum = stool::Byte::count_bits(lsb);
            uint64_t msb_sum = stool::Byte::count_bits(msb);

            return lsb_sum + (2 * msb_sum);
        }
        /**
         * @brief Computes the sum of 16 4-bit values packed in a 64-bit word
         * 
         * Efficiently calculates the sum of sixteen 4-bit values (nibbles)
         * stored in a single 64-bit integer.
         * 
         * @param x 64-bit word containing 16 packed 4-bit values
         * @return uint64_t Sum of all 16 values
         * 
         * @note Each 4-bit value can range from 0-15, so maximum sum is 240
         */
        static inline uint64_t sum16x4bits(uint64_t x)
        {
            const uint64_t mask = 0x0F0F0F0F0F0F0F0FULL;
            uint64_t lo = x & mask;
            uint64_t hi = (x >> 4) & mask;
            uint64_t bytes = lo + hi;

            return (bytes * 0x0101010101010101ULL) >> 56;
        }
        /**
         * @brief Computes the sum of 8 8-bit values packed in a 64-bit word
         * 
         * Efficiently calculates the sum of eight 8-bit values (bytes)
         * stored in a single 64-bit integer.
         * 
         * @param x 64-bit word containing 8 packed 8-bit values
         * @return uint32_t Sum of all 8 values
         * 
         * @note Each 8-bit value can range from 0-255, so maximum sum is 2040
         */
        static inline uint32_t sum8x8bits(uint64_t x)
        {
            x = (x & 0x00FF00FF00FF00FFULL) + ((x >> 8) & 0x00FF00FF00FF00FFULL);
            x = (x & 0x0000FFFF0000FFFFULL) + ((x >> 16) & 0x0000FFFF0000FFFFULL);
            x = (x & 0x00000000FFFFFFFFULL) + (x >> 32);
            return static_cast<uint32_t>(x); // or (uint32_t)(x & 0x7FF)
        }

        /**
         * @brief Computes the sum of 4 16-bit values packed in a 64-bit word
         * 
         * Efficiently calculates the sum of four 16-bit values
         * stored in a single 64-bit integer.
         * 
         * @param x 64-bit word containing 4 packed 16-bit values
         * @return uint32_t Sum of all 4 values
         * 
         * @note Each 16-bit value can range from 0-65535, so maximum sum is 262140
         */
        static inline uint32_t sum4x16bits(uint64_t x)
        {
            x = (x & 0x0000FFFF0000FFFFULL) + ((x >> 16) & 0x0000FFFF0000FFFFULL);
            x = (x & 0x00000000FFFFFFFFULL) + (x >> 32);
            return static_cast<uint32_t>(x);
        }
        /**
         * @brief Computes the sum of 2 32-bit values packed in a 64-bit word
         * 
         * Efficiently calculates the sum of two 32-bit values
         * stored in a single 64-bit integer.
         * 
         * @param x 64-bit word containing 2 packed 32-bit values
         * @return uint64_t Sum of both values
         * 
         * @note Each 32-bit value can range from 0-4294967295
         */
        static inline uint64_t sum2x32bits(uint64_t x)
        {
            uint64_t L = x >> 32;
            uint64_t R = x & (UINT64_MAX >> 32);
            return static_cast<uint64_t>(L + R);
        }

        /**
         * @brief Computes partial sum of 1-bit values up to position i
         * 
         * Calculates the sum of all 1-bit values from position 0 to i (inclusive)
         * in a packed bit array. This is equivalent to counting set bits.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 1-bit values
         * @param i Position up to which to compute the sum (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position 0 to i
         * 
         * @note Position i is in terms of individual bits, not array indices
         */
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
        /**
         * @brief Computes partial sum of 1-bit values between positions i and j
         * 
         * Calculates the sum of all 1-bit values from position i to j (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 1-bit values
         * @param i Starting position (inclusive)
         * @param j Ending position (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position i to j
         * 
         * @note Positions are in terms of individual bits, not array indices
         */
        static uint64_t psum64x1bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = i / 64;
            uint64_t start_bit_index = i % 64;
            uint64_t end_block_index = j / 64;
            uint64_t end_bit_index = j % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += stool::Byte::count_bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += stool::Byte::count_bits(bits[k]);
                }

                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += stool::Byte::count_bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += stool::Byte::count_bits(modified_last_block);
            }
            return sum;
        }

        /**
         * @brief Computes partial sum of 2-bit values up to position i
         * 
         * Calculates the sum of all 2-bit values from position 0 to i (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 2-bit values
         * @param i Position up to which to compute the sum (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position 0 to i
         * 
         * @note Position i refers to the i-th 2-bit value, not bit position
         */
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
        /**
         * @brief Computes partial sum of 2-bit values between positions i and j
         * 
         * Calculates the sum of all 2-bit values from position i to j (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 2-bit values
         * @param i Starting position (inclusive)
         * @param j Ending position (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position i to j
         * 
         * @note Positions refer to 2-bit value indices, not bit positions
         */
        static uint64_t psum32x2bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 2) / 64;
            uint64_t start_bit_index = (i * 2) % 64;
            uint64_t end_block_index = (j * 2) / 64;
            uint64_t end_bit_index = ((j * 2) + 1) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum32x2bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum32x2bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum32x2bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum32x2bits(modified_last_block);
            }

            return sum;
        }

        /**
         * @brief Computes partial sum of 4-bit values up to position i
         * 
         * Calculates the sum of all 4-bit values from position 0 to i (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 4-bit values
         * @param i Position up to which to compute the sum (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position 0 to i
         * 
         * @note Position i refers to the i-th 4-bit value (nibble), not bit position
         */
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
        /**
         * @brief Computes partial sum of 4-bit values between positions i and j
         * 
         * Calculates the sum of all 4-bit values from position i to j (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 4-bit values
         * @param i Starting position (inclusive)
         * @param j Ending position (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position i to j
         * 
         * @note Positions refer to 4-bit value indices, not bit positions
         */
        static uint64_t psum16x4bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 4) / 64;
            uint64_t start_bit_index = (i * 4) % 64;
            uint64_t end_block_index = (j * 4) / 64;
            uint64_t end_bit_index = ((j * 4) + 3) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum16x4bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum16x4bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum16x4bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum16x4bits(modified_last_block);
            }

            return sum;
        }

        /**
         * @brief Computes partial sum of 8-bit values up to position i
         * 
         * Calculates the sum of all 8-bit values from position 0 to i (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 8-bit values
         * @param i Position up to which to compute the sum (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position 0 to i
         * 
         * @note Position i refers to the i-th 8-bit value (byte), not bit position
         */
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
        /**
         * @brief Computes partial sum of 8-bit values between positions i and j
         * 
         * Calculates the sum of all 8-bit values from position i to j (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 8-bit values
         * @param i Starting position (inclusive)
         * @param j Ending position (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position i to j
         * 
         * @note Positions refer to 8-bit value indices, not bit positions
         */
        static uint64_t psum8x8bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 8) / 64;
            uint64_t start_bit_index = (i * 8) % 64;
            uint64_t end_block_index = (j * 8) / 64;
            uint64_t end_bit_index = ((j * 8) + 7) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum8x8bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum8x8bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum8x8bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum8x8bits(modified_last_block);
            }

            return sum;
        }

        /**
         * @brief Computes partial sum of 16-bit values up to position i
         * 
         * Calculates the sum of all 16-bit values from position 0 to i (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 16-bit values
         * @param i Position up to which to compute the sum (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position 0 to i
         * 
         * @note Position i refers to the i-th 16-bit value, not bit position
         */
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
        /**
         * @brief Computes partial sum of 16-bit values between positions i and j
         * 
         * Calculates the sum of all 16-bit values from position i to j (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 16-bit values
         * @param i Starting position (inclusive)
         * @param j Ending position (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position i to j
         * 
         * @note Positions refer to 16-bit value indices, not bit positions
         */
        static uint64_t psum4x16bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 16) / 64;
            uint64_t start_bit_index = (i * 16) % 64;
            uint64_t end_block_index = (j * 16) / 64;
            uint64_t end_bit_index = ((j * 16) + 15) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum4x16bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum4x16bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum4x16bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum4x16bits(modified_last_block);
            }

            return sum;
        }

        /**
         * @brief Computes partial sum of 32-bit values up to position i
         * 
         * Calculates the sum of all 32-bit values from position 0 to i (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 32-bit values
         * @param i Position up to which to compute the sum (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position 0 to i
         * 
         * @note Position i refers to the i-th 32-bit value, not bit position
         */
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
        /**
         * @brief Computes partial sum of 32-bit values between positions i and j
         * 
         * Calculates the sum of all 32-bit values from position i to j (inclusive)
         * in a packed bit array.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 32-bit values
         * @param i Starting position (inclusive)
         * @param j Ending position (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position i to j
         * 
         * @note Positions refer to 32-bit value indices, not bit positions
         */
        static uint64_t psum2x32bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t start_block_index = (i * 32) / 64;
            uint64_t start_bit_index = (i * 32) % 64;
            uint64_t end_block_index = (j * 32) / 64;
            uint64_t end_bit_index = ((j * 32) + 31) % 64;
            uint64_t sum = 0;
            assert(start_block_index < array_size);
            assert(end_block_index < array_size);

            if (start_block_index < end_block_index)
            {
                uint64_t modified_start_block = bits[start_block_index] << start_bit_index;
                sum += sum2x32bits(modified_start_block);

                for (uint64_t k = start_block_index + 1; k < end_block_index; k++)
                {
                    sum += sum2x32bits(bits[k]);
                }
                uint64_t modified_last_block = bits[end_block_index] >> (63 - end_bit_index);
                sum += sum2x32bits(modified_last_block);
            }
            else
            {
                uint64_t maskL = UINT64_MAX >> start_bit_index;
                uint64_t maskR = UINT64_MAX << (63 - end_bit_index);
                uint64_t modified_last_block = bits[end_block_index] & maskL & maskR;
                sum += sum2x32bits(modified_last_block);
            }

            return sum;
        }

        /**
         * @brief Computes partial sum of 64-bit values up to position i
         * 
         * Calculates the sum of all 64-bit values from position 0 to i (inclusive)
         * in an array.
         * 
         * @param bits Pointer to array of 64-bit values
         * @param i Position up to which to compute the sum (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position 0 to i
         * 
         * @note Position i refers to the i-th 64-bit value (array index)
         */
        static uint64_t psum1x64bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            for (uint64_t j = 0; j <= i; j++)
            {
                sum += bits[j];
            }
            return sum;
        }
        /**
         * @brief Computes partial sum of 64-bit values between positions i and j
         * 
         * Calculates the sum of all 64-bit values from position i to j (inclusive)
         * in an array.
         * 
         * @param bits Pointer to array of 64-bit values
         * @param i Starting position (inclusive)
         * @param j Ending position (inclusive)
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position i to j
         * 
         * @note Positions refer to array indices
         */
        static uint64_t psum1x64bits(uint64_t *bits, uint64_t i, uint64_t j, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            for (uint64_t k = i; k <= j; k++)
            {
                sum += bits[k];
            }
            return sum;
        }

        /**
         * @brief Generic partial sum function up to position i
         * 
         * Computes the partial sum from position 0 to i based on the specified
         * bit type. This function dispatches to the appropriate specialized
         * partial sum function.
         * 
         * @param bits Pointer to array of packed values
         * @param i Position up to which to compute the sum (inclusive)
         * @param bit_type The bit width type of packed values
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position 0 to i
         * 
         * @throws std::invalid_argument If bit_type is invalid
         */
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
        /**
         * @brief Generic partial sum function between positions i and j
         * 
         * Computes the partial sum from position i to j based on the specified
         * bit type. This function dispatches to the appropriate specialized
         * partial sum function.
         * 
         * @param bits Pointer to array of packed values
         * @param i Starting position (inclusive)
         * @param j Ending position (inclusive)
         * @param bit_type The bit width type of packed values
         * @param array_size Size of the bits array
         * @return uint64_t Partial sum from position i to j
         * 
         * @throws std::invalid_argument If bit_type is invalid
         */
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

        /**
         * @brief Finds the first position where prefix sum >= y for 2-bit values (branchless)
         * 
         * Efficiently searches for the first position in a 64-bit word containing
         * thirty-two 2-bit values where the prefix sum reaches or exceeds the target value y.
         * Uses branchless bit manipulation for optimal performance.
         * 
         * @param X 64-bit word containing 32 packed 2-bit values
         * @param y Target sum value to search for
         * @return uint32_t Position where prefix sum >= y, or 32 if not found
         * 
         * @note Returns 32 as sentinel value if target sum exceeds total sum
         */
        static inline uint32_t find_prefix_ge_y_2b32_branchless(uint64_t X, uint32_t y)
        {
            const uint64_t M64 = 0x5555555555555555ULL; // ...0101
            const uint32_t M32 = 0x55555555u;
            const uint32_t M16 = 0x5555u;
            const uint32_t M8 = 0x55u;
            const uint32_t M4 = 0x5u;

            uint32_t total = (uint32_t)__builtin_popcountll(X & M64) + ((uint32_t)__builtin_popcountll((X >> 1) & M64) << 1);

            uint32_t y0 = y;
            uint32_t idx = 0;
            uint64_t W = X;

            uint32_t up32 = (uint32_t)(W >> 32);
            uint32_t sum16 = (uint32_t)__builtin_popcount(up32 & M32) + ((uint32_t)__builtin_popcount((up32 >> 1) & M32) << 1);

            uint32_t c1 = (y > sum16); 
            uint32_t m1 = 0u - c1;
            idx += (c1 << 4); // +16
            y -= (sum16 & m1);
            W >>= ((1u - c1) * 32);

            uint32_t up16 = (uint32_t)((W >> 16) & 0xFFFFu);
            uint32_t sum8 = (uint32_t)__builtin_popcount(up16 & M16) + ((uint32_t)__builtin_popcount((up16 >> 1) & M16) << 1);

            uint32_t c2 = (y > sum8);
            uint32_t m2 = 0u - c2;
            idx += (c2 << 3); // +8
            y -= (sum8 & m2);
            W >>= ((1u - c2) * 16);

            uint32_t up8 = (uint32_t)((W >> 8) & 0xFFu);
            uint32_t sum4 = (uint32_t)__builtin_popcount(up8 & M8) + ((uint32_t)__builtin_popcount((up8 >> 1) & M8) << 1);

            uint32_t c3 = (y > sum4);
            uint32_t m3 = 0u - c3;
            idx += (c3 << 2); // +4
            y -= (sum4 & m3);
            W >>= ((1u - c3) * 8);

            uint32_t up4 = (uint32_t)((W >> 4) & 0xFu);
            uint32_t sum2 = (uint32_t)__builtin_popcount(up4 & M4) + ((uint32_t)__builtin_popcount((up4 >> 1) & M4) << 1);

            uint32_t c4 = (y > sum2);
            uint32_t m4 = 0u - c4;
            idx += (c4 << 1); // +2
            y -= (sum2 & m4);
            W >>= ((1u - c4) * 4);

            uint32_t v_first = (uint32_t)((W >> 2) & 0x3u); 
            uint32_t c5 = (y > v_first);
            uint32_t p = idx + c5;

            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (32u & mf);
        }

        /**
         * @brief Finds the first position where prefix sum >= y for 4-bit values (branchless)
         * 
         * Efficiently searches for the first position in a 64-bit word containing
         * sixteen 4-bit values where the prefix sum reaches or exceeds the target value y.
         * Uses branchless bit manipulation for optimal performance.
         * 
         * @param X 64-bit word containing 16 packed 4-bit values
         * @param y Target sum value to search for
         * @return uint32_t Position where prefix sum >= y, or 16 if not found
         * 
         * @note Returns 16 as sentinel value if target sum exceeds total sum
         */
        static inline uint32_t find_prefix_ge_y_nib4x16_branchless(uint64_t X, uint32_t y)
        {
            const uint64_t NIB = 0x0F0F0F0F0F0F0F0FULL;

            uint64_t bytes = (X & NIB) + ((X >> 4) & NIB); // b0..b7
            uint64_t s16 = (bytes & 0x00FF00FF00FF00FFULL) + ((bytes >> 8) & 0x00FF00FF00FF00FFULL); // [b0+b1, b2+b3, b4+b5, b6+b7]
            uint64_t s32 = (s16 & 0x0000FFFF0000FFFFULL) + ((s16 >> 16) & 0x0000FFFF0000FFFFULL); // [b0..b3, b4..b7]

            uint32_t total = (uint32_t)((bytes * 0x0101010101010101ULL) >> 56);

            uint32_t y0 = y;  // 番兵判定用に保存
            uint32_t idx = 0; // 先頭(MSB)から何ニブル進んだか

            uint32_t sum_front8 = (uint32_t)(s32 >> 32); // b4+b5+b6+b7
            uint32_t c1 = (y > sum_front8);              // 0:前半, 1:後半へ
            uint32_t m1 = 0u - c1;                       // 0 or 0xFFFFFFFF
            idx += (c1 << 3);                            // +8
            y -= (sum_front8 & m1);

            uint32_t shift16 = 16u + ((1u - c1) << 5); // 16 + (1-c1)*32
            uint32_t sum_front4 = (uint32_t)((s16 >> shift16) & 0xFFFFu);
            uint32_t c2 = (y > sum_front4);
            uint32_t m2 = 0u - c2;
            idx += (c2 << 2); // +4
            y -= (sum_front4 & m2);

            uint32_t byte_idx_from_lsb = 7u - (idx >> 1);
            uint32_t firstByteSum = (uint32_t)((bytes >> (byte_idx_from_lsb * 8u)) & 0xFFu);
            uint32_t c3 = (y > firstByteSum);
            uint32_t m3 = 0u - c3;
            idx += (c3 << 1); // +2
            y -= (firstByteSum & m3);

            uint32_t nib_shift = (15u - idx) << 2;
            uint32_t loNib = (uint32_t)((X >> nib_shift) & 0xFu);
            uint32_t c4 = (y > loNib); // 0/1
            uint32_t p = idx + c4;

            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (16u & mf);
        }
        /**
         * @brief Finds the first position where prefix sum >= y for 8-bit values (branchless)
         * 
         * Efficiently searches for the first position in a 64-bit word containing
         * eight 8-bit values where the prefix sum reaches or exceeds the target value y.
         * Uses branchless bit manipulation for optimal performance.
         * 
         * @param X 64-bit word containing 8 packed 8-bit values
         * @param y Target sum value to search for
         * @return uint32_t Position where prefix sum >= y, or 8 if not found
         * 
         * @note Returns 8 as sentinel value if target sum exceeds total sum
         */
        static inline uint32_t find_prefix_ge_y_nib8x8_branchless(uint64_t X, uint32_t y)
        {
            uint64_t s16 = (X & 0x00FF00FF00FF00FFULL) + ((X >> 8) & 0x00FF00FF00FF00FFULL); // [b7+b6, b5+b4, b3+b2, b1+b0] (LSB→MSB)

            uint64_t s32 = (s16 & 0x0000FFFF0000FFFFULL) + ((s16 >> 16) & 0x0000FFFF0000FFFFULL); // low32: b7..b4, high32: b3..b0

            uint32_t sum_back4 = (uint32_t)(s32);        // b4..b7
            uint32_t sum_front4 = (uint32_t)(s32 >> 32); // b0..b3
            uint32_t total = sum_front4 + sum_back4;

            uint32_t y0 = y;
            uint32_t idx = 0;

            uint32_t c1 = (y > sum_front4); // 0: 前半, 1: 後半へ
            uint32_t m1 = 0u - c1;          // 0 or 0xFFFFFFFF
            idx += (c1 << 2);               // +4
            y -= (sum_front4 & m1);

            uint32_t shift16 = 16u + ((1u - c1) << 5); // c1=0→48, c1=1→16
            uint32_t sum_front2 = (uint32_t)((s16 >> shift16) & 0xFFFFu);

            uint32_t c2 = (y > sum_front2); // 0: 先頭ペア, 1: 後ろのペアへ
            uint32_t m2 = 0u - c2;
            idx += (c2 << 1); // +2
            y -= (sum_front2 & m2);

            uint32_t byte_idx_from_lsb = 7u - idx; // LSB基準インデックス
            uint32_t firstByte = (uint32_t)((X >> (byte_idx_from_lsb * 8u)) & 0xFFu);

            uint32_t c3 = (y > firstByte); // 0: そこで到達, 1: 次の1要素へ
            uint32_t p = idx + c3;

            uint32_t cf = (y0 > total);
            uint32_t mf = 0u - cf;
            return (p & ~mf) | (8u & mf);
        }

        /**
         * @brief Finds the first position where prefix sum >= y for 16-bit values (branchless)
         * 
         * Efficiently searches for the first position in a 64-bit word containing
         * four 16-bit values where the prefix sum reaches or exceeds the target value y.
         * Uses branchless bit manipulation for optimal performance.
         * 
         * @param X 64-bit word containing 4 packed 16-bit values
         * @param y Target sum value to search for
         * @return uint32_t Position where prefix sum >= y, or 4 if not found
         * 
         * @note Returns 4 as sentinel value if target sum exceeds total sum
         */
        static inline uint32_t find_prefix_ge_y_nib16x4_branchless(uint64_t X, uint32_t y)
        {
            uint32_t e0 = (uint32_t)((X >> 48) & 0xFFFFu);
            uint32_t e1 = (uint32_t)((X >> 32) & 0xFFFFu);
            uint32_t e2 = (uint32_t)((X >> 16) & 0xFFFFu);
            uint32_t e3 = (uint32_t)(X & 0xFFFFu);

            uint32_t s0 = e0;
            uint32_t s1 = s0 + e1;
            uint32_t s2 = s1 + e2;
            uint32_t s3 = s2 + e3;

            uint32_t m = ((s0 >= y) ? 1u : 0u) | ((s1 >= y) ? 2u : 0u) | ((s2 >= y) ? 4u : 0u) | ((s3 >= y) ? 8u : 0u);

            m |= ((m == 0u) ? 16u : 0u);

            return (uint32_t)__builtin_ctz(m);
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 1-bit values
         * 
         * Finds the position in a packed 1-bit array where the cumulative sum
         * first reaches or exceeds the target value i. This is equivalent to
         * finding the i-th set bit.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 1-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Bit position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, otherwise searches for the i-th occurrence
         */
        static int64_t search64x1bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            sum = 0;
            uint64_t k = 0;
            uint64_t v = stool::Byte::count_bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = stool::Byte::count_bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = stool::MSBByte::select1(bits[k], diff - 1);
            sum = i - 1;
            return (k * 64) + f;
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 2-bit values
         * 
         * Finds the position in a packed 2-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 2-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 2-bit value indices
         */
        static int64_t search32x2bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum32x2bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum32x2bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_2b32_branchless(bits[k], diff);
            return (k * 32) + f;
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 4-bit values
         * 
         * Finds the position in a packed 4-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 4-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 4-bit value indices
         */
        static int64_t search16x4bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum16x4bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum16x4bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib4x16_branchless(bits[k], diff);
            return (k * 16) + f;
        }

        /**
         * @brief Searches for the position where cumulative sum reaches target for 8-bit values
         * 
         * Finds the position in a packed 8-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 8-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 8-bit value indices
         */
        static int64_t search8x8bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum8x8bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum8x8bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib8x8_branchless(bits[k], diff);
            return (k * 8) + f;
        }

        /**
         * @brief Searches for the position where cumulative sum reaches target for 16-bit values
         * 
         * Finds the position in a packed 16-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 16-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 16-bit value indices
         */
        static int64_t search4x16bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum4x16bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum4x16bits(bits[k]);
            }

            uint64_t diff = i - sum;
            uint64_t f = find_prefix_ge_y_nib16x4_branchless(bits[k], diff);
            return (k * 4) + f;
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 32-bit values
         * 
         * Finds the position in a packed 32-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit words containing packed 32-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Value position where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is in terms of 32-bit value indices
         */
        static int64_t search2x32bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = sum2x32bits(bits[k]);

            while (sum + v < i)
            {
                sum += v;
                k++;
                assert(k < array_size);
                v = sum2x32bits(bits[k]);
            }

            // uint64_t diff = i - sum;
            uint64_t L = bits[k] >> 32;
            // uint64_t R = bits[k] & 0xFFFFFFFF;
            if (sum + L >= i)
            {
                return (k * 2);
            }
            else
            {
                return (k * 2) + 1;
            }
        }
        /**
         * @brief Searches for the position where cumulative sum reaches target for 64-bit values
         * 
         * Finds the position in a 64-bit array where the cumulative sum
         * first reaches or exceeds the target value i.
         * 
         * @param bits Pointer to array of 64-bit values
         * @param i Target cumulative sum value
         * @param array_size Size of the bits array
         * @return int64_t Array index where cumulative sum reaches i, or -1 if not found
         * 
         * @note Returns 0 if i == 0, position is the array index
         */
        static int64_t search1x64bits(uint64_t *bits, uint64_t i, [[maybe_unused]] uint64_t array_size)
        {
            uint64_t sum = 0;
            if (i == 0)
            {
                return 0;
            }
            uint64_t k = 0;
            uint64_t v = bits[k];

            while (sum + v < i)
            {
                sum += v;
                k++;
                v = bits[k];
            }
            return k;
        }

        /**
         * @brief Generic search function for finding cumulative sum position
         * 
         * Searches for the position where the cumulative sum first reaches or
         * exceeds the target value i, based on the specified bit type.
         * This function dispatches to the appropriate specialized search function.
         * 
         * @param bits Pointer to array of packed values
         * @param i Target cumulative sum value
         * @param bit_type The bit width type of packed values
         * @param total_sum Total sum of all values (for bounds checking)
         * @param array_size Size of the bits array
         * @return int64_t Position where cumulative sum reaches i, or -1 if not found
         * 
         * @throws std::invalid_argument If bit_type is invalid
         * @note Returns -1 if i > total_sum
         */
        static int64_t search(uint64_t *bits, uint64_t i, PackedBitType bit_type, uint64_t total_sum, [[maybe_unused]] uint64_t array_size)
        {
            if (i > total_sum)
            {
                return -1;
            }
            switch (bit_type)
            {
            case PackedBitType::BIT_1:
                return search64x1bits(bits, i, array_size);
            case PackedBitType::BIT_2:
                return search32x2bits(bits, i, array_size);
            case PackedBitType::BIT_4:
                return search16x4bits(bits, i, array_size);
            case PackedBitType::BIT_8:
                return search8x8bits(bits, i, array_size);
            case PackedBitType::BIT_16:
                return search4x16bits(bits, i, array_size);
            case PackedBitType::BIT_32:
                return search2x32bits(bits, i, array_size);
            case PackedBitType::BIT_64:
                return search1x64bits(bits, i, array_size);
            default:
                break;
            }
            throw std::invalid_argument("Invalid bit type");
        }
    };
}
