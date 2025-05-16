#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
// #include "qgram_tree.h"
// #include "include/debug/debug.hpp"

#include "../include/stool.hpp"

std::vector<uint64_t> create_random_integer_vector(uint64_t len, uint64_t max_value)
{
    std::vector<uint64_t> r;
    std::random_device rnd;
    for (uint64_t i = 0; i < len; ++i)
    {
        r.push_back(rnd() % max_value);
    }
    return r;
}

void rmq_performance_test(uint64_t len, uint64_t max_value)
{
    std::vector<uint64_t> array = create_random_integer_vector(len, max_value);
    stool::RMQSparseTable<uint64_t> rmq1;
    stool::RMQSmallSparseTable<uint64_t> rmq2;

    rmq1.build(array);
    rmq2.build(array);

    std::vector<uint64_t> input_left_seq;
    std::vector<uint64_t> input_right_seq;
    std::random_device rnd;
    for (uint64_t i = 0; i < len; ++i)
    {
        uint64_t a1 = rnd() % len;
        uint64_t a2 = rnd() % len;
        if(a1 < a2){
            input_left_seq.push_back(a1);
            input_right_seq.push_back(a2);
        }else{
            input_left_seq.push_back(a2);
            input_right_seq.push_back(a1);
        }
    }

    uint64_t hash1 = 0;
    auto start1 = std::chrono::system_clock::now();
    for (uint64_t i = 0; i < len; ++i)
    {
        hash1 += rmq1.rmq_index(input_left_seq[i], input_right_seq[i], array);
    }
    auto end1 = std::chrono::system_clock::now();

    uint64_t hash2 = 0;
    auto start2 = std::chrono::system_clock::now();
    for (uint64_t i = 0; i < len; ++i)
    {
        hash2 += rmq2.rmq_index(input_left_seq[i], input_right_seq[i], array);
    }
    auto end2 = std::chrono::system_clock::now();

    uint64_t ms_time1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();
    uint64_t ms_time2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();

    std::cout << "hash1: " << hash1 << std::endl;
    std::cout << "hash2: " << hash2 << std::endl;
    std::cout << "RMQSparseTable: " << ms_time1 << " ms" << std::endl;
    std::cout << "RMQSmallSparseTable: " << ms_time2 << " ms" << std::endl;

}


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

    if(mode == 0){
        rmq_performance_test(100000, 1000000);

    }else{
    }

}
