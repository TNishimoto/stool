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

	/*
	std::string CreateRandomString(int len, int alphabetSize)
	{
		std::vector<char> alphabets;
		alphabets.push_back('a');
		alphabets.push_back('b');
		alphabets.push_back('c');
		alphabets.push_back('d');
		alphabets.push_back('e');

		std::random_device rnd;
		std::mt19937 mt(rnd());
		std::uniform_int_distribution<> rand100(0, alphabetSize - 1);

		std::string r;
		for (int i = 0; i < len; i++)
		{
			r.append(1, alphabets[rand100(mt)]);
		}
		return r;
	}
	std::vector<uint64_t> CreateRandomInteger(int len, int max)
	{

		std::random_device rnd;
		std::mt19937 mt(rnd());
		std::uniform_int_distribution<> rand100(0, max - 1);

		std::vector<uint64_t> r;

		for (int i = 0; i < len; i++)
		{
			auto value = rand100(mt);
			r.push_back(value);
		}
		return r;
	}
	template <typename T>
	std::vector<T> create_deterministic_integers(uint64_t len, int64_t max, int64_t min, int64_t seed)
	{
		std::mt19937 mt(seed);
		std::uniform_int_distribution<> rand100(0, (max - min) - 1);

		std::vector<T> r;

		for (size_t i = 0; i < len; i++)
		{
			int64_t value = min + rand100(mt);
			r.push_back(value);
		}
		return r;
	}
	*/
	/*
	template <typename T>
	bool equal_check(const std::vector<T> &vec1, const std::vector<T> &vec2)
	{
		if (vec1.size() != vec2.size())
		{
			std::string s = std::string("String sizes are different!") + ", collect = " + std::to_string(vec1.size()) + ", test = " + std::to_string(vec2.size());

			throw std::logic_error(s);
		}
		for (uint64_t i = 0; i < vec1.size(); i++)
		{
			if (vec1[i] != vec2[i])
			{
				std::string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

				throw std::logic_error("Values are different! " + msg);
			}
		}
		return true;
	}
	*/
	template <typename T>
	bool equal_check(const std::vector<T> &vec1, const std::vector<T> &vec2)
	{
		if (vec1.size() != vec2.size())
		{
			std::string s = std::string("String sizes are different!") + ", collect = " + std::to_string(vec1.size()) + ", test = " + std::to_string(vec2.size());

			throw std::logic_error(s);
		}
		for (uint64_t i = 0; i < vec1.size(); i++)
		{
			if (vec1[i] != vec2[i])
			{
				std::string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

				throw std::logic_error("Values are different! " + msg);
			}
		}
		return true;
	}

	template <typename T>
	bool equal_check(const std::deque<T> &deq1, const std::deque<T> &deq2)
	{
		if (deq1.size() != deq2.size())
		{
			std::string s = std::string("String sizes are different!") + ", collect = " + std::to_string(deq1.size()) + ", test = " + std::to_string(deq2.size());

			throw std::logic_error(s);
		}
		for (uint64_t i = 0; i < deq1.size(); i++)
		{
			if (deq1[i] != deq2[i])
			{
				std::string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

				throw std::logic_error("Values are different! " + msg);
			}
		}
		return true;
	}

	template <typename T>
	bool equal_check(std::string name, const std::vector<T> &vec1, const std::vector<T> &vec2)
	{
		if (vec1.size() != vec2.size())
		{
			std::string s = std::string("[" + name + "] String sizes are different!") + ", collect = " + std::to_string(vec1.size()) + ", test = " + std::to_string(vec2.size());

			throw std::logic_error(s);
		}
		for (uint64_t i = 0; i < vec1.size(); i++)
		{
			if (vec1[i] != vec2[i])
			{
				std::string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

				throw std::logic_error("[" + name + "] Values are different! " + msg);
			}
		}
		return true;
	}

	bool equal_check(const std::string &vec1, const std::string &vec2)
	{
		if (vec1.size() != vec2.size())
		{
			std::string s = std::string("String sizes are different!") + ", collect = " + std::to_string(vec1.size()) + ", test = " + std::to_string(vec2.size());
			throw std::logic_error(s);
		}

		for (uint64_t i = 0; i < vec1.size(); i++)
		{
			if (vec1[i] != vec2[i])
			{
				throw std::logic_error("Values are different!");
			}
		}
		return true;
	}

	template <typename CHAR = uint8_t>
	bool compare_suffixes(const std::vector<CHAR> &text, const uint64_t x, const uint64_t y)
	{
		uint64_t max = x < y ? text.size() - y : text.size() - x;
		for (uint64_t i = 0; i < max; i++)
		{
			CHAR c1 = text[x + i];
			CHAR c2 = text[y + i];
			if (c1 != c2)
			{
				return c1 < c2;
			}
		}
		return x > y;
	}

	template <typename CHAR = uint8_t, typename INDEX = uint64_t>
	std::vector<INDEX> construct_naive_SA(const std::vector<CHAR> &text)
	{
		std::vector<INDEX> r;
		for (uint64_t i = 0; i < text.size(); i++)
		{
			r.push_back(i);
		}

		std::sort(
			r.begin(),
			r.end(),
			[&](const uint64_t &x, const uint64_t &y)
			{
				return compare_suffixes(text, x, y);
			});
		return r;
	}

	class StringGenerator
	{
	public:
		static std::vector<uint8_t> create_random_uint8_t_sequence(uint64_t len, std::vector<uint8_t> &alphabet, int64_t seed)
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