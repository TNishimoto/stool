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
#include "../strings/array_constructor.hpp"


namespace stool
{
    namespace beller
    {

        struct BellerSmallComponent
        {
            uint64_t last_idx;
            uint64_t last_lb;
            bool occB;
            std::set<uint8_t> next_occurrence_set;

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