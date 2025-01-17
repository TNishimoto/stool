#pragma once

#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/wt_gmr.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>

namespace stool
{
    class SDSLFunctions
    {
    public:
        template <typename INDEX = uint64_t>
        static void construct_BWT(const std::vector<uint8_t> &text, const std::vector<INDEX> &sa, sdsl::int_vector<> &outputBWT)
        {
            outputBWT.width(8);
            outputBWT.resize(text.size());

            INDEX n = text.size();
            for (INDEX i = 0; i < n; i++)
            {
                if (sa[i] == 0)
                {
                    outputBWT[i] = (uint8_t)text[n - 1];
                }
                else
                {
                    outputBWT[i] = (uint8_t)text[sa[i] - 1];
                }
            }
        }

        static void to_int_vector(const std::vector<uint8_t> &text, sdsl::int_vector<> &output)
        {
            output.width(8);
            output.resize(text.size());

            uint64_t n = text.size();
            for (uint64_t i = 0; i < n; i++)
            {
                output[i] = text[i];
            }

        }

    };
}
