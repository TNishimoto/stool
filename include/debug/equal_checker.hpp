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

#include "./print.hpp"

namespace stool
{

	/**
	 * @brief A utility class for comparing vectors and containers for equality
	 * 
	 * The EqualChecker class provides static methods to compare two vectors or containers
	 * for equality. It performs element-by-element comparison and throws descriptive
	 * exceptions when differences are found. This class is particularly useful for
	 * debugging and testing purposes.
	 * 
	 * @tparam VEC The type of container to compare (must support size() and operator[])
	 */
	class EqualChecker
	{
	public:
		/**
		 * @brief Compares two vectors for equality and throws an exception if they differ
		 *
		 * This function performs an element-by-element comparison of two vectors.
		 * If the vectors have different sizes or contain different elements at any position,
		 * it throws a std::logic_error with a descriptive message.
		 *
		 * @tparam VEC The type of the vectors to compare (must support size() and operator[])
		 * @param collect_vec The first vector to compare (reference vector)
		 * @param test_vec The second vector to compare (test vector)
		 * @param name Optional name for the check, used in error messages (default: "EQUAL_CHECK")
		 * @return true if the vectors are equal
		 * @throws std::logic_error if vectors have different sizes or contain different elements
		 *
		 * @note The function assumes that VEC supports size() method and operator[] for indexing
		 */
		template <typename VEC>
		static bool equal_check(const VEC &collect_vec, const VEC &test_vec, std::string name = "EQUAL_CHECK")
		{
			if (collect_vec.size() != test_vec.size())
			{
				std::string s = std::string("[" + name + "] String sizes are different!") + ", collect = " + std::to_string(collect_vec.size()) + ", test = " + std::to_string(test_vec.size());

				throw std::logic_error(s);
			}
			for (uint64_t i = 0; i < collect_vec.size(); i++)
			{
				if (collect_vec[i] != test_vec[i])
				{
					std::string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

					throw std::logic_error("[" + name + "] Values are different! " + msg);
				}
			}
			return true;
		}

	};

} // namespace stool