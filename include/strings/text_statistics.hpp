#pragma once
#include "../specialized_collection/forward_rle.hpp"
#include "../debug/message.hpp"

// #include "./byte.hpp"

namespace stool
{
    /**
     * @brief Class that holds statistical information about a text [Unchecked AI's Comment] 
     */
    struct TextStatistics
    {
        uint64_t run_count = UINT64_MAX; ///< Number of runs in the text
        uint64_t str_size = UINT64_MAX; ///< Size of the text
        uint64_t alphabet_size = UINT64_MAX; ///< Size of the alphabet
        std::vector<uint64_t> char_counter; ///< Character frequency counter
        std::vector<int64_t> char_min_pos_vec; ///< Minimum position of each character
        std::vector<int64_t> char_max_pos_vec; ///< Maximum position of each character

        /**
         * @brief Default constructor
         */
        TextStatistics()
        {
        }

        /**
         * @brief Get the bit size of a character
         * @return Bit size of a character
         */
        uint64_t character_bit_size()
        {
            return 8;
            // return std::log2(this->alphabet_size) + 1;
        }

        /**
         * @brief Get the smallest character in the text
         * @return Index of the smallest character in the text. Returns UINT64_MAX if text is empty
         */
        uint64_t get_smallest_character() const
        {
            for (uint64_t i = 0; i < this->char_counter.size(); i++)
            {
                if (char_counter[i] > 0)
                {
                    return i;
                }
            }
            return UINT64_MAX;
        }

        /**
         * @brief Build text statistics from a ForwardRLE object
         * @tparam TEXT_ITERATOR_BEGIN Type of the begin iterator
         * @tparam TEXT_ITERATOR_END Type of the end iterator
         * @param frle ForwardRLE object
         * @param message_paragraph Message paragraph number
         * @return Constructed TextStatistics object
         */
        template <typename TEXT_ITERATOR_BEGIN, typename TEXT_ITERATOR_END>
        static TextStatistics build(stool::ForwardRLE<TEXT_ITERATOR_BEGIN, TEXT_ITERATOR_END, uint8_t> &frle, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            TextStatistics ts;
            ts.run_count = 0;
            ts.str_size = frle.size();
            ts.alphabet_size = 0;
            ts.char_counter.resize(256, 0);
            ts.char_min_pos_vec.resize(256, INT64_MAX);
            ts.char_max_pos_vec.resize(256, INT64_MIN);

            // uint8_t prevChar = 255;
            uint64_t x = 0;
            // uint64_t count_run = 0;

            if (message_paragraph >= 0)
            {
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
                counter -= v.length;

                if (counter <= 0)
                {
                    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Computing the statistics of the given text..." << "[" << (x / 1000000) << "MB]\r" << std::flush;
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

            if (message_paragraph >= 0)
            {
                std::cout << std::endl;
                std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Computing the statistics of the given text... [END]" << std::endl;
            }

            return ts;
        }

        /**
         * @brief Build text statistics from a file
         * @param filename Input filename
         * @param message_paragraph Message paragraph number
         * @return Constructed TextStatistics object
         */
        static TextStatistics build(std::string filename, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {

            stool::OnlineFileReader ofr(filename);
            ofr.open();
            stool::ForwardRLE frle(ofr.begin(), ofr.end(), ofr.size());
            TextStatistics ts = TextStatistics::build(frle, message_paragraph);
            ofr.close();
            return ts;
        }

        /**
         * @brief Build text statistics from a vector
         * @param text Input text vector
         * @param message_paragraph Message paragraph number
         * @return Constructed TextStatistics object
         */
        static TextStatistics build(const std::vector<uint8_t> &text, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            stool::ForwardRLE frle(text.begin(), text.end(), text.size());
            return TextStatistics::build(frle, message_paragraph);
        }

        /**
         * @brief Get the alphabet of the text
         * @return Vector of characters that exist in the text
         */
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

        /**
         * @brief Print text statistics information
         * @param message_paragraph Message paragraph number
         */
        void print(int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::vector<uint8_t> alph = this->get_alphabet();

            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "======= Text Statistics ======" << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "The length of the input text: \t\t " << this->str_size << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "The number of runs on the input text: \t\t " << this->run_count << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Alphabet: \t \t  " << stool::DebugPrinter::to_integer_string_with_characters(alph) << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Alphabet size: \t\t " << this->alphabet_size << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "min sigma: \t \t  " << this->get_smallest_character() << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "The number of characters: " << std::endl;
            for (uint64_t i = 0; i < this->char_counter.size(); i++)
            {
                if (this->char_counter[i] > 0)
                {
                    std::string s = std::to_string(i);
                    s.push_back('(');
                    s.push_back(i);
                    s.push_back(')');
                    std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << s << ": " << this->char_counter[i] << std::endl;
                }
            }

            // std::cout << stool::Message::get_paragraph_string(message_paragraph) << "max sigma: \t \t  " << this->max_char << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "==============================" << std::endl;
        }
    };
} // namespace stool