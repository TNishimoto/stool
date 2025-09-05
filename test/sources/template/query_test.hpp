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
    class QueryTest
    {
    public:
        template <typename TEST_OBJECT, typename CORRECT_OBJECT>
        static void query_test(TEST_OBJECT &test_obj, CORRECT_OBJECT &correct_obj, uint64_t number_of_trials, 
            std::function<void(TEST_OBJECT &, CORRECT_OBJECT &, uint64_t, int64_t)> query_check_function, 
            [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            for(uint64_t i = 0; i < number_of_trials; i++){
                query_check_function(test_obj, correct_obj, i, message_paragraph+1);
            }
        }

        template <typename TEST_OBJECT, typename CORRECT_OBJECT>
        static void query_test(std::string query_name, uint64_t number_of_trials1, uint64_t number_of_trials2, 
            std::function<TEST_OBJECT(uint64_t, int64_t)> test_obj_builder_function, 
            std::function<CORRECT_OBJECT(uint64_t, int64_t)> correct_obj_builder_function, 
            std::function<void(TEST_OBJECT &, CORRECT_OBJECT &, uint64_t, int64_t)> query_check_function, 
            [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Query Test: " << query_name << std::endl;
            for(uint64_t i = 0; i < number_of_trials1; i++){
                TEST_OBJECT test_obj = test_obj_builder_function(i, message_paragraph+1);
                CORRECT_OBJECT correct_obj = correct_obj_builder_function(i, message_paragraph+1);
                query_test<TEST_OBJECT, CORRECT_OBJECT>(test_obj, correct_obj, number_of_trials2, query_check_function, message_paragraph+1);
            }
            std::cout << "[DONE]" << std::endl;
        }


    };

} // namespace name
