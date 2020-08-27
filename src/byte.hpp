#include <iostream>
#include <bitset>
#include <array>

namespace stool {
	class Byte {
	private:
		inline static int64_t numofbits4(int64_t bits)
		{
			int64_t num;

			num = (bits >> 1) & 03333333333;
			num = bits - num - ((num >> 1) & 03333333333);
			num = ((num + (num >> 3)) & 0707070707) % 077;

			return num;
		}
		inline static int64_t numofbits5(long bits)
		{
			bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
			bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
			bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
			bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
			return (bits & 0x0000ffff) + (bits >> 16 & 0x0000ffff);
		}
	public:

		inline static int64_t countBits(int64_t bits) {
			return numofbits4(bits);
		}
		inline static int64_t numberOfLeadingZero(int64_t x) {
			return numofbits5((~x) & (x - 1));
		}
		inline static bool getBit(int64_t x, int64_t nth) {
			return ((x >> nth) & 0x00000001) > 0;
		}

	};

	class Log {
	public:
		const std::array<long long, 10> logArr{ 0, 
			(long long)std::pow(2,1), (long long)std::pow(2,2), (long long)std::pow(2,3), (long long)std::pow(2,4), (long long)std::pow(2,5), 
			(long long)std::pow(2,6), (long long)std::pow(2,7), (long long)std::pow(2,8), (long long)std::pow(2,9) };
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
				throw - 1;
			}
			return r;
		}
		inline static int64_t log2(int64_t n)
		{
			if (n <= 0)
			{
				throw - 1;
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
						//int64_t r = (int64_t)(n - m);
						return t;
					}
					if (m == 0) throw - 1;
					m *= 2;
					t++;
				}
			}
		}
	};
}