#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{
    /**
     * @brief A utility class for writing and reading binary data to/from vectors. [Unchecked AI's Comment] 
     * 
     * The FileWriter class provides static methods for serializing and deserializing
     * primitive data types (16-bit, 32-bit, and 64-bit unsigned integers) to and from
     * byte vectors. This class is useful for binary data manipulation and file I/O operations.
     * 
     * The class uses little-endian byte order for all operations.
     */
    class FileWriter
    {
    public:
        /**
         * @brief Adds a 64-bit unsigned integer to a vector of bytes.
         * 
         * This method serializes a 64-bit unsigned integer into 8 bytes and appends
         * them to the provided vector. The bytes are written in little-endian order.
         * 
         * @param data The vector to which the value will be added.
         * @param value The 64-bit unsigned integer to add.
         * @return The number of bytes added to the vector (always 8).
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
         * 
         * This method serializes a 32-bit unsigned integer into 4 bytes and appends
         * them to the provided vector. The bytes are written in little-endian order.
         * 
         * @param data The vector to which the value will be added.
         * @param value The 32-bit unsigned integer to add.
         * @return The number of bytes added to the vector (always 4).
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
         * 
         * This method serializes a 16-bit unsigned integer into 2 bytes and appends
         * them to the provided vector. The bytes are written in little-endian order.
         * 
         * @param data The vector to which the value will be added.
         * @param value The 16-bit unsigned integer to add.
         * @return The number of bytes added to the vector (always 2).
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
         * 
         * This method deserializes 8 bytes from the provided vector starting at the
         * specified position and reconstructs a 64-bit unsigned integer. The bytes
         * are read in little-endian order. The position is advanced by 8 bytes.
         * 
         * @param data The vector from which the value will be loaded.
         * @param pos The position in the vector to start loading from. This parameter
         *           is modified to point to the next byte after the loaded value.
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
         * 
         * This method deserializes 4 bytes from the provided vector starting at the
         * specified position and reconstructs a 32-bit unsigned integer. The bytes
         * are read in little-endian order. The position is advanced by 4 bytes.
         * 
         * @param data The vector from which the value will be loaded.
         * @param pos The position in the vector to start loading from. This parameter
         *           is modified to point to the next byte after the loaded value.
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
         * 
         * This method deserializes 2 bytes from the provided vector starting at the
         * specified position and reconstructs a 16-bit unsigned integer. The bytes
         * are read in little-endian order. The position is advanced by 2 bytes.
         * 
         * @param data The vector from which the value will be loaded.
         * @param pos The position in the vector to start loading from. This parameter
         *           is modified to point to the next byte after the loaded value.
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