#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{
    /**
     * @brief A utility class for writing and reading binary data to/from vectors. 
     */
    class ByteVectorFunctions
    {
    public:
        /**
         * @brief Converts a given 64-bit integer \a v into 8 bytes and appends them to byte vector \a data.
         * 
         * @return 8.
         */
        static uint8_t add64(std::vector<uint8_t> &data, uint64_t v)
        {
            for (size_t i = 0; i < 8; ++i)
            {
                uint8_t w = (v >> (i * 8)) & 0xFF;
                data.push_back(w);
            }
            return 8;
        }
        /**
         * @brief Converts a given 32-bit integer \a v into 4 bytes and appends them to byte vector \a data.
         *          
         * @return 4.
         */
        static uint8_t add32(std::vector<uint8_t> &data, uint32_t v)
        {
            for (size_t i = 0; i < 4; ++i)
            {
                uint8_t w = (v >> (i * 8)) & 0xFF;
                data.push_back(w);
            }
            return 4;
        }
        /**
         * @brief Converts a given 16-bit integer \a v into 4 bytes and appends them to byte vector \a data.
         *          
         * @return 4.
         */
        static uint8_t add16(std::vector<uint8_t> &data, uint16_t v)
        {
            for (size_t i = 0; i < 4; ++i)
            {
                uint8_t w = (v >> (i * 8)) & 0xFF;
                data.push_back(w);
            }
            return 2;
        }
        /**
         * @brief Loads a 64-bit unsigned integer \a v from byte vector \a data[pos..pos+7] for a given position \a pos and returns it.
         * 
         * @param pos The position in the vector to start loading from. This parameter
         *           is modified to point to the next byte after the loaded value.
         * @return The 64-bit integer $v$.
         */
        static uint64_t load64(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t value = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                value |= static_cast<uint64_t>(data[pos++]) << (i * 8);
            }
            return value;
        }
        /**
         * @brief Loads a 32-bit unsigned integer \a v from byte vector \a data[pos..pos+3] for a given position \a pos and returns it.
         * 
         * @param pos The position in the vector to start loading from. This parameter
         *           is modified to point to the next byte after the loaded value.
         * @return The 32-bit integer $v$.
         */
        static uint64_t load32(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint32_t value = 0;
            for (size_t i = 0; i < 4; ++i)
            {
                value |= static_cast<uint32_t>(data[pos++]) << (i * 8);
            }
            return value;
        }
        /**
         * @brief Loads a 16-bit unsigned integer \a v from byte vector \a data[pos..pos+1] for a given position \a pos and returns it.
         * 
         * @param pos The position in the vector to start loading from. This parameter
         *           is modified to point to the next byte after the loaded value.
         * @return The 16-bit integer $v$.
         */
        static uint64_t load16(const std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint16_t value = 0;
            for (size_t i = 0; i < 2; ++i)
            {
                value |= static_cast<uint16_t>(data[pos++]) << (i * 8);
            }
            return value;
        }

    };

}