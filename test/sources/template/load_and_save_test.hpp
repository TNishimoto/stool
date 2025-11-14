#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <bitset>
#include "../../../include/all.hpp"

namespace stool
{
    class LoadAndSaveTest
    {
    public:
        template <typename OBJECT>
        static void load_and_save_file_test(OBJECT &obj, std::function<bool(OBJECT &, OBJECT &)> equal_check_function, std::string filepath, [[maybe_unused]] int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            {
                std::ofstream os;
                os.open(filepath, std::ios::binary);
                if (!os)
                {
                    std::cerr << "Error: Could not open file for writing." << std::endl;
                    throw std::runtime_error("File open error");
                }
                OBJECT::store_to_file(obj, os);
            }

            OBJECT obj2;
            {
                std::ifstream ifs;
                ifs.open(filepath, std::ios::binary);
                if (!ifs)
                {
                    std::cerr << "Error: Could not open file for reading." << std::endl;
                    throw std::runtime_error("File open error");
                }

                OBJECT tmp = OBJECT::load_from_file(ifs);
                obj2.swap(tmp);
            }
            bool b = equal_check_function(obj, obj2);
            if (!b)
            {
                throw std::runtime_error("Error: save_and_load_test failed");
            }
        }

        template <typename OBJECT>
        static void load_and_save_bytes_test(OBJECT &obj, std::function<bool(OBJECT &, OBJECT &)> equal_check_function, [[maybe_unused]] int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            uint64_t pos = 0;
            std::vector<uint8_t> bytes;
            OBJECT::store_to_bytes(obj, bytes, pos);
            pos = 0;
            OBJECT obj2 = OBJECT::load_from_bytes(bytes, pos);
            bool b = equal_check_function(obj, obj2);
            if (!b)
            {
                throw std::runtime_error("Error: save_and_load_bytes_test failed");
            }
        }

        template <typename OBJECT>
        static void load_and_save_file_test(uint64_t number_of_trials, std::function<OBJECT(uint64_t, int64_t)> builder_function, 
                                       std::function<bool(OBJECT &, OBJECT &)> equal_check_function, std::string filepath, [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Load and Save File Test: " << std::endl;

            for(uint64_t i = 0; i < number_of_trials; i++){
                OBJECT obj = builder_function(i, message_paragraph+1);
                load_and_save_file_test(obj, equal_check_function, filepath, message_paragraph+1);
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename OBJECT>
        static void load_and_save_bytes_test(uint64_t number_of_trials, std::function<OBJECT(uint64_t, int64_t)> builder_function, 
                                       std::function<bool(OBJECT &, OBJECT &)> equal_check_function, [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Load and Save Bytes Test: " << std::endl;

            for(uint64_t i = 0; i < number_of_trials; i++){
                OBJECT obj = builder_function(i, message_paragraph+1);
                load_and_save_bytes_test(obj, equal_check_function, message_paragraph+1);
            }
            std::cout << "[DONE]" << std::endl;
        }

    };

} // namespace name
