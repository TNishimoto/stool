#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <exception>
#include <algorithm>
#include <fstream>
#include <cassert>

#include "./debug_printer.hpp"

namespace stool
{

	/**
	 * @brief A utility class for comparing containers for equality
	 */
	class EqualChecker
	{
	public:
		/**
		 * @brief Check if two given arrays have the same elements or not
		 *
		 * @tparam ARRAY The type of the arrays to compare (must support size() and operator[])
		 * @param collect_array The first array to compare (reference array)
		 * @param test_array The second array to compare (test array)
		 * @param name Optional name for the check, used in error messages (default: "EQUAL_CHECK")
		 * @return true if the arrays are equal
		 * @throws std::logic_error if arrays have different sizes or contain different elements
		 *
		 * @note The function assumes that ARRAY supports size() method and operator[] for indexing
		 */
		template <typename ARRAY>
		static bool equal_check(const ARRAY &collect_array, const ARRAY &test_array, std::string name = "EQUAL_CHECK")
		{
			if (collect_array.size() != test_array.size())
			{
				std::string s = std::string("[" + name + "] Array sizes are different!") + ", collect = " + std::to_string(collect_array.size()) + ", test = " + std::to_string(test_array.size());

				throw std::logic_error(s);
			}
			for (uint64_t i = 0; i < collect_array.size(); i++)
			{
				if (collect_array[i] != test_array[i])
				{
					std::string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

					throw std::logic_error("[" + name + "] Values are different! " + msg);
				}
			}
			return true;
		}

	};

} // namespace stool