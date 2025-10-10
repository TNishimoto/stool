#pragma once
#include <unordered_set>
#include <cassert>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

namespace stool
{
    /**
     * @brief A utility class for string comparison and analysis operations. [Unchecked AI's Comment] 
     * 
     * This class provides static methods for computing Longest Common Prefix (LCP),
     * Longest Common Suffix (LCS), and lexicographical comparison of strings
     * represented as vectors of uint8_t.
     */
    class SortString
    {
    public:
        /**
         * @brief Computes the Longest Common Prefix (LCP) and lexicographical comparison of two strings.
         *
         * This function calculates the LCP of two input strings and determines their lexicographical order.
         * It returns a pair containing the LCP length and a comparison result.
         *
         * @param str1 The first input string as a vector of uint8_t.
         * @param str2 The second input string as a vector of uint8_t.
         * @return std::pair<uint64_t, int8_t> A pair where the first element is the LCP length and the second element is the comparison result:
         *         -1 if str1 < str2, 1 if str1 > str2, and 0 if str1 == str2.
         */
        static std::pair<uint64_t, int8_t> lcp_and_compare(const std::vector<uint8_t> &str1, const std::vector<uint8_t> &str2)
        {
            uint64_t lcp = SortString::lcp(str1, str2);
            int8_t com = 0;
            if(lcp < str1.size() && lcp < str2.size()){
                com = str1[lcp] <  str2[lcp] ? -1 : 1;
            }else{
                if(str1.size() < str2.size()){
                    com = -1;
                }else if(str1.size() > str2.size()){
                    com = 1;
                }else{
                    com = 0;
                }
            }
            return std::pair<uint64_t, int8_t>(lcp, com);

        }

        /**
         * @brief Computes the Longest Common Prefix (LCP) of two strings.
         *
         * This function finds the length of the longest common prefix between two strings.
         * The LCP is the number of characters that match from the beginning of both strings.
         *
         * @param str1 The first input string as a vector of uint8_t.
         * @param str2 The second input string as a vector of uint8_t.
         * @return uint64_t The length of the longest common prefix.
         */
        static uint64_t lcp(const std::vector<uint8_t> &str1, const std::vector<uint8_t> &str2)
        {
            uint64_t min = std::min(str1.size(), str2.size());
            for (uint64_t i = 0; i < min; i++)
            {
                if (str1[i] != str2[i])
                {
                    return i;
                }
            }
            return min;
        }

        /**
         * @brief Computes the Longest Common Suffix (LCS) of two strings.
         *
         * This function finds the length of the longest common suffix between two strings.
         * The LCS is the number of characters that match from the end of both strings.
         *
         * @param str1 The first input string as a vector of uint8_t.
         * @param str2 The second input string as a vector of uint8_t.
         * @return uint64_t The length of the longest common suffix.
         */
        static uint64_t lcs(const std::vector<uint8_t> &str1, const std::vector<uint8_t> &str2)
        {
            int64_t min = std::min(str1.size(), str2.size());
            for (int64_t i = 0; i < min; i++)
            {
                if (str1[str1.size() - 1 - i] != str2[str2.size() - 1 - i])
                {
                    return i;
                }
            }
            return min;
        }

    };
}