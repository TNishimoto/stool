#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include "./char_interval.hpp"
#include "./interval_search_data_structure.hpp"

#include "../strings/lcp_interval.hpp"
#include "../debug/message.hpp"
#include "../strings/sa_bwt_lcp.hpp"

// #include "sa_lcp.hpp"
// using namespace std;
// using namespace sdsl;

namespace stool
{
    namespace beller
    {

        struct BellerSmallComponent
        {
            uint64_t last_idx;
            uint64_t last_lb;
            bool occB;
            std::set<uint8_t> nextOccurrenceSet;

            BellerSmallComponent()
            {
            }
            void initialize()
            {
                last_idx = UINT64_MAX;
                last_lb = UINT64_MAX;
                occB = false;
            }
        };
    } // namespace beller
} // namespace stool