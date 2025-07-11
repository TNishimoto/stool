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

namespace stool
{
    namespace bwt
    {
        
        /**
         * @brief LF (Last-First) mapping data structure for Burrows-Wheeler Transform
         * 
         * This class provides efficient LF mapping operations on the Burrows-Wheeler Transform (BWT)
         * of a text. The LF mapping is a fundamental operation that allows navigation through
         * the BWT and enables various string processing algorithms.
         * 
         * The data structure consists of:
         * - The BWT itself as an integer vector
         * - A C array for character counting
         * - A wavelet tree (GMR variant) for efficient rank operations
         * - End marker information for BWT navigation
         */
        class LFDataStructure
        {
            sdsl::int_vector<> bwt;           ///< The Burrows-Wheeler Transform as an integer vector
            std::vector<uint64_t> C;          ///< C array for character counting and LF mapping
            sdsl::wt_gmr<> wt;                ///< Wavelet tree (GMR variant) for efficient rank operations
            uint64_t end_marker_position;     ///< Position of the end marker in the BWT
            uint8_t end_marker = 0;           ///< The end marker character

        public:
            using INDEX = uint64_t;           ///< Index type used throughout the class

            /**
             * @brief Performs LF mapping operation
             * 
             * The LF mapping is defined as LF(i) = C[c] + rank(i, c) where c = BWT[i].
             * This operation is fundamental for navigating through the BWT and is used
             * in various string processing algorithms.
             * 
             * @param i The position in the BWT
             * @return The LF-mapped position
             */
            uint64_t lf(uint64_t i) const
            {
                uint8_t c = this->bwt[i];
                uint64_t cNum = this->wt.rank(i, c);
                return C[c] + cNum;
            }

            /**
             * @brief Constructs the suffix array from the BWT
             * 
             * This function reconstructs the suffix array using the LF mapping.
             * It starts from the end marker position and iteratively applies LF
             * to traverse the BWT and build the complete suffix array.
             * 
             * @return A vector containing the suffix array
             */
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

            /**
             * @brief Gets the position of the end marker in the BWT
             * 
             * @return The position of the end marker
             */
            uint64_t get_end_marker_position() const {
                return this->end_marker_position;
            }

            /**
             * @brief Gets the size of the original text
             * 
             * @return The size of the text (equal to BWT size)
             */
            uint64_t get_text_size() const {
                return this->bwt.size();
            }

            private:

            /**
             * @brief Constructs the C array from character counter vector
             * 
             * The C array stores the cumulative count of characters smaller than each character.
             * This is used in LF mapping: C[c] gives the number of characters smaller than c.
             * 
             * @param char_counter_vector Vector containing character frequencies
             * @return The constructed C array
             */
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

            /**
             * @brief Constructs a wavelet tree (GMR variant) from the BWT
             * 
             * The wavelet tree enables efficient rank and select operations on the BWT,
             * which are essential for LF mapping operations.
             * 
             * @param bwt The Burrows-Wheeler Transform
             * @param message_paragraph Message level for progress reporting
             * @return The constructed wavelet tree
             */
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

            /**
             * @brief Constructs the BWT from the original text and suffix array
             * 
             * This function computes the BWT by extracting characters from the text
             * based on the positions given by the suffix array.
             * 
             * @tparam INDEX The index type (default: uint64_t)
             * @param text The original text
             * @param sa The suffix array
             * @return The constructed BWT as an integer vector
             */
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

            /**
             * @brief Builds an LFDataStructure from a BWT vector
             * 
             * This static factory method constructs a complete LFDataStructure from a BWT.
             * It performs all necessary preprocessing including:
             * - Converting the BWT to an integer vector
             * - Computing character frequencies and the C array
             * - Finding the end marker and its position
             * - Constructing the wavelet tree for efficient operations
             * 
             * @param bwt The Burrows-Wheeler Transform as a vector of bytes
             * @param message_paragraph Message level for progress reporting
             * @return A fully constructed LFDataStructure
             */
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