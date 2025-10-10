#pragma once
#include <sdsl/bit_vectors.hpp>
#include <sdsl/wt_gmr.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>

#include "./char_interval.hpp"

namespace stool
{
    /*!
     * @brief A data structure for efficient interval searching in text sequences [Unchecked AI's Comment] 
     * 
     * IntervalSearchDataStructure provides functionality for searching intervals in text sequences
     * using wavelet tree based data structures. It includes methods for:
     * - Finding character intervals in text ranges
     * - Computing occurrence counts of characters
     * - Efficient range queries using wavelet trees
     * 
     * The class is particularly useful for:
     * - Pattern matching applications
     * - Text indexing and searching
     * - Suffix array interval computations
     * 
     * @note This implementation uses SDSL wavelet trees for efficient querying
     *       and supports generic character types through templates
     */
    template <typename CHAR>
    class IntervalSearchDataStructure
    {
    public:
        using WT = sdsl::wt_huff<>;
        std::vector<uint64_t> *C;
        WT *wt;
        CHAR lastChar;
        std::vector<CHAR> cs;
        std::vector<uint64_t> cs1;
        std::vector<uint64_t> cs2;

        uint64_t get_text_size() const {
            return this->wt->size();
        }

        void initialize(WT *_wt, std::vector<uint64_t> *_C, CHAR _lastChar)
        {
            this->wt = _wt;
            this->C = _C;
            this->lastChar = _lastChar;

            cs.resize(256, 0);
            cs1.resize(256, 0);
            cs2.resize(256, 0);
        }
        template <typename INDEX_SIZE>
        uint64_t getIntervals(INDEX_SIZE i, INDEX_SIZE j, std::vector<CharInterval<INDEX_SIZE, CHAR>> &output)
        {
            using CHARINTV = CharInterval<INDEX_SIZE, CHAR>;
            std::vector<CHARINTV> r;
            uint64_t k;
            uint64_t newJ = j + 1 == wt->size() ? wt->size() : j + 2;
            uint64_t p = 0;

            // std::cout << "@[" << i << "/" << j << ", " << wt->size() << "]" << std::endl;

            sdsl::interval_symbols(*wt, i + 1, newJ, k, cs, cs1, cs2);

            bool b = j + 1 < wt->size();
            for (INDEX_SIZE x = 0; x < k; x++)
            {
                INDEX_SIZE left = (*C)[cs[x]] + cs1[x];
                INDEX_SIZE right = left + (cs2[x] - cs1[x] - 1);

                // uint64_t right = C[cs[x]] + cs2[x]+1;

                if (j + 1 == wt->size() && cs[x] == lastChar)
                {
                    right++;
                    b = true;
                }

                // std::cout << ((int)cs[x]) << "/" << left << "/" << right << std::endl;
                output[p++] = CHARINTV(left, right, cs[x]);
                // r.push_back(CHARINTV(left, right, cs[x]));
            }
            if (!b)
            {
                INDEX_SIZE num = wt->rank(wt->size(), lastChar) + 1;
                INDEX_SIZE left = (*C)[lastChar] + num - 1;
                INDEX_SIZE right = left;
                output[p++] = CHARINTV(left, right, lastChar);
                // r.push_back(CHARINTV(left, right, lastChar));
            }

            return p;
        }
    };
} // namespace stool