#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <bitset>
#include "../../../include/light_stool.hpp"
#include "../string_test.hpp"

namespace stool
{
    class DynamicStringTest
    {
    public:
        template <typename TEXT>
        static void insert_character_test(TEXT &test_text, stool::NaiveDynamicString &naive_text, const std::vector<uint8_t> &alphabet, uint64_t number_of_insertion, [[maybe_unused]] bool detailed_check, uint64_t seed)
        {
            /*
            stool::NaiveDynamicString naive_text;
            test_text.clear();
            naive_text.clear();
            */

            // test_text.set_alphabet(alphabet);

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
        static void insert_character_test(uint64_t max_text_size, uint64_t number_of_insertion, uint64_t number_of_trials,
                                          std::function<void(TEXT &, const std::vector<uint8_t> &, const std::vector<uint8_t> &)> builder_function, bool detailed_check, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Insert Character Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;
                uint64_t len = 2;
                std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);


                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;

                    for (uint64_t i = 0; i < number_of_trials; i++)
                    {
                        std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        stool::NaiveDynamicString naive_text;
                        for (char c : _text)
                        {
                            naive_text.push_back(c);
                        }
                        TEXT test_text;
                        builder_function(test_text, _text, alphabet);

                        insert_character_test(test_text, naive_text, alphabet, number_of_insertion, detailed_check, seed++);
                    }
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename TEXT>
        static void remove_character_test(TEXT &test_text, stool::NaiveDynamicString &naive_text, uint64_t number_of_removals, bool detailed_check, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

            for(uint64_t i = 0; i < number_of_removals; i++){
                uint64_t nth = get_rand_uni_int(mt64) % (naive_text.size());
                naive_text.remove(nth);
                test_text.remove(nth);

                assert((uint64_t)test_text.size() == (uint64_t)naive_text.size());
                if(naive_text.size() == 0){
                    break;
                }
                if(detailed_check){
                    std::string test_str = test_text.to_string();
                    std::string naive_str = naive_text.to_string();
                    stool::EqualChecker::equal_check(test_str, naive_str);        
                }
            }


            if ((int64_t)test_text.size() != (int64_t)naive_text.size())
            {
                throw std::runtime_error("Error: test_text.size() != naive_text.size()");
            }

            std::string test_str = test_text.to_string();
            std::string naive_str = naive_text.to_string();
            stool::EqualChecker::equal_check(test_str, naive_str);
        }

        template <typename TEXT>
        static void remove_character_test(uint64_t max_text_size, uint64_t number_of_removals, uint64_t number_of_trials, std::function<void(TEXT &, const std::vector<uint8_t> &, const std::vector<uint8_t> &)> builder_function, bool detailed_check, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Remove Character Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;
                uint64_t len = 2;
                std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);


                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;

                    for (uint64_t i = 0; i < number_of_trials; i++)
                    {
                        std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        stool::NaiveDynamicString naive_text;
                        for (char c : _text)
                        {
                            naive_text.push_back(c);
                        }
                        TEXT test_text;
                        builder_function(test_text, _text, alphabet);

                        remove_character_test(test_text, naive_text, number_of_removals, detailed_check, seed++);
                    }
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename TEXT>
        static void random_test(uint64_t text_size, uint64_t alphabet_type, uint64_t number_of_queries, uint64_t number_of_trials, 
            std::function<void(TEXT &, const std::vector<uint8_t> &, const std::vector<uint8_t> &)> builder_function, bool detailed_check, uint64_t seed, [[maybe_unused]] int message_paragraph = stool::Message::SHOW_MESSAGE)
        {

            std::cout << "RANDOM_TEST: \t" << std::endl;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

            for (uint64_t i = 0; i < number_of_trials; i++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph+1) << i << ": " << std::flush;
                uint64_t counter = 0;
                std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(alphabet_type);
                std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(text_size, alphabet, seed++);
                stool::NaiveDynamicString naive_text;
                for (char c : _text)
                {
                    naive_text.push_back(c);
                }
                TEXT test_text;
                builder_function(test_text, _text, alphabet);

                while (counter < number_of_queries)
                {
                    std::cout << "+" << std::flush;

                    uint64_t type = get_rand_value(mt) % 6;
                    if (type == 0 || type == 1)
                    {
                        insert_character_test(test_text, naive_text, alphabet, 5, detailed_check, seed++);
                    }
                    else if (type == 2 || type == 3)
                    {
                        remove_character_test(test_text, naive_text, 1, detailed_check, seed++);
                    }
                    else if (type == 4)
                    {
                        stool::StringTest::access_character_test(test_text, naive_text.text, 5, seed++);
                    }
                    else
                    {
                        stool::StringTest::locate_query_test(test_text, naive_text.text, 5, 100, seed++);
                    }
                    counter++;

                    if (detailed_check)
                    {
                        std::string test_str = test_text.to_string();
                        std::string naive_str = naive_text.to_string();
                        stool::EqualChecker::equal_check(test_str, naive_str);
                    }
                }

                std::string test_str = test_text.to_string();
                std::string naive_str = naive_text.to_string();
                stool::EqualChecker::equal_check(test_str, naive_str);

                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

    };

} // namespace name
