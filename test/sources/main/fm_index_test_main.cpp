#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
#include "../../include/stool.hpp"


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
        uint64_t len = 10000;
        uint64_t max_alphabet_type = stool::RandomString::get_max_alphabet_type();
        uint64_t seed = 0;
        for (uint64_t alphabet_type = 0; alphabet_type < max_alphabet_type; alphabet_type++)
        {
            std::vector<uint8_t> alphabet = stool::RandomString::create_alphabet(alphabet_type);
            uint64_t end_marker = 0;
            for (uint64_t x = 0; x < 100; x++)
            {
                std::vector<uint8_t> text = stool::RandomString::create_random_sequence<uint8_t>(len, alphabet, seed++);
                text.push_back(end_marker);
                std::vector<uint64_t> sa = stool::ArrayConstructor::construct_naive_suffix_array(text);
                std::vector<uint8_t> bwt = stool::ArrayConstructor::construct_BWT(text, sa);
                auto ds = stool::bwt::LFDataStructure::build(bwt);
                
                std::vector<uint64_t> test_sa = ds.construct_suffix_array();

                stool::EqualChecker::equal_check(sa, test_sa, "test SA");

                std::cout << "+" << std::flush;
                
            }
                std::cout << std::endl;

        }
    }
}
