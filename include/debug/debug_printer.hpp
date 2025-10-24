#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include "./converter_to_string.hpp"

namespace stool
{

	/**
	 * @brief A utility class for printing data structures
	 */
	class DebugPrinter
	{
		public:

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
			std::cout << name << ": " << ConverterToString::to_integer_string(items) << std::endl;
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
			std::cout << name << ": " << ConverterToString::to_character_string(items, separator) << std::endl;
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
			std::cout << name << ": " << ConverterToString::to_integer_string_with_characters(items, separator) << std::endl;
		}

		

/**
		 * @brief Prints a suffix array and cyclic shifts for understanding a given BWT (Burrows-Wheeler Transform)
		 * 
		 * @param bwt The BWT array
		 * @param sa The suffix array corresponding to the BWT
		 */
		static void print_bwt_table(const std::vector<uint8_t> &bwt, const std::vector<uint64_t> &sa)
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

	};

	
} // namespace stool