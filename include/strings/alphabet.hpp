#pragma once
#include <vector>

namespace stool
{
    /**
     * @brief A utility class for creating alphabets.
     */
    class Alphabet
    {
    public:
        /**
         * @brief Creates a binary alphabet containing 'a' and 'b'
         *
         * @return std::vector<uint8_t> A vector containing the characters 'a' and 'b'
         */
        static std::vector<uint8_t> create_ab_alphabet()
        {
            return std::vector<uint8_t>{'a', 'b'};
        }

        /**
         * @brief Creates an alphabet containing the first 8 lowercase letters (a-h)
         */
        static std::vector<uint8_t> create_abcdefgh_alphabet()
        {
            return std::vector<uint8_t>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
        }

        /**
         * @brief Creates a binary alphabet containing 'A', 'C', 'G', 'T'
         */
        static std::vector<uint8_t> create_ACGT_alphabet()
        {
            return std::vector<uint8_t>{'A', 'C', 'G', 'T'};
        }

        /**
         * @brief Creates an alphabet containing all uppercase English letters (A-Z)
         */
        static std::vector<uint8_t> create_AtoZ_alphabet()
        {
            return std::vector<uint8_t>{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
        }

        /**
         * @brief Creates a full alphabet containing ASCII values from 2 to 249
         */
        static std::vector<uint8_t> create_full_alphabet()
        {
            std::vector<uint8_t> r;
            for (uint64_t i = 2; i < 250; i++)
            {
                r.push_back(i);
            }
            return r;
        }

        /**
         * @brief Returns the maximum alphabet type index
         */
        static uint64_t get_max_alphabet_type()
        {
            return 4;
        }

        /**
         * @brief Creates an alphabet based on the specified type
         *
         * @param alphabet_type The type of alphabet to create:
         *                      - 0: Binary alphabet (a, b)
         *                      - 1: DNA alphabet (A, C, G, T)
         *                      - 2: 8-letter alphabet (a-h)
         *                      - 3: English alphabet (A-Z)
         *                      - 4: Full alphabet (ASCII 2-249)
         * @return std::vector<uint8_t> The requested alphabet
         */
        static std::vector<uint8_t> create_alphabet(uint64_t alphabet_type)
        {
            if (alphabet_type == 0)
            {
                return create_ab_alphabet();
            }
            else if (alphabet_type == 1)
            {
                return create_ACGT_alphabet();
            }
            else if (alphabet_type == 2)
            {
                return create_abcdefgh_alphabet();
            }
            else if (alphabet_type == 3)
            {
                return create_AtoZ_alphabet();
            }
            else
            {
                return create_full_alphabet();
            }
        }
    };
}