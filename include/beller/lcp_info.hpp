#pragma once

namespace stool
{
    namespace beller
    {
        /*!
         * @brief A structure to store LCP (Longest Common Prefix) information
         * 
         * LCPInfo stores both the LCP value and its corresponding position in the text.
         * It includes:
         * - LCP value storage
         * - Position tracking
         * - Default and parameterized constructors
         * 
         * The structure is particularly useful for:
         * - LCP array construction algorithms
         * - Suffix array based text processing
         * - Pattern matching applications
         * 
         * @note This implementation uses uint64_t for both LCP values and positions
         *       to handle large texts efficiently
         */
        struct LCPInfo
        {
            uint64_t lcp;
            uint64_t position;
            LCPInfo()
            {
                this->lcp = UINT64_MAX;
                this->position = UINT64_MAX;
            }
            LCPInfo(uint64_t _lcp, uint64_t _position) : lcp(_lcp), position(_position) {}
        };
    } // namespace beller
} // namespace stool