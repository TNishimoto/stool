#pragma once
#include "./online_file_reader.hpp"
#include "./specialized_collection/forward_rle.hpp"

#include "./byte.hpp"
#include "./message.hpp"

namespace stool
{
    struct TextStatistics
    {
        uint64_t run_count = UINT64_MAX;
        uint64_t str_size = UINT64_MAX;
        uint64_t alphabet_size = UINT64_MAX;
        std::vector<uint64_t> char_counter;
        std::vector<int64_t> char_min_pos_vec;
        std::vector<int64_t> char_max_pos_vec;


        TextStatistics()
        {
        }
        uint64_t character_bit_size()
        {
            return 8;
            // return std::log2(this->alphabet_size) + 1;
        }
        uint64_t get_smallest_character() const {
            for (uint64_t i = 0; i < this->char_counter.size(); i++)
            {
                if (char_counter[i] > 0)
                {
                    return i;
                }
            }
            return UINT64_MAX;
        }

        template <typename TEXT_ITERATOR_BEGIN, typename TEXT_ITERATOR_END>
        static TextStatistics build(stool::ForwardRLE<TEXT_ITERATOR_BEGIN, TEXT_ITERATOR_END, uint8_t> &frle, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            TextStatistics ts;
            ts.run_count = 0;
            ts.str_size = 0;
            ts.alphabet_size = 0;
            ts.char_counter.resize(256, 0);
            ts.char_min_pos_vec.resize(256, INT64_MAX);
            ts.char_max_pos_vec.resize(256, INT64_MIN);


            // uint8_t prevChar = 255;
            uint64_t x = 0;
            // uint64_t count_run = 0;

            if(message_paragraph >= 0){
                std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Computing the statistics of the given text... \r" << std::flush;
            }

            int64_t counter_max = 50000000;
            int64_t counter = counter_max;

            for (stool::CharacterRun<uint8_t, uint64_t> v : frle)
            {
                uint8_t c = v.character;
                ts.char_counter[c] += v.length;
                ts.run_count++;

                ts.char_min_pos_vec[c] = std::min(ts.char_min_pos_vec[c], (int64_t)x);
                ts.char_max_pos_vec[c] = std::max(ts.char_max_pos_vec[c], (int64_t)x);

                x += v.length;
                counter-= v.length;    

                if(counter <= 0){
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Computing the statistics of the given text..." << "[" << (x/1000000)  << "MB]\r"  << std::flush;
                    counter = counter_max;
                }

            }

            for (uint64_t i = 0; i < ts.char_counter.size(); i++)
            {
                if (ts.char_counter[i] > 0)
                {
                    ts.alphabet_size++;
                }
            }

            if(message_paragraph >= 0){
                std::cout << std::endl;
                std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Computing the statistics of the given text... [END]" << std::endl;

            }

            return ts;
        }

        static TextStatistics build(std::string filename, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {

            stool::OnlineFileReader ofr(filename);
            ofr.open();
            stool::ForwardRLE frle(ofr.begin(), ofr.end(), ofr.size());
            TextStatistics ts = TextStatistics::build(frle, message_paragraph);
            ofr.close();
            return ts;
        }
        static TextStatistics build(const std::vector<uint8_t> &text, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            stool::ForwardRLE frle(text.begin(), text.end(), text.size());
            return TextStatistics::build(frle, message_paragraph);
        }

        std::vector<uint8_t> get_alphabet() const
        {
            std::vector<uint8_t> r;
            for (uint64_t i = 0; i < this->char_counter.size(); i++)
            {
                if (char_counter[i] > 0)
                {
                    r.push_back(i);
                }
            }
            return r;
        }
        void print(int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "======= Text Statistics ======" << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "The length of the input text: \t\t " << this->str_size << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "The number of runs on BWT: \t\t " << this->run_count << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Alphabet size: \t\t " << this->alphabet_size << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "min sigma: \t \t  " << this->get_smallest_character() << std::endl;
            //std::cout << stool::Message::get_paragraph_string(message_paragraph) << "max sigma: \t \t  " << this->max_char << std::endl;
            std::vector<uint8_t> alph = this->get_alphabet();
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Alphabet: \t \t  " << stool::DebugPrinter::to_integer_string_with_characters(alph) << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "==============================" << std::endl;
        }
    };
} // namespace stool