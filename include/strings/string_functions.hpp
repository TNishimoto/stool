#pragma once
#include <string>
#include <vector>
#include <cassert>

namespace stool
{

    /**
     * @brief A utility class for string manipulation functions.
     */
    class StringFunctions
    {
    public:
        /**
         * @brief Extracts the unique characters (alphabet) from the input text.
         * @param text The input text as a vector of uint8_t.
         * @return A vector containing the unique characters in the text.
         */
        static std::vector<uint8_t> get_alphabet(const std::vector<uint8_t> &text)
        {
            std::vector<bool> checker;
            checker.resize(256, false);
            for (auto c : text)
            {
                checker[c] = true;
            }

            std::vector<uint8_t> r;
            for (size_t i = 0; i < checker.size(); i++)
            {
                if (checker[i])
                {
                    r.push_back(i);
                }
            }
            return r;
        }

        /**
         * @brief Gets the suffix of the text starting from index i.
         * @param text The input text as a vector of uint8_t.
         * @param i The starting index for the suffix.
         * @return A vector containing the suffix of the text.
         */
        static std::vector<uint8_t> get_suffix(const std::vector<uint8_t> &text, uint64_t i)
        {
            std::vector<uint8_t> r;
            for (uint64_t x = i; x < text.size(); x++)
            {
                r.push_back(text[x]);
            }
            return r;
        }
        /**
         * @brief Gets the suffix of the text starting from index i as a string.
         * @param text The input text as a vector of uint8_t.
         * @param i The starting index for the suffix.
         * @return A string containing the suffix of the text.
         */
        static std::string get_suffix_str(const std::vector<uint8_t> &text, uint64_t i)
        {
            std::string r;
            for (uint64_t x = i; x < text.size(); x++)
            {
                r.push_back(text[x]);
            }
            return r;
        }

        /**
         * @brief Computes the Longest Common Extension (LCE) between a text and a pattern.
         * @param text The input text as a vector of uint8_t.
         * @param i The starting index in the text.
         * @param pattern The pattern to compare with the text.
         * @return The length of the longest common extension.
         */
        static uint64_t LCE(const std::vector<uint8_t> &text, uint64_t i, const std::vector<uint8_t> &pattern)
        {
            for (uint64_t x = 0; x < pattern.size(); x++)
            {
                if (i + x < text.size())
                {
                    if (text[i + x] != pattern[x])
                    {
                        return x;
                    }
                }
                else
                {
                    return x;
                }
            }
            return pattern.size();
        }

        /**
         * @brief Computes the Longest Common Extension (LCE) between two suffixes of a string.
         * @param text The input text as a string.
         * @param i The starting index of the first suffix.
         * @param j The starting index of the second suffix.
         * @return The length of the longest common extension.
         */
        static uint64_t LCE(const std::string &text, uint64_t i, uint64_t j)
        {
            if (i > j)
                return LCE(text, j, i);
            uint64_t max = text.size() - j;
            uint64_t x = 0;
            for (x = 0; x < max; x++)
            {
                if (text[i + x] != text[j + x])
                {
                    break;
                }
            }
            return x;
        }
        /**
         * @brief Reverses the input text in-place.
         * @param text The input text to be reversed.
         */
        static void reverse(std::string &text)
        {
            std::string tmp = text;
            for (uint64_t i = 0; i < text.size(); i++)
            {
                text[i] = tmp[text.size() - 1 - i];
            }
        }
        /**
         * @brief Reverses the input text and returns it as a new vector.
         * @param text The input text as a vector of uint8_t.
         * @return A new vector containing the reversed text.
         */
        static std::vector<uint8_t> to_reversed_string(std::vector<uint8_t> &text)
        {
            std::vector<uint8_t> r;
            r.resize(text.size(), 0);

            size_t size = text.size();
            for (size_t i = 0; i < size; i++)
            {
                r[i] = text[size - i - 1];
            }
            return r;
        }

        /**
         * @brief Generates all possible strings of a given length using the provided alphabet.
         * @param len The length of the strings to generate.
         * @param alphabets The alphabet to use for generating strings.
         * @return A vector of strings containing all possible combinations.
         */
        static std::vector<std::string> get_all_strings(uint64_t len, std::vector<uint8_t> &alphabets)
        {
            std::vector<std::string> r;

            if (len == 0)
            {
                return r;
            }
            else if (len == 1)
            {
                for (uint8_t c : alphabets)
                {
                    std::string s;
                    s.push_back(c);
                    r.push_back(s);
                }
                return r;
            }
            else
            {
                std::vector<std::string> pref_vec = get_all_strings(len - 1, alphabets);
                for (std::string &s : pref_vec)
                {
                    for (uint8_t c : alphabets)
                    {
                        std::string new_str = s + (char)c;
                        r.push_back(new_str);
                    }
                }
                return r;
            }
        }
        /**
         * @brief Generates all possible strings of a given length using a specified alphabet size.
         * @param len The length of the strings to generate.
         * @param alphabet_size The size of the alphabet to use.
         * @return A vector of strings containing all possible combinations.
         */
        static std::vector<std::string> get_all_strings(uint64_t len, uint8_t alphabet_size)
        {
            std::vector<uint8_t> alphabets;
            alphabets.push_back('a');
            alphabets.push_back('b');
            alphabets.push_back('c');
            alphabets.push_back('d');
            alphabets.push_back('e');
            alphabets.push_back('f');
            alphabets.push_back('g');
            alphabets.push_back('h');
            alphabets.push_back('i');
            alphabets.push_back('j');
            alphabets.push_back('k');
            alphabets.push_back('l');
            alphabets.push_back('m');
            alphabets.push_back('n');
            alphabets.push_back('o');
            alphabets.push_back('p');
            alphabets.push_back('q');
            alphabets.push_back('r');
            alphabets.push_back('s');
            alphabets.push_back('t');
            alphabets.push_back('u');
            while (alphabets.size() > alphabet_size)
            {
                alphabets.pop_back();
            }
            return get_all_strings(len, alphabets);
        }

        /**
         * @brief Compares two suffixes of a text.
         * @param text The input text as a vector of characters.
         * @param x The starting index of the first suffix.
         * @param y The starting index of the second suffix.
         * @return True if the first suffix is less than the second, false otherwise.
         */
        template <typename CHAR = uint8_t>
        static bool compare_suffixes(const std::vector<CHAR> &text, const uint64_t x, const uint64_t y)
        {
            uint64_t max = x < y ? text.size() - y : text.size() - x;
            for (uint64_t i = 0; i < max; i++)
            {
                CHAR c1 = text[x + i];
                CHAR c2 = text[y + i];
                if (c1 != c2)
                {
                    return c1 < c2;
                }
            }
            return x > y;
        }

        /**
         * @brief Constructs a naive suffix array for the given text.
         * @param text The input text as a vector of characters.
         * @return A vector containing the indices of the suffixes in lexicographical order.
         */
        template <typename CHAR = uint8_t>
        static std::vector<uint64_t> construct_naive_suffix_array(const std::vector<CHAR> &text)
        {
            std::vector<uint64_t> r;
            for (uint64_t i = 0; i < text.size(); i++)
            {
                r.push_back(i);
            }

            std::sort(
                r.begin(),
                r.end(),
                [&](const uint64_t &x, const uint64_t &y)
                {
                    return compare_suffixes(text, x, y);
                });
            return r;
        }
        /**
         * @brief Locates all occurrences of a pattern in the text.
         * @param text The input text as a vector of uint8_t.
         * @param pattern The pattern to search for.
         * @return A vector containing the starting positions of all occurrences of the pattern.
         */
        static std::vector<uint64_t> locate_query(const std::vector<uint8_t> &text, const std::vector<uint8_t> &pattern)
        {
            if (pattern.size() == 0)
            {
                std::vector<uint64_t> positions;
                for(uint64_t i = 0; i < text.size();i++){
                    positions.push_back(i);
                }
                return positions;
            }
            else
            {
                std::vector<uint64_t> positions;

                if (pattern.empty() || text.size() < pattern.size())
                {
                    return positions;
                }

                auto it = text.begin();
                while (it != text.end())
                {
                    it = std::search(it, text.end(), pattern.begin(), pattern.end());

                    if (it != text.end())
                    {
                        uint64_t find_pos = std::distance(text.begin(), it);
                        positions.push_back(find_pos);
                        ++it;
                    }
                }

                return positions;
            }
        }
      /**
       * @brief Counts the number of occurrences of a character in the text up to a given index.
       * @param text The input text as a vector of characters.
       * @param i The index up to which to count.
       * @param c The character to count.
       * @return The number of occurrences of the character.
       */
        template <typename CHAR = uint8_t>
        static int64_t rank_query(const std::vector<CHAR> &text, uint64_t i, CHAR c){
            uint64_t counter = 0;
            for(uint64_t x = 0; x < i;x++){
                if(text[x] == c){
                    counter++;
                }
            }
            return counter;
        }
		/**
		 * @brief Finds the position of the (i+1)-th occurrence of a character in the text.
		 * @param text The input text as a vector of characters.
		 * @param i The index of the occurrence to find.
		 * @param c The character to search for.
		 * @return The position of the (i+1)-th occurrence, or -1 if it does not exist.
		 */
        template <typename CHAR = uint8_t>
        static int64_t select_query(const std::vector<CHAR> &text, uint64_t i, CHAR c){
            uint64_t counter = 0;
            for(uint64_t x = 0; x < text.size();x++){
                if(text[x] == c){
                    counter++;
                }
                if(counter == i+1){
                    return x;
                }
            }
            return -1;
        }

    };
} // namespace stool