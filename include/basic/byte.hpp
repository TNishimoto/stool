#pragma once
#include <iostream>
#include <bitset>
#include <array>
#include <cmath>

namespace stool
{
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
		inline static int64_t numofbits5(long bits)
		{
			bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
			bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
			bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
			bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
			return (bits & 0x0000ffff) + (bits >> 16 & 0x0000ffff);
		}

	public:
		inline static int64_t countBits(uint64_t bits)
		{
			return __builtin_popcountll(bits);
		}
		/*
		inline static int64_t numberOfLeadingZero(int64_t x)
		{
			return numofbits5((~x) & (x - 1));
		}
		*/
		inline static bool getBit(int64_t x, int64_t nth)
		{
			return ((x >> nth) & 0x00000001) > 0;
		}
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

		/*
		 * Return the position of the (i+1)-th 1 in the given bits if such bit exists; otherwise return -1.
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
				uint64_t c = countBits(v);
				if (counter + c >= nth)
				{

					for (uint64_t j = 0; j < 8; j++)
					{
						if (getBit(bits, bs + j))
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

		/*
		 * Return the position of the (i+1)-th 0 in the given bits if such bit exists; otherwise return -1.
		 */
		inline static int64_t select0(uint64_t bits, uint64_t i)
		{
			return select1(~bits, i);
		}

		static uint64_t zero_pad_tail(uint64_t code, uint8_t len)
		{
			uint64_t mask = (len < 64) ? ~((1ULL << len) - 1) : 0;
			uint64_t pref = code & mask;
			return pref;
		}
		static uint64_t zero_pad_head(uint64_t code, uint8_t len)
		{
			uint64_t mask = len < 64 ? UINT64_MAX >> len : 0;
			uint64_t suf = code & mask;
			return suf;
		}

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
		static std::string to_string(uint64_t code)
		{
			std::bitset<64> bits(code);
			std::string bitString = bits.to_string();
			return bitString;
		}
	};

	class Log
	{
	public:
		inline static int64_t logIterate(int64_t n)
		{
			return log2Iterate(n, 0);
		}
		inline static int64_t log2Iterate(int64_t n, int64_t m)
		{
			int64_t r;
			if (n > 1)
			{
				int64_t log = log2(n);
				r = log2Iterate(log, m + 1);
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
		inline static int64_t log2(int64_t n)
		{
			if (n <= 0)
			{
				throw std::out_of_range("Log_{2}(n) error: n <= 0");
			}
			else
			{
				int64_t t = 0;
				long long m = 1;
				while (true)
				{
					long long upper = m * 2;
					if (m <= n && n < upper)
					{
						// int64_t r = (int64_t)(n - m);
						return t;
					}
					if (m == 0)
					{
						throw std::out_of_range("Log2 Error!");
					}
					m *= 2;
					t++;
				}
			}
		}
	};
} // namespace stool