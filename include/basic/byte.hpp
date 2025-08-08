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
	 *
	 * This class provides static methods for low-level byte manipulations
	 * including bit operations, bit counting, and bit insertion operations.
	 * All methods are implemented as static functions for efficient bit-level
	 * processing of integer values.
	 */
	class Byte
	{
	private:
		/*
		inline static int64_t numofbits4(int64_t bits)
		{
			int64_t num;

			num = (bits >> 1) & 03333333333;
			num = bits - num - ((num >> 1) & 03333333333);
			num = ((num + (num >> 3)) & 0707070707) % 077;

			return num;
		}
		*/

	public:
		/*!
		 * @brief Counts the number of 1 bits in a 32-bit integer using bit manipulation
		 *
		 * This method implements a bit counting algorithm that uses bit manipulation
		 * operations to count the number of set bits (1s) in a 32-bit integer.
		 * It uses a series of masking and shifting operations to count bits in parallel.
		 * The algorithm processes bits in groups of 2, then 4, then 8, and finally 16
		 * to efficiently compute the total number of 1 bits.
		 *
		 * @param bits The 32-bit integer value to count bits in
		 * @return The total number of 1 bits in the input value
		 */
		inline static int64_t numofbits5(long bits)
		{
			bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
			bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
			bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
			bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
			return (bits & 0x0000ffff) + (bits >> 16 & 0x0000ffff);
		}

		/*!
		 * @brief Counts the number of trailing zeros in a 64-bit integer
		 *
		 * This method determines the number of trailing zeros (0 bits) at the least significant end
		 * of a 64-bit unsigned integer using the built-in __builtin_ctzll function. For a non-zero input,
		 * it returns the position of the least significant 1 bit. For an input of 0, it returns -1.
		 *
		 * @param x The 64-bit unsigned integer to count trailing zeros in
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
		 * @brief Counts the number of 1 bits in a 64-bit integer using built-in function
		 *
		 * This method uses the built-in __builtin_popcountll function to efficiently count
		 * the number of set bits (1s) in a 64-bit integer. The built-in function is typically
		 * implemented using hardware-specific instructions for optimal performance.
		 *
		 * @param bits The 64-bit integer value to count bits in
		 * @return The total number of 1 bits in the input value
		 */
		inline static int64_t count_bits(uint64_t bits)
		{
			return __builtin_popcountll(bits);
		}

		static uint64_t zero_pad_tail(uint64_t code, uint8_t len)
		{
			uint64_t mask = (len < 64) ? ~((1ULL << len) - 1) : 0;
			uint64_t pref = code & mask;
			return pref;
		}
		/*!
		 * @brief Pads zeros at the head of a 64-bit integer
		 *
		 * This method takes a 64-bit integer and a length parameter, and sets all bits
		 * before the specified length to zero, preserving only the rightmost 'len' bits.
		 * It uses bit manipulation to create a mask that clears (sets to 0) all bits
		 * before the specified length position.
		 *
		 * @param code The 64-bit integer to be padded
		 * @param len The number of rightmost bits to preserve (0-64)
		 * @return The modified 64-bit integer with zeros padded at the head
		 */
		static uint64_t zero_pad_head(uint64_t code, uint8_t len)
		{
			uint64_t mask = len < 64 ? UINT64_MAX >> len : 0;
			uint64_t suf = code & mask;
			return suf;
		}

		/*!
		 * @brief Converts a 64-bit integer code to a binary string representation
		 *
		 * This method takes a 64-bit unsigned integer and converts it to a string
		 * containing its binary representation. It uses std::bitset to perform the
		 * conversion, resulting in a 64-character string of '0' and '1' characters,
		 * with the most significant bits on the left.
		 *
		 * @param code The 64-bit integer code to convert to string
		 * @return A string containing the binary representation of the code
		 */
		static std::string to_string(uint64_t code, bool reverse = true)
		{

			std::bitset<64> bits(code);
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

		static std::string to_string(const std::vector<uint64_t> &bits, bool use_partition = false)
		{
			std::string s;

			for (uint64_t i = 0; i < bits.size(); i++)
			{
				s += to_string(bits[i], true);
				if (use_partition && i != bits.size() - 1)
				{
					s += "|";
				}
			}
			return s;
		}

		static std::string to_string(const std::vector<bool> &bits, bool use_partition = false)
		{
			std::string s;

			for (uint64_t i = 0; i < bits.size(); i++)
			{
				s += bits[i] ? '1' : '0';
				if (use_partition && i % 64 == 63)
				{
					s.push_back(' ');
				}
			}

			return s;
		}

		static std::string to_bit_string(uint64_t x){
			std::bitset<64> b(x);
			return b.to_string();
		}
		static std::string to_bit_string(const std::vector<uint64_t> &bits){
			std::string s = "";
			for(uint64_t i = 0; i < bits.size(); i++){
				s += to_bit_string(bits[i]);
				if(i + 1 < bits.size()){
					s += " ";
				}
			}
			return s;
		}
        template <typename T>
        static void print(T &bits, uint64_t array_size){
			for(uint64_t i = 0; i < array_size; i++){
				std::cout << to_bit_string(bits[i]) << " ";
			}
			std::cout << std::endl;
        }

	};

} // namespace stool