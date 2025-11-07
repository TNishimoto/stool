#pragma once
#include <iostream>
#include <bitset>
#include <array>
#include <cmath>

#include "./lsb_byte.hpp"


namespace stool
{

	/*!
	* @brief A utility class for logarithmic functions
     * \ingroup BasicClasses
	*/

	class Log
	{
	public:

		/*!
		* @brief Compute log^{*}_{2}(n), where n >= 1
		*/
		inline static int64_t log_iterate(int64_t n)
		{
			return log2_iterate(n, 0);
		}
		/*!
		* @brief Helper method for log^{*}_{2}(n)
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
		* @brief Computes ⌊ log_{2}(n) ⌋.
		*/
		inline static int64_t log2_floor(int64_t n)
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

		/*!
		* @brief Computes ⌈ log_{2}(n) ⌉.
		*/
		inline static int64_t log2_ceil(int64_t n)
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