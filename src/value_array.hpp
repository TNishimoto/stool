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
    static void translate(const vector<X> &input, vector<Y> &output)
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
    ValueArray(ValueArray && obj){
        this->byteSize = obj.byteSize;
        this->num = obj.num;
        this->arr.swap(obj.arr);
    }

    uint64_t operator[](uint64_t i) const;

    template <typename BYTE>
    void set(const vector<BYTE> &_arr, bool isShrink)
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
    void change(uint64_t i, uint64_t value){
        if(this->byteSize == 1){
            //std::vector<uint8_t> r;
            //r.push_back(value);

            uint8_t x = value; 
            memcpy(&this->arr[this->byteSize * i], &x, this->byteSize);    

            //this->arr[this->num * i] = value;
            //memcpy(&this->arr[this->num * i], &x, this->byteSize);           

        }else if(this->byteSize == 2){
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
        }else if(this->byteSize == 4){
            uint32_t x = value; 
            memcpy(&this->arr[this->byteSize * i], &x, this->byteSize);           
        }else{
            uint64_t x = value; 
            memcpy(&this->arr[this->byteSize * i], &x, this->byteSize);           

        }
    }
    void resize(uint64_t _size, uint64_t _byteSize){
        this->arr.resize(_size * _byteSize, 0);
        this->num = _size;
        this->byteSize = _byteSize;
    }
    template <typename BYTE>
    void fitDecode(vector<BYTE> &output) const
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
    void decode(vector<BYTE> &output) const
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