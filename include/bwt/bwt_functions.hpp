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
#include "../strings/array_constructor.hpp"
#include "../debug/print.hpp"


namespace stool
{
    namespace bwt
    {
        /**
         * @brief A class providing utility functions for Burrows-Wheeler Transform (BWT) operations [Unchecked AI's Comment] 
         * 
         * This class contains static methods for constructing and manipulating BWT-related data structures
         * including LF (Last-to-First) mapping, FL (First-to-Last) mapping, C array construction,
         * and various BWT utility functions.
         */
        class BWTFunctions
        {
        public:
            /**
             * @brief Computes the LF (Last-to-First) mapping for a given position
             * 
             * The LF mapping is a fundamental operation in BWT that maps a position in the BWT
             * to the corresponding position in the original text using the wavelet tree.
             * 
             * @param i The position in the BWT
             * @param bwt The Burrows-Wheeler Transform as an integer vector
             * @param C The C array containing cumulative character counts
             * @param wt The wavelet tree for the BWT
             * @return The corresponding position in the original text
             */
            static uint64_t LF(uint64_t i, const sdsl::int_vector<> &bwt, const std::vector<uint64_t> &C, const sdsl::wt_gmr<> &wt)
            {
                uint8_t c = bwt[i];
                uint64_t cNum = wt.rank(i, c);
                return C[c] + cNum;
            }

            /**
             * @brief Constructs the LF (Last-to-First) array for a given BWT
             * 
             * The LF array maps each position in the BWT to its corresponding position
             * in the original text, enabling efficient backward traversal.
             * 
             * @param bwt The Burrows-Wheeler Transform as a vector of bytes
             * @param C The C array containing cumulative character counts
             * @param message_paragraph The message level for progress reporting
             * @return A vector containing the LF mapping for each position
             */
            static std::vector<uint64_t> construct_LF_array(const std::vector<uint8_t> &bwt, const std::vector<uint64_t> &C, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {
                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing LF array..." << std::flush;
                }

                std::vector<uint64_t> rank_array;
                rank_array.resize(C.size(), 0);

                std::vector<uint64_t> LF;
                LF.resize(bwt.size(), 0);
                for(uint64_t i = 0; i < bwt.size(); i++){
                    LF[i] = C[bwt[i]] + rank_array[bwt[i]];
                    rank_array[bwt[i]]++;                    
                }
                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << "[DONE]" << std::endl;
                }

                return LF;
            }

            /**
             * @brief Constructs the FL (First-to-Last) array from the LF array
             * 
             * The FL array is the inverse of the LF array, mapping positions in the original text
             * back to positions in the BWT.
             * 
             * @param bwt The Burrows-Wheeler Transform as a vector of bytes
             * @param lf_array The pre-computed LF array
             * @param message_paragraph The message level for progress reporting
             * @return A vector containing the FL mapping for each position
             */
            static std::vector<uint64_t> construct_FL_array(const std::vector<uint8_t> &bwt, const std::vector<uint64_t> &lf_array, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {
                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing FL array..." << std::flush;
                }

                std::vector<uint64_t> fl_array;
                fl_array.resize(bwt.size(), 0);

                for(uint64_t i = 0; i < bwt.size(); i++){
                    fl_array[lf_array[i]] = i;
                }
                if(message_paragraph != stool::Message::NO_MESSAGE){
                    std::cout << "[DONE]" << std::endl;
                }

                return fl_array;
            }

            /**
             * @brief Constructs the C array from a given text
             * 
             * The C array contains cumulative counts of characters in lexicographic order,
             * which is essential for BWT operations and LF mapping.
             * 
             * @tparam TEXT The type of the input text container
             * @tparam OUTPUT The type of the output C array container
             * @param text The input text to analyze
             * @param output The output C array to be populated
             * @param message_paragraph The message level for progress reporting
             */
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

            /**
             * @brief Constructs the C array from a wavelet tree and last character
             * 
             * This version constructs the C array using a wavelet tree representation
             * of the BWT and the last character of the original text.
             * 
             * @tparam OUTPUT The type of the output C array container
             * @param wt The wavelet tree representing the BWT
             * @param lastChar The last character of the original text
             * @param output The output C array to be populated
             */
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

            /**
             * @brief Constructs a frequency array from a wavelet tree and last character
             * 
             * The frequency array contains the count of each character in the text,
             * including special handling for the last character.
             * 
             * @param wt The wavelet tree representing the BWT
             * @param last_char The last character of the original text
             * @param output The output frequency array to be populated
             */
            static void construct_frequency_array(sdsl::wt_huff<> &wt, uint8_t last_char, std::vector<uint64_t> &output)
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
                            std::cout << "Error: construct_frequency_array" << std::endl;
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

            /**
             * @brief Computes the FL (First-to-Last) mapping for a given position
             * 
             * The FL mapping is the inverse of LF mapping, using the C array and wavelet tree
             * to find the corresponding position in the BWT.
             * 
             * @param i The position in the original text
             * @param C The C array containing cumulative character counts
             * @param wt The wavelet tree for the BWT
             * @return The corresponding position in the BWT
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

            /**
             * @brief Finds the starting position of the original text in the BWT
             * 
             * The starting position is where the end-of-string character (0) appears
             * in the BWT, which corresponds to the beginning of the original text.
             * 
             * @param bwt The Burrows-Wheeler Transform as an integer vector
             * @return The position of the end-of-string character, or UINT64_MAX if not found
             */
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

            /**
             * @brief Gets the character at a given position in the sorted suffix array (F column)
             * 
             * This function reconstructs the F column of the BWT matrix and returns
             * the character at the specified position.
             * 
             * @param i The position in the F column
             * @param C The C array containing cumulative character counts
             * @return The character at position i in the F column
             */
            static uint8_t get_upper_char(uint64_t i, std::vector<uint64_t> &C)
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

            /**
             * @brief Gets the character at a given position in the F column
             * 
             * This function returns the character at the specified position in the F column
             * of the BWT matrix, which represents the first column of sorted suffixes.
             * 
             * @param i The position in the F column
             * @param C The C array containing cumulative character counts
             * @return The character at position i in the F column
             */
            static uint8_t get_F_char(uint64_t i, std::vector<uint64_t> &C)
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