#pragma once
#include <cassert>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <stack>
#include <chrono>
#include "../debug/message.hpp"

namespace stool
{
	/*!
	* @brief Checks if a text contains a special marker character
	* 
	* This function verifies that a given text contains a special marker character 'c'
	* only at the end of the text. It checks three conditions:
	* 1. The marker character appears exactly once at the end of the text
	* 2. No characters in the text are less than the marker character
	* 3. The marker character does not appear anywhere else in the text
	*
	* @tparam CHAR The character type of the text
	* @param text The input text to check
	* @param c The special marker character to look for
	* @return true if the text satisfies all conditions, throws an exception otherwise
	* @throws std::logic_error if any of the conditions are violated
	*/
	template <typename CHAR>
	bool checkTextWithSpecialMarker(std::vector<CHAR> &text, CHAR c)
	{
		uint64_t p = UINT64_MAX;
		for (uint64_t i = 0; i < text.size(); i++)
		{
			if (text[i] == c)
			{
				p = i;
				break;
			}
			else if (text[i] < c)
			{
				throw std::logic_error("The characters of the input text must not be less than '0'.");
			}
		}
		if (p == text.size() - 1)
		{
			return true;
		}
		else if (p == UINT64_MAX)
		{
			throw std::logic_error("The last character of the input text must be '0'");
		}
		else
		{
			throw std::logic_error("The input text must not contain '0' except for the last character.");
		}
	}
	
	
	/*!
	* @brief Constructs the Inverse Suffix Array (ISA) from a Suffix Array
	* 
	* This function builds the Inverse Suffix Array from a given Suffix Array.
	* The ISA maps each suffix position to its rank in the sorted order of suffixes.
	* For each position i in the text, ISA[SA[i]] = i.
	*
	* @tparam CHAR The character type of the text (defaults to uint8_t)
	* @tparam INDEX The index type for positions (defaults to uint64_t)
	* @param text The input text
	* @param sa The suffix array of the text
	* @param message_paragraph Controls output verbosity (-1 for no output)
	* @return The constructed Inverse Suffix Array
	*/
	template <typename CHAR = uint8_t, typename INDEX = uint64_t>
	std::vector<INDEX> construct_ISA(const std::vector<CHAR> &text, const std::vector<INDEX> &sa, int message_paragraph = stool::Message::SHOW_MESSAGE)
	{
		if (message_paragraph >= 0 && text.size() > 0)
		{
			std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing Inverse Suffix Array from Suffix Array... " << std::flush;
		}
		std::chrono::system_clock::time_point st1, st2;
		st1 = std::chrono::system_clock::now();

		std::vector<INDEX> isa;
		uint64_t n = text.size();
		isa.resize(n);

		for (uint64_t i = 0; i < text.size(); ++i)
		{
			isa[sa[i]] = i;
		}
		st2 = std::chrono::system_clock::now();

		if (message_paragraph >= 0 && text.size() > 0)
		{
			uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
			uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
			uint64_t per_time = ((double)ms_time / (double)text.size()) * 1000000;

			std::cout << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
		}

		return isa;
	}

	/*!
	* @brief Constructs the Longest Common Prefix (LCP) array from a Suffix Array and Inverse Suffix Array
	* 
	* This function computes the LCP array for a given text using its suffix array and inverse suffix array.
	* The LCP array stores the length of the longest common prefix between consecutive suffixes in the sorted order.
	*
	* @tparam CHAR The character type of the text (defaults to uint8_t)
	* @tparam INDEX The index type for positions (defaults to uint64_t)
	* @param text The input text
	* @param sa The suffix array of the text
	* @param isa The inverse suffix array of the text
	* @param message_paragraph Controls output verbosity (-1 for no output)
	* @return The constructed LCP array
	*/

	template <typename CHAR = uint8_t, typename INDEX = uint64_t>
	std::vector<INDEX> construct_LCP_array(const std::vector<CHAR> &text, const std::vector<INDEX> &sa, const std::vector<INDEX> &isa, int message_paragraph = stool::Message::SHOW_MESSAGE)
	{
		if (message_paragraph >= 0 && text.size() > 0)
		{
			std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing LCP Array from SA and ISA... " << std::flush;
		}
		std::chrono::system_clock::time_point st1, st2;
		st1 = std::chrono::system_clock::now();


		std::vector<INDEX> lcp;
		lcp.resize(text.size(), 0);
		INDEX n = text.size();
		INDEX k = 0;

		for (INDEX i = 0; i < n; i++)
		{

			assert(i < n);
			INDEX x = isa[i];
			assert(x < n);

			if (x == 0)
			{
				lcp[x] = 0;
			}
			else
			{
				// std::cout << n << "/" << x << "/" << sa[x] << "/" << sa[x-1] << "/" << k << std::endl;
				while (sa[x] + k < n && sa[x - 1] + k < n && text[sa[x] + k] == text[sa[x - 1] + k])
				{
					assert(sa[x] + k < n);
					assert(sa[x - 1] + k < n);

					k++;
				}
			}
			lcp[x] = k;

			// Printer::print(lcp);
			// uint64_t prevLength = text.size() - sa[x-1];
			// assert((n - sa[x-1]) >= k);
			assert(x == 0 || (x > 0 && ((n - sa[x - 1]) >= k)));

			if (k > 0)
				k--;
		}


		st2 = std::chrono::system_clock::now();

		if (message_paragraph >= 0 && text.size() > 0)
		{
			uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
			uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
			uint64_t per_time = ((double)ms_time / (double)text.size()) * 1000000;

			std::cout << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
		}
		return lcp;
	}

	/*!
	* @brief Constructs the LCP array from a Suffix Array
	* 
	* This function constructs the LCP array from a given Suffix Array.
	* It first constructs the Inverse Suffix Array (ISA) and then uses it to compute the LCP array.
	*
	* @tparam CHAR The character type of the text (defaults to uint8_t)
	* @tparam INDEX The index type for positions (defaults to uint64_t)
	* @param text The input text
	* @param sa The suffix array of the text
	* @param message_paragraph Controls output verbosity (-1 for no output)
	* @return The constructed LCP array
	*/
	template <typename CHAR, typename INDEX>
	std::vector<INDEX> construct_LCP_array(const std::vector<CHAR> &text, const std::vector<INDEX> &sa,  int message_paragraph = stool::Message::SHOW_MESSAGE)
	{


		std::vector<INDEX> isa = stool::construct_ISA<CHAR, INDEX>(text, sa);
		return construct_LCP_array<CHAR, INDEX>(text, sa, isa, message_paragraph);



		// lcp.resize(text.size(), 0);
	}

	/*!
	* @brief Constructs the Burrows-Wheeler Transform (BWT) from a Suffix Array
	* 
	* This function computes the Burrows-Wheeler Transform of a given text using its suffix array.
	* For each position i in the suffix array, it takes the character at position (sa[i] - 1) 
	* if sa[i] > 0, or the last character of the text if sa[i] = 0.
	*
	* @tparam CHAR The character type of the text and BWT
	* @tparam INDEX The integer type used for the suffix array
	* @param text The input text
	* @param sa The suffix array of the text
	* @param message_paragraph Controls output verbosity (-1 for no output)
	* @return The Burrows-Wheeler Transform of the text
	*/

	template <typename CHAR, typename INDEX>
	std::vector<CHAR> construct_BWT(const std::vector<CHAR> &text, const std::vector<INDEX> &sa,  int message_paragraph = stool::Message::SHOW_MESSAGE)
	{
		if (message_paragraph >= 0 && text.size() > 0)
		{
			std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing BWT from SA... " << std::flush;
		}
		std::chrono::system_clock::time_point st1, st2;
		st1 = std::chrono::system_clock::now();


		std::vector<CHAR> bwt;
		bwt.resize(text.size());
		INDEX n = text.size();
		for (INDEX i = 0; i < text.size(); i++)
		{
			if (sa[i] == 0)
			{
				bwt[i] = text[n - 1];
			}
			else
			{
				bwt[i] = text[sa[i] - 1];
			}
		}


		st2 = std::chrono::system_clock::now();

		if (message_paragraph >= 0 && text.size() > 0)
		{
			uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
			uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
			uint64_t per_time = ((double)ms_time / (double)text.size()) * 1000000;

			std::cout << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
		}

		return bwt;
	}

	
	/*!
	* @brief Constructs the Inverse Suffix Array (ISA) from a text and its suffix array
	*
	* The Inverse Suffix Array maps each position in the text to its corresponding position
	* in the suffix array. For each position i in the text, ISA[SA[i]] = i.
	* This is useful for various string processing algorithms and is often used in
	* conjunction with the suffix array and LCP array.
	*
	* @param text The input text
	* @param sa The suffix array of the text
	* @return The Inverse Suffix Array
	*/
	std::vector<uint64_t> construct_ISA(const std::string &text, const std::vector<uint64_t> &sa)
	{

		std::vector<uint64_t> isa;
		uint64_t n = text.size();
		isa.resize(n);

		for (uint64_t i = 0; i < text.size(); ++i)
		{
			isa[sa[i]] = i;
		}
		return isa;
	}

	/*!
	* @brief Constructs the Longest Common Prefix (LCP) array from a text and its suffix array
	*
	* This function computes the LCP array for a given text using its suffix array and inverse suffix array.
	* The LCP array stores the length of the longest common prefix between consecutive suffixes in the sorted order.
	*
	* @param text The input text
	* @param sa The suffix array of the text
	* @param isa The inverse suffix array of the text
	* @return The constructed LCP array
	*/
	std::vector<uint64_t> construct_LCP_array(const std::string &text, const std::vector<uint64_t> &sa, const std::vector<uint64_t> &isa)
	{
		std::vector<uint64_t> lcp;
		lcp.resize(text.size(), 0);
		uint64_t n = text.size();
		uint64_t k = 0;
		if (text.size() > 1000000)
			std::cout << "Constructing LCP Array" << std::flush;

		for (uint64_t i = 0; i < n; i++)
		{

			assert(i < n);
			uint64_t x = isa[i];
			assert(x < n);

			if (x == 0)
			{
				lcp[x] = 0;
			}
			else
			{
				while (sa[x] + k < n && sa[x - 1] + k < n && text[sa[x] + k] == text[sa[x - 1] + k])
				{
					assert(sa[x] + k < n);
					assert(sa[x - 1] + k < n);

					k++;
				}
			}
			lcp[x] = k;

			assert(x == 0 || (x > 0 && ((n - sa[x - 1]) >= k)));

			if (k > 0)
				k--;
		}

		if (n > 1000000)
			std::cout << "[END]" << std::endl;
		return lcp;
	}

} // namespace stool