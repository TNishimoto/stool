#pragma once
#include <iostream>
#include <bitset>
#include <array>
#include <cmath>

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

	static int number_of_leading_zero(uint64_t x) {
		if(x == 0){
			return -1;
		}else{
			return __builtin_ctzll(x);
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
		/*
		inline static int64_t numberOfLeadingZero(int64_t x)
		{
			return numofbits5((~x) & (x - 1));
		}
		*/
		/*! 
		* @brief Checks if a specific bit is set in a 64-bit integer
		* 
		* This method examines a specific bit position in a 64-bit integer value
		* and returns true if that bit is set (1) or false if it is unset (0).
		* It uses bit shifting and masking operations to isolate and check the
		* desired bit.
		*
		* @param x The 64-bit integer value to check
		* @param nth The position of the bit to check (0-63)
		* @return true if the bit at the specified position is 1, false otherwise
		*/
		inline static bool get_bit(int64_t x, int64_t nth)
		{
			return ((x >> nth) & 0x00000001) > 0;
		}
		/*! 
		* @brief Inserts a bit at a specified position in a 64-bit integer
		* 
		* This method inserts a new bit at the specified position in a 64-bit integer,
		* shifting the existing bits to accommodate the insertion. The bits to the left
		* of the insertion point are shifted left by one position, while the bits to
		* the right remain unchanged. The new bit is placed at the specified position.
		*
		* @param bits The original 64-bit integer value
		* @param pos The position where the new bit should be inserted (0-63)
		* @param value The boolean value to insert (true for 1, false for 0)
		* @return The modified 64-bit integer with the new bit inserted
		*/
		inline static uint64_t insert_bit(uint64_t bits, int64_t pos, bool value)
		{
			if (pos == 0)
			{
				return (bits << 1) | (value ? 1ULL : 0ULL);
			}
			else if (pos == 63)
			{
				return (bits >> 1) | ((value ? 1ULL : 0ULL) << 63);
			}
			else
			{
				uint64_t left = (bits >> (pos)) << (pos + 1);
				uint64_t right = (bits << (64 - pos)) >> (64 - pos);
				uint64_t center = (value ? 1ULL : 0ULL) << pos;

				return left | center | right;
			}
		}

		/*! 
		* @brief Writes a bit at a specified position in a 64-bit integer
		* 
		* This method overwrites a bit at the specified position in a 64-bit integer
		* with a new value. Unlike insert_bit which shifts bits to make room for insertion,
		* this method simply replaces the existing bit at the given position with the new value.
		* The other bits in the integer remain unchanged.
		*
		* @param bits The original 64-bit integer value
		* @param pos The position where the bit should be written (0-63)
		* @param value The boolean value to write (true for 1, false for 0)
		* @return The modified 64-bit integer with the new bit written
		*/
		inline static uint64_t write_bit(uint64_t bits, int64_t pos, bool value)
		{
			if (pos == 0)
			{
				return ((bits >> 1) << 1) | (value ? 1ULL : 0ULL);
			}
			else if (pos == 63)
			{
				return ((bits << 1) >> 1) | ((value ? 1ULL : 0ULL) << 63);
			}
			else
			{
				uint64_t left = (bits >> (pos + 1)) << (pos + 1);
				uint64_t right = (bits << (64 - pos)) >> (64 - pos);
				uint64_t center = (value ? 1ULL : 0ULL) << pos;
				return left | center | right;
			}
		}

		/*! 
		* @brief Removes a bit at a specified position and shifts bits to fill the gap
		* 
		* This method removes a bit at the specified position in a 64-bit integer
		* and shifts all bits after that position one position to the left to fill
		* the gap. The rightmost bit will be set to 0.
		*
		* @param bits The original 64-bit integer value
		* @param pos The position of the bit to remove (0-63)
		* @return The modified 64-bit integer with the bit removed and shifted
		*/
		inline static uint64_t remove_bit(uint64_t bits, uint64_t pos)
		{
			if (pos == 0)
			{
				return bits >> 1;
			}
			else if (pos == 63)
			{
				return (bits << 1) >> 1;
			}
			else
			{
				uint64_t left = (bits >> (pos + 1)) << pos;
				uint64_t right = (bits << (64 - pos)) >> (64 - pos);
				return left | right;
			}
		}


		/*!
		* @brief Finds the position of the (i+1)-th set bit (1) in a 64-bit integer
		*
		* Return the position of the (i+1)-th 1 in the given bits if such bit exists; otherwise return -1.
		* @param bits The 64-bit integer to search
		* @param i The zero-based index of the set bit to find (finds i+1 th occurrence)
		* @return The position (0-63) of the (i+1)-th set bit, or -1 if not found
		*/
		inline static int64_t select1(uint64_t bits, uint64_t i)
		{
			uint64_t nth = i + 1;
			uint64_t mask = UINT8_MAX;
			uint64_t counter = 0;
			for (uint64_t i = 0; i < 8; i++)
			{
				uint64_t bs = i * 8;
				uint64_t mask2 = mask << bs;
				uint64_t v = bits & mask2;
				uint64_t c = count_bits(v);
				if (counter + c >= nth)
				{

					for (uint64_t j = 0; j < 8; j++)
					{
						if (get_bit(bits, bs + j))
						{
							counter++;
							if (counter == nth)
							{
								return bs + j;
							}
						}
					}
					return -1;
				}
				counter += c;
			}
			return -1;
		}


		/*!
		* @brief Finds the position of the (i+1)-th unset bit (0) in a 64-bit integer
		*
		* Return the position of the (i+1)-th 0 in the given bits if such bit exists; otherwise return -1.
		* @param bits The 64-bit integer to search
		* @param i The zero-based index of the unset bit to find (finds i+1 th occurrence)
		* @return The position (0-63) of the (i+1)-th unset bit, or -1 if not found
		*/
		inline static int64_t select0(uint64_t bits, uint64_t i)
		{
			return select1(~bits, i);
		}
		/*!
		* @brief Pads zeros at the tail of a 64-bit integer
		*
		* This method takes a 64-bit integer and a length parameter, and sets all bits
		* after the specified length to zero, preserving only the leftmost 'len' bits.
		* It uses bit manipulation to create a mask that clears (sets to 0) all bits
		* after the specified length position.
		*
		* @param code The 64-bit integer to be padded
		* @param len The number of leftmost bits to preserve (0-64)
		* @return The modified 64-bit integer with zeros padded at the tail
		*/

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
		* @brief Pads zeros in a specified range within a 64-bit integer
		*
		* This method takes a 64-bit integer, a starting position, and a length parameter,
		* and sets all bits within the specified range to zero while preserving bits outside
		* the range. It uses bit manipulation to create masks that clear (set to 0) only the
		* bits within the specified range, from pos to pos+len-1.
		*
		* @param code The 64-bit integer to be padded
		* @param pos The starting position of the range to pad with zeros (0-63)
		* @param len The length of the range to pad with zeros
		* @return The modified 64-bit integer with zeros padded in the specified range
		*/

		static uint64_t zero_pad(uint64_t code, uint8_t pos, uint8_t len)
		{
			if (len > 0)
			{

				uint64_t mask1_len = 64 - pos;
				uint64_t mask1 = (mask1_len < 64) ? ~((1ULL << mask1_len) - 1) : 0;
				uint64_t pref = code & mask1;

				uint64_t end_pos = std::min(63, pos + len - 1);
				uint64_t mask2_len = end_pos + 1;
				uint64_t mask2 = mask2_len < 64 ? UINT64_MAX >> mask2_len : 0;
				uint64_t suf = code & mask2;
				return pref | suf;
			}
			else
			{
				return code;
			}
		}

		/*!
		* @brief Gets the length of a code by finding the position of the leftmost 1 bit
		*
		* This method determines the effective length of a 64-bit integer code by finding
		* the position of the leftmost (most significant) 1 bit. It uses the built-in
		* __builtin_clzll function to count leading zeros, then subtracts that from 64
		* to get the position of the first 1 bit. Returns 0 for an input of 0.
		*
		* @param code The 64-bit integer code to measure
		* @return The position of the leftmost 1 bit plus 1, or 0 if code is 0
		*/
		static uint8_t get_code_length(uint64_t code)
		{
			if (code > 0)
			{

				return 64 - __builtin_clzll(code);
			}
			else
			{
				return 0;
			}
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
		static std::string to_string(uint64_t code)
		{
			std::bitset<64> bits(code);
			std::string bitString = bits.to_string();
			return bitString;
		}
	};

} // namespace stool