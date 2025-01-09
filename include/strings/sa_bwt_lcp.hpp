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

	template <typename CHAR = uint8_t, typename INDEX = uint64_t>
	std::vector<INDEX> constructISA(const std::vector<CHAR> &text, const std::vector<INDEX> &sa, int message_paragraph = stool::Message::SHOW_MESSAGE)
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

	template <typename CHAR = uint8_t, typename INDEX = uint64_t>
	std::vector<INDEX> constructLCP(const std::vector<CHAR> &text, const std::vector<INDEX> &sa, const std::vector<INDEX> &isa, int message_paragraph = stool::Message::SHOW_MESSAGE)
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
	template <typename CHAR, typename INDEX>
	std::vector<INDEX> constructLCP(const std::vector<CHAR> &text, const std::vector<INDEX> &sa,  int message_paragraph = stool::Message::SHOW_MESSAGE)
	{


		std::vector<INDEX> isa = stool::constructISA<CHAR, INDEX>(text, sa);
		return constructLCP<CHAR, INDEX>(text, sa, isa, message_paragraph);



		// lcp.resize(text.size(), 0);
	}

	template <typename CHAR, typename INDEX>
	std::vector<CHAR> constructBWT(const std::vector<CHAR> &text, const std::vector<INDEX> &sa,  int message_paragraph = stool::Message::SHOW_MESSAGE)
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

	std::vector<uint64_t> constructISA(const std::string &text, const std::vector<uint64_t> &sa)
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
	std::vector<uint64_t> constructLCP(const std::string &text, const std::vector<uint64_t> &sa, const std::vector<uint64_t> &isa)
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