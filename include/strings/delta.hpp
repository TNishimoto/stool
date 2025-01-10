#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "../basic/rational.hpp"

namespace stool
{
    class DeltaFunctions
    {
        public:

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

        static std::vector<uint64_t> create_count_profile_array(const std::vector<uint64_t> &lcp_array)
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

        static std::vector<uint64_t> construct_distinct_substring_counter_array(const std::vector<uint64_t> &lcp_array)
        {
            uint64_t text_size = lcp_array.size();
            std::vector<uint64_t> output;

            std::vector<uint64_t> profiler = create_count_profile_array(lcp_array);
            output.push_back(0);

            for (uint64_t i = 0; i < profiler.size(); i++)
            {
                uint64_t counter = profiler[i];
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
