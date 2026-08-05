#include <iostream>
#include <string>
#include <memory>
#include "../include/all.hpp"
#include "cmdline/cmdline.h"



template <typename T>
void mainfunc(std::string input, std::string output)
{
    auto start = std::chrono::system_clock::now();

    std::vector<T> text;
    std::cout << "Loading Text..." << std::endl;
    stool::FileReader::load_vector(input, text);

    std::cout << "Constructing Suffix Array..." << std::endl;
    std::vector<uint64_t> sa = stool::sais_suffix_array(text);
    std::vector<uint64_t> lcp_array = stool::ArrayConstructor::construct_LCP_array(text, sa);

    uint64_t max_lcp = *std::max_element(lcp_array.begin(), lcp_array.end());

    std::cout << "Computing delta..." << std::flush;
    std::vector<uint64_t> distinct_substring_counter_array = stool::SubstringComplexityFunctions::construct_distinct_substring_counter_array(lcp_array);
    std::vector<uint64_t> delta_array;
    for (uint64_t i = 1; i <= distinct_substring_counter_array.size(); i++){
        uint64_t i_delta = distinct_substring_counter_array[i] / i;
        delta_array.push_back(i_delta);
    }

    uint64_t delta_position = 0;
    uint64_t max_delta = delta_array[0];
    for (uint64_t i = 0; i < delta_array.size(); i++){
        if (delta_array[i] > max_delta){
            max_delta = delta_array[i];
            delta_position = i;
        }
    }

    std::vector<uint64_t> sampled_detla_array;
    for (uint64_t i = 0; i < delta_array.size(); i++){
        sampled_detla_array.push_back(delta_array[i]);
        if(sampled_detla_array.size() == 100){
            break;
        }
    }

    stool::DebugPrinter::print_integers(sampled_detla_array, "The first 100 values in the delta array");
    if(output.size() > 0){
        stool::FileWriter::write_vector_as_text(output, delta_array);
    }


    std::cout << "[END]" << std::endl;
    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[32m";
    std::cout << "______________________INFO______________________" << std::endl;
    std::cout << "File name         \t : " << input << std::endl;
    std::cout << "Text length       \t : " << text.size() << std::endl;
    std::cout << "Delta             \t : " << max_delta << std::endl;
    std::cout << "Delta position    \t : " << delta_position << std::endl;
    std::cout << "Max LCP           \t : " << max_lcp << std::endl;
    double charperms = (double)text.size() / elapsed;
    //std::cout << "The number of RLBWT : " << rlbwt.size() << std::endl;
    std::cout << "Excecution time   \t : " << ((uint64_t)elapsed) << "ms";
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
    p.add<std::string>("output_file", 'o', "output file path", false);

    p.add<std::string>("char_type", 'c', "char_type", true, "uint8_t");

    p.parse_check(argc, argv);
    std::string inputFile = p.get<std::string>("input_file");
    std::string outputFile = p.get<std::string>("output_file");
    std::string char_type = p.get<std::string>("char_type");

    /*
    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".dsa";
    }
    */

    if (char_type == "uint8_t")
    {
        mainfunc<uint8_t>(inputFile, outputFile);
    }
    else if (char_type == "uint16_t")
    {
        mainfunc<uint16_t>(inputFile, outputFile);
    }
    else if (char_type == "uint32_t")
    {
        mainfunc<uint32_t>(inputFile, outputFile);
    }
    else if (char_type == "uint64_t")
    {
        mainfunc<uint64_t>(inputFile, outputFile);
    }
    else if (char_type == "int8_t")
    {
        mainfunc<int8_t>(inputFile, outputFile);
    }
    else if (char_type == "int16_t")
    {
        mainfunc<int16_t>(inputFile, outputFile);
    }
    else if (char_type == "int32_t")
    {
        mainfunc<int32_t>(inputFile, outputFile);
    }
    else if (char_type == "int64_t")
    {
        mainfunc<int64_t>(inputFile, outputFile);
    }
    else
    {
        std::cout << "Invalid char_type: " << char_type << std::endl;
        std::cout << "Valid char_types are: uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t" << std::endl;
        throw std::runtime_error("Invalid char_type");
    }
}