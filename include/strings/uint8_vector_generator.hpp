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

#include "../debug/print.hpp"

namespace stool
{
	/**
	 * @brief A utility class for generating various types of alphabets and random sequences
	 * 
	 * This class provides static methods to create different types of alphabets
	 * (binary, DNA, English letters, etc.) and generate random sequences or
	 * substrings from these alphabets. It's primarily used for testing and
	 * benchmarking string algorithms.
	 */
	class UInt8VectorGenerator
	{
	public:
		/**
		 * @brief Creates a binary alphabet containing 'a' and 'b'
		 * 
		 * @return std::vector<uint8_t> A vector containing the characters 'a' and 'b'
		 */
		static std::vector<uint8_t> create_ab_alphabet()
		{
			return std::vector<uint8_t>{'a', 'b'};
		}
		
		/**
		 * @brief Creates an alphabet containing the first 8 lowercase letters (a-h)
		 * 
		 * @return std::vector<uint8_t> A vector containing characters 'a' through 'h'
		 */
		static std::vector<uint8_t> create_abcdefgh_alphabet()
		{
			return std::vector<uint8_t>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
		}
		
		/**
		 * @brief Creates a DNA alphabet containing the four nucleotide bases
		 * 
		 * @return std::vector<uint8_t> A vector containing 'A', 'C', 'G', 'T'
		 */
		static std::vector<uint8_t> create_ACGT_alphabet()
		{
			return std::vector<uint8_t>{'A', 'C', 'G', 'T'};
		}
		
		/**
		 * @brief Creates an alphabet containing all uppercase English letters (A-Z)
		 * 
		 * @return std::vector<uint8_t> A vector containing characters 'A' through 'Z'
		 */
		static std::vector<uint8_t> create_AtoZ_alphabet()
		{
			return std::vector<uint8_t>{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
		}
		
		/**
		 * @brief Creates a full alphabet containing ASCII values from 2 to 249
		 * 
		 * This creates a large alphabet with 248 different characters,
		 * excluding null (0) and start of heading (1) control characters.
		 * 
		 * @return std::vector<uint8_t> A vector containing ASCII values 2-249
		 */
		static std::vector<uint8_t> create_full_alphabet()
		{
			std::vector<uint8_t> r;
			for(uint64_t i = 2; i < 250;i++){
				r.push_back(i);
			}
			return r;
		}
		
		/**
		 * @brief Returns the maximum alphabet type index
		 * 
		 * @return uint64_t The maximum valid alphabet type (4, representing full alphabet)
		 */
		static uint64_t get_max_alphabet_type(){
			return 4;
		}

		/**
		 * @brief Creates an alphabet based on the specified type
		 * 
		 * @param alphabet_type The type of alphabet to create:
		 *                      - 0: Binary alphabet (a, b)
		 *                      - 1: DNA alphabet (A, C, G, T)
		 *                      - 2: 8-letter alphabet (a-h)
		 *                      - 3: English alphabet (A-Z)
		 *                      - 4: Full alphabet (ASCII 2-249)
		 * @return std::vector<uint8_t> The requested alphabet
		 */
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
		
		/**
		 * @brief Generates a random sequence of specified length using the given alphabet
		 * 
		 * @param len The length of the sequence to generate
		 * @param alphabet The alphabet to use for generating characters
		 * @param seed The random seed for reproducible results
		 * @return std::vector<uint8_t> A random sequence of the specified length
		 */
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
		
		/**
		 * @brief Creates a random substring from the given text with random length
		 * 
		 * @param text The source text to extract a substring from
		 * @param seed The random seed for reproducible results
		 * @return std::vector<uint8_t> A random substring from the text
		 */
		static std::vector<uint8_t> create_random_substring(const std::vector<uint8_t> &text, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<> rand100(0, text.size() - 1);
			uint64_t len = rand100(mt);
			return create_random_substring(text, len, seed);
		}
		
		/**
		 * @brief Creates a random substring from the given text with specified length
		 * 
		 * @param text The source text to extract a substring from
		 * @param len The length of the substring to extract
		 * @param seed The random seed for reproducible results
		 * @return std::vector<uint8_t> A random substring of the specified length
		 * @throws std::logic_error If the requested length is greater than the text length
		 */
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