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
        class BWTFunctions
        {
        public:
            static uint64_t LF(uint64_t i, const sdsl::int_vector<> &bwt, const std::vector<uint64_t> &C, const sdsl::wt_gmr<> &wt)
            {
                uint8_t c = bwt[i];
                uint64_t cNum = wt.rank(i, c);
                return C[c] + cNum;
            }

            template <typename TEXT, typename OUTPUT>
            static void construct_C_array(TEXT &text, OUTPUT &output, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {
                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing C array..." << std::flush;
                }

                uint64_t CHARMAX = UINT8_MAX + 1;
                output.resize(CHARMAX, 0);
                std::vector<uint64_t> tmp;
                tmp.resize(CHARMAX, 0);
                for(uint64_t i = 0; i < text.size(); i++){
                    tmp[text[i]]++;
                }
                for(uint64_t i = 1; i < CHARMAX; i++){
                    output[i] = output[i - 1] + tmp[i - 1];
                }

                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << "[DONE]" << std::endl;
                }

            }


            template <typename OUTPUT>
            static void construct_C_array(sdsl::wt_huff<> &wt, uint8_t lastChar, OUTPUT &output)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;

                std::vector<uint64_t> tmp;

                tmp.resize(CHARMAX, 0);
                output.resize(CHARMAX, 0);

                for (uint64_t i = 0; i < wt.size() - 1; i++)
                {
                    assert(wt[i] >= 0 && wt[i] < tmp.size());
                    tmp[wt[i]]++;
                }
                tmp[lastChar]++;

                for (uint64_t i = 1; i < output.size(); i++)
                {
                    output[i] = output[i - 1] + tmp[i - 1];
                }
            }

            static void constructFrequencyArray(sdsl::wt_huff<> &wt, uint8_t last_char, std::vector<uint64_t> &output)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;

                output.resize(CHARMAX, 0);
                for (uint64_t i = 0; i < CHARMAX; i++)
                {
                    uint64_t k = wt.rank(wt.size(), i);
                    if (i == 0)
                    {
                        /*
                        if(k == 2){
                            k = 1;
                        }else{
                            std::cout << "Error: constructFrequencyArray" << std::endl;
                            assert(false);
                            throw -1;
                        }
                        */
                    }
                    else if (i == last_char)
                    {
                        k++;
                    }
                    else if (i == 8)
                    {
                        k = 0;
                    }
                    output[i] = k;
                }
            }
            /*
            static void constructCArray(sdsl::wt_huff<> &wt, uint8_t last_char, std::vector<uint64_t> &output)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                std::vector<uint64_t> freqArr;
                constructFrequencyArray(wt, last_char, freqArr);
                // stool::Printer::print(freqArr);

                output.resize(CHARMAX, 0);

                for (uint64_t i = 1; i < CHARMAX; i++)
                {
                    output[i] = output[i - 1] + freqArr[i - 1];
                }
            }
            */


            

            
            static uint64_t FL(uint64_t i, std::vector<uint64_t> &C, sdsl::wt_gmr<> &wt)
            {
                uint64_t x = 0;
                for (x = 0; x < C.size(); x++)
                {
                    // std::cout << "xxx:" << x << "/" << C[x] << std::endl;
                    if (C[x] <= i && i < C[x + 1])
                    {
                        break;
                    }
                }
                uint8_t c = x;
                uint64_t nth = i - C[x];

                // std::cout << "xxx:" << c << "/" << nth << std::endl;
                return wt.select((nth + 1), c);
            }
            static uint64_t get_start_pos(sdsl::int_vector<> &bwt)
            {
                for (uint64_t i = 0; i < bwt.size(); i++)
                {
                    if (bwt[i] == 0)
                    {
                        return i;
                    }
                }
                return UINT64_MAX;
            }
            static uint8_t getUpperChar(uint64_t i, std::vector<uint64_t> &C)
            {
                std::vector<uint8_t> chars;
                for (uint64_t x = 0; x < C.size(); x++)
                {
                    for (uint64_t p = C[x]; p < C[x + 1]; p++)
                    {
                        chars.push_back(x);
                    }
                }
                if (i == 0)
                {
                    return chars[chars.size() - 1];
                }
                else
                {
                    return chars[i - 1];
                }
            }
            static uint8_t getFChar(uint64_t i, std::vector<uint64_t> &C)
            {
                std::vector<uint8_t> chars;
                for (uint64_t x = 0; x < C.size(); x++)
                {
                    for (uint64_t p = C[x]; p < C[x + 1]; p++)
                    {
                        chars.push_back(x);
                    }
                }
                return chars[i];
            }
        };
    }

} // namespace stool