//#include "cpplinq.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "print.hpp"
#include "value_array.hpp"

using namespace std;

namespace stool{
    
    uint64_t ValueArray::size(){
        return this->num;
    }
    ValueArray::ValueArray()
    {
    }

    uint64_t ValueArray::operator[](uint64_t i) const
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
    void ValueArray::write(ofstream &writer)
    {
        if (!writer)
            throw - 1;
        writer.write((const char *)(&this->byteSize), sizeof(uint64_t));
        writer.write((const char *)(&this->num), sizeof(uint64_t));
        writer.write((const char *)(&this->arr[0]), sizeof(uint8_t) * this->arr.size());
    }
    void ValueArray::write(string filename)
    {
        ofstream out(filename, ios::out | ios::binary);
        this->write(out);
        out.close();
    }
    void ValueArray::load(ifstream &stream)
    {
        if (!stream)
        {
            std::cerr << "error reading file! " << endl;
            throw - 1;
        }

        uint64_t fileSymbol;
        stream.read((char *)(&this->byteSize), sizeof(uint64_t));
        stream.read((char *)(&this->num), sizeof(uint64_t));
        uint64_t arraySize = this->byteSize * this->num;
        this->arr.resize(arraySize);
        stream.read((char *)(&this->arr[0]), sizeof(uint8_t) * this->arr.size());
    }
    void ValueArray::load(string filename)
    {
        ifstream stream;
        stream.open(filename, ios::binary);
        this->load(stream);
        stream.close();
    }
}