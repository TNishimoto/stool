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
    class UpdateTest
    {
    public:
        template <typename TEST_OBJECT, typename CORRECT_OBJECT>
        static void update_test(TEST_OBJECT &test_obj, CORRECT_OBJECT &correct_obj, uint64_t number_of_trials, 
            std::function<void(TEST_OBJECT &, CORRECT_OBJECT &, uint64_t, int64_t)> update_function, 
            std::function<bool(TEST_OBJECT &, CORRECT_OBJECT &)> equal_check_function, bool detail_cehck, [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            for(uint64_t i = 0; i < number_of_trials; i++){
                update_function(test_obj, correct_obj, i, message_paragraph+1);
                if(detail_cehck){
                    equal_check_function(test_obj, correct_obj);
                }
            }
            equal_check_function(test_obj, correct_obj);
        }

        template <typename TEST_OBJECT, typename CORRECT_OBJECT>
        static void update_test(std::string update_name, uint64_t number_of_trials1, uint64_t number_of_trials2, 
            std::function<TEST_OBJECT(uint64_t, int64_t)> test_obj_builder_function, 
            std::function<CORRECT_OBJECT(uint64_t, int64_t)> correct_obj_builder_function, 
            std::function<void(TEST_OBJECT &, CORRECT_OBJECT &, uint64_t, int64_t)> update_function, 
            std::function<bool(TEST_OBJECT &, CORRECT_OBJECT &)> equal_check_function, bool detail_cehck, [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Update Test: " << update_name << std::endl;
            for(uint64_t i = 0; i < number_of_trials1; i++){
                TEST_OBJECT test_obj = test_obj_builder_function(i, message_paragraph+1);
                CORRECT_OBJECT correct_obj = correct_obj_builder_function(i, message_paragraph+1);
                update_test<TEST_OBJECT, CORRECT_OBJECT>(test_obj, correct_obj, number_of_trials2, update_function, equal_check_function, detail_cehck, message_paragraph+1);
            }
            std::cout << "[DONE]" << std::endl;
        }




    };

} // namespace name
