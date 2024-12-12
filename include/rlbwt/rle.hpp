#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "./bwt_analysis_result.hpp"
#include "../online_file_reader.hpp"
#include "../specialized_collection/elias_fano_vector.hpp"
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

            static RLE<uint8_t> build_from_BWT(std::vector<uint8_t> &bwt, stool::rlbwt2::BWTAnalysisResult &analysisResult)
            {
                throw -1;
            }


            static RLE<uint8_t> build(std::string filename, stool::rlbwt2::BWTAnalysisResult &analysisResult)
            {
                sdsl::int_vector<8> head_char_vec;
                stool::EliasFanoVector lpos_vec;
                uint64_t smallest_character = 0;

                //head_char_vec.width(8);
                stool::EliasFanoVectorBuilder run_bits;
                std::ifstream inp;
                std::vector<uint8_t> buffer;
                uint64_t bufferSize = 8192;
                buffer.resize(8192);

                analysisResult.analyze(filename);
                smallest_character = analysisResult.min_char;

                head_char_vec.resize(analysisResult.run_count);
                run_bits.initialize(analysisResult.str_size + 1, analysisResult.run_count + 1);

                inp.open(filename, std::ios::binary);
                bool inputFileExist = inp.is_open();
                if (!inputFileExist)
                {
                    std::cout << filename << " cannot open." << std::endl;

                    throw std::runtime_error("error");
                }
                uint64_t textSize = stool::OnlineFileReader::get_text_size(inp);
                uint8_t prevChar = 255;
                uint64_t x = 0;
                uint64_t currentRunP = 0;

                while (true)
                {
                    bool b = stool::OnlineFileReader::read(inp, buffer, bufferSize, textSize);
                    if (!b)
                    {
                        break;
                    }

                    for (uint64_t i = 0; i < buffer.size(); i++)
                    {
                        uint8_t c = (uint8_t)buffer[i];
                        if (prevChar != c || x == 0)
                        {
                            run_bits.push_bit(true);
                            run_bits.push_bit(false);
                            head_char_vec[currentRunP++] = c;
                            prevChar = c;
                        }
                        else
                        {
                            run_bits.push_bit(false);

                            // run_bits.push_back(0);
                        }
                        x++;
                    }
                }

                run_bits.push_bit(true);
                run_bits.finish();

                std::cout << "BWT using memory = " << sdsl::size_in_bytes(head_char_vec) / 1000 << "[KB]" << std::endl;
                std::cout << "Run bits using memory = " << run_bits.get_using_memory() / 1000 << "[KB]" << std::endl;

                inp.close();

                analysisResult.print();
                lpos_vec.build_from_builder(run_bits);

                RLE<uint8_t> rle;
                rle.initialize(head_char_vec, lpos_vec, smallest_character);
                return rle;
            }

        };

    } // namespace stnode_on_rlbwt
} // namespace stool