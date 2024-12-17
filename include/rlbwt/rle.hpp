#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "../text_statistics.hpp"
#include "../online_file_reader.hpp"
#include "../specialized_collection/elias_fano_vector.hpp"
#include "../specialized_collection/forward_rle.hpp"

#include "../string_functions.hpp"

namespace stool
{
    namespace rlbwt2
    {

        template <typename CHAR = uint8_t>
        class RLE
        {
        public:
            using char_type = CHAR;
            using INDEX = uint64_t;
            using LPOS_TYPE = stool::EliasFanoVector;

        private:
            sdsl::int_vector<8> head_char_vec;
            LPOS_TYPE lpos_vec;
            INDEX smallest_character = 0;

        public:
            RLE()
            {
            }

            const sdsl::int_vector<8> *get_head_char_vec() const
            {
                return &this->head_char_vec;
            }
            const LPOS_TYPE *get_lpos_vec() const
            {
                return &this->lpos_vec;
            }

            void initialize(sdsl::int_vector<8> &_head_char_vec, LPOS_TYPE &_lpos_vec, INDEX _smallest_character)
            {
                this->head_char_vec.swap(_head_char_vec);
                this->lpos_vec.swap(_lpos_vec);
                this->smallest_character = _smallest_character;
            }

            CHAR get_char_by_run_index(INDEX _run_index) const
            {
                return (this->head_char_vec)[_run_index];
            }

            CHAR get_smallest_character() const
            {
                return this->smallest_character;
            }

            uint64_t get_lindex_containing_the_position(uint64_t lposition) const
            {
                uint64_t x = this->lpos_vec.rank(lposition + 1) - 1;
                return x;
            }
            INDEX get_run(INDEX i) const
            {
                return (lpos_vec)[(i + 1)] - (lpos_vec)[i];
            }
            INDEX get_lpos(INDEX i) const
            {
                return (lpos_vec)[i];
            }
            INDEX rle_size() const
            {
                return (head_char_vec).size();
            }

            INDEX str_size() const
            {
                return (lpos_vec)[(lpos_vec).size() - 1];
            }
            INDEX get_end_rle_lposition() const
            {
                for (INDEX i = 0; i < head_char_vec.size(); i++)
                {
                    if ((head_char_vec)[i] == this->smallest_character)
                    {
                        return i;
                    }
                }
                return std::numeric_limits<INDEX>::max();
            }
            INDEX get_end_marker_position() const
            {
                uint64_t x = this->get_end_rle_lposition();
                return this->get_lpos(x);
            }

            uint64_t get_using_memory() const
            {
                return sdsl::size_in_bytes(this->head_char_vec) + this->lpos_vec.get_using_memory();
            }

            static RLE<uint8_t> build_from_BWT(const std::vector<uint8_t> &bwt, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {
                TextStatistics ar = TextStatistics::build(bwt, message_paragraph);
                stool::ForwardRLE frle(bwt.begin(), bwt.end(), bwt.size());
                return RLE::build(frle, ar.run_count, ar.get_smallest_character(), message_paragraph);

            }

            template <typename TEXT_ITERATOR_BEGIN, typename TEXT_ITERATOR_END>
            static RLE<uint8_t> build(ForwardRLE<TEXT_ITERATOR_BEGIN, TEXT_ITERATOR_END, uint8_t> &frle, uint64_t run_count, uint8_t smallest_character, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {
                if (message_paragraph >= 0 && frle.size() > 0)
                {
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing RLE..." << std::endl;
                }
                std::chrono::system_clock::time_point st1, st2;
                st1 = std::chrono::system_clock::now();


                sdsl::int_vector<8> head_char_vec;
                stool::EliasFanoVector lpos_vec;

                // head_char_vec.width(8);
                stool::EliasFanoVectorBuilder run_bits;
                head_char_vec.resize(run_count);
                run_bits.initialize(frle.size() + 1, run_count + 1);
                uint64_t currentRunP = 0;

                for (CharacterRun<uint8_t, uint64_t> v : frle)
                {
                    run_bits.push_bit(true);
                    for (uint64_t i = 1; i <= v.length; i++)
                    {
                        run_bits.push_bit(false);
                    }
                    head_char_vec[currentRunP++] = v.character;
                }
                run_bits.push_bit(true);
                run_bits.finish();
                lpos_vec.build_from_builder(run_bits);

                RLE<uint8_t> rle;
                rle.initialize(head_char_vec, lpos_vec, smallest_character);

                st2 = std::chrono::system_clock::now();
               if (message_paragraph >= 0 && frle.size() > 0)
                {
                    uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
                    uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
                    uint64_t per_time = ((double)ms_time / (double)frle.size()) * 1000000;

                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
                }

                return rle;
            }

            static RLE<uint8_t> build_from_file(std::string filename, int message_paragraph = stool::Message::SHOW_MESSAGE)
            {
                TextStatistics ar = TextStatistics::build(filename, message_paragraph);

                stool::OnlineFileReader ofr(filename);
                ofr.open();
                stool::ForwardRLE frle(ofr.begin(), ofr.end(), ofr.size());
                RLE<uint8_t> r = RLE::build(frle, ar.run_count, ar.get_smallest_character(), message_paragraph);
                ofr.close();
                return r;
            }
        };

    } // namespace stnode_on_rlbwt
} // namespace stool