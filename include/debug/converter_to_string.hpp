#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include "../basic/log.hpp"

namespace stool
{
    /// \defgroup DebugClasses Classes for debugging
    /// @{
    /// @}


	/**
	 * @brief A utility class for converting data structures to strings
     * \ingroup DebugClasses
	 */
	class ConverterToString
	{
		public:

		/**
		 * @brief Converts a sequence of numeric values to a comma-separated string representation
		 * 
		 * @tparam SEQUENCE The type of the sequence (must support size() and operator[])
		 * @param items The sequence to convert
		 * @return std::string A string representation in format "[item1, item2, ...]"
		 */
		template <typename SEQUENCE>
		static std::string to_integer_string(const SEQUENCE &items)
		{
			std::string s = "";
			s += "[";
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
		 * @brief Converts a sequence of characters to a string representation
		 * 
		 * @tparam SEQUENCE The type of the sequence (must support size() and operator[])
		 * @param items The sequence of characters to convert
		 * @param separator The separator string between characters (default: ", ")
		 * @return std::string A string representation in format "[char1, char2, ...]"
		 */
		template <typename SEQUENCE>
		static std::string to_character_string(const SEQUENCE &items, std::string separator = ", ")
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
		 * @note If the character is a control character, it returns "/" followed by the character's numeric value. Otherwise, returns the character as a string.
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
		template <typename SEQUENCE>
		static std::string to_string(const SEQUENCE &items)
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
				if (i != items.size() - 1)
					s += separator;
			}
			s += "]";
			return s;
		}

	};

	
} // namespace stool