#pragma once
#include <iostream>
#include <bitset>
#include <array>
#include <cmath>
#include <vector>
#include <algorithm>
namespace stool
{
	/*!
	 * @brief Utility class for byte operations
	 */
	class Byte
	{

	public:
		/*
		inline static int64_t numofbits5(long bits)
			{
				bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
				bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
				bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
				bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
				return (bits & 0x0000ffff) + (bits >> 16 & 0x0000ffff);
			}
			*/

		/*!
		 * @brief Returns the number of zeros to the left of the leftmost 1 in a given 64-bit integer x.
		 * @return The number of trailing zeros, or -1 if x is 0
		 */
		static int number_of_leading_zero(uint64_t x)
		{
			if (x == 0)
			{
				return -1;
			}
			else
			{
				return __builtin_clzll(x);
			}
		}

		/*!
		 * @brief Returns the rank of the leftmost 1 in a given 64-bit integer x (e.g., count_trailing_zeros(1000) = 3).
		 * @return The rank of the leftmost 1, or -1 if x is 0
		 */
		static int count_trailing_zeros(uint64_t x)
		{
			if (x == 0)
			{
				return -1;
			}
			else
			{
				return __builtin_ctzll(x);
			}
		}

		/*!
		 * @brief Counts the number of 1 bits in a 64-bit integer x
		 */
		inline static int64_t popcount(uint64_t x)
		{
			return __builtin_popcountll(x);
		}

		

		/*!
		 * @brief Converts a 64-bit integer x to a binary string
		 */
		static std::string to_bit_string(uint64_t x, bool reverse)
		{

			std::bitset<64> bits(x);
			std::string bitString = bits.to_string();
			if (reverse)
			{
				return bitString;
			}
			else
			{
				std::reverse(bitString.begin(), bitString.end());
				return bitString;
			}
		}

		/*!
		 * @brief Converts a 64-bit vector X to a binary string
		 */
		static std::string to_bit_string(const std::vector<uint64_t> &X, bool use_partition = false)
		{
			std::string s;

			for (uint64_t i = 0; i < X.size(); i++)
			{
				s += to_bit_string(X[i], true);
				if (use_partition && i != X.size() - 1)
				{
					s += "|";
				}
			}
			return s;
		}

		/*!
		 * @brief Converts a boolean vector X to a binary string
		 */
		static std::string to_bit_string(const std::vector<bool> &X, bool use_partition = false)
		{
			std::string s;

			for (uint64_t i = 0; i < X.size(); i++)
			{
				s += X[i] ? '1' : '0';
				if (use_partition && i % 64 == 63)
				{
					s.push_back(' ');
				}
			}

			return s;
		}

		/*!
		 * @brief Prints a 64-bit vector X to a binary string
		 */
		template <typename T>
		static void print(T &X, uint64_t size_of_X)
		{
			for (uint64_t i = 0; i < size_of_X; i++)
			{
				std::cout << to_bit_string(X[i]) << " ";
			}
			std::cout << std::endl;
		}
	};

} // namespace stool