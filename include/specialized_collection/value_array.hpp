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
    class VectorTranslator
    {
    public:
        template <typename X, typename Y>
        static void translate(const std::vector<X> &input, std::vector<Y> &output)
        {
            output.resize(input.size());
            for (uint64_t i = 0; i < input.size(); i++)
            {
                output[i] = input[i];
            }
        }
    };

    class ValueArray
    {

        uint64_t byteSize;
        uint64_t num;
        std::vector<uint8_t> arr;

    public:
        uint64_t size()
        {
            return this->num;
        }
        void write(std::ofstream &writer) const
        {
            if (!writer)
                throw -1;
            writer.write((const char *)(&this->byteSize), sizeof(uint64_t));
            writer.write((const char *)(&this->num), sizeof(uint64_t));
            writer.write((const char *)(&this->arr[0]), sizeof(uint8_t) * this->arr.size());
        }
        void write(std::string filename) const
        {

            std::ofstream out(filename, std::ios::out | std::ios::binary);
            this->write(out);
            out.close();
        }
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
        void load(std::string filename)
        {
            std::ifstream stream;
            stream.open(filename, std::ios::binary);
            this->load(stream);
            stream.close();
        }
        ValueArray()
        {
        }
        ValueArray(ValueArray &&obj)
        {
            this->byteSize = obj.byteSize;
            this->num = obj.num;
            this->arr.swap(obj.arr);
        }

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

        void swap(ValueArray &obj)
        {
            this->num = obj.num;
            this->arr.swap(obj.arr);
            this->byteSize = obj.byteSize;
        }

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
                    VectorTranslator::translate(_arr, o);
                    this->set(o, false);
                }
                else if (max <= UINT16_MAX)
                {
                    std::vector<uint16_t> o;
                    VectorTranslator::translate(_arr, o);
                    this->set(o, false);
                }
                else if (max <= UINT32_MAX)
                {
                    std::vector<uint32_t> o;
                    VectorTranslator::translate(_arr, o);
                    this->set(o, false);
                }
                else
                {
                    std::vector<uint64_t> o;
                    VectorTranslator::translate(_arr, o);
                    this->set(o, false);
                }
            }
        }
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
        void resize(uint64_t _size, uint64_t _byteSize)
        {
            this->arr.resize(_size * _byteSize, 0);
            this->num = _size;
            this->byteSize = _byteSize;
        }
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
        template <typename BYTE>
        void decode(std::vector<BYTE> &output) const
        {
            if (this->byteSize == 1)
            {
                std::vector<uint8_t> decArr;
                this->fit_decode(decArr);
                VectorTranslator::translate(decArr, output);
            }
            else if (this->byteSize == 2)
            {
                std::vector<uint16_t> decArr;
                this->fit_decode(decArr);
                VectorTranslator::translate(decArr, output);
            }
            else if (this->byteSize == 4)
            {
                std::vector<uint32_t> decArr;
                this->fit_decode(decArr);
                VectorTranslator::translate(decArr, output);
            }
            else if (this->byteSize == 8)
            {
                std::vector<uint64_t> decArr;
                this->fit_decode(decArr);
                VectorTranslator::translate(decArr, output);
            }
            else
            {
                std::cout << "exception!" << std::endl;
                throw -1;
            }
        }
        uint64_t get_using_memory() const
        {
            return this->arr.size() + sizeof(this->byteSize) + sizeof(this->num) + 3;
        }
    };
} // namespace stool