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
    * @brief Class for computing substring complexity delta
    * 
    * This class provides methods for computing substring complexity delta. 
    * See https://arxiv.org/abs/2007.08357 for the definition of delta.
     * \ingroup StringClasses
    */
    class SubstringComplexityFunctions
    {
    public:

        /*!
        * @brief Constructs a distinct substring counter array (DSCA) from a text and its suffix array 
        * 
        * This function constructs a DSCA from a text and its suffix array.
        * Here, DSCA[i] contains the number of distinct substrings of length i, and its length is the same as the text.
        * 
		 * @tparam TEXT The type of the text (e.g., std::vector<uint8_t>, std::vector<char>, std::string)
        * @tparam INDEX The index type for positions (defaults to uint64_t)
        * @param text The input text T
        * @param sa The SA of T
        * @return The DSCA of T
        */
        template <typename TEXT = std::vector<uint8_t>, typename INDEX = uint64_t>
        static std::vector<uint64_t> construct_distinct_substring_counter_array(const TEXT &text, const std::vector<INDEX> &sa)
        {
            std::vector<uint64_t> DSCA;
            DSCA.resize(text.size() + 1, 0);
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
                            auto c1 = text[sa[i - 1] + j];
                            auto c2 = text[sa[i] + j];
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

                        DSCA[j + 1]++;
                    }
                }
            }
            std::cout << "[End]" << std::endl;
            uint64_t k = 1;
            while (true)
            {
                uint64_t suf_count = text.size() - k + 1;
                if (DSCA[k] == suf_count)
                {
                    break;
                }
                else
                {
                    k++;
                }
            }
            DSCA.resize(k + 1);
            return DSCA;
        }

        /*! 
        * @brief Computes the substring complexity delta using a distinct substring counter array (DSCA)
        * 
        * @param DSCA The DSCA of a text T
        * @return The substring complexity delta of T
        */  
        static uint64_t compute_delta(const std::vector<uint64_t> &DSCA)
        {
            uint64_t max_delta = 0;
            for (uint64_t i = 1; i < DSCA.size(); i++)
            {
                uint64_t i_delta = DSCA[i] / i;
                if (i_delta > max_delta)
                {
                    max_delta = i_delta;
                }
            }
            return max_delta;
        }

        /*!
        * @brief Computes the details of delta using a distinct substring counter array (DSCA)
        * 
        * @param DSCA The DSCA of a text T
        * @return A pair containing the maximum delta as a rational number and the position where it occurs
        */
        static std::pair<stool::Rational, uint64_t> compute_detailed_delta(const std::vector<uint64_t> &DSCA)
        {
            stool::Rational max_delta = stool::Rational(0, 1);
            uint64_t x = 0;
            for (uint64_t i = 1; i < DSCA.size(); i++)
            {
                stool::Rational i_delta(DSCA[i], i);
                if (i_delta >= max_delta)
                {
                    max_delta = i_delta;
                    x = i;
                }
            }
            return std::pair<stool::Rational, uint64_t>(max_delta, x);
        }

        /*!
        * @brief Computes the LCP statistics (LCPS) array from an LCP array
        * 
        * @param lcp_array an LCP array
        * @return LCPS array. Here, LCPS[i] contains the number of LCP values equal to i.
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
        * @brief Constructs a distinct substring counter array (DSCA) from an LCP array 
        * 
        * @param lcp_array The LCP array of a text T
        * @return The DSCA of T
        */
        static std::vector<uint64_t> construct_distinct_substring_counter_array(const std::vector<uint64_t> &lcp_array)
        {
            std::vector<uint64_t> profiler = compute_LCP_statistics(lcp_array);
            return construct_distinct_substring_counter_array_from_lcp_statistics(profiler, lcp_array.size());
        }

        /*!
        * @brief Constructs a distinct substring counter array (DSCA) from LCP statistics (LCPS) array
        * 
        * @param lcp_statistics The LCPS array of a text T
        * @param text_size The length of T
        * @return The DSCA of T
        */
        static std::vector<uint64_t> construct_distinct_substring_counter_array_from_lcp_statistics(const std::vector<uint64_t> &lcp_statistics, uint64_t text_size)
        {
            std::vector<uint64_t> DSCA;
            DSCA.push_back(0);

            for (uint64_t i = 0; i < lcp_statistics.size(); i++)
            {
                uint64_t counter = lcp_statistics[i];
                uint64_t prev_value = DSCA[DSCA.size() - 1];
                uint64_t new_value = prev_value == 0 ? counter : (prev_value + counter - 1);
                DSCA.push_back(new_value);

                uint64_t suf_count = text_size - i;
                if (new_value == suf_count)
                {
                    break;
                }
            }
            return DSCA;
        }

        /*!
        * @brief Prints the distinct substring counter array (DSCA) in a formatted table
        * 
        * @param lcp_array The LCP array used to construct the DSCA
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
