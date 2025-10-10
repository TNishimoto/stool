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

namespace stool
{

	/**
	 * @brief Debug printing utility class providing various formatting and output functions [Unchecked AI's Comment] 
	 * 
	 * This class contains static methods for converting data structures to formatted strings
	 * and printing them for debugging purposes. It supports various data types including
	 * vectors, characters, and integers with different formatting options.
	 */
	class DebugPrinter
	{
		public:
		/**
		 * @brief Extracts the first character from a string, handling escape sequences
		 * 
		 * @param text The input string to extract the first character from
		 * @param default_character The character to return if the string is empty
		 * @return uint8_t The first character or the default character if string is empty
		 * 
		 * If the string starts with a backslash followed by a number, it converts
		 * the number to an integer character value. Otherwise, returns the first character.
		 */
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

		/**
		 * @brief Converts a vector of numeric values to a comma-separated string representation
		 * 
		 * @tparam VEC The type of the vector (must support size() and operator[])
		 * @param items The vector to convert
		 * @return std::string A string representation in format "[item1, item2, ...]"
		 */
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
		
		/**
		 * @brief Converts a vector of characters to a string representation
		 * 
		 * @tparam VEC The type of the vector (must support size() and operator[])
		 * @param items The vector of characters to convert
		 * @param separator The separator string between characters (default: ", ")
		 * @return std::string A string representation in format "[char1, char2, ...]"
		 */
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

		/**
		 * @brief Converts a single character to a visible string representation
		 * 
		 * @param c The character to convert
		 * @return std::string A string representation of the character
		 * 
		 * If the character is a control character, it returns "/" followed by the
		 * character's numeric value. Otherwise, returns the character as a string.
		 */
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

		/**
		 * @brief Converts a string to a visible representation, handling control characters
		 * 
		 * @param str The input string to convert
		 * @return std::string A string where control characters are represented as "/number"
		 */
		static std::string to_visible_string(const std::string &str)
		{
			std::string s = "";
			for (size_t i = 0; i < (size_t)str.size(); i++)
			{
				s += to_visible_string(str[i]);
			}
			return s;
		}
		
		/**
		 * @brief Converts a vector to a string by concatenating all elements
		 * 
		 * @tparam VEC The type of the vector (must support size() and operator[])
		 * @param items The vector to convert
		 * @return std::string A string containing all elements concatenated
		 */
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

		/**
		 * @brief Converts a vector to a string showing both numeric values and character representations
		 * 
		 * @tparam VEC The type of the vector (must support size() and operator[])
		 * @param items The vector to convert
		 * @param separator The separator string between elements (default: ", ")
		 * @return std::string A string representation showing both numeric and character values
		 */
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

		/**
		 * @brief Prints a vector of integers to stdout with a label
		 * 
		 * @tparam VEC The type of the vector (must support size() and operator[])
		 * @param items The vector to print
		 * @param name The label to print before the vector (default: "PRINT_INTEGERS")
		 */
		template <typename VEC>
		static void print_integers(const VEC &items, const std::string name = "PRINT_INTEGERS")
		{
			std::cout << name << ": " << to_integer_string(items) << std::endl;
		}
		
		/**
		 * @brief Prints a vector of characters to stdout with a label
		 * 
		 * @tparam VEC The type of the vector (must support size() and operator[])
		 * @param items The vector of characters to print
		 * @param name The label to print before the vector (default: "PRINT_INTEGERS")
		 * @param separator The separator string between characters (default: ", ")
		 */
		template <typename VEC>
		static void print_characters(const VEC &items, const std::string name = "PRINT_INTEGERS", std::string separator = ", ")
		{
			std::cout << name << ": " << to_character_string(items, separator) << std::endl;
		}

		/**
		 * @brief Prints a vector showing both numeric values and character representations
		 * 
		 * @tparam VEC The type of the vector (must support size() and operator[])
		 * @param items The vector to print
		 * @param name The label to print before the vector (default: "PRINT_INTEGERS")
		 * @param separator The separator string between elements (default: ", ")
		 */
		template <typename VEC>
		static void print_integers_with_characters(const VEC &items, const std::string name = "PRINT_INTEGERS", std::string separator = ", ")
		{
			std::cout << name << ": " << to_integer_string_with_characters(items, separator) << std::endl;
		}

		/**
		 * @brief Prints a logarithmic profile of the values in a vector
		 * 
		 * @tparam VEC The type of the vector (must support size() and operator[])
		 * @param items The vector to analyze
		 * @param name The label to print before the profile (default: "PRINT_INTEGERS")
		 * 
		 * Creates a histogram showing the distribution of values based on their
		 * ceiling log2 values. Useful for analyzing the distribution of data.
		 */
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

		static std::string to_64bits(uint64_t x){
			std::bitset<64> b(x);
			return b.to_string();
		}



	};

	/**
	 * @brief General purpose printing utility class for various data structures
	 * 
	 * This class provides comprehensive printing and formatting capabilities for
	 * different data types including vectors, deques, strings, and specialized
	 * data structures like BWT tables and suffix arrays.
	 */
	class Printer
	{
	public:
		/**
		 * @brief Converts a vector of numeric values to a comma-separated string
		 * 
		 * @tparam X The type of elements in the vector
		 * @param items The vector to convert
		 * @return std::string A string representation in format "[item1, item2, ...]"
		 */
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
		
		/**
		 * @brief Converts a deque of numeric values to a comma-separated string
		 * 
		 * @tparam X The type of elements in the deque
		 * @param items The deque to convert
		 * @return std::string A string representation in format "[item1, item2, ...]"
		 */
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
		
		/**
		 * @brief Appends a vector's string representation to an existing result string
		 * 
		 * @tparam X The type of elements in the vector
		 * @param items The vector to convert
		 * @param result The string to append the result to
		 */
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

		/**
		 * @brief Converts a vector to a padded string representation with special handling for UINT64_MAX values
		 * 
		 * @tparam X The type of elements in the vector
		 * @param items The vector to convert
		 * @param result The string to append the result to
		 * @param padding The minimum width for each element (padded with spaces)
		 * 
		 * Special values are handled as follows:
		 * - UINT64_MAX is displayed as "-"
		 * - UINT64_MAX-1 is displayed as "*"
		 * - Other values are displayed as their string representation
		 */
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
		
		/**
		 * @brief Converts a string to a padded string representation
		 * 
		 * @param items The string to convert
		 * @param result The string to append the result to
		 * @param padding The minimum width for each character (padded with spaces)
		 */
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
		
		/**
		 * @brief Creates a padded string representation of a vector
		 * 
		 * @tparam X The type of elements in the vector
		 * @param items The vector to convert
		 * @param padding The minimum width for each element
		 * @return std::string A padded string representation
		 */
		template <class X>
		static std::string toString(std::vector<X> &items, uint64_t padding)
		{
			std::string s = "";
			to_integer_string<X>(items, s, padding);
			return s;
		}
		
		/**
		 * @brief Creates a padded string representation of a string
		 * 
		 * @param str The string to convert
		 * @param padding The minimum width for each character
		 * @return std::string A padded string representation
		 */
		static std::string toString(std::string &str, uint64_t padding)
		{
			std::string s = "";
			to_integer_string(str, s, padding);
			return s;
		}

		/**
		 * @brief Prints a vector to stdout
		 * 
		 * @tparam X The type of elements in the vector
		 * @param items The vector to print
		 */
		template <class X>
		static void print(const std::vector<X> &items)
		{
			std::cout << to_integer_string<X>(items) << std::endl;
		}

		/**
		 * @brief Prints a deque to stdout
		 * 
		 * @tparam X The type of elements in the deque
		 * @param items The deque to print
		 */
		template <class X>
		static void print(const std::deque<X> &items)
		{
			std::cout << to_integer_string<X>(items) << std::endl;
		}

		/**
		 * @brief Prints a vector to stdout with a label
		 * 
		 * @tparam X The type of elements in the vector
		 * @param name The label to print before the vector
		 * @param items The vector to print
		 */
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

		/**
		 * @brief Prints a vector of characters to stdout with a label
		 * 
		 * @tparam X The type of elements in the vector (typically char or uint8_t)
		 * @param name The label to print before the vector
		 * @param items The vector of characters to print
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

		/**
		 * @brief Prints a bit vector to stdout as a binary string
		 * 
		 * @tparam X The type of the bit vector (must support size() and operator[])
		 * @param name The label to print before the bit string
		 * @param items The bit vector to print
		 */
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
		
		/**
		 * @brief Prints a deque to stdout with a label
		 * 
		 * @tparam X The type of elements in the deque
		 * @param name The label to print before the deque
		 * @param items The deque to print
		 */
		template <class X>
		static void print(const std::string name, const std::deque<X> &items)
		{
			std::cout << name << ": " << to_integer_string<X>(items) << std::endl;
		}
		
		/**
		 * @brief Prints a string-like container to stdout, handling null characters
		 * 
		 * @tparam X The type of the container (must support iteration)
		 * @param items The container to print
		 * 
		 * Null characters (0) are printed as "[$]", while other characters are printed normally.
		 */
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
		
		/**
		 * @brief Converts a string-like container to a string, handling null characters
		 * 
		 * @tparam X The type of the container (must support iteration)
		 * @param items The container to convert
		 * @return std::string A string representation where null characters are shown as "[$]"
		 */
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

		/**
		 * @brief Prints a BWT (Burrows-Wheeler Transform) table in a formatted manner
		 * 
		 * @param bwt The BWT array
		 * @param sa The suffix array corresponding to the BWT
		 * 
		 * Prints a table showing the index, suffix array position, BWT character,
		 * and the corresponding suffix for each position in the BWT.
		 */
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

		/**
		 * @brief Prints a suffix array table in a formatted manner
		 * 
		 * @param text The original text string
		 * @param sa The suffix array
		 * 
		 * Prints a table showing the index, suffix array position, and the
		 * corresponding suffix for each position in the suffix array.
		 */
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