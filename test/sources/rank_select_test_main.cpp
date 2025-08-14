#include <cassert>
#include <chrono>
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
// #include "../../include/io.hpp"
// #include "../../include/cmdline.h"
#include "../../include/light_stool.hpp"


/*
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy" // suppress specific warning
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>
#pragma GCC diagnostic pop
*/

int64_t compute_select1(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = 0; j < bv.size(); j++){
        if(bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}
int64_t compute_select0(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = 0; j < bv.size(); j++){
        if(!bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}

int64_t compute_rev_select1(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = bv.size()-1; j >= 0; j--){
        if(bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}
int64_t compute_rev_select0(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = bv.size()-1; j >= 0; j--){
        if(!bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}


/*
int64_t compute_rev_select1(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = 0; j < bv.size(); j++){
        if(bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}
*/


/*
void rank_test(std::vector<uint8_t> &text, stool::WT &wt)
{
    uint64_t CHARMAX = UINT8_MAX + 1;
    std::vector<uint64_t> C_run_sum;
    C_run_sum.resize(CHARMAX, 1);
    uint64_t rle = text.size();
    for (uint64_t i = 0; i < rle; i++)
    {
        uint8_t c = text[i];
        uint64_t rank1 = wt.rank(i+1, c);
        std::cout << i << "/" << (int) c << "/" << C_run_sum[c] << "/" << rank1 << std::endl;
        if (C_run_sum[c] != rank1)
        {
            std::cout << "#" << (int)c << std::endl;
        }
        assert(C_run_sum[c] == rank1);
        C_run_sum[c]++;
    }
}

void c_run_sum_test(){

    uint64_t seed = 0;
    std::vector<uint8_t> alph{ 1, 2, 3, 4, 5, 6, 7, 8};    
    //std::vector<uint8_t> alph = stool::UInt8VectorGenerator::create_ACGT_alphabet();
    std::vector<uint8_t> seq = stool::UInt8VectorGenerator::create_random_sequence(100, alph, seed);
    stool::Printer::print_string(seq);
    sdsl::int_vector<8> text;
    //text.width(8);
    text.resize(seq.size());
    for (uint64_t i = 0; i < seq.size(); i++)
    {
        text[i] = seq[i];
    }
    stool::WT wt;
    construct_im(wt, text);

    rank_test(seq, wt);
}
*/

void bit_select_test(uint64_t seed, uint64_t max_counter){
    std::cout << "bit_select_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        if(i % 100 == 0){
            std::cout << "+" << std::flush;
        }
        uint64_t random_value = get_rand_value(mt);
        uint64_t rank1 = stool::Byte::count_bits(random_value);
        uint64_t rank0 = 64 - rank1;

        std::vector<bool> bv;
        for(uint64_t j = 0; j < 64; j++){
            bv.push_back(random_value & (1ULL << (63 - j)));
        }

        std::vector<uint64_t> select1_results;
        std::vector<uint64_t> select0_results;

        std::vector<uint64_t> select1_results_msb_test;
        std::vector<uint64_t> select0_results_msb_test;
        std::vector<uint64_t> select1_results_lsb_test;
        std::vector<uint64_t> select0_results_lsb_test;

        std::vector<uint64_t> rev_select1_results;
        std::vector<uint64_t> rev_select0_results;

        std::vector<uint64_t> rev_select1_results_test;
        std::vector<uint64_t> rev_select0_results_test;


        select1_results_lsb_test.resize(rank1);
        for(uint64_t j = 0; j < rank1; j++){
            select1_results.push_back(compute_select1(bv, j));
            select1_results_msb_test.push_back(stool::MSBByte::select1(random_value, j));
            select1_results_lsb_test[rank1 - 1 - j] = 63 - stool::LSBByte::select1(random_value, j);
        }
        try{
            stool::EqualChecker::equal_check(select1_results, select1_results_msb_test, "select1_resultsA");
        }
        catch(const std::exception& e){
            std::cout << "select1_resultsA: " << e.what() << std::endl;
            std::cout << "select1_results: " << stool::Byte::to_bit_string(random_value) << std::endl;
            std::cout << "select1_results: " << stool::DebugPrinter::to_integer_string(select1_results) << std::endl;
            std::cout << "select1_results_msb_test: " << stool::DebugPrinter::to_integer_string(select1_results_msb_test) << std::endl;

            throw e;
        }
        stool::EqualChecker::equal_check(select1_results, select1_results_lsb_test, "select1_resultsB");

        select0_results_lsb_test.resize(rank0);
        for(uint64_t j = 0; j < rank0; j++){
            select0_results.push_back(compute_select0(bv, j));
            select0_results_msb_test.push_back(stool::MSBByte::select0(random_value, j));
            select0_results_lsb_test[rank0 - 1 - j] = 63 - stool::LSBByte::select0(random_value, j);
        }
        stool::EqualChecker::equal_check(select0_results, select0_results_msb_test, "select0_resultsA");
        stool::EqualChecker::equal_check(select0_results, select0_results_lsb_test, "select0_resultsB");

        for(uint64_t j = 0; j < rank1; j++){
            rev_select1_results.push_back(compute_rev_select1(bv, j));
            int64_t p = stool::LSBByte::select1(random_value, j);
            if(p != -1){
                rev_select1_results_test.push_back(63 - p);
            }else{
                rev_select1_results_test.push_back(-1);
            }
        }
        stool::EqualChecker::equal_check(rev_select1_results, rev_select1_results_test, "rev_select1_results");
        
        for(uint64_t j = 0; j < rank0; j++){
            rev_select0_results.push_back(compute_rev_select0(bv, j));
            int64_t p = stool::LSBByte::select0(random_value, j);
            if(p != -1){
                rev_select0_results_test.push_back(63 - p);
            }else{
                rev_select0_results_test.push_back(-1);
            }
        }
        stool::EqualChecker::equal_check(rev_select0_results, rev_select0_results_test, "rev_select0_results");
        

    }
    std::cout << "[DONE]" << std::endl;
}


int main()
{
    bit_select_test(0, 10000);
    
    //c_run_sum_test();


    // rankTest();
}
