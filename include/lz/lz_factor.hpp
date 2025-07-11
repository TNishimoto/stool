#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <stack>
#include <set>

namespace stool
{

    /**
     * @brief Represents a factor in LZ77 compression algorithm
     * 
     * This class represents a single factor in the LZ77 factorization of a string.
     * A factor can be either a literal character or a reference to a previous occurrence
     * in the string with a specified length.
     */
    struct LZFactor
    {
    public:
        /** @brief Reference position in the string (for reference factors) or character value (for literal factors) */
        uint64_t reference;
        /** @brief Length of the factor (UINT64_MAX for literal factors) */
        uint64_t length;

        /**
         * @brief Constructs a reference factor
         * @param _reference The reference position in the string
         * @param _length The length of the factor
         */
        LZFactor(uint64_t _reference, uint64_t _length)
        {
            this->reference = _reference;
            this->length = _length;
        }
        
        /**
         * @brief Constructs a literal factor from a character
         * @param c The character to represent as a literal factor
         */
        LZFactor(char c)
        {
            this->reference = (uint8_t)c;
            this->length = UINT64_MAX;
        }
        
        /**
         * @brief Default constructor
         */
        LZFactor()
        {
        }

        /**
         * @brief Creates a literal factor from a character
         * @param c The character to represent
         * @return A new LZFactor representing the literal character
         */
        static LZFactor create_char_factor(char c)
        {
            LZFactor f;
            f.reference = (uint8_t)c;
            f.length = UINT64_MAX;
            return f;
        }
        
        /**
         * @brief Creates a reference factor
         * @param reference The reference position in the string
         * @param length The length of the factor
         * @return A new LZFactor representing a reference to a previous occurrence
         */
        static LZFactor create_reference_factor(uint64_t reference, uint64_t length)
        {
            LZFactor f;
            f.reference = reference;
            f.length = length;
            return f;
        }

        /**
         * @brief Gets the length of this factor
         * @return 1 for literal factors, the stored length for reference factors
         */
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
        
        /**
         * @brief Checks if this factor represents a literal character
         * @return true if this is a literal factor, false if it's a reference factor
         */
        bool is_char() const
        {
            return this->length == UINT64_MAX;
        }
        
        /**
         * @brief Gets the character value of this factor
         * @return The character value (only valid for literal factors)
         */
        char get_char() const
        {
            return (char)this->reference;
        }

        /**
         * @brief Converts this factor to a string representation
         * @return String representation: "[reference, length]" for reference factors, or the character for literal factors
         */
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

        /**
         * @brief Converts a vector of LZ factors to a comma-separated string representation
         * @param input Vector of LZ factors to convert
         * @param output Output string containing the string representation
         */
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