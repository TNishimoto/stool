#pragma once
#include "./byte.hpp"
#include <cassert>
#if defined(__x86_64__) || defined(_M_X64)
  #if defined(__BMI2__)
    #include <immintrin.h>   // これで _pdep_u64 が宣言される
    // （必要なら）#include <bmi2intrin.h> でもOK
  #endif
#endif

namespace stool
{
    namespace __LSB_BYTE{
        inline static constexpr int8_t get_1bit_position(uint8_t value, int rank) {
            int count = 0;
            for (int bit = 0; bit < 8; ++bit) {
                if (value & (1 << bit)) {
                    if (count == rank) return bit;
                    ++count;
                }
            }
            return -1;
        }
        
        inline static constexpr std::array<std::array<int8_t, 8>, 256> build_lookup_table_for_select1() {
            std::array<std::array<int8_t, 8>, 256> table{};
        
            for (int i = 0; i < 256; ++i) {
                for (int j = 0; j < 8; ++j) {
                    table[i][j] = get_1bit_position(static_cast<uint8_t>(i), j);
                }
            }
        
            return table;
        }

        
        inline static constexpr std::array<std::array<int8_t, 8>, 256> select1_table = stool::__LSB_BYTE::build_lookup_table_for_select1();    
    }

    /*!
     * @brief A class for handling least significant bit (LSB) operations on bytes [Unchecked AI's Comment]
     * 
     * This class provides utility functions for manipulating and analyzing bits
     * in byte-level data, focusing on operations related to the least significant bits.
     * It includes functionality for:
     * - Bit manipulation (getting, setting, inserting bits)
     * - Bit counting and position finding
     * - Lookup table based optimizations for common operations
     * 
     * The class is particularly useful for:
     * - Bit-level data structures
     * - Compression algorithms
     * - Low-level bit manipulation tasks
     * 
     * @note This implementation includes optimized lookup tables and bit manipulation techniques
     *       for improved performance on byte-level operations
     */

    class LSBByte
    {
        public:
        /*!
         * @brief Checks if a specific bit is set in a 64-bit integer [Unchecked AI's Comment]
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
         * @brief Inserts a bit at a specified position in a 64-bit integer [Unchecked AI's Comment]
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
         * @brief Writes a bit at a specified position in a 64-bit integer [Unchecked AI's Comment]
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
         * @brief Removes a bit at a specified position and shifts bits to fill the gap [Unchecked AI's Comment]
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


        static int64_t naive_select_ith_1(uint64_t bits, uint64_t i)
        {
            uint64_t ith = 0;
            for(uint64_t x = 0; x<64;x++){
                bool b = get_bit(bits, x);
                if(b){
                    ith++;
                }
                if(ith == i){
                    return x;
                }
            }
            return -1;
        }
        static int64_t select_ith_1(uint64_t bits, uint64_t i)
        {
            #if defined(__BMI2__)
            

            unsigned cnt = Byte::popcount(bits);
            if (i >= cnt)
                return -1;                                              // 存在しない
            uint64_t src = 1ull << i;                             // r番目の1を表す単一ビット
            uint64_t bit = _pdep_u64(src, bits);                        // xの中の該当位置に配置
            uint64_t _result = static_cast<int>(LSBByte::select1(bit)); // その位置を返す

            return _result;

#else

            // 事前チェック（総1数 < i+1 は失敗）
            // sの最終バイト = 総1数 なので、本当は後段だけで検出できるが、
            // 早期終了しておくと無駄を抑えられる（popcountは1命令）。
            if (i >= static_cast<unsigned>(Byte::popcount(bits)))
                return -1;

            // 1) byte popcount（各バイトに 0..8）
            uint64_t b = bits - ((bits & 0xAAAAAAAAAAAAAAAAULL) >> 1);
            b = (b & 0x3333333333333333ULL) + ((b >> 2) & 0x3333333333333333ULL);
            b = (b + (b >> 4)) & 0x0F0F0F0F0F0F0F0FULL;

            // 2) byte prefix sums
            constexpr uint64_t L8 = 0x0101010101010101ULL;
            uint64_t s = b * L8; // 各バイト: 先頭からの累積個数

            // 3) 最初に累積 >= i+1 となるバイトを検出
            constexpr uint64_t H8 = 0x8080808080808080ULL;
            uint64_t rank = static_cast<uint64_t>(i + 1) * L8;
            uint64_t mask = ((s | H8) - rank) & H8; // 各バイトのMSBが (s_byte >= rank_byte) なら1

            // 万一該当なし（理論上ここには来ない）は -1
            if (mask == 0){
                assert(naive_select_ith_1(bits, i) == -1);
                return -1;
            }

            int byte_index = __builtin_ctzll(mask) / 8;

            // 4) バイト内ランク r とバイト値
            uint64_t sb = (byte_index == 0) ? 0 : ((s >> ((byte_index - 1) * 8)) & 0xFF);
            unsigned r = i - static_cast<unsigned>(sb);
            uint8_t byte = static_cast<uint8_t>(bits >> (byte_index * 8));

            // 5) バイト内 select（テーブル）
            int bit_in_byte = __LSB_BYTE::select1_table[byte][r];
            int64_t result = static_cast<int64_t>(byte_index * 8 + bit_in_byte);

            assert(result == naive_select_ith_1(bits, i));
            return result;
#endif
        }



        static int64_t select1(uint64_t bits)
        {
            if(bits != 0){
                return __builtin_ffsll(bits) - 1;
            }else{
                return -1;
            }
        }


        /*!
         * @brief Finds the position of the (i+1)-th unset bit (0) in a 64-bit integer [Unchecked AI's Comment]
         *
         * Return the position of the (i+1)-th 0 in the given bits if such bit exists; otherwise return -1.
         * @param bits The 64-bit integer to search
         * @param i The zero-based index of the unset bit to find (finds i+1 th occurrence)
         * @return The position (0-63) of the (i+1)-th unset bit, or -1 if not found
         */
        static int64_t select_ith_0(uint64_t bits, uint64_t i)
        {
            return select_ith_1(~bits, i);
        }

         /*!
		* @brief Pads zeros in a specified range within a 64-bit integer [Unchecked AI's Comment]
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
		* @brief Gets the length of a code by finding the position of the leftmost 1 bit [Unchecked AI's Comment]
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
    static std::string to_bit_string(uint64_t x, uint64_t bit_size)
    {
        std::string s = Byte::to_bit_string(x);
        if(bit_size == 0){
            return "";
        }else{
            return s.substr(s.size() - bit_size, bit_size);
        }
    }


    };
}
