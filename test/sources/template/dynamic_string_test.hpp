#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <bitset>
#include "../../../include/light_stool.hpp"

namespace stool
{
    class DynamicStringTest
    {
    public:
        template <typename TEXT>
        static void insert_character_test(TEXT &test_text, const std::vector<uint8_t> &alphabet, uint64_t number_of_insertion, uint64_t seed)
        {

            stool::NaiveDynamicString naive_text;
            test_text.clear();
            naive_text.clear();

            test_text.set_alphabet(alphabet);

            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, number_of_insertion);
            for (uint64_t i = 0; i < number_of_insertion; i++)
            {
                uint64_t c = alphabet[get_rand_uni_int(mt64) % alphabet.size()];
                uint64_t nth = get_rand_uni_int(mt64) % (naive_text.size() + 1);
                naive_text.insert(nth, c);
                test_text.insert(nth, c);

                assert((uint64_t)test_text.size() == (uint64_t)naive_text.size());
            }

            std::string test_str = test_text.to_string();
            std::string naive_str = naive_text.to_string();
            stool::EqualChecker::equal_check(test_str, naive_str);
        }

        template <typename TEXT>
        static void insert_character_test(TEXT &test_text, uint64_t number_of_insertion, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Insert Character Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {

                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << std::flush;
                uint64_t len = 2;
                std::vector<uint8_t> chars = stool::UInt8VectorGenerator::create_alphabet(type);

                while (len < number_of_insertion)
                {
                    std::cout << "+" << std::flush;
                    insert_character_test(test_text, chars, len, seed++);
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename TEXT>
        static void remove_character_test(TEXT &test_text, const std::vector<uint8_t> &alphabet, uint64_t text_size, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

            std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(text_size, alphabet, seed);

            test_text.clear();
            test_text.build_from_text(_text, alphabet);

            stool::NaiveDynamicString naive_text;
            for (char c : _text)
            {
                naive_text.push_back(c);
            }

            while (naive_text.size() > 0)
            {
                uint64_t nth = get_rand_uni_int(mt64) % (naive_text.size());
                naive_text.remove(nth);
                test_text.remove(nth);

                if (naive_text.size() % 100 == 0)
                {
                    std::string test_str = test_text.to_string();
                    std::string naive_str = naive_text.to_string();
                    stool::EqualChecker::equal_check(test_str, naive_str);
                }
            }

            if (test_text.size() != 0)
            {
                std::cout << "Error: test_text.size() != 0" << std::endl;
            }
        }

        template <typename TEXT>
        static void remove_character_test(TEXT &test_text, uint64_t max_text_size, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Remove Character Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << std::flush;
                uint64_t len = 2;
                std::vector<uint8_t> chars = stool::UInt8VectorGenerator::create_alphabet(type);

                while (len < max_text_size)
                {
                    std::cout << "+" << std::flush;
                    remove_character_test(test_text, chars, len, seed++);
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        
    };

} // namespace name
