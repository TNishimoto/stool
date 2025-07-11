#pragma once
// #include "cpplinq.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <cstring>
// using namespace std;

namespace stool
{

    /**
     * @brief A memory-efficient array that automatically chooses the smallest data type
     * 
     * ValueArray stores a sequence of unsigned integers using the smallest possible
     * data type (uint8_t, uint16_t, uint32_t, or uint64_t) based on the maximum value
     * in the data. This provides significant memory savings when storing arrays with
     * small values.
     * 
     * The class supports:
     * - Automatic type selection for optimal memory usage
     * - File I/O operations (save/load)
     * - Random access to elements
     * - Type conversion and decoding
     */
    class ValueArray
    {

        uint64_t byteSize;  ///< Size of each element in bytes
        uint64_t num;       ///< Number of elements in the array
        std::vector<uint8_t> arr;  ///< Raw byte array storing the data

    public:
/**
         * @brief Translates a vector from type X to type Y
         * 
         * @tparam X Source type
         * @tparam Y Destination type
         * @param input Source vector
         * @param output Destination vector (will be resized to match input size)
         */
        template <typename X, typename Y>
        static void translate(const std::vector<X> &input, std::vector<Y> &output)
        {
            output.resize(input.size());
            for (uint64_t i = 0; i < input.size(); i++)
            {
                output[i] = input[i];
            }
        }

        /**
         * @brief Returns the number of elements in the array
         * 
         * @return uint64_t Number of elements
         */
        uint64_t size()
        {
            return this->num;
        }

        /**
         * @brief Writes the ValueArray to an output file stream
         * 
         * @param writer Output file stream
         * @throws -1 if the stream is not valid
         */
        void write(std::ofstream &writer) const
        {
            if (!writer)
                throw -1;
            writer.write((const char *)(&this->byteSize), sizeof(uint64_t));
            writer.write((const char *)(&this->num), sizeof(uint64_t));
            writer.write((const char *)(&this->arr[0]), sizeof(uint8_t) * this->arr.size());
        }

        /**
         * @brief Writes the ValueArray to a file
         * 
         * @param filename Name of the output file
         */
        void write(std::string filename) const
        {

            std::ofstream out(filename, std::ios::out | std::ios::binary);
            this->write(out);
            out.close();
        }

        /**
         * @brief Loads the ValueArray from an input file stream
         * 
         * @param stream Input file stream
         * @throws -1 if the stream is not valid or reading fails
         */
        void load(std::ifstream &stream)
        {
            if (!stream)
            {
                std::cerr << "error reading file! " << std::endl;
                throw -1;
            }

            // uint64_t fileSymbol;
            stream.read((char *)(&this->byteSize), sizeof(uint64_t));
            stream.read((char *)(&this->num), sizeof(uint64_t));
            uint64_t arraySize = this->byteSize * this->num;
            this->arr.resize(arraySize);
            stream.read((char *)(&this->arr[0]), sizeof(uint8_t) * this->arr.size());
        }

        /**
         * @brief Loads the ValueArray from a file
         * 
         * @param filename Name of the input file
         */
        void load(std::string filename)
        {
            std::ifstream stream;
            stream.open(filename, std::ios::binary);
            this->load(stream);
            stream.close();
        }

        /**
         * @brief Default constructor
         */
        ValueArray()
        {
        }

        /**
         * @brief Move constructor
         * 
         * @param obj ValueArray to move from
         */
        ValueArray(ValueArray &&obj)
        {
            this->byteSize = obj.byteSize;
            this->num = obj.num;
            this->arr.swap(obj.arr);
        }

        /**
         * @brief Access element at index i
         * 
         * @param i Index of the element to access
         * @return uint64_t Value at index i
         */
        uint64_t operator[](uint64_t i) const
        {

            if (this->byteSize > 2)
            {
                if (this->byteSize == 4)
                {
                    uint32_t r;
                    memcpy(&r, &this->arr[i * this->byteSize], this->byteSize);
                    return r;
                }
                else
                {
                    uint64_t r;
                    memcpy(&r, &this->arr[i * this->byteSize], this->byteSize);
                    return r;
                }
            }
            else
            {
                if (this->byteSize == 2)
                {
                    uint16_t r;
                    memcpy(&r, &this->arr[i * this->byteSize], this->byteSize);
                    return r;
                }
                else
                {

                    uint8_t r;
                    memcpy(&r, &this->arr[i * this->byteSize], this->byteSize);
                    return r;
                }
            }
        }

        /**
         * @brief Swaps the contents of this ValueArray with another
         * 
         * @param obj ValueArray to swap with
         */
        void swap(ValueArray &obj)
        {
            this->num = obj.num;
            this->arr.swap(obj.arr);
            this->byteSize = obj.byteSize;
        }

        /**
         * @brief Sets the ValueArray from a vector of values
         * 
         * @tparam BYTE Type of the input vector elements
         * @param _arr Input vector
         * @param isShrink If true, automatically choose the smallest data type based on maximum value
         */
        template <typename BYTE>
        void set(const std::vector<BYTE> &_arr, bool isShrink)
        {
            if (!isShrink)
            {
                this->num = _arr.size();
                this->byteSize = sizeof(BYTE);
                this->arr.resize(this->num * this->byteSize);
                memcpy(&this->arr[0], &_arr[0], this->num * this->byteSize);
            }
            else
            {
                uint64_t max = 0;
                for (uint64_t i = 0; i < _arr.size(); i++)
                {
                    if (max < _arr[i])
                        max = _arr[i];
                }
                if (max <= UINT8_MAX)
                {
                    std::vector<uint8_t> o;
                    ValueArray::translate(_arr, o);
                    this->set(o, false);
                }
                else if (max <= UINT16_MAX)
                {
                    std::vector<uint16_t> o;
                    ValueArray::translate(_arr, o);
                    this->set(o, false);
                }
                else if (max <= UINT32_MAX)
                {
                    std::vector<uint32_t> o;
                    ValueArray::translate(_arr, o);
                    this->set(o, false);
                }
                else
                {
                    std::vector<uint64_t> o;
                    ValueArray::translate(_arr, o);
                    this->set(o, false);
                }
            }
        }

        /**
         * @brief Changes the value at a specific index
         * 
         * @param i Index of the element to change
         * @param value New value to set
         */
        void change(uint64_t i, uint64_t value)
        {
            if (this->byteSize == 1)
            {
                // std::vector<uint8_t> r;
                // r.push_back(value);

                uint8_t x = value;
                memcpy(&this->arr[this->byteSize * i], &x, this->byteSize);

                // this->arr[this->num * i] = value;
                // memcpy(&this->arr[this->num * i], &x, this->byteSize);
            }
            else if (this->byteSize == 2)
            {
                uint16_t x = value;
                std::vector<uint16_t> r;
                r.push_back(value);
                /*
                uint16_t x = value;
                uint8_t x1 = value >> 8;
                uint8_t x2 = (value << 8) >> 8;
                this->arr[this->num * i] = x2;
                this->arr[this->num * i + 1] = x1;
                */

                memcpy(&this->arr[this->byteSize * i], &x, this->byteSize);
            }
            else if (this->byteSize == 4)
            {
                uint32_t x = value;
                memcpy(&this->arr[this->byteSize * i], &x, this->byteSize);
            }
            else
            {
                uint64_t x = value;
                memcpy(&this->arr[this->byteSize * i], &x, this->byteSize);
            }
        }

        /**
         * @brief Resizes the ValueArray to a new size with specified byte size
         * 
         * @param _size New number of elements
         * @param _byteSize Size of each element in bytes
         */
        void resize(uint64_t _size, uint64_t _byteSize)
        {
            this->arr.resize(_size * _byteSize, 0);
            this->num = _size;
            this->byteSize = _byteSize;
        }

        /**
         * @brief Decodes the ValueArray to a vector of the specified type
         * 
         * This method requires that the byte size matches the size of the target type.
         * 
         * @tparam BYTE Type of the output vector elements
         * @param output Output vector (will be resized)
         * @throws -1 if byte size doesn't match the target type size
         */
        template <typename BYTE>
        void fit_decode(std::vector<BYTE> &output) const
        {
            output.resize(this->num);
            if (this->byteSize == sizeof(BYTE))
            {
                memcpy(&output[0], &this->arr[0], this->num * this->byteSize);
            }
            else
            {
                std::cout << "exception!" << std::endl;
                throw -1;
            }
        }

        /**
         * @brief Decodes the ValueArray to a vector of the specified type
         * 
         * This method automatically handles type conversion regardless of the stored byte size.
         * 
         * @tparam BYTE Type of the output vector elements
         * @param output Output vector (will be resized)
         * @throws -1 if the byte size is not supported (1, 2, 4, or 8 bytes)
         */
        template <typename BYTE>
        void decode(std::vector<BYTE> &output) const
        {
            if (this->byteSize == 1)
            {
                std::vector<uint8_t> decArr;
                this->fit_decode(decArr);
                ValueArray::translate(decArr, output);
            }
            else if (this->byteSize == 2)
            {
                std::vector<uint16_t> decArr;
                this->fit_decode(decArr);
                ValueArray::translate(decArr, output);
            }
            else if (this->byteSize == 4)
            {
                std::vector<uint32_t> decArr;
                this->fit_decode(decArr);
                ValueArray::translate(decArr, output);
            }
            else if (this->byteSize == 8)
            {
                std::vector<uint64_t> decArr;
                this->fit_decode(decArr);
                ValueArray::translate(decArr, output);
            }
            else
            {
                std::cout << "exception!" << std::endl;
                throw -1;
            }
        }

        /**
         * @brief Returns the memory usage in bytes
         * 
         * @return uint64_t Total memory usage including array data and metadata
         */
        uint64_t get_using_memory() const
        {
            return this->arr.size() + sizeof(this->byteSize) + sizeof(this->num) + 3;
        }
    };
} // namespace stool