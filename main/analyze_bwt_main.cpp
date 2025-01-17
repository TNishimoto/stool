#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
// #include "qgram_tree.h"
// #include "include/debug/debug.hpp"

#include "../include/stool.hpp"

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
    //p.add<uint>("option", 'b', "option", false, 0);
    

    p.parse_check(argc, argv);
    //uint64_t mode = p.get<uint>("option");
    std::string input_file_path = p.get<std::string>("input_file");

    std::chrono::system_clock::time_point st1, st2;
    st1 = std::chrono::system_clock::now();

    stool::TextStatistics ts = stool::TextStatistics::build(input_file_path);

    std::vector<uint8_t> text;
    stool::IO::load_text(input_file_path, text);
    sdsl::int_vector<> int_text;
    stool::SDSLFunctions::to_int_vector(text, int_text);
    std::vector<uint64_t> lcp_statistics = stool::beller::LCPEnumerator::compute_lcp_statistics(int_text);
    std::vector<uint64_t> distinct_substring_counter_array = stool::SubstringComplexityFunctions::construct_distinct_substring_counter_array_from_lcp_statistics(lcp_statistics, text.size());
    uint64_t delta = stool::SubstringComplexityFunctions::compute_delta(distinct_substring_counter_array);

    uint64_t lcp_max = lcp_statistics.size() - 1;
    uint64_t lcp_sum = 0;
    for(uint64_t i = 0; i < lcp_statistics.size(); i++){
        lcp_sum += i * lcp_statistics[i];
    }
    uint64_t lcp_avg = lcp_sum / text.size();

    st2 = std::chrono::system_clock::now();
    

    std::cout << "\033[36m";
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "Input File:" << input_file_path << std::endl;
    ts.print();

    uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
    std::cout << "Total Time: " << sec_time << " sec" << std::endl;
    std::cout << "lcp max: " << lcp_max << std::endl;
    std::cout << "lcp avg: " << lcp_avg << std::endl;
    std::cout << "delta: " << delta << std::endl;

    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;



    //stool::IO::load_text(input_file_path, bwt);


}
