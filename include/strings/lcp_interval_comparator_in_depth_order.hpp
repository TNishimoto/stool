#pragma once
#include "./lcp_interval.hpp"

namespace stool
{
    /**
	 * @brief Comparator for LCP intervals in depth-first order 
	 * @tparam INDEX The index type
     * \ingroup StringClasses
	 */
	template <typename INDEX = uint64_t>
	struct LCPIntervalComparatorInDepthOrder
	{
		/**
		 * @brief Compares two LCP intervals for depth-first sorting
		 * @param x The first LCP interval
		 * @param y The second LCP interval
		 * @return true if \p x should come before \py in depth-first order
		 */
		bool operator()(const LCPInterval<INDEX> &x, const LCPInterval<INDEX> &y)
		{
			if (x.lcp == y.lcp)
			{
				if (x.i == y.i)
				{
					return x.j < y.j;
				}
				else
				{
					return x.i < y.i;
				}
			}
			else
			{
				return x.lcp < y.lcp;
			}
		}
	};
}