#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
// #include "qgram_tree.h"
// #include "include/debug/debug.hpp"
#include "../../include/stool.hpp"

#include <filesystem>

template <typename RLBWT, typename LF_DATA>
void mode1(RLBWT &static_rlbwt, LF_DATA &rle_wt)
{
    uint64_t text_size = static_rlbwt.str_size();
    uint64_t counter = 0;
    stool::rlbwt2::BackwardISA<LF_DATA> isa_ds;
    uint64_t p1 = static_rlbwt.get_end_rle_lposition();
    uint64_t p2 = static_rlbwt.get_lpos(p1);
    uint64_t p3 = rle_wt.lf(p2);
    isa_ds.set(&rle_wt, p3, text_size);

    uint64_t checksum = 0;

    std::chrono::system_clock::time_point st1, st2;

    st1 = std::chrono::system_clock::now();
    for (auto it = isa_ds.begin(); it != isa_ds.end(); ++it)
    {
        counter++;
        checksum += (counter ^ (*it));
        if (counter % 10000000 == 0)
        {
            std::cout << "[" << counter << "/" << text_size << "]" << std::endl;
        }
    }
    st2 = std::chrono::system_clock::now();

    uint64_t time1 = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();
    std::cout << counter << std::endl;

    std::cout << "checksum: " << checksum << std::endl;
    std::cout << "time: " << (time1 / (1000 * 1000)) << "[ms]" << std::endl;
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
    p.add<uint>("type", 'f', "type", true);

    p.add<std::string>("input_path", 'i', "input_path", false);

    p.add<uint>("detailed_check", 'u', "detailed_check", false, 0);

    p.parse_check(argc, argv);
    uint64_t mode = p.get<uint>("mode");
    uint64_t type = p.get<uint>("type");
    std::string input_path = p.get<std::string>("input_path");
    bool detailed_check = p.get<uint>("detailed_check") == 0 ? false : true;

    stool::rlbwt2::TextStatistics analyzer;
    using RLBWT = stool::rlbwt2::RLE<uint8_t>;
    // stool::rlbwt2::RLE<uint8_t> static_rlbwt = stool::rlbwt2::RLEBuilder::build(input_path, analyzer);
    RLBWT static_rlbwt = RLBWT::build(input_path, analyzer);

    stool::WT wt = stool::rlbwt2::WaveletTreeOnHeadChars::build(&static_rlbwt);

        if (type == 0)
        {
            stool::rlbwt2::LightFPosDataStructure fpos_array;
            fpos_array.build(static_rlbwt.get_head_char_vec(), *static_rlbwt.get_lpos_vec(), &wt);
            stool::rlbwt2::LFDataStructure<RLBWT, stool::rlbwt2::LightFPosDataStructure> rle_wt(&static_rlbwt, &fpos_array);
            rle_wt.verifyBWT();
            mode1(static_rlbwt, rle_wt);
        }
        else
        {
            std::vector<uint64_t> fpos_array = stool::rlbwt2::FPosDataStructure::construct_fpos_array(*static_rlbwt.get_head_char_vec(), *static_rlbwt.get_lpos_vec());
            stool::rlbwt2::LFDataStructure<RLBWT, std::vector<uint64_t>> rle_wt(&static_rlbwt, &fpos_array);
            rle_wt.verifyBWT();

            mode1(static_rlbwt, rle_wt);
        }

    /*
    if (true)
    {
        stool::rlbwt2::LightFPosDataStructure fpos_array1;
        fpos_array1.build(static_rlbwt.get_head_char_vec(), *static_rlbwt.get_lpos_vec(), &wt);
        std::vector<uint64_t> fpos_array2 = stool::rlbwt2::FPosDataStructure::construct_fpos_array(*static_rlbwt.get_head_char_vec(), *static_rlbwt.get_lpos_vec());

        for (uint8_t x = 0; x <= 8; x++)
        {
            std::cout << "c: " << (int)x << std::endl; 
            for (uint64_t i = 0; i < fpos_array2.size(); i++)
            {
                uint8_t c = static_rlbwt.get_char_by_run_index(i);
                uint64_t len = static_rlbwt.get_run(i);
                if (c == (uint64_t)x)
                {
                    std::cout << i << "/len: " << len << "/" << fpos_array1[i] << "/" << fpos_array2[i] << std::endl;
                    if (fpos_array1[i] != fpos_array2[i])
                    {
                        throw std::logic_error("Error: fpos");
                    }
                }
            }
        }

        std::cout << "OK!" << std::endl;
    }
    else
    {
    }
    */
}
