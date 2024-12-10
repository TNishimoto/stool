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

#include <queue>
#include "../sa_bwt_lcp.hpp"
#include "../print.hpp"

// #include "sa_lcp.hpp"
// using namespace std;
// using namespace sdsl;

namespace stool
{
    namespace bwt
    {
        class LFDataStructure
        {
            sdsl::int_vector<> bwt;
            std::vector<uint64_t> C;
            sdsl::wt_gmr<> wt;
            uint8_t end_marker = 0;

        public:
            uint64_t LF(uint64_t i)
            {
                uint8_t c = this->bwt[i];
                uint64_t cNum = this->wt.rank(i, c);
                return C[c] + cNum;
            }
            std::vector<uint64_t> construct_suffix_array()
            {
                std::vector<uint64_t> sa;
                sa.resize(this->bwt.size(), UINT64_MAX);
                uint64_t size = sa.size();
                uint64_t p = this->wt.select(1, this->end_marker);
                sa[p] = 0;
                uint64_t current_sa_value = size - 1;

                for (uint64_t i = 0; i < size; i++)
                {
                    p = BWTFunctions::LF(p, this->bwt, this->C, this->wt);
                    sa[p] = current_sa_value;
                    current_sa_value--;
                }
                return sa;
            }

            static std::vector<uint64_t> construct_c_array(const sdsl::int_vector<> &bwt)
            {
                std::vector<uint64_t> output;
                uint64_t CHARMAX = UINT8_MAX + 1;

                std::vector<uint64_t> tmp;

                tmp.resize(CHARMAX, 0);
                output.resize(CHARMAX, 0);
                uint64_t size = bwt.size();

                for (uint64_t i = 0; i < size; i++)
                {
                    // assert(text[i] >= 0 && text[i] < tmp.size());
                    tmp[bwt[i]]++;
                }

                for (uint64_t i = 1; i < output.size(); i++)
                {
                    output[i] = output[i - 1] + tmp[i - 1];
                }
                return output;
            }
            static sdsl::wt_gmr<> construct_wt_gmr(const sdsl::int_vector<> &bwt)
            {
                sdsl::wt_gmr<> wt;
                construct_im(wt, bwt);
                return wt;
            }
            template <typename INDEX = uint64_t>
            static sdsl::int_vector<> construct_bwt(const std::vector<uint8_t> &text, const std::vector<INDEX> &sa)
            {
                sdsl::int_vector<> outputBWT;
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
                return outputBWT;
            }

            static LFDataStructure build(const std::vector<uint8_t> &bwt)
            {
                uint64_t text_size = bwt.size();
                sdsl::int_vector<> _bwt;
                _bwt.width(8);
                _bwt.resize(text_size);

                uint64_t end_marker = UINT64_MAX;
                for (uint64_t i = 0; i < text_size; i++)
                {
                    _bwt[i] = bwt[i];
                    end_marker = std::min((uint64_t)bwt[i], end_marker);
                }
                sdsl::wt_gmr<> wt_gmr = stool::bwt::LFDataStructure::construct_wt_gmr(_bwt);
                std::vector<uint64_t> c_array = stool::bwt::LFDataStructure::construct_c_array(_bwt);

                LFDataStructure r;
                r.bwt.swap(_bwt);
                r.wt.swap(wt_gmr);
                r.C.swap(c_array);
                r.end_marker = end_marker;
                return r;
            }
        };
    }

} // namespace stool