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
#include "../strings/sa_bwt_lcp.hpp"
#include "../debug/print.hpp"

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
            uint64_t end_marker_position;
            uint8_t end_marker = 0;

        public:
            using INDEX = uint64_t;

            uint64_t lf(uint64_t i) const
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
            uint64_t get_end_marker_position() const {
                return this->end_marker_position;
            }
            uint64_t get_text_size() const {
                return this->bwt.size();
            }
            private:

            static std::vector<uint64_t> construct_c_array(const std::vector<uint64_t> &char_counter_vector)
            {
                uint64_t CHARMAX = 256;
                std::vector<uint64_t> output;
                output.resize(CHARMAX, 0);

                for (uint64_t i = 1; i < output.size(); i++)
                {
                    output[i] = output[i - 1] + char_counter_vector[i - 1];
                }
                return output;
            }
            static sdsl::wt_gmr<> construct_wt_gmr(const sdsl::int_vector<> &bwt, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {

                if (message_paragraph >= 0 && bwt.size() > 0)
                {
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing WaveletTree(gmr)..." << std::flush;
                }

                sdsl::wt_gmr<> wt;
                construct_im(wt, bwt);
                if (message_paragraph >= 0 && bwt.size() > 0)
                {
                    std::cout  << "[END]" << std::endl;
                }

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
            public:

            static LFDataStructure build(const std::vector<uint8_t> &bwt, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {
                uint64_t text_size = bwt.size();

                if (message_paragraph >= 0 && bwt.size() > 0)
                {
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing LFDataStructure from BWT..." << std::endl;
                }

                std::chrono::system_clock::time_point st1, st2;
                st1 = std::chrono::system_clock::now();

                sdsl::int_vector<> _bwt;
                _bwt.width(8);
                _bwt.resize(text_size);

                std::vector<uint64_t> char_counter_vec;
                char_counter_vec.resize(256, 0);

                uint64_t end_marker_position = UINT64_MAX;
                uint64_t end_marker = UINT64_MAX;
                for (uint64_t i = 0; i < text_size; i++)
                {
                    _bwt[i] = bwt[i];
                    char_counter_vec[bwt[i]]++;
                    if((uint64_t)bwt[i] < end_marker){
                        end_marker = bwt[i];
                        end_marker_position = i;
                    }
                }
                std::vector<uint64_t> c_array = stool::bwt::LFDataStructure::construct_c_array(char_counter_vec);

                sdsl::wt_gmr<> wt_gmr = stool::bwt::LFDataStructure::construct_wt_gmr(_bwt, stool::Message::add_message_paragraph(message_paragraph));

                LFDataStructure r;
                r.bwt.swap(_bwt);
                r.wt.swap(wt_gmr);
                r.C.swap(c_array);
                r.end_marker = end_marker;
                r.end_marker_position = end_marker_position;


                st2 = std::chrono::system_clock::now();
                if (message_paragraph >= 0 && bwt.size() > 0)
                {
                    uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
                    uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
                    uint64_t per_time = ((double)ms_time / (double)bwt.size()) * 1000000;

                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
                }

                return r;
            }
        };
    }

} // namespace stool