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
        static void access_character_test(uint64_t max_text_size, uint64_t number_of_access, uint64_t number_of_trials, bool use_end_marker,uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Access Character Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {

                uint64_t len = 2;
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;

                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;
                    for (uint64_t i = 0; i < number_of_trials; i++)
                    {
                        std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);
                        std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        if (use_end_marker)
                        {
                            text.push_back(0);
                            alphabet.push_back(0);
                            std::sort(alphabet.begin(), alphabet.end());
                        }

                        TEXT test_text = TEXT::build_from_text(text, alphabet);
                        access_character_test(test_text, text, number_of_access, seed++);
                    }

                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename TEXT>
        static void locate_query_test(TEXT &test_text, const std::vector<uint8_t> &original_text, const std::vector<uint64_t> &sa, std::vector<uint8_t> &pattern)
        {
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

                if (text_str.size() < 1000)
                {
                    std::cout << "Text: " << text_str << std::endl;
                }
                else
                {
                    std::cout << "Text: " << "Omitted" << std::endl;
                }

                std::cout << "Pattern: " << pattern_str << "(" << pattern.size() << ")" << std::endl;
                std::cout << std::endl;

                stool::DebugPrinter::print_integers(occurrences, "occurrences");
                stool::DebugPrinter::print_integers(test_occurrences, "test_occurrences");

                // test_text.print_info();
                throw std::runtime_error("Error: locate_query_test failed");
            }
        };

        template <typename TEXT>
        static void locate_query_test(TEXT &test_text, const std::vector<uint8_t> &original_text, const std::vector<uint64_t> &sa, uint64_t number_of_queries, uint64_t max_query_length, uint64_t seed, [[maybe_unused]] int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            if(original_text.size() == 0){
                throw std::runtime_error("Error: original_text.size() == 0");

            }
            std::mt19937_64 mt64(seed);
            for (uint64_t i = 0; i < number_of_queries; i++)
            {
                uint64_t pos = mt64() % original_text.size();
                uint64_t pattern_length = (mt64() % (original_text.size() - pos)) + 1;

                if (pattern_length > max_query_length)
                {
                    pattern_length = max_query_length;
                }


                assert(pos + pattern_length <= original_text.size());

                std::vector<uint8_t> pattern = std::vector<uint8_t>(original_text.begin() + pos, original_text.begin() + pos + pattern_length);
                locate_query_test(test_text, original_text, sa, pattern);
            }
        };

        template <typename TEXT>
        static void locate_query_test(uint64_t max_len, uint64_t alphabet_type, uint64_t number_of_trials, uint64_t number_of_pattern_trials,
                                      std::function<std::vector<uint64_t>(const std::vector<uint8_t> &)> sa_builder_function, bool use_end_marker, uint64_t seed)
        {
            std::cout << "LOCATE_QUERY_TEST \t" << std::endl;

            for (uint64_t len = 4; len <= max_len; len *= 2)
            {
                std::cout << stool::Message::get_paragraph_string(2) << "len = " << len << ": " << std::flush;
                auto alphabet = stool::UInt8VectorGenerator::create_alphabet(alphabet_type);

                for (uint64_t i = 0; i < number_of_trials; i++)
                {
                    std::cout << "+" << std::flush;

                    std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(alphabet_type);
                    std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                    if (use_end_marker)
                    {
                        text.push_back(0);
                        alphabet.push_back(0);
                        std::sort(alphabet.begin(), alphabet.end());
                    }

                    TEXT test_index = TEXT::build_from_text(text, alphabet);

                    uint64_t max_query_length = len / 2;

                    std::vector<uint64_t> sa = sa_builder_function(text);

                    locate_query_test(test_index, text, sa, number_of_pattern_trials, max_query_length, seed);
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename TEST_TEXT>
        static void locate_query_test_using_text_file(std::string text_file_path, uint64_t number_of_pattern_trials, uint64_t max_query_length,
                                                      std::function<std::vector<uint64_t>(const std::vector<uint8_t> &)> sa_builder_function, uint64_t seed)
        {
            std::cout << "LOCATE_QUERY_TEST \t" << std::endl;
            std::mt19937_64 mt64(seed);

            std::cout << "Loading text file: " << text_file_path << " " << std::flush;
            std::vector<uint8_t> text;
            stool::IO::load_text(text_file_path, text);
            std::cout << "[DONE]" << std::endl;

            auto alphabet = stool::StringFunctions::get_alphabet(text);

            std::cout << "Building test index..." << std::flush;
            TEST_TEXT test_index = TEST_TEXT::build_from_text(text, alphabet, stool::Message::SHOW_MESSAGE);
            std::cout << "[DONE]" << std::endl;

            std::cout << "Building suffix array..." << std::flush;
            std::vector<uint64_t> sa = sa_builder_function(text);
            std::cout << "[DONE]" << std::endl;

            for (uint64_t len = 4; len <= max_query_length; len *= 10)
            {
                std::cout << stool::Message::get_paragraph_string(2) << "len = " << len << ": " << std::flush;

                if (len < text.size())
                {
                    for (uint64_t i = 0; i < number_of_pattern_trials; i++)
                    {
                        std::cout << "+" << std::flush;

                        uint64_t pos = mt64() % (text.size() - len);

                        std::vector<uint8_t> pattern = std::vector<uint8_t>(text.begin() + pos, text.begin() + pos + len);
                        locate_query_test(test_index, text, sa, pattern);
                    }
                }

                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename TEXT>
        static void save_and_load_test(TEXT &test_text, std::function<bool(TEXT &, TEXT &)> equal_check_function, std::string filepath, [[maybe_unused]] int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            {
                std::ofstream os;
                os.open(filepath, std::ios::binary);
                if (!os)
                {
                    std::cerr << "Error: Could not open file for writing." << std::endl;
                    throw std::runtime_error("File open error");
                }
                TEXT::save(test_text, os, stool::Message::NO_MESSAGE);    
    
            }


            TEXT test_text2;
            {
                std::ifstream ifs;
                ifs.open(filepath, std::ios::binary);
                if (!ifs)
                {
                    std::cerr << "Error: Could not open file for reading." << std::endl;
                    throw std::runtime_error("File open error");
                }
    
                TEXT tmp = TEXT::load_from_file(ifs);
                test_text2.swap(tmp);

            }
            bool b = equal_check_function(test_text, test_text2);
            if (!b)
            {
                throw std::runtime_error("Error: save_and_load_test failed");
            }
        }

        template <typename TEXT>
        static void save_and_load_test(uint64_t max_text_size, uint64_t number_of_trials,
                                       std::function<bool(TEXT &, TEXT &)> equal_check_function, bool use_end_marker , std::string filepath, uint64_t seed, [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Save and Load Test: " << std::endl;

            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {

                uint64_t len = 2;
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;

                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;
                    for (uint64_t i = 0; i < number_of_trials; i++)
                    {
                        std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);
                        std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        if (use_end_marker)
                        {
                            text.push_back(0);
                            alphabet.push_back(0);
                            std::sort(alphabet.begin(), alphabet.end());
                        }

                        TEXT test_text = TEXT::build_from_text(text, alphabet);
                        save_and_load_test(test_text, equal_check_function, filepath, message_paragraph);
                    }

                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }
    };
}