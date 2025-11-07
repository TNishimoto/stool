#pragma once
#include <string>
#include <vector>
#include <cassert>

namespace stool
{

    /**
     * @brief A utility class for functions on strings. 
     * \ingroup StringClasses
     */
    class StringFunctions
    {
    public:
        /*!
         * @brief Checks if a text \p T[0..n-1] contains a end marker character \p c
         *
         * @tparam CHAR The character type of the text
         * @return true if the text satisfies all conditions, throws an exception otherwise
         * @throws std::logic_error if any of the conditions are violated
         * @note The character \p c must satisfy the following conditions: (i) any character in \p T[0..n-2] is smaller than \p c and (ii) the last character of \p T is \p c.
         */
        template <typename CHAR>
        static bool check_text_with_end_marker(const std::vector<CHAR> &T, CHAR c)
        {
            uint64_t p = UINT64_MAX;
            for (uint64_t i = 0; i < T.size(); i++)
            {
                if (T[i] == c)
                {
                    p = i;
                    break;
                }
                else if (T[i] < c)
                {
                    throw std::logic_error("The characters of the input text must not be less than '0'.");
                }
            }
            if (p == T.size() - 1)
            {
                return true;
            }
            else if (p == UINT64_MAX)
            {
                throw std::logic_error("The last character of the input text must be '0'");
            }
            else
            {
                throw std::logic_error("The input text must not contain '0' except for the last character.");
            }
        }

        /**
         * @brief Collects the unique characters (alphabet) from the input text \p T[0..n-1].
         * @return A vector containing the unique characters in \p T (the characters are sorted in increasing order).
         */
        static std::vector<uint8_t> get_alphabet(const std::vector<uint8_t> &T)
        {
            std::vector<bool> checker;
            checker.resize(256, false);
            for (auto c : T)
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
         * @brief Gets the i-th suffix of the text \p T[0..n-1].
         */
        static std::vector<uint8_t> get_suffix(const std::vector<uint8_t> &T, uint64_t i)
        {
            std::vector<uint8_t> r;
            for (uint64_t x = i; x < T.size(); x++)
            {
                r.push_back(T[x]);
            }
            return r;
        }
        /**
         * @brief Gets the i-th suffix of the text \p T[0..n-1].
         */
        static std::string get_suffix_str(const std::vector<uint8_t> &T, uint64_t i)
        {
            std::string r;
            for (uint64_t x = i; x < T.size(); x++)
            {
                r.push_back(T[x]);
            }
            return r;
        }

        /**
         * @brief Computes the Longest Common Extension (LCE) between a text \p T[0..n-1] and a pattern \p P[0..m-1] (i.e., the length of the longest common prefix of \p T[i..] and \p P).
         */
        static uint64_t lce(const std::vector<uint8_t> &T, uint64_t i, const std::vector<uint8_t> &P)
        {
            for (uint64_t x = 0; x < P.size(); x++)
            {
                if (i + x < T.size())
                {
                    if (T[i + x] != P[x])
                    {
                        return x;
                    }
                }
                else
                {
                    return x;
                }
            }
            return P.size();
        }

        /**
         * @brief Computes the Longest Common Extension (LCE) between two positions \p i and \p j in a text \p T[0..n-1] (i.e., the length of the longest common prefix of \p T[i..n-1] and \p T[j..n-1]).
         */
        static uint64_t lce(const std::string &T, uint64_t i, uint64_t j)
        {
            if (i > j)
                return lce(T, j, i);
            uint64_t max = T.size() - j;
            uint64_t x = 0;
            for (x = 0; x < max; x++)
            {
                if (T[i + x] != T[j + x])
                {
                    break;
                }
            }
            return x;
        }

        /**
         * @brief Reverses the input text \p T[0..n-1] in-place.
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
         * @brief Reverses the input text \p T and returns it as a new vector.
         */
        static std::vector<uint8_t> to_reversed_string(const std::vector<uint8_t> &T)
        {
            std::vector<uint8_t> r;
            r.resize(T.size(), 0);

            size_t size = T.size();
            for (size_t i = 0; i < size; i++)
            {
                r[i] = T[size - i - 1];
            }
            return r;
        }

        /**
         * @brief Generates all possible strings of a given length \p m using the provided alphabet (e.g., get_all_strings(2, {a, b}) returns {"aa", "ab", "ba", "bb"}).
         */
        static std::vector<std::string> get_all_strings(uint64_t m, const std::vector<uint8_t> &alphabets)
        {
            std::vector<std::string> r;

            if (m == 0)
            {
                return r;
            }
            else if (m == 1)
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
                std::vector<std::string> pref_vec = get_all_strings(m - 1, alphabets);
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
         * @brief Generates all possible strings of a given length \p m using a specified alphabet size (e.g., get_all_strings(2, 2) returns {"aa", "ab", "ba", "bb"}).
         * @note The alphabet is the set of characters {a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u}.
         */
        static std::vector<std::string> get_all_strings(uint64_t m, uint8_t alphabet_size)
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
            return get_all_strings(m, alphabets);
        }

        

        
        /**
         * @brief Locates all occurrences of a pattern P in a text T.
         * @return A vector containing the starting positions of all occurrences of the pattern (the occurrences are sorted in increasing order).
         */
        static std::vector<uint64_t> locate_query(const std::vector<uint8_t> &T, const std::vector<uint8_t> &P)
        {
            if (P.size() == 0)
            {
                std::vector<uint64_t> positions;
                for (uint64_t i = 0; i < T.size(); i++)
                {
                    positions.push_back(i);
                }
                return positions;
            }
            else
            {
                std::vector<uint64_t> positions;

                if (P.empty() || T.size() < P.size())
                {
                    return positions;
                }

                auto it = T.begin();
                while (it != T.end())
                {
                    it = std::search(it, T.end(), P.begin(), P.end());

                    if (it != T.end())
                    {
                        uint64_t find_pos = std::distance(T.begin(), it);
                        positions.push_back(find_pos);
                        ++it;
                    }
                }

                return positions;
            }
        }

        /**
         * @brief Counts the number of occurrences of a character \p c in a given suffix \p T[i..n-1] of a text \p T[0..n-1].
         */
        template <typename CHAR = uint8_t>
        static int64_t rank_query(const std::vector<CHAR> &T, uint64_t i, CHAR c)
        {
            uint64_t counter = 0;
            for (uint64_t x = 0; x < i; x++)
            {
                if (T[x] == c)
                {
                    counter++;
                }
            }
            return counter;
        }

        /**
         * @brief Finds the position of the (i+1)-th occurrence of a character \p c in a text \p T[0..n-1].
         * @return The position of the (i+1)-th occurrence, or -1 if it does not exist.
         */
        template <typename CHAR = uint8_t>
        static int64_t select_query(const std::vector<CHAR> &T, uint64_t i, CHAR c)
        {
            uint64_t counter = 0;
            for (uint64_t x = 0; x < T.size(); x++)
            {
                if (T[x] == c)
                {
                    counter++;
                }
                if (counter == i + 1)
                {
                    return x;
                }
            }
            return -1;
        }

        /**
         * @brief Returns the length of the longest common suffix (LCS) of two strings \p T and \p P.
         */
        static uint64_t lcs(const std::vector<uint8_t> &T, const std::vector<uint8_t> &P)
        {
            int64_t min = std::min(T.size(), P.size());
            for (int64_t i = 0; i < min; i++)
            {
                if (T[T.size() - 1 - i] != P[P.size() - 1 - i])
                {
                    return i;
                }
            }
            return min;
        }

        /**
         * @brief Returns the length of the longest common prefix (LCP) of two strings \p T and \p P.
         */
        static uint64_t lcp(const std::vector<uint8_t> &T, const std::vector<uint8_t> &P)
        {
            uint64_t min = std::min(T.size(), P.size());
            for (uint64_t i = 0; i < min; i++)
            {
                if (T[i] != P[i])
                {
                    return i;
                }
            }
            return min;
        }

        /**
         * @brief Returns the LCP between two strings \p T and \p P and the comparison result of the first different characters.
         * @return A pair containing the LCP and the comparison result of the first different characters (i.e., -1 if the first string is lexicographically less than the second string, 1 if the first string is lexicographically greater than the second string, 0 if the two strings are equal).
         */
        static std::pair<uint64_t, int8_t> lcp_and_compare(const std::vector<uint8_t> &T, const std::vector<uint8_t> &P)
        {
            uint64_t lcp = StringFunctions::lcp(T, P);
            int8_t com = 0;
            if(lcp < T.size() && lcp < P.size()){
                com = T[lcp] <  P[lcp] ? -1 : 1;
            }else{
                if(T.size() < P.size()){
                    com = -1;
                }else if(T.size() > P.size()){
                    com = 1;
                }else{
                    com = 0;
                }
            }
            return std::make_pair(lcp, com);

        }
    };
} // namespace stool