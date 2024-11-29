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

#include "./print.hpp"

namespace stool
{
	class UInt8VectorGenerator
	{
	public:
		static std::vector<uint8_t> create_ab_alphabet()
		{
			return std::vector<uint8_t>{'a', 'b'};
		}
		static std::vector<uint8_t> create_abcdefgh_alphabet()
		{
			return std::vector<uint8_t>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
		}
		static std::vector<uint8_t> create_ACGT_alphabet()
		{
			return std::vector<uint8_t>{'A', 'C', 'G', 'T'};
		}
		static std::vector<uint8_t> create_AtoZ_alphabet()
		{
			return std::vector<uint8_t>{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
		}
		static std::vector<uint8_t> create_full_alphabet()
		{
			std::vector<uint8_t> r;
			for(uint64_t i = 2; i < 250;i++){
				r.push_back(i);
			}
			return r;
		}

		static std::vector<uint8_t> create_alphabet(uint64_t alphabet_type)
		{
			if (alphabet_type == 0)
			{
				return create_ab_alphabet();
			}
			else if (alphabet_type == 1)
			{
				return create_ACGT_alphabet();
			}
			else if(alphabet_type == 2)
			{
				return create_abcdefgh_alphabet();
			}else if(alphabet_type == 3){
				return create_AtoZ_alphabet();
			}else{
				return create_full_alphabet();
			}
		}
		static std::vector<uint8_t> create_random_sequence(uint64_t len, const std::vector<uint8_t> &alphabet, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<> rand100(0, alphabet.size() - 1);
			std::vector<uint8_t> r;

			for (size_t i = 0; i < len; i++)
			{
				r.push_back(alphabet[rand100(mt)]);
			}
			return r;
		}
		static std::vector<uint8_t> create_random_substring(const std::vector<uint8_t> &text, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<> rand100(0, text.size() - 1);
			uint64_t len = rand100(mt);
			return create_random_substring(text, len, seed);
		}
		static std::vector<uint8_t> create_random_substring(const std::vector<uint8_t> &text, uint64_t len, int64_t seed)
		{
			if (len > text.size())
			{
				throw std::logic_error("Error:create_random_substring");
			}
			else
			{
				std::mt19937 mt(seed);
				std::uniform_int_distribution<> rand100(0, text.size() - len - 1);
				uint64_t pos = rand100(mt);
				std::vector<uint8_t> r;
				for (uint64_t i = pos; i < pos + len; i++)
				{
					r.push_back(text[i]);
				}
				return r;
			}
		}
	};

} // namespace stool