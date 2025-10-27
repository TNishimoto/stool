#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
#include "../../include/light_stool.hpp"

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

    if (mode == 1)
    {
        uint64_t len = 100;
        for (uint64_t i = 0; i < 3; i++)
        {
            std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_ACGT_alphabet();

            for (uint64_t x = 0; x < 100; x++)
            {
                std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(len, alphabet, x);
                std::vector<uint64_t> sa = stool::ArrayConstructor::construct_naive_suffix_array(text);

                std::cout << "+" << std::flush;
                for (uint64_t j = 0; j < 100; j++)
                {
                    std::vector<uint8_t> pattern = stool::UInt8VectorGenerator::create_random_substring(text, j);

                    std::vector<uint64_t> result1 = stool::StringFunctions::locate_query(text, pattern);
                    std::vector<uint64_t> result2 = stool::StringFunctionsOnSA::locate_query(text, pattern, sa);
                    stool::EqualChecker::equal_check(result1, result2);
                }
            }
            std::cout << std::endl;
        }
    }
    else if (mode == 2)
    {
        for (uint64_t x = 0; x < 100; x++)
        {
            std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_ACGT_alphabet();
            std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(100, alphabet, x);
            stool::ForwardRLE frle(text.begin(), text.end(), text.size());
            std::vector<uint8_t> text2 = frle.to_text_vector();

            std::cout << "+" << std::flush;

            stool::EqualChecker::equal_check(text, text2);
        }

        std::cout << std::endl;
    }
    else if (mode == 3){
        for (uint64_t x = 0; x < 100; x++)
        {
            std::vector<uint8_t> alphabet = stool::UInt8VectorGenerator::create_ACGT_alphabet();
            std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(100, alphabet, x);
            std::string filename = "test_file.txt";
            
            stool::FileWriter::write_vector(filename, text);
            stool::OnlineFileReader ofr(filename);
            ofr.open();
            std::vector<uint8_t> text2;
            for(uint8_t c : ofr){
                text2.push_back(c);
            }
            ofr.close();

            std::cout << "+" << std::flush;

            stool::EqualChecker::equal_check(text, text2);
        }
        std::cout << std::endl;

    }
}
