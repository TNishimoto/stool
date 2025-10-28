#pragma once
#include "./uint8_vector_generator.hpp"

namespace stool
{
	/**
	 * @brief A utility class for generating various types of random strings and sequences. [Obsolete Implementations] 
	 */
	class StringGenerator
	{
	public:
		/**
		 * @brief Creates a random sequence of integers with a specified seed.
		 * @param len The length of the sequence to generate.
		 * @param alphabet_size The size of the alphabet (range of possible values).
		 * @param seed The seed for the random number generator.
		 * @return A vector of random integers in the range [0, alphabet_size-1].
		 */
		template<typename T>
		static std::vector<T> create_random_sequence(uint64_t len, uint64_t alphabet_size, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<uint64_t> rand100(0, alphabet_size - 1);
			std::vector<T> r;

			for (size_t i = 0; i < len; i++)
			{
				uint64_t value = rand100(mt);
				assert(value < alphabet_size);
				r.push_back(value);
			}
			return r;
		}



		/**
		 * @brief Creates a random sequence of integers using a random seed.
		 * @param len The length of the sequence to generate.
		 * @param alphabet_size The size of the alphabet (range of possible values).
		 * @return A vector of random integers in the range [0, alphabet_size-1].
		 */
		static std::vector<uint32_t> create_random_sequence(uint64_t len, uint64_t alphabet_size)
		{
			std::random_device rnd;
			int64_t p = rnd();
			return create_random_sequence<uint32_t>(len, alphabet_size, p);
		}

		/**
		 * @brief Converts a sequence of integers to a string using an 8-character alphabet.
		 * @param original_text The input sequence of integers.
		 * @return A vector of uint8_t characters representing the converted string.
		 *
		 * The function maps each integer to one of 8 characters: 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'.
		 * The mapping is done using modulo 8 operation on each integer.
		 */
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

		/**
		 * @brief Creates a binary string of specified length using a given seed.
		 * @param len The length of the binary string to generate.
		 * @param seed The seed for the random number generator.
		 * @return A vector of uint8_t characters representing a binary string ('a' or 'b').
		 */
		static std::vector<uint8_t> create_uint8_t_binary_string(uint64_t len, int64_t seed)
		{
			std::vector<uint32_t> seed_seq = StringGenerator::create_random_sequence<uint32_t>(len, 2, seed);
			return create_uint8_t_string(seed_seq);
		}

		/**
		 * @brief Creates a binary string of specified length using a random seed.
		 * @param len The length of the binary string to generate.
		 * @return A vector of uint8_t characters representing a binary string ('a' or 'b').
		 */
		static std::vector<uint8_t> create_uint8_t_binary_string(uint64_t len)
		{
			std::vector<uint32_t> seed_seq = StringGenerator::create_random_sequence(len, 2);
			return create_uint8_t_string(seed_seq);
		}

		/**
		 * @brief Creates a binary string of random length up to a maximum.
		 * @param max_len The maximum length of the binary string.
		 * @return A vector of uint8_t characters representing a binary string ('a' or 'b').
		 */
		static std::vector<uint8_t> create_uint8_t_binary_string_of_random_length(uint64_t max_len)
		{
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<> rand100(0, max_len);
			uint64_t len = rand100(mt);
			return create_uint8_t_binary_string(len);
		}

		/**
		 * @brief Creates an 8-ary string of specified length using a given seed.
		 * @param len The length of the 8-ary string to generate.
		 * @param seed The seed for the random number generator.
		 * @return A vector of uint8_t characters representing an 8-ary string ('a' through 'h').
		 */
		static std::vector<uint8_t> create_uint8_t_8_ary_string(uint64_t len, int64_t seed)
		{
			std::vector<uint32_t> seed_seq = StringGenerator::create_random_sequence<uint32_t>(len, 8, seed);
			return create_uint8_t_string(seed_seq);
		}

		/**
		 * @brief Creates an 8-ary string of specified length using a random seed.
		 * @param len The length of the 8-ary string to generate.
		 * @return A vector of uint8_t characters representing an 8-ary string ('a' through 'h').
		 */
		static std::vector<uint8_t> create_uint8_t_8_ary_string(uint64_t len)
		{
			std::vector<uint32_t> seed_seq = StringGenerator::create_random_sequence(len, 8);
			return create_uint8_t_string(seed_seq);
		}

		/**
		 * @brief Creates an 8-ary string of random length up to a maximum.
		 * @param max_len The maximum length of the 8-ary string.
		 * @return A vector of uint8_t characters representing an 8-ary string ('a' through 'h').
		 */
		static std::vector<uint8_t> create_uint8_t_8_ary_string_of_random_length(uint64_t max_len)
		{
			std::random_device rnd;
			std::mt19937 mt(rnd());
			std::uniform_int_distribution<> rand100(0, max_len);
			uint64_t len = rand100(mt);
			return create_uint8_t_8_ary_string(len);
		}

		/**
		 * @brief Recursively generates all possible permutations of a given length using the provided alphabet.
		 * @param len The target length of each permutation.
		 * @param alphabet The set of characters to use for permutations.
		 * @param prefix The current prefix being built during recursion.
		 * @param output Reference to the output vector where all permutations will be stored.
		 *
		 * This is a helper function used internally by create_all_permutations().
		 */
		static void create_all_permutations(uint64_t len, const std::vector<uint8_t> &alphabet, const std::string &prefix, std::vector<std::string> &output)
		{
			if (prefix.size() > len)
			{
				for (uint8_t c : alphabet)
				{
					std::string new_prefix = prefix + (std::string(1, c));
					create_all_permutations(len, alphabet, new_prefix, output);
				}
			}
			else
			{
				output.push_back(prefix);
			}
		}

		/**
		 * @brief Generates all possible permutations of a given length using the provided alphabet.
		 * @param len The length of each permutation to generate.
		 * @param alphabet The set of characters to use for permutations.
		 * @return A vector containing all possible permutations of the specified length.
		 *
		 * This function generates all possible combinations of characters from the alphabet
		 * that have the specified length. For example, with alphabet {'a', 'b'} and length 2,
		 * it would generate {"aa", "ab", "ba", "bb"}.
		 */
		static std::vector<std::string> create_all_permutations(uint64_t len, const std::vector<uint8_t> &alphabet)
		{
			std::string prefix = "";
			std::vector<std::string> output;
			create_all_permutations(len, alphabet, prefix, output);
			return output;
		}

		static std::vector<bool> create_random_bit_vector(uint64_t len, std::mt19937_64 &mt64)
		{
			std::uniform_int_distribution<uint64_t> get_rand_value(0, 1);
			std::vector<bool> bv;
			for (uint64_t i = 0; i < len; i++)
			{
				bv.push_back(get_rand_value(mt64) == 1);
			}
			return bv;
		}
		static std::vector<bool> create_random_bit_vector(uint64_t len, uint64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<uint64_t> get_rand_value(0, 1);
			std::vector<bool> bv;
			for (uint64_t i = 0; i < len; i++)
			{
				bv.push_back(get_rand_value(mt) == 1);
			}
			return bv;
		}

		static std::vector<uint64_t> create_random_integer_sequence(uint64_t len, uint64_t max_value, int64_t seed)
		{
			std::mt19937 mt(seed);
			std::uniform_int_distribution<uint64_t> get_rand_value(0, max_value);
			std::vector<uint64_t> r;

			for (size_t i = 0; i < len; i++)
			{
				r.push_back(get_rand_value(mt));
			}
			return r;
		}


	};

} // namespace stool