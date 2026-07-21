#include <iostream>
#include <string>
#include <memory>
#include "../include/all.hpp"
#include "cmdline/cmdline.h"



template <typename T>
void mainfunc(std::string input)
{
    auto start = std::chrono::system_clock::now();

    std::vector<T> text;
    std::cout << "Loading Text..." << std::endl;
    stool::FileReader::load_vector(input, text);

    std::cout << "Constructing Suffix Array..." << std::endl;
    std::vector<uint64_t> sa = stool::sais_suffix_array(text);
    std::vector<uint64_t> lcp_array = stool::ArrayConstructor::construct_LCP_array(text, sa);

    std::cout << "Computing delta..." << std::flush;
    std::vector<uint64_t> distinct_substring_counter_array = stool::SubstringComplexityFunctions::construct_distinct_substring_counter_array(lcp_array);
    uint64_t delta = stool::SubstringComplexityFunctions::compute_delta(distinct_substring_counter_array);
    std::cout << "[END]" << std::endl;
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[32m";
    std::cout << "______________________INFO______________________" << std::endl;
    std::cout << "File name \t : " << input << std::endl;

    std::cout << "Text length \t : " << text.size() << std::endl;
    std::cout << "Delta \t \t : " << delta << std::endl;
    double charperms = (double)text.size() / elapsed;
    //std::cout << "The number of RLBWT : " << rlbwt.size() << std::endl;
    std::cout << "Excecution time : " << ((uint64_t)elapsed) << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;

    std::cout << "_______________________________________________________" << std::endl;
    std::cout << "\033[39m" << std::endl;


}

int main(int argc, char *argv[])
{
    std::cout << "\033[41m";
    #ifdef RELEASE_BUILD
        std::cout << "Running in Release mode";
    #elif defined(DEBUG_BUILD)
    
        std::cout << "Running in Debug mode";
    #else
        std::cout << "Running in Unknown mode";
    #endif
    std::cout << "\e[m" << std::endl;

    cmdline::parser p;
    p.add<std::string>("input_file", 'i', "input file path", true);
    p.add<std::string>("char_type", 'c', "char_type", true, "uint8_t");

    p.parse_check(argc, argv);
    std::string inputFile = p.get<std::string>("input_file");
    std::string char_type = p.get<std::string>("char_type");

    /*
    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".dsa";
    }
    */

    if (char_type == "uint8_t")
    {
        mainfunc<uint8_t>(inputFile);
    }
    else if (char_type == "uint16_t")
    {
        mainfunc<uint16_t>(inputFile);
    }
    else if (char_type == "uint32_t")
    {
        mainfunc<uint32_t>(inputFile);
    }
    else if (char_type == "uint64_t")
    {
        mainfunc<uint64_t>(inputFile);
    }
    else if (char_type == "int8_t")
    {
        mainfunc<int8_t>(inputFile);
    }
    else if (char_type == "int16_t")
    {
        mainfunc<int16_t>(inputFile);
    }
    else if (char_type == "int32_t")
    {
        mainfunc<int32_t>(inputFile);
    }
    else if (char_type == "int64_t")
    {
        mainfunc<int64_t>(inputFile);
    }
    else
    {
        std::cout << "Invalid char_type: " << char_type << std::endl;
        std::cout << "Valid char_types are: uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t" << std::endl;
        throw std::runtime_error("Invalid char_type");
    }
}