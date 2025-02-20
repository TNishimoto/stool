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
	
	/*
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

	template <typename VEC>
	bool equal_check(std::string name, const VEC &collect_vec, const VEC &test_vec)
	{
		if (collect_vec.size() != test_vec.size())
		{
			std::string s = std::string("[" + name + "] String sizes are different!") + ", collect = " + std::to_string(collect_vec.size()) + ", test = " + std::to_string(test_vec.size());

			throw std::logic_error(s);
		}
		for (uint64_t i = 0; i < collect_vec.size(); i++)
		{
			if (collect_vec[i] != test_vec[i])
			{
				std::string msg = "collect_vec[" + std::to_string(i) + "] != test_vec[" + std::to_string(i) + "]";

				throw std::logic_error("[" + name + "] Values are different! " + msg);
			}
		}
		return true;
	}
	template <typename VEC>
	bool equal_check(const VEC &collect_vec, const VEC &test_vec){
		return equal_check("EQUAL_CHECK", collect_vec, test_vec);
	}
	/*
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
	*/

	


} // namespace stool