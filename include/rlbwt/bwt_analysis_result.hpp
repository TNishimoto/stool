#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <stdexcept>
#include <iostream>
#include <fstream>
// #include "common/io.h"
// #include "common/print.hpp"
// #include "other_functions.hpp"
// #include "OnlineRlbwt/online_rlbwt.hpp"
// #include "rlbwt.hpp"
// #include "stool/src/elias_fano_vector.hpp"
// #include "../io.hpp"
#include "../online_file_reader.hpp"

// #include "../elias_fano_vector.hpp"
#include "../byte.hpp"

namespace stool
{
    namespace rlbwt2
    {
        struct BWTAnalysisResult
        {
            uint64_t run_count;
            uint64_t str_size;

            uint64_t min_char = UINT64_MAX;
            uint64_t min_char_pos;
            uint64_t min_char_count;

            int64_t max_char = -1;
            uint64_t max_char_pos = 0;
            uint64_t max_char_count = 0;

            uint64_t alphabet_count = 0;
            std::vector<uint64_t> char_coutner;

            // std::vector<uint64_t> character_count_vec;
            /*
            std::vector<uint64_t> character_to_id_vec;
            std::vector<uint64_t> id_to_character_vec;
            */

            BWTAnalysisResult()
            {
                uint64_t size = UINT8_MAX + 1;

                this->char_coutner.resize(size, 0);
                /*
                this->character_to_id_vec.resize(size, 0);
                */
            }
            uint64_t character_bit_size()
            {
                return 8;
                // return std::log2(this->alphabet_count) + 1;
            }

            template <typename TEXT_ITERATOR_BEGIN, typename TEXT_ITERATOR_END>
            void analyze(stool::ForwardRLE<TEXT_ITERATOR_BEGIN, TEXT_ITERATOR_END, uint8_t> &frle)
            {
                this->char_coutner.clear();
                this->char_coutner.resize(256, 0);
                this->run_count = 0;
                this->str_size = frle.size();
                this->alphabet_count = 0;

                uint8_t prevChar = 255;
                uint64_t x = 0;
                uint64_t count_run = 0;

                for (stool::CharacterRun<uint8_t, uint64_t> v : frle)
                {
                    uint8_t c = v.character;
                    char_coutner[c] += v.length;
                    this->run_count++;

                    if (c < this->min_char)
                    {
                        this->min_char = c;
                        this->min_char_pos = x;
                        this->min_char_count = v.length;
                    }
                    else if (c == this->min_char)
                    {
                        this->min_char_count += v.length;
                    }

                    if (c > this->max_char)
                    {
                        this->max_char = c;
                        this->max_char_pos = x;
                        this->max_char_count = v.length;
                    }
                    else if (c == this->max_char)
                    {
                        this->max_char_count = v.length;
                    }
                    x += v.length;
                }

                for (uint64_t i = 0; i < this->char_coutner.size(); i++)
                {
                    if (this->char_coutner[i] > 0)
                    {
                        this->alphabet_count++;
                    }
                }
            }


            void analyze(std::string filename)
            {
                stool::OnlineFileReader ofr(filename);
                ofr.open();
                stool::ForwardRLE frle(ofr.begin(), ofr.end(), ofr.size());
                this->analyze(frle);
                ofr.close();
            }
            void analyze(const std::vector<uint8_t> &text)
            {
                stool::ForwardRLE frle(text.begin(), text.end(), text.size());
                this->analyze(frle);

            }

            std::vector<uint8_t> get_alphabet() const
            {
                std::vector<uint8_t> r;
                for (uint64_t i = 0; i < this->char_coutner.size(); i++)
                {
                    if (char_coutner[i] > 0)
                    {
                        r.push_back(i);
                    }
                }
                return r;
            }
            void print()
            {
                std::cout << "\033[31m";
                std::cout << "______________________RESULT______________________" << std::endl;
                std::cout << "The length of the input text: \t\t " << this->str_size << std::endl;
                std::cout << "The number of runs on BWT: \t\t " << this->run_count << std::endl;
                std::cout << "Alphabet size: \t\t " << this->alphabet_count << std::endl;
                std::cout << "min sigma: \t \t  " << this->min_char << std::endl;
                std::cout << "max sigma: \t \t  " << this->max_char << std::endl;

                std::vector<uint8_t> alph = this->get_alphabet();

                stool::Printer::print_chars("Alphabet", alph);

                // std::cout << "log sigma \t\t : " << this->character_bit_size() << std::endl;
                // uint64_t x = run_count * (stool::Log::log2(str_size / run_count));

                // std::cout << "r log (n/r) = " << x << std::endl;
                /*
                for (uint64_t i = 0; i < this->id_to_character_vec.size(); i++)
                {
                    if(this->id_to_character_vec[i] != UINT64_MAX){
                        std::cout << "[" << (char)id_to_character_vec[i] << "->" << i << "] ";
                    }
                }
                std::cout << std::endl;
                */

                std::cout << "_______________________________________________________" << std::endl;
                std::cout << "\033[39m" << std::endl;
            }
        };
        /*
        static BWTAnalysisResult load_RLBWT_from_file(std::string filename, sdsl::int_vector<> &diff_char_vec, EliasFanoVectorBuilder &run_bits)
        {

            diff_char_vec.width(8);

            std::ifstream inp;
            std::vector<char> buffer;
            uint64_t bufferSize = 8192;
            buffer.resize(8192);

            BWTAnalysisResult analysisResult;
            analysisResult.analyze(filename);

            //diff_char_vec.width(analysisResult.character_bit_size());

            diff_char_vec.resize(analysisResult.run_count);
            run_bits.initialize(analysisResult.str_size + 1, analysisResult.run_count + 1);

            inp.open(filename, std::ios::binary);
            bool inputFileExist = inp.is_open();
            if (!inputFileExist)
            {
                std::cout << filename << " cannot open." << std::endl;

                throw std::runtime_error("error");
            }
            uint64_t textSize = stool::FileReader::getTextSize(inp);
            uint8_t prevChar = 255;
            uint64_t x = 0;
            uint64_t currentRunP = 0;

            while (true)
            {
                bool b = stool::FileReader::read(inp, buffer, bufferSize, textSize);
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
                        diff_char_vec[currentRunP++] = c;
                        prevChar = c;
                    }
                    else
                    {
                        run_bits.push_bit(false);

                        //run_bits.push_back(0);
                    }
                    x++;
                }
            }

            run_bits.push_bit(true);
            run_bits.finish();

            inp.close();
            analysisResult.print();
            return analysisResult;
        }
        */
        /*
        static std::vector<uint64_t> construct_lpos_array(std::vector<bool> &run_bits)
        {
            std::vector<uint64_t> r;
            uint64_t prev_i = 0;
            for (uint64_t i = 0; i < run_bits.size(); i++)
            {
                if (run_bits[i])
                {
                    if (r.size() == 0)
                    {
                        r.push_back(i);
                    }
                    else
                    {
                        uint64_t diff = i - prev_i - 1;
                        uint64_t x = r[r.size() - 1] + diff;
                        r.push_back(x);
                    }
                    prev_i = i;
                }
            }
            return r;
        }
        */
    } // namespace rlbwt2
} // namespace stool