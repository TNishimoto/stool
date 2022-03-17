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
    class SortString
    {
    public:
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
            /*
            uint64_t min = std::min(str1.size(), str2.size());
            for (uint64_t i = 0; i < min; i++)
            {
                if (str1[i] != str2[i])
                {
                    return str1[i] < str2[i];
                }
            }
            if (str1.size() != str2.size())
            {
                return str1.size() < str2.size();
            }
            return false;
            */
        }

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