#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{

    /**
     * @brief A utility class for file writing operations
     */
    class FileWriter
    {
    public:
        /**
         * @brief Writes vector data to an output stream
         *
         * @throws std::runtime_error If the file cannot be opened for writing
         */
        template <typename T>
        static void write_vector(std::ofstream &out, std::vector<T> &data)
        {
            if (!out){
                throw std::runtime_error("Failed to open file for writing");
            }
            out.write(reinterpret_cast<const char *>(&data[0]), data.size() * sizeof(T));
        }

        /**
         * @brief Writes the size of a vector and the vector data to an output stream
         *
         * @throws std::runtime_error If the file cannot be opened for writing
         */
        template <typename T>
        static void write_size_and_vector(std::ofstream &out, std::vector<T> &data)
        {
            if (!out){
                throw std::runtime_error("Failed to open file for writing");
            }

            uint64_t len = data.size();
            out.write(reinterpret_cast<const char *>(&len), sizeof(uint64_t));
            out.write(reinterpret_cast<const char *>(&data[0]), data.size() * sizeof(T));
        }


        /**
         * @brief Writes vector data to a file
         */
        template <typename T>
        static void write_vector(std::string &filename, std::vector<T> &data)
        {
            std::ofstream out(filename, std::ios::out | std::ios::binary);
            write_vector(out, data);
            out.close();
        }

        /**
         * @brief Writes string data to an output stream
         *
         * @param os The output file stream
         * @param text The string data to write
         * @return bool True if successful, false otherwise
         */
        static void write_string(std::ofstream &out, std::string &data)
        {
            if (!out){
                throw std::runtime_error("Failed to open file for writing");
            }
            out.write((const char *)(&data[0]), sizeof(char) * data.size());
        }

        /**
         * @brief Writes string data to a file
         */
        static void write_string(std::string &filename, std::string &data)
        {
            std::ofstream out(filename, std::ios::out | std::ios::binary);
            write_string(out, data);
            out.close();
        }

        /**
         * @brief Writes bits from a container to an output stream
         * @tparam CONTAINER The container type containing the bit data
         */
        template <typename CONTAINER>
        static void write_bits(std::ofstream &out, CONTAINER &data)
        {
            if (!out){
                throw std::runtime_error("Failed to open file for writing");
            }

            uint64_t xsize = data.size();
            out.write((char *)(&xsize), sizeof(uint64_t));
            uint64_t x = 0;
            std::vector<uint8_t> buffer;
            while (x * 8 < data.size())
            {
                uint64_t SIZE = data.size() - (x * 8);
                uint64_t w = std::min(SIZE, (uint64_t)8UL);
                uint8_t _byte = 0;
                for (size_t j = 0; j < w; ++j)
                    _byte = (_byte << 1) | data[(x * 8) + j];
                buffer.push_back(_byte);
                if (buffer.size() > 8192)
                {
                    out.write((char *)(&buffer[0]), sizeof(uint8_t) * buffer.size());
                    buffer.clear();
                }
                x++;
            }
            if (buffer.size() > 0)
            {
                out.write((char *)(&buffer[0]), sizeof(uint8_t) * buffer.size());
                buffer.clear();
            }
        }
    };
} // namespace stool