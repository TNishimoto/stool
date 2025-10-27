#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <bitset>
#include "../../../include/light_stool.hpp"
#include "./string_test.hpp"

namespace stool
{
    class DynamicStringTest
    {
    public:
        template <typename DYNAMIC_TEXT, typename NAIVE_TEXT = stool::NaiveDynamicString>
        static void insert_character_test(DYNAMIC_TEXT &test_text, NAIVE_TEXT &naive_text, const std::vector<uint8_t> &alphabet, uint64_t number_of_insertion, bool use_end_marker, [[maybe_unused]] bool detailed_check, uint64_t seed)
        {

            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, number_of_insertion);
            for (uint64_t i = 0; i < number_of_insertion; i++)
            {
                uint64_t c = alphabet[get_rand_uni_int(mt64) % alphabet.size()];
                uint64_t nth = get_rand_uni_int(mt64) % (naive_text.size() + 1);
                if (use_end_marker)
                {
                    nth = get_rand_uni_int(mt64) % (naive_text.size());
                    c = alphabet[(get_rand_uni_int(mt64) % (alphabet.size() - 1)) + 1];
                }

                naive_text.insert_string(nth, c);
                test_text.insert_string(nth, c);

                assert((uint64_t)test_text.size() == (uint64_t)naive_text.size());

                if (detailed_check)
                {
                    std::string test_str = test_text.get_text_str();
                    std::string naive_str = naive_text.get_text_str();

                    try
                    {
                        stool::EqualChecker::equal_check(test_str, naive_str);
                    }
                    catch (std::logic_error &e)
                    {
                        std::cout << "Error in insert_character_test" << std::endl;
                        std::cout << "test_str : " << test_str << ", len = " << test_str.size() << std::endl;
                        std::cout << "naive_str: " << naive_str << ", len = " << naive_str.size() << std::endl;
                        throw std::logic_error("Error in insert_character_test");
                    }
                }
            }

            std::string test_str = test_text.get_text_str();
            std::string naive_str = naive_text.get_text_str();

            try
            {
                stool::EqualChecker::equal_check(test_str, naive_str);
            }
            catch (std::logic_error &e)
            {
                std::cout << "Error in insert_character_test" << std::endl;
                std::cout << "test_str : " << test_str << ", len = " << test_str.size() << std::endl;
                std::cout << "naive_str: " << naive_str << ", len = " << naive_str.size() << std::endl;
                throw std::logic_error("Error in insert_character_test");
            }
        }

        template <typename DYNAMIC_TEXT, typename NAIVE_TEXT = stool::NaiveDynamicString>
        static void insert_character_test(uint64_t max_text_size, uint64_t number_of_insertion, uint64_t number_of_trials, bool use_end_marker, bool detailed_check, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Insert Character Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;
                uint64_t len = 2;

                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;

                    for (uint64_t i = 0; i < number_of_trials; i++)
                    {
                        std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);
                        std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        if (use_end_marker)
                        {
                            _text.push_back(0);
                            alphabet.push_back(0);
                            std::sort(alphabet.begin(), alphabet.end());
                        }

                        NAIVE_TEXT naive_text = NAIVE_TEXT::build_from_text(_text, alphabet);
                        DYNAMIC_TEXT test_text = DYNAMIC_TEXT::build_from_text(_text, alphabet);

                        insert_character_test(test_text, naive_text, alphabet, number_of_insertion, use_end_marker, detailed_check, seed++);
                    }
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename DYNAMIC_TEXT, typename NAIVE_TEXT = stool::NaiveDynamicString>
        static void remove_character_test(DYNAMIC_TEXT &test_text, NAIVE_TEXT &naive_text, uint64_t number_of_removals, bool use_end_marker, bool detailed_check, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

            for (uint64_t i = 0; i < number_of_removals; i++)
            {
                if (use_end_marker)
                {
                    if (test_text.size() == 1)
                    {
                        break;
                    }
                    uint64_t nth = get_rand_uni_int(mt64) % (naive_text.size() - 1);
                    naive_text.delete_substring(nth);
                    test_text.delete_substring(nth);
                }
                else
                {
                    if (test_text.size() == 0)
                    {
                        break;
                    }
                    uint64_t nth = get_rand_uni_int(mt64) % (naive_text.size());
                    naive_text.delete_substring(nth);
                    test_text.delete_substring(nth);
                }

                assert((uint64_t)test_text.size() == (uint64_t)naive_text.size());
                if (detailed_check)
                {
                    std::string test_str = test_text.get_text_str();
                    std::string naive_str = naive_text.get_text_str();
                    stool::EqualChecker::equal_check(test_str, naive_str);
                }
            }

            if ((int64_t)test_text.size() != (int64_t)naive_text.size())
            {
                throw std::runtime_error("Error: test_text.size() != naive_text.size()");
            }

            std::string test_str = test_text.get_text_str();
            std::string naive_str = naive_text.get_text_str();
            stool::EqualChecker::equal_check(test_str, naive_str);
        }

        template <typename DYNAMIC_TEXT, typename NAIVE_TEXT = stool::NaiveDynamicString>
        static void remove_character_test(uint64_t max_text_size, uint64_t number_of_removals, uint64_t number_of_trials, bool use_end_marker, bool detailed_check, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Remove Character Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;
                uint64_t len = 2;

                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;

                    for (uint64_t i = 0; i < number_of_trials; i++)
                    {
                        std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);
                        std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        if (use_end_marker)
                        {
                            _text.push_back(0);
                            alphabet.push_back(0);
                            std::sort(alphabet.begin(), alphabet.end());
                        }

                        NAIVE_TEXT naive_text = NAIVE_TEXT::build_from_text(_text, alphabet);
                        DYNAMIC_TEXT test_text = DYNAMIC_TEXT::build_from_text(_text, alphabet);

                        remove_character_test(test_text, naive_text, number_of_removals, use_end_marker, detailed_check, seed++);
                    }
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename DYNAMIC_TEXT, typename NAIVE_TEXT = stool::NaiveDynamicString>
        static void insert_string_test(DYNAMIC_TEXT &test_text, NAIVE_TEXT &naive_text, [[maybe_unused]] const std::vector<uint8_t> &alphabet, uint64_t number_of_insertion, uint64_t max_string_length, bool use_end_marker, [[maybe_unused]] bool detailed_check, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, number_of_insertion);
            for (uint64_t i = 0; i < number_of_insertion; i++)
            {
                uint64_t text_size = naive_text.size();
                uint64_t copy_pos = 0;
                uint64_t copy_length = 0;
                uint64_t insertion_pos = 0;

                if (use_end_marker)
                {
                    copy_pos = (get_rand_uni_int(mt64) % (text_size - 1));
                    copy_length = (get_rand_uni_int(mt64) % (max_string_length)) + 1;
                    insertion_pos = get_rand_uni_int(mt64) % text_size;

                    if (copy_pos + copy_length >= text_size)
                    {
                        copy_length = text_size - copy_pos - 1;
                    }
                }
                else
                {
                    copy_pos = get_rand_uni_int(mt64) % text_size;
                    copy_length = (get_rand_uni_int(mt64) % max_string_length) + 1;
                    insertion_pos = get_rand_uni_int(mt64) % (text_size + 1);

                    if (copy_pos + copy_length > text_size)
                    {
                        copy_length = text_size - copy_pos;
                    }
                }

                std::vector<uint8_t> copy_string;
                for (uint64_t j = copy_pos; j < copy_pos + copy_length; j++)
                {
                    copy_string.push_back(naive_text.access_text(j));
                }

                naive_text.insert_string(insertion_pos, copy_string);
                test_text.insert_string(insertion_pos, copy_string);

                assert((uint64_t)test_text.size() == (uint64_t)naive_text.size());

                if (detailed_check)
                {
                    std::string test_str = test_text.get_text_str();
                    std::string naive_str = naive_text.get_text_str();

                    try
                    {
                        stool::EqualChecker::equal_check(test_str, naive_str);
                    }
                    catch (std::logic_error &e)
                    {
                        std::cout << "Error in insert_string_test" << std::endl;
                        std::cout << "test_str : " << test_str << ", len = " << test_str.size() << std::endl;
                        std::cout << "naive_str: " << naive_str << ", len = " << naive_str.size() << std::endl;
                        throw std::logic_error("Error in insert_string_test");
                    }
                }
            }

            std::string test_str = test_text.get_text_str();
            std::string naive_str = naive_text.get_text_str();

            try
            {
                stool::EqualChecker::equal_check(test_str, naive_str);
            }
            catch (std::logic_error &e)
            {
                std::cout << "Error in insert_string_test" << std::endl;
                std::cout << "test_str : " << test_str << ", len = " << test_str.size() << std::endl;
                std::cout << "naive_str: " << naive_str << ", len = " << naive_str.size() << std::endl;
                throw std::logic_error("Error in insert_string_test");
            }
        }

        template <typename DYNAMIC_TEXT, typename NAIVE_TEXT = stool::NaiveDynamicString>
        static void insert_string_test(uint64_t max_text_size, uint64_t number_of_insertion, uint64_t number_of_trials, uint64_t max_string_length, bool use_end_marker, bool detailed_check, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Insert String Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;
                uint64_t len = 2;

                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;

                    for (uint64_t i = 0; i < number_of_trials; i++)
                    {
                        std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);
                        std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        if (use_end_marker)
                        {
                            _text.push_back(0);
                            alphabet.push_back(0);
                            std::sort(alphabet.begin(), alphabet.end());
                        }

                        NAIVE_TEXT naive_text = NAIVE_TEXT::build_from_text(_text, alphabet);
                        DYNAMIC_TEXT test_text = DYNAMIC_TEXT::build_from_text(_text, alphabet);

                        insert_string_test(test_text, naive_text, alphabet, number_of_insertion, max_string_length, use_end_marker, detailed_check, seed++);
                    }
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename DYNAMIC_TEXT, typename NAIVE_TEXT = stool::NaiveDynamicString>
        static void delete_string_test(DYNAMIC_TEXT &test_text, NAIVE_TEXT &naive_text, [[maybe_unused]] const std::vector<uint8_t> &alphabet, uint64_t number_of_deletion, uint64_t max_string_length, bool use_end_marker, [[maybe_unused]] bool detailed_check, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);
            for (uint64_t i = 0; i < number_of_deletion; i++)
            {
                if (use_end_marker)
                {
                    if (naive_text.size() == 1)
                    {
                        break;
                    }
                }
                else
                {
                    if (naive_text.size() == 0)
                    {
                        break;
                    }
                }
                uint64_t text_size = naive_text.size();
                uint64_t deletion_pos = 0;
                uint64_t deletion_length = 0;

                if (use_end_marker)
                {
                    deletion_pos = (get_rand_uni_int(mt64) % (text_size - 1));
                    deletion_length = (get_rand_uni_int(mt64) % (max_string_length)) + 1;

                    if (deletion_pos + deletion_length >= text_size)
                    {
                        deletion_length = text_size - deletion_pos - 1;
                    }
                }
                else
                {
                    deletion_pos = get_rand_uni_int(mt64) % text_size;
                    deletion_length = (get_rand_uni_int(mt64) % max_string_length) + 1;

                    if (deletion_pos + deletion_length > text_size)
                    {
                        deletion_length = text_size - deletion_pos;
                    }
                }

                naive_text.delete_string(deletion_pos, deletion_length);
                test_text.delete_string(deletion_pos, deletion_length);

                assert((uint64_t)test_text.size() == (uint64_t)naive_text.size());

                if (detailed_check)
                {
                    std::string test_str = test_text.get_text_str();
                    std::string naive_str = naive_text.get_text_str();

                    try
                    {
                        stool::EqualChecker::equal_check(test_str, naive_str);
                    }
                    catch (std::logic_error &e)
                    {
                        std::cout << "Error in delete_string_test" << std::endl;
                        std::cout << "test_str : " << test_str << ", len = " << test_str.size() << std::endl;
                        std::cout << "naive_str: " << naive_str << ", len = " << naive_str.size() << std::endl;
                        throw std::logic_error("Error in delete_string_test");
                    }
                }
            }

            std::string test_str = test_text.get_text_str();
            std::string naive_str = naive_text.get_text_str();

            try
            {
                stool::EqualChecker::equal_check(test_str, naive_str);
            }
            catch (std::logic_error &e)
            {
                std::cout << "Error in delete_string_test" << std::endl;
                std::cout << "test_str : " << test_str << ", len = " << test_str.size() << std::endl;
                std::cout << "naive_str: " << naive_str << ", len = " << naive_str.size() << std::endl;
                throw std::logic_error("Error in delete_string_test");
            }
        }

        template <typename DYNAMIC_TEXT, typename NAIVE_TEXT = stool::NaiveDynamicString>
        static void delete_string_test(uint64_t max_text_size, uint64_t number_of_deletion, uint64_t number_of_trials, uint64_t max_string_length, bool use_end_marker, bool detailed_check, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Delete String Test: " << std::endl;
            for (uint64_t type = 0; type <= stool::UInt8VectorGenerator::get_max_alphabet_type(); type++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << "Alphabet Type: " << type << ", len = " << std::flush;
                uint64_t len = 2;

                while (len < max_text_size)
                {
                    std::cout << len << " " << std::flush;

                    for (uint64_t i = 0; i < number_of_trials; i++)
                    {
                        std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(type);
                        std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, seed++);
                        if (use_end_marker)
                        {
                            _text.push_back(0);
                            alphabet.push_back(0);
                            std::sort(alphabet.begin(), alphabet.end());
                        }

                        NAIVE_TEXT naive_text = NAIVE_TEXT::build_from_text(_text, alphabet);
                        DYNAMIC_TEXT test_text = DYNAMIC_TEXT::build_from_text(_text, alphabet);

                        delete_string_test(test_text, naive_text, alphabet, number_of_deletion, max_string_length, use_end_marker, detailed_check, seed++);
                    }
                    len *= 2;
                }
                std::cout << std::endl;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename TEXT, typename NAIVE_TEXT, bool USE_ACCESS, bool USE_LOCATE>
        static void random_test(TEXT &text_index, NAIVE_TEXT &original_text, const std::vector<uint8_t> &alphabet, uint64_t number_of_queries, bool use_end_marker, bool detailed_check, uint64_t seed, [[maybe_unused]] int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << "RANDOM_TEST: \t" << std::endl;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

            for (uint64_t i = 0; i < number_of_queries; i++)
            {
                assert((uint64_t)text_index.size() == (uint64_t)original_text.size());

                uint64_t type = get_rand_value(mt) % 6;
                if (type == 0 || type == 1)
                {
                    std::cout << "I" << std::flush;
                    insert_character_test(text_index, original_text, alphabet, 5, use_end_marker ,detailed_check, seed++);
                }
                else if (type == 2 || type == 3)
                {
                    std::cout << "R" << std::flush;
                    remove_character_test<TEXT, NAIVE_TEXT>(text_index, original_text, 1, use_end_marker ,detailed_check, seed++);
                }
                else if (type == 4)
                {
                    if constexpr (USE_ACCESS)
                    {
                        std::cout << "A" << std::flush;
                        stool::StringTest::access_character_test(text_index, original_text.text, 5, seed++);
                    }
                }
                else
                {
                    if constexpr (USE_LOCATE)
                    {
                        std::cout << "L" << std::flush;

                        std::vector<uint8_t> current_text = original_text.get_text();
                        std::vector<uint64_t> sa = stool::ArrayConstructor::construct_naive_suffix_array(current_text);
                        stool::StringTest::locate_query_test(text_index, original_text.text, sa, 5, 100, seed++);
                    }
                }
                if (detailed_check)
                {
                    std::string test_str = text_index.get_text_str();
                    std::string naive_str = original_text.get_text_str();
                    stool::EqualChecker::equal_check(test_str, naive_str);
                }
            }

            std::string test_str = text_index.get_text_str();
            std::string naive_str = original_text.get_text_str();
            stool::EqualChecker::equal_check(test_str, naive_str);

            std::cout << std::endl;
            std::cout << "[DONE]" << std::endl;

        }


        template <typename TEXT, typename NAIVE_TEXT, bool USE_ACCESS, bool USE_LOCATE>
        static void random_test(uint64_t text_size, uint64_t alphabet_type, uint64_t number_of_queries, uint64_t number_of_trials, bool use_end_marker, bool detailed_check, uint64_t seed, [[maybe_unused]] int message_paragraph = stool::Message::SHOW_MESSAGE)
        {

            std::cout << "RANDOM_TEST: \t" << std::endl;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

            for (uint64_t i = 0; i < number_of_trials; i++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph + 1) << i << ": " << std::flush;

                std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_alphabet(alphabet_type);
                std::vector<uint8_t> _text = stool::UInt8VectorGenerator::create_random_sequence(text_size, alphabet, seed++);
                if (use_end_marker)
                {
                    _text.push_back(0);
                    alphabet.push_back(0);
                    std::sort(alphabet.begin(), alphabet.end());
                }

                NAIVE_TEXT naive_text = NAIVE_TEXT::build_from_text(_text, alphabet);
                TEXT test_text = TEXT::build_from_text(_text, alphabet);

                random_test<TEXT, NAIVE_TEXT, USE_ACCESS, USE_LOCATE>(test_text, naive_text, alphabet, number_of_queries, use_end_marker, detailed_check, seed++, stool::Message::add_message_paragraph(message_paragraph));
            }
            std::cout << "[DONE]" << std::endl;
        }
    };

} // namespace name
