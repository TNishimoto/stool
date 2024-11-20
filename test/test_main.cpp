#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
#include "../include/light_stool.hpp"

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

    p.add<uint>("mode", 'm', "mode", true);

    p.parse_check(argc, argv);
    uint64_t mode = p.get<uint>("mode");

    if (mode == 0)
    {
        uint64_t len = 100;
        for (uint64_t i = 0; i < 3; i++)
        {
            std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_ACGT_alphabet();

            for (uint64_t x = 0; x < 100; x++)
            {
                std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, x);
                std::vector<uint64_t> sa = stool::StringFunctions::construct_naive_suffix_array(text);

                std::cout << "+" << std::flush;
                for (uint64_t j = 0; j < 100; j++)
                {
                    std::vector<uint8_t> pattern = stool::UInt8VectorGenerator::create_random_substring(text, j);

                    std::vector<uint64_t> result1 = stool::StringFunctions::locate_query(text, pattern);
                    std::vector<uint64_t> result2 = stool::StringFunctionsOnSA::locate_query(text, pattern, sa);
                    stool::equal_check(result1, result2);
                }
            }
                std::cout << std::endl;

        }
    }
}
