#pragma once
#include <iostream>
#include <bitset>
#include <array>
#include <cmath>

#include "./lsb_byte.hpp"


namespace stool
{

	/*!
	* @brief Class for calculating logarithmic values
	*
	* This class provides static methods for calculating logarithmic values,
	* particularly logarithms base 2. It includes iterative methods for
	* calculating repeated logarithms (log log ... log) and basic logarithm
	* calculations. The class handles error cases by throwing exceptions for
	* invalid inputs such as non-positive numbers.
	*
	* The class contains three main methods:
	* - log_iterate: Calculates repeated logarithm (number of times log2 can be applied)
	* - log2_iterate: Helper method for logIterate that recursively applies log2
	* - log2: Calculates the floor of the binary logarithm using iterative doubling
	*/

	class Log
	{
	public:

		/*!
		* @brief Calculates the number of times log2 can be applied to a number
		*
		* This method determines how many times the logarithm base 2 can be iteratively
		* applied to a given number before reaching 1 or less. For example, if n=16:
		* log2(16)=4, log2(4)=2, log2(2)=1, so the result would be 3 iterations.
		* If the input is less than or equal to 0, it throws an exception.
		*
		* @param n The input number to calculate iterations for
		* @return The number of times log2 can be applied before reaching 1
		* @throws std::out_of_range if n <= 0
		*/
		inline static int64_t log_iterate(int64_t n)
		{
			return log2_iterate(n, 0);
		}
		/*!
		* @brief Helper method for log_iterate that recursively applies log2
		*
		* This method recursively calculates how many times log2 can be applied to a number
		* before reaching 1 or less. It maintains a counter parameter m to track the number
		* of iterations. The method handles the base cases where n=1 (returns current count)
		* or n<=0 (throws exception), and recursively applies log2 in other cases.
		*
		* @param n The current number in the iteration
		* @param m The current count of iterations performed
		* @return The total number of possible log2 iterations
		* @throws -1 if n <= 0
		*/
		inline static int64_t log2_iterate(int64_t n, int64_t m)
		{
			int64_t r;
			if (n > 1)
			{
				int64_t log = log2(n);
				r = log2_iterate(log, m + 1);
			}
			else if (n == 1)
			{
				r = m;
			}
			else
			{
				r = -1;
				throw -1;
			}
			return r;
		}

		/*!
		* @brief Calculates the base-2 logarithm of an integer
		*
		* This method computes the floor of the base-2 logarithm of a given positive integer.
		* For example, log2(8)=3, log2(7)=2. The method uses a binary search approach by 
		* comparing the input against powers of 2 to find the largest power that is less 
		* than or equal to the input.
		*
		* @param n The positive integer to calculate the logarithm for
		* @return The floor of the base-2 logarithm of n
		* @throws std::out_of_range if n <= 0
		*/
		inline static int64_t log2floor(int64_t n)
		{
			if (n <= 0)
			{
				throw std::out_of_range("Log_{2}(n) error: n <= 0");
			}
			else
			{
				uint64_t b = LSBByte::get_code_length(n);
				return b-1;
			}

		}

		inline static int64_t log2ceil(int64_t n)
		{

			if (n <= 0)
			{
				throw std::out_of_range("Log_{2}(n) error: n <= 0, n = " + std::to_string(n));
			}
			else
			{
				uint64_t b = LSBByte::get_code_length(n);
				uint64_t m = 1 << (b-1);
				if(n == (int64_t)m){
					return b-1;
				}else{
					return b;
				}
			}
		}

	};
} // namespace stool