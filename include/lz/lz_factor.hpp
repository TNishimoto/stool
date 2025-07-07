#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <stack>

// #include "stool/include/light_stool.hpp"

// #include "print.hpp"
#include <set>

namespace stool
{

    /*
    This class represents the factor of LZ77.
    */
    struct LZFactor
    {
    public:
        uint64_t reference;
        uint64_t length;

        LZFactor(uint64_t _reference, uint64_t _length)
        {
            this->reference = _reference;
            this->length = _length;
        }
        LZFactor(char c)
        {
            this->reference = (uint8_t)c;
            this->length = UINT64_MAX;
        }
        LZFactor()
        {
        }

        static LZFactor create_char_factor(char c)
        {
            LZFactor f;
            f.reference = (uint8_t)c;
            f.length = UINT64_MAX;
            return f;
        }
        static LZFactor create_reference_factor(uint64_t reference, uint64_t length)
        {
            LZFactor f;
            f.reference = reference;
            f.length = length;
            return f;
        }

        uint64_t get_length() const
        {
            if (this->is_char())
            {
                return 1;
            }
            else
            {
                return this->length;
            }
        }
        bool is_char() const
        {
            return this->length == UINT64_MAX;
        }
        char get_char() const
        {
            return (char)this->reference;
        }

        std::string to_string() const
        {
            if (!this->is_char())
            {
                return "[" + std::to_string(reference) + ", " + std::to_string(length) + "]";
            }
            else
            {
                std::string s = "";
                s += (char)this->reference;
                return s;
            }
        }

        static void to_factor_string(const std::vector<LZFactor> &input, std::string &output)
        {
            output.clear();
            for (uint64_t i = 0; i < input.size(); i++)
            {
                std::string s = input[i].to_string();
                output += s;
                if (i != input.size() - 1)
                    output += ",";
            }
        }

        // static void decompress(std::vector<LZFactor> &factors, string &output, bool allowRightReference);
    };


}