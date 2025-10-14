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
	
	/**
	 * @brief A utility class for constructing various arrays (e.g., ISA, LCP array, BWT)
	 */
	class ArrayConstructor
	{
	public:
		

		/*!
		 * @brief Constructs the Inverse Suffix Array (ISA) from a suffix array (SA)
		 *
		 * @tparam INDEX The index type for positions (defaults to uint64_t)
		 * @param sa text's suffix array
		 * @param message_paragraph The paragraph depth of message logs (-1 for no output)
		 * @return ISA
		 */
		template <typename INDEX = uint64_t>
		static std::vector<INDEX> construct_ISA(const std::vector<INDEX> &sa, int message_paragraph = stool::Message::SHOW_MESSAGE)
		{
			if (message_paragraph >= 0 && sa.size() > 0)
			{
				std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing Inverse Suffix Array from Suffix Array... " << std::flush;
			}
			std::chrono::system_clock::time_point st1, st2;
			st1 = std::chrono::system_clock::now();

			std::vector<INDEX> isa;
			uint64_t n = sa.size();
			isa.resize(n);

			for (uint64_t i = 0; i < sa.size(); ++i)
			{
				isa[sa[i]] = i;
			}
			st2 = std::chrono::system_clock::now();

			if (message_paragraph >= 0 && sa.size() > 0)
			{
				uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
				uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
				uint64_t per_time = ((double)ms_time / (double)sa.size()) * 1000000;

				std::cout << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
			}

			return isa;
		}

		/*!
		 * @brief Constructs the LCP of text T array from T, SA, and ISA
		 *
		 *
		 * @tparam TEXT The type of the text (e.g., std::vector<uint8_t>, std::vector<char>, std::string)
		 * @tparam INDEX The index type for positions (defaults to uint64_t)
		 * @param text The input text T
		 * @param sa The SA of T
		 * @param isa The ISA of T
		 * @param message_paragraph The paragraph depth of message logs (-1 for no output)
		 * @return The LCP array of T
		 */

		template <typename TEXT = std::vector<uint8_t>, typename INDEX = uint64_t>
		static std::vector<INDEX> construct_LCP_array(const TEXT &text, const std::vector<INDEX> &sa, const std::vector<INDEX> &isa, int message_paragraph = stool::Message::SHOW_MESSAGE)
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
		 * @brief Constructs the LCP of text T array from T, SA, and ISA. Here, ISA is constructed internally.
		 *
		 * @tparam TEXT The type of the text (e.g., std::vector<uint8_t>, std::vector<char>, std::string)
		 * @tparam INDEX The index type for positions (defaults to uint64_t)
		 * @param text The input text T
		 * @param sa The SA of T
		 * @param message_paragraph The paragraph depth of message logs (-1 for no output)
		 * @return The LCP array of T
		 */
		template <typename TEXT = std::vector<uint8_t>, typename INDEX = uint64_t>
		static std::vector<INDEX> construct_LCP_array(const TEXT &text, const std::vector<INDEX> &sa, int message_paragraph = stool::Message::SHOW_MESSAGE)
		{

			std::vector<INDEX> isa = stool::ArrayConstructor::construct_ISA<INDEX>(sa, message_paragraph);
			return construct_LCP_array<TEXT, INDEX>(text, sa, isa, message_paragraph);

			// lcp.resize(text.size(), 0);
		}


		/**
		 * @brief Constructs the Differential Suffix Array (DSA) from a SA
		 * 
		 * The Differential Suffix Array stores the differences between consecutive
		 * elements in the suffix array. For position i, DSA[i] = SA[i] - SA[i-1]
		 * (with DSA[0] = SA[0]). This representation is useful for compression
		 * and certain string processing algorithms.
		 * 
		 * @param sa The SA of T
		 * @param message_paragraph The paragraph depth of message logs (-1 for no output)
		 * @return The DSA of T
		 */
		static std::vector<int64_t> construct_DSA(const std::vector<uint64_t> &sa, int message_paragraph = stool::Message::SHOW_MESSAGE)
		{
			if (message_paragraph >= 0 && sa.size() > 0)
			{
				std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing Diffrential Suffix Array from Suffix Array... " << std::flush;
			}
			std::chrono::system_clock::time_point st1, st2;
			st1 = std::chrono::system_clock::now();

			std::vector<int64_t> dsa;
			dsa.resize(sa.size(), 0);
			for (uint64_t i = 0; i < sa.size(); i++)
			{
				if (i == 0)
				{
					dsa[i] = sa[i];
				}
				else
				{
					dsa[i] = ((int64_t)sa[i]) - ((int64_t)sa[i - 1]);
				}
			}

			st2 = std::chrono::system_clock::now();

			if (message_paragraph >= 0 && sa.size() > 0)
			{
				uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
				uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
				uint64_t per_time = ((double)ms_time / (double)sa.size()) * 1000000;

				std::cout << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
			}

			return dsa;
		}

		/*!
		 * @brief Constructs the Burrows-Wheeler Transform (BWT) from a text and its SA 
		 *
		 * @tparam TEXT The type of the text (e.g., std::vector<uint8_t>, std::vector<char>, std::string)
		 * @tparam INDEX The integer type used for the SA (defaults to uint64_t)
		 * @param text The input text T
		 * @param sa The SA of T
		 * @param message_paragraph The paragraph depth of message logs (-1 for no output)
		 * @return The BWT of T
		 */

		template <typename TEXT = std::vector<uint8_t>, typename INDEX = uint64_t>
		static TEXT construct_BWT(const TEXT &text, const std::vector<INDEX> &sa, int message_paragraph = stool::Message::SHOW_MESSAGE)
		{
			if (message_paragraph >= 0 && text.size() > 0)
			{
				std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing BWT from SA... " << std::flush;
			}
			std::chrono::system_clock::time_point st1, st2;
			st1 = std::chrono::system_clock::now();

			TEXT bwt;
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


		
	};

} // namespace stool