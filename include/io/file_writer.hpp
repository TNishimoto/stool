#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{
    class FileWriter
    {
    public:
        /**
         * @brief Adds a 64-bit unsigned integer to a vector of bytes.
         * @param data The vector to which the value will be added.
         * @param value The 64-bit unsigned integer to add.
         * @return The number of bytes added to the vector.
         */
        static uint8_t add(std::vector<uint8_t> &data, uint64_t value)
        {
            for (size_t i = 0; i < 8; ++i)
            {
                uint8_t v = (value >> (i * 8)) & 0xFF;
                data.push_back(v);
            }
            return 8;
        }
        /**
         * @brief Adds a 32-bit unsigned integer to a vector of bytes.
         * @param data The vector to which the value will be added.
         * @param value The 32-bit unsigned integer to add.
         * @return The number of bytes added to the vector.
         */
        static uint8_t add(std::vector<uint8_t> &data, uint32_t value)
        {
            for (size_t i = 0; i < 4; ++i)
            {
                uint8_t v = (value >> (i * 8)) & 0xFF;
                data.push_back(v);
            }
            return 4;
        }
        /**
         * @brief Adds a 16-bit unsigned integer to a vector of bytes.
         * @param data The vector to which the value will be added.
         * @param value The 16-bit unsigned integer to add.
         * @return The number of bytes added to the vector.
         */
        static uint8_t add(std::vector<uint8_t> &data, uint16_t value)
        {
            for (size_t i = 0; i < 4; ++i)
            {
                uint8_t v = (value >> (i * 8)) & 0xFF;
                data.push_back(v);
            }
            return 2;
        }
        /**
         * @brief Loads a 64-bit unsigned integer from a vector of bytes.
         * @param data The vector from which the value will be loaded.
         * @param pos The position in the vector to start loading from.
         * @return The loaded 64-bit unsigned integer.
         */
        static uint64_t load64(std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint64_t value = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                value |= static_cast<uint64_t>(data[pos++]) << (i * 8);
            }
            return value;
        }
        /**
         * @brief Loads a 32-bit unsigned integer from a vector of bytes.
         * @param data The vector from which the value will be loaded.
         * @param pos The position in the vector to start loading from.
         * @return The loaded 32-bit unsigned integer.
         */
        static uint64_t load32(std::vector<uint8_t> &data, uint64_t &pos)
        {
            uint32_t value = 0;
            for (size_t i = 0; i < 4; ++i)
            {
                value |= static_cast<uint32_t>(data[pos++]) << (i * 8);
            }
            return value;
        }
        /**
         * @brief Loads a 16-bit unsigned integer from a vector of bytes.
         * @param data The vector from which the value will be loaded.
         * @param pos The position in the vector to start loading from.
         * @return The loaded 16-bit unsigned integer.
         */
        static uint64_t load16(std::vector<uint8_t> &data, uint64_t &pos)
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