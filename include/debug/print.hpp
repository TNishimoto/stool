#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <exception>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <deque>
#include <time.h>
#include "../basic/log.hpp"

// #include <stdio.h>
// #include <windows.h>
// #include <Psapi.h>      // Psapi.Lib
// using namespace std;

namespace stool
{

	class Counter
	{
		char c = '.';
		uint64_t baseline = 2;
		uint64_t i = 2;

	public:
		Counter()
		{
		}
		Counter(uint64_t _baseline) : baseline(_baseline), i(_baseline)
		{
		}
		Counter(char _c, uint64_t _baseline) : c(_c), baseline(_baseline), i(_baseline)
		{
		}

		void increment()
		{
			if (this->i == 0)
			{
				this->baseline *= 2;
				this->i = this->baseline;
				std::cout << this->c << std::flush;
			}
			else
			{
				--this->i;
			}
		}
	};

	class DebugPrinter
	{
		public:
		static uint8_t get_first_character(const std::string &text, uint8_t default_character = '\0'){
			if(text.size() == 0){
				return default_character;
			}
			if(text[0] == '\\' && text.size() > 1){
				auto tmp = text.substr(1);
				return std::stoi(tmp);
			}else{
				return text[0];
			}
		
		}

		template <typename VEC>
		static std::string to_integer_string(const VEC &items)
		{
			std::string s = "";
			s += "[";
			// int k = items.size();
			for (size_t i = 0; i < (size_t)items.size(); i++)
			{
				s += std::to_string(items[i]);
				if (i != items.size() - 1)
					s += ", ";
			}
			s += "]";
			return s;
		}
		template <typename VEC>
		static std::string to_character_string(const VEC &items, std::string separator = ", ")
		{
			std::string s = "";
			s += "[";
			// int k = items.size();
			for (size_t i = 0; i < (size_t)items.size(); i++)
			{
				s.push_back(items[i]);
				if (i != items.size() - 1)
					s += separator;
			}
			s += "]";
			return s;
		}

		static std::string to_visible_string(uint8_t c){
			std::string s = "";
			if(std::iscntrl(c)){
				s.push_back('/');
				s += std::to_string(c);
			}else{
				s.push_back(c);
			}
			return s;
		}


		static std::string to_visible_string(const std::string &str)
		{
			std::string s = "";
			for (size_t i = 0; i < (size_t)str.size(); i++)
			{
				s += to_visible_string(str[i]);
			}
			return s;
		}
		template <typename VEC>
		static std::string to_string(const VEC &items)
		{
			std::string s = "";
			for (size_t i = 0; i < (size_t)items.size(); i++)
			{
				s.push_back(items[i]);
			}
			return s;
		}

		template <typename VEC>
		static std::string to_integer_string_with_characters(const VEC &items, std::string separator = ", ")
		{
			std::string s = "";
			s += "[";
			// int k = items.size();
			for (size_t i = 0; i < (size_t)items.size(); i++)
			{
				s += to_visible_string(items[i]);
				/*
				s += std::to_string(items[i]);
				s.push_back('(');
				s.push_back(items[i]);
				s.push_back(')');
				*/

				if (i != items.size() - 1)
					s += separator;
			}
			s += "]";
			return s;
		}

		template <typename VEC>
		static void print_integers(const VEC &items, const std::string name = "PRINT_INTEGERS")
		{
			std::cout << name << ": " << to_integer_string(items) << std::endl;
		}
		template <typename VEC>
		static void print_characters(const VEC &items, const std::string name = "PRINT_INTEGERS", std::string separator = ", ")
		{
			std::cout << name << ": " << to_character_string(items, separator) << std::endl;
		}

		template <typename VEC>
		static void print_integers_with_characters(const VEC &items, const std::string name = "PRINT_INTEGERS", std::string separator = ", ")
		{
			std::cout << name << ": " << to_integer_string_with_characters(items, separator) << std::endl;
		}

		template <typename VEC>
		static void print_log_profile(const VEC &items, const std::string name = "PRINT_INTEGERS")
		{
			std::vector<uint64_t> log_profile;
			for(uint64_t i = 0; i < items.size(); i++) {
				uint64_t log_value = 0;
				if(items[i] > 0) {
				log_value = stool::Log::log2ceil(items[i]);

				}
				while(log_profile.size() <= log_value) {
					log_profile.push_back(0);
				}
				log_profile[log_value]++;
			}

			std::cout << "=========" << name << "=========" << std::endl;
			for(uint64_t i = 0; i < log_profile.size(); i++) {
				std::cout << i << ": " << log_profile[i] << std::endl;
			}
			std::cout << "==================" << std::endl;
		}




	};

	class Printer
	{
	public:
		template <class X>
		static std::string to_integer_string(const std::vector<X> &items)
		{
			std::string s = "";
			s += "[";
			// int k = items.size();
			for (size_t i = 0; i < (size_t)items.size(); i++)
			{
				s += std::to_string(items[i]);
				if (i != items.size() - 1)
					s += ", ";
			}
			s += "]";
			return s;
		}
		template <class X>
		static std::string to_integer_string(const std::deque<X> &items)
		{
			std::string s = "";
			s += "[";
			// int k = items.size();
			for (size_t i = 0; i < (size_t)items.size(); i++)
			{
				s += std::to_string(items[i]);
				if (i != items.size() - 1)
					s += ", ";
			}
			s += "]";
			return s;
		}
		template <class X>
		static void to_integer_string(const std::vector<X> &items, std::string &result)
		{
			result += "[";
			// int k = items.size();
			for (size_t i = 0; i < (size_t)items.size(); i++)
			{
				result += std::to_string(items[i]);
				if (i != items.size() - 1)
					result += ", ";
			}
			result += "]";
		}

		template <class X>
		static void to_integer_string(const std::vector<X> &items, std::string &result, uint64_t padding)
		{
			result += "[";
			// int k = items.size();
			for (size_t i = 0; i < items.size(); i++)
			{
				std::string s = "";

				if ((uint64_t)items[i] == UINT64_MAX)
				{
					s = "-";
				}
				else if ((uint64_t)items[i] == UINT64_MAX - 1)
				{
					s = "*";
				}
				else
				{
					s = std::to_string(items[i]);
				}
				while (s.size() < padding)
				{
					s += " ";
				}

				result += s;
				if (i != items.size() - 1)
					result += ",";
			}
			result += "]";
		}
		static void to_integer_string(const std::string &items, std::string &result, uint64_t padding)
		{
			result += "[";
			// int k = items.size();
			for (uint64_t i = 0; i < items.size(); i++)
			{
				std::string s = "";

					s = items[i];
				while (s.size() < padding)
				{
					s += " ";
				}

				result += s;
				if (i != items.size() - 1)
					result += ",";
			}
			result += "]";
		}
		template <class X>
		static std::string toString(std::vector<X> &items, uint64_t padding)
		{
			std::string s = "";
			to_integer_string<X>(items, s, padding);
			return s;
		}
		static std::string toString(std::string &str, uint64_t padding)
		{
			std::string s = "";
			to_integer_string(str, s, padding);
			return s;
		}

		template <class X>
		static void print(const std::vector<X> &items)
		{
			std::cout << to_integer_string<X>(items) << std::endl;
		}

		template <class X>
		static void print(const std::deque<X> &items)
		{
			std::cout << to_integer_string<X>(items) << std::endl;
		}

		template <class X>
		static void print(const std::string name, const std::vector<X> &items)
		{
			std::cout << name << ": " << to_integer_string<X>(items) << std::endl;
		}

		/*
		template <typename VEC>
		static void print_integers(const std::string name, const VEC &items)
		{
			std::cout << name << ": " << to_integer_string<X>(items) << std::endl;
		}
		*/

		template <class X>
		static void print_chars(const std::string name, const std::vector<X> &items)
		{
			std::string result;
			result += "[";
			// int k = items.size();
			for (uint64_t i = 0; i < items.size(); i++)
			{
				std::string s;
				s.push_back(items[i]);
				result += s;
				if (i != items.size() - 1)
					result += ", ";
			}
			result += "]";

			std::cout << name << ": " << result << std::endl;
		}

		template <class X>
		static void print_bits(const std::string name, const X &items)
		{
			std::string s;
			for (uint64_t i = 0; i < items.size(); i++)
			{
				s += items[i] ? "1" : "0";
			}
			std::cout << name << ": " << s << std::endl;
		}
		template <class X>
		static void print(const std::string name, const std::deque<X> &items)
		{
			std::cout << name << ": " << to_integer_string<X>(items) << std::endl;
		}
		template <class X>
		static void print_string(const X &items)
		{
			for (auto c : items)
			{
				if (c != 0)
				{
					std::cout << c;
				}
				else
				{
					std::cout << "[$]";
				}
			}
			std::cout << std::endl;
		}
		template <class X>
		static std::string to_string(X &items)
		{
			std::string s = "";
			for (auto c : items)
			{
				if (c != 0)
				{
					s += c;
				}
				else
				{
					s += "[$]";
				}
			}
			return s;
		}

		static void print_bwt_table(std::vector<uint8_t> &bwt, std::vector<uint64_t> &sa)
		{
			std::string text;
			text.resize(sa.size());
			for (uint64_t i = 0; i < sa.size(); i++)
			{
				if (sa[i] > 0)
				{
					text[sa[i] - 1] = bwt[i];
				}
				else
				{
					text[text.size() - 1] = bwt[i];
				}
			}

			std::cout << "===========================" << std::endl;
			for (uint64_t i = 0; i < sa.size(); i++)
			{
				std::string s;
				uint64_t len = sa.size() - sa[i];
				for (uint64_t p = 0; p < len; p++)
				{
					s.push_back(text[sa[i] + p]);
				}
				std::string str1 = std::to_string(i);
				while (str1.size() < 3)
				{
					str1.push_back(' ');
				}
				std::string str2 = std::to_string(sa[i]);
				while (str2.size() < 3)
				{
					str2.push_back(' ');
				}

				std::cout << str1 << " " << str2 << " : " << bwt[i] << " | " << s << std::endl;
			}
			std::cout << "===========================" << std::endl;
		}

		static void print_sa_table(std::vector<uint8_t> &text, std::vector<uint64_t> &sa)
		{

			std::cout << "===========================" << std::endl;
			for (uint64_t i = 0; i < sa.size(); i++)
			{
				std::string s;
				uint64_t len = sa.size() - sa[i];
				for (uint64_t p = 0; p < len; p++)
				{
					s.push_back(text[sa[i] + p]);
				}
				std::string str1 = std::to_string(i);
				while (str1.size() < 3)
				{
					str1.push_back(' ');
				}
				std::string str2 = std::to_string(sa[i]);
				while (str2.size() < 3)
				{
					str2.push_back(' ');
				}

				std::cout << str1 << " " << str2 << " | " << s << std::endl;
			}
			std::cout << "===========================" << std::endl;
		}

		/*
	template <class X>
	static void write(std::string filename, std::vector<X> &items)
	{
		std::ofstream out(filename, std::ios::out | std::ios::binary);
		if (!out)
		{
			throw - 1;
		}
		std::string s = to_integer_string<X>(items);
		out.write((const char *)&s[0], sizeof(char) * s.size());
	}
	static bool write(std::ofstream &os, std::string &text)
	{
		os.write((const char *)(&text[0]), sizeof(char) * text.size());
		return true;
	}
	static bool write(std::string filename, std::string &text)
	{
		std::ofstream out(filename, std::ios::out | std::ios::binary);
		if (!out)
			return 1;
		return write(out, text);
	}
	static bool debugWrite(std::string &text)
	{
		std::ofstream out("debug.txt", std::ios::out | std::ios::binary | std::ios::app);
		if (!out)
			return 1;
		return write(out, text);
	}
	template <class X>
	static void debugWrite(std::vector<X> &items)
	{
		std::ofstream out("debug.txt", std::ios::out | std::ios::binary | std::ios::app);
		if (!out)
		{
			throw - 1;
		}
		std::string s = to_integer_string<X>(items);
		out.write((const char *)&s[0], sizeof(char) * s.size());
	}
	*/
	};
} // namespace stool