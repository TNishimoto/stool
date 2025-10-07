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
        /*!
         * @brief A small helper component for Beller's LCP array construction algorithm
         * 
         * BellerSmallComponent provides auxiliary functionality for the main Beller algorithm
         * implementation. It includes:
         * - Tracking of last processed indices and bounds
         * - Occurrence flag management
         * - Character occurrence set maintenance
         * 
         * The class is particularly useful for:
         * - State tracking during LCP computation
         * - Character occurrence bookkeeping
         * - Efficient interval bound management
         * 
         * @note This implementation is designed to work closely with BellerComponent
         *       for efficient LCP array construction
         */

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