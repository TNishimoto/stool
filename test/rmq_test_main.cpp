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

template <typename RMQ>
void test_rmq1(RMQ _rmq,uint64_t i)
{
    uint64_t len = 1;
    for (uint64_t i = 0; i < 13; i++)
    {

        std::vector<uint64_t> array = create_random_integer_vector(len, 10000);
        _rmq.build(array);

        std::cout << i << "/" << len << std::endl;

        for (uint64_t j = 0; j < len; j++)
        {
            for (uint64_t k = j; k < len; k++)
            {
                uint64_t rmq_index1 = _rmq.rmq_index(j, k, array);
                uint64_t naive_rmq_index = stool::RMQSparseTable<uint64_t>::naive_rmq_index(j, k, array);


                if (rmq_index1 != naive_rmq_index)
                {
                    stool::DebugPrinter::print_integers(array);
                    std::cout << "error" << std::endl;
                    std::cout << "j: " << j << ", k: " << k << std::endl;
                    std::cout << "rmq_index: " << rmq_index1 << ", naive_rmq_index: " << naive_rmq_index << std::endl;
                    throw std::runtime_error("error1");
                }

            }
        }

        len *= 2;
    }
}

template <typename RMQ1, typename RMQ2>
void test_rmq2(RMQ1 _rmq1, RMQ2 _rmq2,uint64_t i)
{
    uint64_t len = 1;
    for (uint64_t i = 0; i < 13; i++)
    {

        std::vector<uint64_t> array = create_random_integer_vector(len, 10000);
        _rmq1.build(array);
        _rmq2.build(array);
        std::cout << i << "/" << len << std::endl;

        for (uint64_t j = 0; j < len; j++)
        {
            for (uint64_t k = j; k < len; k++)
            {
                uint64_t rmq_index1 = _rmq1.rmq_index(j, k, array);
                uint64_t rmq_index2 = _rmq2.rmq_index(j, k, array);


                if (rmq_index1 != rmq_index2)
                {
                    stool::DebugPrinter::print_integers(array);
                    std::cout << "error" << std::endl;
                    throw std::runtime_error("error1");
                }

            }
        }

        len *= 2;
    }
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
        stool::RMQSparseTable<uint64_t> rmq1;
        stool::RMQSmallSparseTable<uint64_t> rmq2;

        test_rmq1(rmq1, 12);
        test_rmq1(rmq2, 12);

    }else{
        stool::RMQSparseTable<uint64_t> rmq1;
        stool::RMQSmallSparseTable<uint64_t> rmq2;

        test_rmq2(rmq1, rmq2, 12);
    }

}
