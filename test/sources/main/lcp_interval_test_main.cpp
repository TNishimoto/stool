#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
#include "../../include/stool.hpp"

using LCPINTV = stool::LCPInterval<uint64_t>;
using INDEX = uint64_t;

bool equal_check_lcp_intervals(std::vector<stool::LCPInterval<INDEX>> &item1, std::vector<stool::LCPInterval<INDEX>> &item2, std::string name = "")
{

    sort(item1.begin(), item1.end(), [](const LCPINTV &lhs, const LCPINTV &rhs)
         {
                if (lhs.lcp != rhs.lcp)
                {
                    return lhs.lcp < rhs.lcp;
                }
                else
                {
                    return lhs.i < rhs.i;
                } });

    sort(item2.begin(), item2.end(), [](const LCPINTV &lhs, const LCPINTV &rhs)
         {
                if (lhs.lcp != rhs.lcp)
                {
                    return lhs.lcp < rhs.lcp;
                }
                else
                {
                    return lhs.i < rhs.i;
                } });

    bool b = true;
    if (item1.size() != item2.size())
    {
        std::cout << "Distinct Size!" << item1.size() << "/" << item2.size() << std::endl;
        b = false;
    }
    else
    {
        for (uint64_t i = 0; i < item1.size(); i++)
        {

            if (item1[i].i != item2[i].i || item1[i].j != item2[i].j || item1[i].lcp != item2[i].lcp)
            {
                std::cout << "Distinct Value!" << std::endl;
                std::cout << item1[i].to_string() << "/" << item2[i].to_string() << std::endl;

                b = false;
                break;
            }
        }
    }
    if (!b)
    {
        std::cout << name << " Error!" << std::endl;
        if (item1.size() < 100)
        {
            std::cout << "Test: " << item1.size() << "/ Collect: " << item2.size() << std::endl;

            std::cout << "Test Vec:" << std::endl;
            for (auto it : item1)
            {
                std::cout << it.to_string();
            }
            std::cout << std::endl;

            std::cout << "Correct Vec:" << std::endl;
            for (auto it : item2)
            {
                std::cout << it.to_string();
            }
            std::cout << std::endl;
        }
        throw -1;
    }
    return true;
}

void test(uint64_t len, uint64_t seed){
std::vector<uint8_t> alph = stool::UInt8VectorGenerator::create_ACGT_alphabet();
        std::vector<uint8_t> text = stool::UInt8VectorGenerator::create_random_sequence(len, alph, seed);
        text.push_back(0);


        std::vector<uint64_t> sa = stool::ArrayConstructor::construct_naive_suffix_array(text);
        std::vector<LCPINTV> correct_intervals = stool::StringFunctionsOnSA::naive_compute_lcp_intervals(text, sa);

        sdsl::int_vector<> sdsl_BWT; 
        stool::SDSLFunctions::construct_BWT(text, sa, sdsl_BWT);

        std::vector<LCPINTV> beller_intervals = stool::beller::LCPIntervalEnumerator::compute_lcp_intervals(sdsl_BWT);

        equal_check_lcp_intervals(beller_intervals,correct_intervals, "Beller");

        std::vector<uint64_t> beller_lcp_array = stool::beller::LCPEnumerator::construct_LCP_array(sdsl_BWT);
        std::vector<uint64_t> correct_lcp_array = stool::ArrayConstructor::construct_LCP_array(text, sa, stool::Message::NO_MESSAGE);

        stool::EqualChecker::equal_check(correct_lcp_array, beller_lcp_array, "LCP Array");
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

    if (mode == 0)
    {
        uint64_t seed = 0;

        for(uint64_t i = 0; i<100;i++){
            std::cout << "+" << std::flush;
            test(200, seed + i);
        }
        



        /*
        for (uint64_t i = 0; i < correct_intervals.size(); i++)
        {
            std::cout << correct_intervals[i].to_string() << std::endl;
        }
        */
    }
    else
    {
    }
}
