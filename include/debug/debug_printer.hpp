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

	
} // namespace stool