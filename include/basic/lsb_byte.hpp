#pragma once
#include "./byte.hpp"

namespace stool
{
    class LSBByte
    {
        public:
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
        static bool get_bit(int64_t x, int64_t nth)
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
        static uint64_t insert_bit(uint64_t bits, int64_t pos, bool value)
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
        static uint64_t write_bit(uint64_t bits, int64_t pos, bool value)
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
        static uint64_t remove_bit(uint64_t bits, uint64_t pos)
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
        static int64_t select1(uint64_t bits, uint64_t i)
        {
            uint64_t nth = i + 1;
            uint64_t mask = UINT8_MAX;
            uint64_t counter = 0;
            for (uint64_t i = 0; i < 8; i++)
            {
                uint64_t bs = i * 8;
                uint64_t mask2 = mask << bs;
                uint64_t v = bits & mask2;
                uint64_t c = Byte::count_bits(v);
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
        static int64_t select0(uint64_t bits, uint64_t i)
        {
            return select1(~bits, i);
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

    };
}
