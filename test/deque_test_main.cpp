#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
// #include "qgram_tree.h"
// #include "include/debug/debug.hpp"

#include "../include/light_stool.hpp"
#include "./vlc_deque_test.hpp"
#include "./simple_deque_test.hpp"
#include "./integer_deque_test.hpp"

int main(int argc, char *argv[])
{
#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!" << std::endl;
    std::cout << "\e[m" << std::endl;
    // std::cout << "\033[30m" << std::endl;
#endif
#ifdef SLOWDEBUG
    std::cout << "\033[41m";
    std::cout << "SLOWDEBUG MODE!" << std::endl;
    std::cout << "\e[m" << std::endl;
    // std::cout << "\033[30m" << std::endl;
#endif

    cmdline::parser p;

    // p.add<std::string>("input_file", 'i', "input file name", true);
    p.add<uint>("mode", 'm', "mode", true);

    p.parse_check(argc, argv);
    uint64_t mode = p.get<uint>("mode");

    /*
     std::string str = "ababbbababa";
     std::vector<uint8_t> text = to_vector(str);


     stool::drfmi::DynamicBWT dbwt;
     dbwt.wavelet_tree.push_many(255, str);
     for(uint64_t i = 0;i < dbwt.wavelet_tree.size();i++){
         std::cout << (char)dbwt.wavelet_tree[i];

     }
     std::cout << std::endl;
     */

    // insert_test(80, 4);

    if (mode == 0)
    {
    }    
    else if (mode == 5)
    {
        uint64_t num = 16;
        uint64_t seed = 0;
        for (uint64_t i = 0; i < 10; i++)
        {
            std::cout << "Num: " << num << std::flush;
            uint64_t max_value = 2;
            for (uint64_t j = 0; j < 10; j++)
            {
                for (uint64_t x = 0; x < 10; x++)
                {
                    stool::VLCDequeTest::test1(num, max_value, seed);

                    stool::VLCDequeTest::test2(num, max_value, seed);
                    stool::VLCDequeTest::test3(num, max_value, seed);
                    stool::VLCDequeTest::test4(num, max_value, seed);

                    std::cout << "+" << std::flush;
                    ++seed;
                }
                std::cout << "/" << std::flush;
                max_value *= 2;
            }
            std::cout << std::endl;

            num *= 2;
        }
    }
    else if (mode == 6)
    {
        uint64_t num = 16;
        uint64_t seed = 0;
        for (uint64_t i = 0; i < 10; i++)
        {
            std::cout << "Num: " << num << std::flush;
            uint64_t max_value = 2;
            if (num > 10000)
            {
                break;
            }
            for (uint64_t j = 0; j < 10; j++)
            {

                for (uint64_t x = 0; x < 10; x++)
                {

                    stool::SimpleDequeTest::test1(num, max_value, seed);
                    stool::SimpleDequeTest::test2(num, max_value, seed);
                    stool::SimpleDequeTest::test3(num, max_value, seed);
                    stool::SimpleDequeTest::test4(num, max_value, seed);

                    std::cout << "+" << std::flush;
                    ++seed;
                }
                std::cout << "/" << std::flush;
                max_value *= 2;
            }
            std::cout << std::endl;

            num *= 2;
        }
    }
    else if (mode == 9)
    {
        uint64_t num = 16;
        uint64_t seed = 0;
        for (uint64_t i = 0; i < 5; i++)
        {
            std::cout << "Num: " << num << std::flush;
            uint64_t max_value = 2;
            if (num > 10000)
            {
                break;
            }
            for (uint64_t j = 0; j < 3; j++)
            {

                for (uint64_t x = 0; x < 5; x++)
                {

                    stool::IntegerDequeTest::test1(num, max_value, seed);
                    //stool::IntegerDequeTest::test2(num, max_value, seed);
                    //stool::IntegerDequeTest::test3(num, max_value, seed);
                    //stool::IntegerDequeTest::test4(num, max_value, seed);

                    std::cout << "+" << std::flush;
                    ++seed;
                }
                std::cout << "/" << std::flush;
                max_value = max_value << 8;
            }
            std::cout << std::endl;

            num *= 2;
        }
    }
}
