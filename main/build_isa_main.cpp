#include <iostream>
#include <string>
#include <memory>
#include "cmdline/cmdline.h"
#include "../include/all.hpp"




template <typename T>
void mainfunc(std::string input, std::string outputFile, bool textOutput)
{
    auto start = std::chrono::system_clock::now();


    std::vector<T> text;
    std::cout << "Loading Text..." << std::endl;
    stool::FileReader::load_vector(input, text);

    std::cout << "Constructing Suffix Array..." << std::endl;
    std::vector<uint64_t> sa = stool::sais_suffix_array(text);

    std::cout << "Constructing Inverse Suffix Array..." << std::endl;
    std::vector<uint64_t> isa = stool::ArrayConstructor::construct_ISA(sa);

    if(textOutput) {
        std::cout << "Writing Inverse Suffix Array as Text..." << std::endl;
        stool::FileWriter::write_vector_as_text(outputFile, isa);
    }else{
        std::cout << "Writing Inverse Suffix Array..." << std::endl;
        stool::FileWriter::write_vector(outputFile, isa);    
    }

    auto end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "File : " << input << std::endl;
    std::cout << "Output file : " << outputFile << std::endl;
    std::cout << "The length of the input text : " << text.size() << std::endl;
    double charperms = (double)text.size() / elapsed;
    //std::cout << "The number of RLBWT : " << rlbwt.size() << std::endl;
    std::cout << "Excecution time : " << ((uint64_t)elapsed) << "ms";
    std::cout << "[" << charperms << "chars/ms]" << std::endl;
    std::cout << "==================================" << std::endl;
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
    p.add<std::string>("output_file", 'o', "output bwt file path", false, "");
    p.add<bool>("text_output", 't', "output text file", false, false);
    //p.add<int64_t>("special_character", 's', "special character", false, 0);
    p.add<std::string>("char_type", 'c', "char_type", true, "uint8_t");

    p.parse_check(argc, argv);
    std::string inputFile = p.get<std::string>("input_file");
    std::string outputFile = p.get<std::string>("output_file");
    //int64_t specialCharacter = p.get<int64_t>("special_character");
    std::string char_type = p.get<std::string>("char_type");
    bool textOutput = p.get<bool>("text_output");
    
    if (outputFile.size() == 0)
    {
        outputFile = inputFile + ".sa";
    }

    if (char_type == "uint8_t")
    {
        mainfunc<uint8_t>(inputFile, outputFile, textOutput);
    }
    else if (char_type == "uint16_t")
    {
        mainfunc<uint16_t>(inputFile, outputFile, textOutput);
    }
    else if (char_type == "uint32_t")
    {
        mainfunc<uint32_t>(inputFile, outputFile, textOutput);
    }
    else if (char_type == "uint64_t")
    {
        mainfunc<uint64_t>(inputFile, outputFile, textOutput);
    }
    else if (char_type == "int8_t")
    {
        mainfunc<int8_t>(inputFile, outputFile, textOutput);
    }
    else if (char_type == "int16_t")
    {
        mainfunc<int16_t>(inputFile, outputFile, textOutput);
    }
    else if (char_type == "int32_t")
    {
        mainfunc<int32_t>(inputFile, outputFile, textOutput);
    }
    else if (char_type == "int64_t")
    {
        mainfunc<int64_t>(inputFile, outputFile, textOutput);
    }
    else
    {
        std::cout << "Invalid char_type: " << char_type << std::endl;
        std::cout << "Valid char_types are: uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t" << std::endl;
        throw std::runtime_error("Invalid char_type");
    }
}