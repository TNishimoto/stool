#pragma once
//#include "cpplinq.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <cstring>
using namespace std;

namespace stool
{
class VectorTranslator
{
public:
    template <typename X, typename Y>
    static void translate(vector<X> &input, vector<Y> &output)
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
    vector<uint8_t> arr;

public:
    uint64_t size();
    void write(ofstream &writer);
    void write(string filename);
    void load(ifstream &stream);
    void load(string filename);
    ValueArray();
    uint64_t operator[](uint64_t i);

    template <typename BYTE>
    void set(vector<BYTE> &_arr, bool isShrink)
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
                vector<uint8_t> o;
                VectorTranslator::translate(_arr, o);
                this->set(o, false);
            }
            else if (max <= UINT16_MAX)
            {
                vector<uint16_t> o;
                VectorTranslator::translate(_arr, o);
                this->set(o, false);
            }
            else if (max <= UINT32_MAX)
            {
                vector<uint32_t> o;
                VectorTranslator::translate(_arr, o);
                this->set(o, false);
            }
            else
            {
                vector<uint64_t> o;
                VectorTranslator::translate(_arr, o);
                this->set(o, false);
            }
        }
    }
    template <typename BYTE>
    void fitDecode(vector<BYTE> &output)
    {
        output.resize(this->num);
        if (this->byteSize == sizeof(BYTE))
        {
            memcpy(&output[0], &this->arr[0], this->num * this->byteSize);
        }
        else
        {
            std::cout << "exception!" << std::endl;
            throw - 1;
        }
    }
    template <typename BYTE>
    void decode(vector<BYTE> &output)
    {
        if (this->byteSize == 1)
        {
            vector<uint8_t> decArr;
            this->fitDecode(decArr);
            VectorTranslator::translate(decArr, output);
        }
        else if (this->byteSize == 2)
        {
            vector<uint16_t> decArr;
            this->fitDecode(decArr);
            VectorTranslator::translate(decArr, output);
        }
        else if (this->byteSize == 4)
        {
            vector<uint32_t> decArr;
            this->fitDecode(decArr);
            VectorTranslator::translate(decArr, output);
        }
        else if (this->byteSize == 8)
        {
            vector<uint64_t> decArr;
            this->fitDecode(decArr);
            VectorTranslator::translate(decArr, output);
        }
        else
        {
            std::cout << "exception!" << std::endl;
            throw - 1;
        }
    }
};
} // namespace stool