#pragma once
#include "./lcp_interval.hpp"

namespace stool
{
    /**
     * @brief Comparator for LCP intervals in preorder traversal [Unchecked AI's Comment]
     * @tparam INDEX The index type
     */
    template <typename INDEX = uint64_t>
    struct LCPIntervalPreorderComp
    {
        /**
         * @brief Compares two LCP intervals for preorder sorting
         * @param x The first LCP interval
         * @param y The second LCP interval
         * @return true if x should come before y in preorder
         */
        bool operator()(const LCPInterval<INDEX> &x, const LCPInterval<INDEX> &y)
        {
            if (x.i == y.i)
            {
                if (x.j == y.j)
                {
                    return x.lcp < y.lcp;
                }
                else
                {
                    return x.j > y.j;
                }
            }
            else
            {
                return x.i < y.i;
            }
        }
    };
}