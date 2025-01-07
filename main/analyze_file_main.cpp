#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
// #include "qgram_tree.h"
// #include "include/debug/debug.hpp"

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

    // p.add<std::string>("input_file", 'i', "input file name", true);
    p.add<std::string>("input_file", 'i', "input file name", true);
    p.add<uint>("mode", 'm', "mode", false, 0);
    

    p.parse_check(argc, argv);
    uint64_t mode = p.get<uint>("mode");
        std::string input_file_path = p.get<std::string>("input_file");

    if(mode == 0){
    stool::TextStatistics ts = stool::TextStatistics::build(input_file_path);
    ts.print();

    } else if(mode == 1){
        std::vector<uint8_t> text;
        stool::IO::load_text(input_file_path, text);
        for(uint64_t i = 0; i < text.size();i++){
            if(text[i] <= 12){
                text[i] += 3;
            }
        }
        std::string output_path = input_file_path + ".mod";
        stool::IO::write(output_path, text);
        std::cout << "Finished!" << std::endl;

    } else{
        std::vector<uint8_t> text;
        stool::IO::load_text(input_file_path, text);
        for(uint64_t i = 0; i < text.size();i++){
            if(text[i] <= 131){
                text[i] += 3;
            }
        }
        std::string output_path = input_file_path + ".mod";
        stool::IO::write(output_path, text);
        std::cout << "Finished!" << std::endl;


    }


    //stool::IO::load_text(input_file_path, bwt);


}
