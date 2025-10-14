#pragma once
#include "./lcp_interval_preorder_comp.hpp"

namespace stool
{
	/**
	 * @brief Utility class for sorting LCP intervals [Unchecked AI's Comment] 
	 */
	class LCPIntervalSort
	{
		public: 
		/**
		 * @brief Sorts LCP intervals in preorder
		 * @param items Vector of LCP intervals to sort
		 */
		template <typename INDEX = uint64_t>
		static void sort_in_preorder(std::vector<stool::LCPInterval<INDEX>> &items)
		{
			std::sort(
				items.begin(),
				items.end(),
				stool::LCPIntervalPreorderComp<INDEX>());
		}
	};
}