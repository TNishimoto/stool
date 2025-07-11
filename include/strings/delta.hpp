#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "../basic/rational.hpp"

namespace stool
{

    /*! 
    * @brief Class for computing substring complexity functions
    * 
    * This class provides methods for computing various complexity functions related to substrings
    * of a given text. It includes functions for constructing distinct substring counter arrays,
    * computing delta values, and creating count profile arrays.
    */
    class SubstringComplexityFunctions
    {
    public:

        /*!
        * @brief Constructs a distinct substring counter array from text and suffix array
        * 
        * This function computes the number of distinct substrings of each length in the given text.
        * It uses the suffix array to efficiently determine which substrings are unique.
        * 
        * @tparam T The data type of the text elements (default: char)
        * @param text The input text as a vector of elements
        * @param sa The suffix array of the text
        * @return A vector where index i contains the number of distinct substrings of length i
        * 
        * @note The function prints progress information during computation
        * @note The returned array is truncated when all remaining substrings are distinct
        */
        template <typename T = char>
        static std::vector<uint64_t> construct_distinct_substring_counter_array(const std::vector<T> &text, const std::vector<uint64_t> &sa)
        {
            std::vector<uint64_t> distinct_substring_counter_array;
            distinct_substring_counter_array.resize(text.size() + 1, 0);
            std::cout << "Computing delta: "
                      << "0/" << text.size() << std::flush;
            for (uint64_t i = 0; i < text.size(); i++)
            {
                bool b = true;
                if (i % 100 == 0)
                {
                    std::cout << "\r"
                              << "Computing delta: " << i << "/" << text.size() << std::flush;
                }

                uint64_t max = text.size() - sa[i];
                // std::cout << std::endl;
                for (uint64_t j = 0; j < max; j++)
                {
                    if (i == 0)
                    {
                        b = false;
                    }
                    else if (b)
                    {
                        uint64_t prevpos = sa[i - 1] + j;
                        if (prevpos < text.size())
                        {
                            T c1 = text[sa[i - 1] + j];
                            T c2 = text[sa[i] + j];
                            if (c1 != c2)
                            {
                                b = false;
                            }
                        }
                        else
                        {
                            b = false;
                        }
                    }
                    if (!b)
                    {
                        /*
                        if(j == 1){
                        for (uint64_t x = 0; x <= j; x++)
                        {
                            std::cout << text[sa[i] + x];
                        }
                        std::cout << j << std::endl;

                        }
                        */

                        distinct_substring_counter_array[j + 1]++;
                    }
                }
            }
            std::cout << "[End]" << std::endl;
            uint64_t k = 1;
            while (true)
            {
                uint64_t suf_count = text.size() - k + 1;
                if (distinct_substring_counter_array[k] == suf_count)
                {
                    break;
                }
                else
                {
                    k++;
                }
            }
            distinct_substring_counter_array.resize(k + 1);
            return distinct_substring_counter_array;
        }

        /*! 
        * @brief Computes the substring complexity delta using a distinct substring counter array
        * 
        * The delta value is the maximum ratio of distinct substrings to substring length.
        * This is a measure of the complexity of the text.
        * 
        * @param distinct_substring_counter_array The array of distinct substring counts
        * @return The substring complexity determined by the given array
        */  
        static uint64_t compute_delta(std::vector<uint64_t> &distinct_substring_counter_array)
        {
            uint64_t max_delta = 0;
            for (uint64_t i = 1; i < distinct_substring_counter_array.size(); i++)
            {
                uint64_t i_delta = distinct_substring_counter_array[i] / i;
                if (i_delta > max_delta)
                {
                    max_delta = i_delta;
                }
            }
            return max_delta;
        }

        /*!
        * @brief Computes detailed delta information including the exact rational value and position
        * 
        * This function provides more precise information about the substring complexity
        * by returning the exact rational value of the maximum delta and the position where it occurs.
        * 
        * @param distinct_substring_counter_array The array of distinct substring counts
        * @return A pair containing the maximum delta as a rational number and the position where it occurs
        */
        static std::pair<stool::Rational, uint64_t> compute_detailed_delta(std::vector<uint64_t> &distinct_substring_counter_array)
        {
            stool::Rational max_delta = stool::Rational(0, 1);
            uint64_t x = 0;
            for (uint64_t i = 1; i < distinct_substring_counter_array.size(); i++)
            {
                stool::Rational i_delta(distinct_substring_counter_array[i], i);
                if (i_delta >= max_delta)
                {
                    max_delta = i_delta;
                    x = i;
                }
            }
            return std::pair<stool::Rational, uint64_t>(max_delta, x);
        }

        /*!
        * @brief Computes LCP (Longest Common Prefix) statistics from an LCP array
        * 
        * This function creates a histogram of LCP values, counting how many times each LCP value occurs.
        * 
        * @param lcp_array The LCP array to analyze
        * @return A vector where index i contains the count of LCP values equal to i
        */
        static std::vector<uint64_t> compute_LCP_statistics(const std::vector<uint64_t> &lcp_array)
        {
            uint64_t max = *std::max_element(lcp_array.begin(), lcp_array.end());
            std::vector<uint64_t> r;
            r.resize(max + 1, 0);
            for (auto &it : lcp_array)
            {
                r[it]++;
            }
            return r;
        }

        /*!
        * @brief Constructs a distinct substring counter array from an LCP array
        * 
        * This function uses the LCP array to efficiently compute the distinct substring counter array
        * without needing the original text and suffix array.
        * 
        * @param lcp_array The LCP array of the text
        * @return A vector where index i contains the number of distinct substrings of length i
        */
        static std::vector<uint64_t> construct_distinct_substring_counter_array(const std::vector<uint64_t> &lcp_array)
        {
            std::vector<uint64_t> profiler = compute_LCP_statistics(lcp_array);
            return construct_distinct_substring_counter_array_from_lcp_statistics(profiler, lcp_array.size());
        }

        /*!
        * @brief Constructs a distinct substring counter array from LCP statistics
        * 
        * This function builds the distinct substring counter array using pre-computed LCP statistics
        * and the text size. It's useful when you already have the LCP histogram.
        * 
        * @param lcp_statistics The histogram of LCP values
        * @param text_size The size of the original text
        * @return A vector where index i contains the number of distinct substrings of length i
        */
        static std::vector<uint64_t> construct_distinct_substring_counter_array_from_lcp_statistics(const std::vector<uint64_t> &lcp_statistics, uint64_t text_size)
        {
            std::vector<uint64_t> output;
            output.push_back(0);

            for (uint64_t i = 0; i < lcp_statistics.size(); i++)
            {
                uint64_t counter = lcp_statistics[i];
                uint64_t prev_value = output[output.size() - 1];
                uint64_t new_value = prev_value == 0 ? counter : (prev_value + counter - 1);
                output.push_back(new_value);

                uint64_t suf_count = text_size - i;
                if (new_value == suf_count)
                {
                    break;
                }
            }
            return output;
        }

        /*!
        * @brief Prints the distinct substring counter array in a formatted table
        * 
        * This function displays the distinct substring counter array along with the ratio
        * of distinct substrings to substring length for each position. It automatically
        * determines how many entries to display based on the delta pattern.
        * 
        * @param lcp_array The LCP array used to construct the distinct substring counter array
        */
        static void print_distinct_substring_counter_array(const std::vector<uint64_t> &lcp_array)
        {
            std::vector<uint64_t> table = construct_distinct_substring_counter_array(lcp_array);
            uint64_t x = table.size();

            for (uint64_t i = 2; i < table.size(); i++)
            {
                Rational value1(table[i - 1], i - 1);
                Rational value2(table[i], i);
                if (value2 >= value1)
                {
                    x = i + 5;
                }
            }
            uint64_t max = std::min((uint64_t)table.size(), x);

            std::cout << "============ distinct_substring_counter_array ===============" << std::endl;
            for (uint64_t i = 1; i < max; i++)
            {
                Rational value(table[i], i);
                std::cout << "d = " << i << ", count: " << table[i] << ", count/d: " << value.to_string(true) << std::endl;
            }
        }
    };

}
