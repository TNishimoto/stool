#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <bitset>
#include "../../include/light_stool.hpp"

namespace stool{
    class StringTest{
        public:
        template <typename TEXT>
        static void access_character_test(TEXT &test_text, const std::vector<uint8_t> &alphabet, uint64_t text_size, uint64_t number_of_access, uint64_t seed){
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

            std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(text_size, alphabet, seed);

            test_text.clear();
            test_text.set_alphabet(alphabet);

            stool::NaiveDynamicString naive_text;
            for(char c: _text){
                test_text.push_back(c);
                naive_text.push_back(c);
            }

            for(uint64_t i = 0; i < number_of_access;i++){
                uint64_t nth = get_rand_uni_int(mt64) % (naive_text.size());
                uint8_t c = naive_text.text[nth];
                uint8_t c2 = test_text[nth];

                if(c != c2){
                    std::cout << "Error: c != c2" << std::endl;
                    throw std::runtime_error("Error: c != c2");
                }
            }
        }

        template <typename TEXT>
        static void access_character_test1(TEXT &test_text, uint64_t max_text_size, uint64_t number_of_access, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Access Character Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << std::flush;
                uint64_t len = 2;
                std::vector<uint8_t> chars = stool::UInt8VectorGenerator::create_alphabet(type);

                while (len < max_text_size)
                {
                    std::cout << "+" << std::flush;
                    access_character_test(test_text, chars, len, number_of_access, seed++);
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }
    };
}