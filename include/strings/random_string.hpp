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

#include "../debug/debug_printer.hpp"

namespace stool
{
	/**
	 * @brief A utility class for generating random strings
	 */
	class RandomString
	{
	public:
		/**
		 * @brief Generates a random sequence of length \p d using the given alphabet \p U
		 */
		template <typename CHAR = uint8_t>
		static std::vector<CHAR> create_random_sequence(uint64_t d, const std::vector<CHAR> &U, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<> rand100(0, U.size() - 1);
			std::vector<CHAR> r;

			for (size_t i = 0; i < d; i++)
			{
				r.push_back(U[rand100(mt)]);
			}
			return r;
		}

		/**
		 * @brief Generates a random sequence of length \p d using the given alphabet [0, sigma-1]
		 */
		template <typename CHAR = uint8_t>
		static std::vector<CHAR> create_random_sequence(uint64_t d, uint64_t sigma, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<> rand100(0, sigma - 1);
			std::vector<CHAR> r;

			for (size_t i = 0; i < d; i++)
			{
				r.push_back((CHAR)rand100(mt));
			}
			return r;
		}

		/**
		 * @brief Generates a random bit vector of length \p d
		 */
		static std::vector<bool> create_random_bit_sequence(uint64_t d, int64_t seed)
		{
			return create_random_sequence<bool>(d, 2, seed);
		}

		/**
		 * @brief Creates a random substring from the given text \p T with random length
		 */
		template <typename CHAR = uint8_t>
		static std::vector<CHAR> create_random_substring(const std::vector<CHAR> &T, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<> rand100(0, T.size() - 1);
			uint64_t len = rand100(mt);
			return create_random_substring(T, len, seed);
		}

		/**
		 * @brief Creates a random substring of length \p d from the given text \p T with specified length
		 * @throws std::logic_error If the requested length is greater than the text length
		 */
		template <typename CHAR = uint8_t>
		static std::vector<CHAR> create_random_substring(const std::vector<CHAR> &T, uint64_t d, int64_t seed)
		{
			if (d > T.size())
			{
				throw std::logic_error("Error:create_random_substring");
			}
			else
			{
				std::mt19937 mt(seed);
				std::uniform_int_distribution<> rand100(0, T.size() - d - 1);
				uint64_t pos = rand100(mt);
				std::vector<uint8_t> r;
				for (uint64_t i = pos; i < pos + d; i++)
				{
					r.push_back(T[i]);
				}
				return r;
			}
		}
	};

} // namespace stool