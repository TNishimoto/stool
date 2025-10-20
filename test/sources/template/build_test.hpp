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
    class BuildTest
    {
    public:
        template <typename OBJECT>
        static void build_test(uint64_t number_of_trials, std::function<OBJECT(uint64_t, int64_t)> builder_function, 
                                       std::function<bool(OBJECT &, uint64_t, int64_t)> equal_check_function, [[maybe_unused]]int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Build Test: " << std::endl;

            for(uint64_t i = 0; i < number_of_trials; i++){
                OBJECT obj = builder_function(i, message_paragraph+1);

                    equal_check_function(obj, i, message_paragraph+1);
            }
            std::cout << "[DONE]" << std::endl;
        }


    };

} // namespace name
