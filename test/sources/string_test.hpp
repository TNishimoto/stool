#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <bitset>
#include "../../include/light_stool.hpp"

namespace stool
{
    class StringTest
    {
    public:
        template <typename TEXT>
        static void access_character_test(TEXT &test_text, const std::vector<uint8_t> &naive_text, uint64_t number_of_access, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

            for (uint64_t i = 0; i < number_of_access; i++)
            {
                uint64_t nth = get_rand_uni_int(mt64) % (naive_text.size());
                uint8_t c = naive_text[nth];
                uint8_t c2 = test_text[nth];

                if (c != c2)
                {
                    std::cout << "Error: c != c2" << std::endl;
                    throw std::runtime_error("Error: c != c2");
                }
            }
        }

        template <typename TEXT>
        static void access_character_test(uint64_t max_text_size, uint64_t number_of_access, uint64_t number_of_trials, 
            std::function<void(TEXT &, const std::vector<uint8_t> &, const std::vector<uint8_t> &)> builder_function, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Access Character Test: " << std::endl;
            TEXT test_text;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {

                uint64_t len = 2;
                std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;

                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;
                    for(uint64_t i = 0; i < number_of_trials; i++){
                        std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        stool::NaiveDynamicString naive_text;
                        for (char c : _text)
                        {
                            naive_text.push_back(c);
                        }
                        TEXT test_text;
                        builder_function(test_text, _text, alphabet);

                        access_character_test(test_text, _text, number_of_access, seed++);

                    }
    
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename TEXT>
        static void locate_query_test(TEXT &test_text, const std::vector<uint8_t> &original_text, uint64_t number_of_queries, uint64_t max_query_length, uint64_t seed, [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::mt19937_64 mt64(seed);
            std::vector<uint64_t> sa = stool::StringFunctions::construct_naive_suffix_array(original_text);

            for (uint64_t i = 0; i < number_of_queries; i++)
            {
                uint64_t pos = mt64() % original_text.size();
                uint64_t pattern_length = mt64() % (original_text.size() - pos) + 1;

                if (pattern_length > max_query_length)
                {
                    pattern_length = max_query_length;
                }

                std::vector<uint8_t> pattern = std::vector<uint8_t>(original_text.begin() + pos, original_text.begin() + pos + pattern_length);

                std::vector<uint64_t> occurrences = stool::StringFunctionsOnSA::locate_query(original_text, pattern, sa);
                std::vector<uint64_t> test_occurrences = test_text.locate_query(pattern);

                std::sort(occurrences.begin(), occurrences.end());
                std::sort(test_occurrences.begin(), test_occurrences.end());
                try
                {
                    stool::EqualChecker::equal_check(occurrences, test_occurrences, "locate_query_test");
                }
                catch (const std::exception &e)
                {
                    std::cout << std::endl;
                    std::string text_str = std::string(original_text.begin(), original_text.end());
                    std::string pattern_str = std::string(pattern.begin(), pattern.end());

                    std::cout << "Text: " << text_str << std::endl;
                    std::cout << "Pattern: " << pattern_str << std::endl;
                    std::cout << std::endl;

                    stool::DebugPrinter::print_integers(occurrences, "occurrences");
                    stool::DebugPrinter::print_integers(test_occurrences, "test_occurrences");

                    test_text.print_info();
                    throw std::runtime_error("Error: locate_query_test failed");
                }
            }
        };

        template <typename TEST_TEXT>
        static void locate_query_test(uint64_t max_len, uint64_t alphabet_type, [[maybe_unused]] uint64_t number_of_trials, uint64_t number_of_pattern_trials, 
            std::function<void(TEST_TEXT&, const std::vector<uint8_t>&, const std::vector<uint8_t>&)> builder_function, uint64_t seed)
        {
            std::cout << "LOCATE_QUERY_TEST \t" << std::endl;
            TEST_TEXT test_index;

            for (uint64_t len = 4; len <= max_len; len *= 2)
            {
                std::cout << stool::Message::get_paragraph_string(2) << "len = " << len << ": " << std::flush;
                auto alphabet = stool::UInt8VectorGenerator::create_alphabet(alphabet_type);

                for(uint64_t i = 0; i < number_of_trials; i++){
                    std::cout << "+" << std::flush;
                    std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                    builder_function(test_index, text, alphabet);
    
                    uint64_t max_query_length = len / 2;
    
                    locate_query_test(test_index, text, number_of_pattern_trials, max_query_length, seed);
    
                }
                std::cout << std::endl;

            }
            std::cout << "[DONE]" << std::endl;
        }
    };
}