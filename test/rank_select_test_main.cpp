#include <cassert>
#include <chrono>
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
// #include "../../include/io.hpp"
// #include "../../include/cmdline.h"
#include "../include/stool.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy" // suppress specific warning
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>
#pragma GCC diagnostic pop

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

int main()
{
    /*
    std::vector<uint64_t> r = create_random_integer_vector(30000, 3000);
    stool::Printer::print(r);

    stool::ValueArray va;
    va.set(r, true);

    std::vector<uint64_t> r2;
    va.decode(r2);
    stool::Printer::print(r2);

    stool::EliasFanoVector efs;
    efs.construct(&r2);

    stool::EliasFanoVector efs2(std::move(efs));

    std::vector<uint64_t> r3 = efs2.to_vector();
    stool::Printer::print(r3);

    std::vector<uint64_t> r4;
    for (auto it : efs2)
        r4.push_back(it);
    stool::Printer::print(r4);

    */

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

    // rankTest();
}
