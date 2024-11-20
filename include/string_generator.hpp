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
			else
			{
				return create_abcdefgh_alphabet();
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

	class StringGenerator
	{
	public:
		static std::vector<uint32_t> create_random_sequence(uint64_t len, uint64_t alphabet_size, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<> rand100(0, alphabet_size - 1);
			std::vector<uint32_t> r;

			for (size_t i = 0; i < len; i++)
			{
				r.push_back(rand100(mt));
			}
			return r;
		}



		static std::vector<uint32_t> create_random_sequence(uint64_t len, uint64_t alphabet_size)
		{
			std::random_device rnd;
			int64_t p = rnd();
			return create_random_sequence(len, alphabet_size, p);
		}
		
		static std::vector<uint8_t> create_uint8_t_string(std::vector<uint32_t> &original_text)
		{
			std::vector<uint8_t> chars = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
			std::vector<uint8_t> r;
			r.resize(original_text.size(), 1);

			for (size_t i = 0; i < original_text.size(); i++)
			{
				uint64_t ith = original_text[i] % 8;
				assert(ith < chars.size());
				r[i] = chars[ith];
				assert(r[i] != 0);
			}
			return r;
		}
		

		/*
		static std::vector<uint8_t> create_uint8_t_binary_string(std::vector<uint32_t> &original_text)
		{
			std::vector<uint8_t> r;

			for (size_t i = 0; i < original_text.size(); i++)
			{
				r.push_back(original_text[i] % 2 ? 'a' : 'b');
			}
			return r;
		}
		*/
		static std::vector<uint8_t> create_uint8_t_binary_string(uint64_t len, int64_t seed)
		{
			std::vector<uint32_t> seed_seq = StringGenerator::create_random_sequence(len, 2, seed);
			return create_uint8_t_string(seed_seq);
		}
		static std::vector<uint8_t> create_uint8_t_binary_string(uint64_t len)
		{
			std::vector<uint32_t> seed_seq = StringGenerator::create_random_sequence(len, 2);
			return create_uint8_t_string(seed_seq);
		}
		static std::vector<uint8_t> create_uint8_t_binary_string_of_random_length(uint64_t max_len)
		{
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<> rand100(0, max_len);
			uint64_t len = rand100(mt);
			return create_uint8_t_binary_string(len);
		}

		static std::vector<uint8_t> create_uint8_t_8_ary_string(uint64_t len, int64_t seed)
		{
			std::vector<uint32_t> seed_seq = StringGenerator::create_random_sequence(len, 8, seed);
			return create_uint8_t_string(seed_seq);
		}
		static std::vector<uint8_t> create_uint8_t_8_ary_string(uint64_t len)
		{
			std::vector<uint32_t> seed_seq = StringGenerator::create_random_sequence(len, 8);
			return create_uint8_t_string(seed_seq);
		}
		static std::vector<uint8_t> create_uint8_t_8_ary_string_of_random_length(uint64_t max_len)
		{
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<> rand100(0, max_len);
			uint64_t len = rand100(mt);
			return create_uint8_t_8_ary_string(len);
		}
	};

} // namespace stool