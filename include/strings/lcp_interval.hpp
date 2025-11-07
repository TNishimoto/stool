#pragma once
#include <unordered_set>
#include <cassert>
#include <string>
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <stack>
#include <cstdint>
#include <cstdlib>
#include <stdio.h>

namespace stool
{
	
	/**
	 * @brief Class for representing the LCP interval in a suffix array (SA)
	 * 
	 * @tparam INDEX The type used for indexing (typically uint64_t or uint32_t)
     * \ingroup StringClasses
	 */
	template <typename INDEX = uint64_t>
	class LCPInterval
	{
	public:
		INDEX i;  ///< Starting position in the suffix array
		INDEX j;  ///< Ending position in the suffix array  
		INDEX lcp; ///< Length of the longest common prefix

		/**
		 * @brief Default constructor
		 */
		LCPInterval() {}

		/**
		 * @brief Constructor with parameters
		 * @param _i Starting position in the suffix array
		 * @param _j Ending position in the suffix array
		 * @param _lcp Length of the longest common prefix
		 */
		LCPInterval(INDEX _i, INDEX _j, INDEX _lcp)
		{
			this->i = _i;
			this->j = _j;
			this->lcp = _lcp;
		}

		/**
		 * @brief Creates a special LCP interval
		 * @return the special LCP interval
		 */
		static LCPInterval<INDEX> create_end_marker()
		{
			return LCPInterval<INDEX>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max());
		}

		/**
		 * @brief Less-than comparison operator
		 * @return true if this->i < right.i
		 */
		bool operator<(const LCPInterval &right) const
		{
			if (this->i == right.i)
			{
				if (this->j == right.j)
				{
					return this->lcp < right.lcp;
				}
				else
				{
					return this->j < right.j;
				}
			}
			else
			{
				return this->i < right.i;
			}
		}

		/**
		 * @brief Equality comparison operator
		 * @return true if the intervals are equal
		 */
		bool operator==(const LCPInterval &rhs) const
		{
			const LCPInterval &lhs = *this;
			bool b = lhs.i == rhs.i && lhs.j == rhs.j && lhs.lcp == rhs.lcp;
			return b;
		}

		/**
		 * @brief Inequality comparison operator
		 */
		bool operator!=(const LCPInterval &rhs) const
		{
			return !(*this == rhs);
		}

		/**
		 * @brief Converts the interval to a string representation
		 * @return String representation in format "[i, j, lcp]"
		 */
		std::string to_string() const
		{
			return "[" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(lcp) + "]";
		}

		/**
		 * @brief Checks if this interval is the special LCP interval
		 * @return true if this is the special LCP interval
		 */
		bool is_special_marker() const
		{
			return this->i == std::numeric_limits<INDEX>::max() && this->j == std::numeric_limits<INDEX>::max() && this->lcp == std::numeric_limits<INDEX>::max();
		}

		/**
		 * @brief Generates a CSV line representation of this LCP interval
		 * @param id The identifier for this interval
		 * @param text The input text
		 * @param sa The suffix array of T
		 * @return CSV formatted string with interval information
		 */
		template <typename SA>
		std::string get_CSV_line(uint64_t id, std::vector<char> &text, SA &sa)
		{
			std::string tmp = "";
			tmp += (std::to_string(id));
			tmp += ("\t");
			tmp += (std::to_string(this->j - this->i + 1));
			tmp += ("\t");
			tmp += (std::to_string(this->i));
			tmp += ("..");
			tmp += (std::to_string(this->j));
			tmp += ("\t");
			tmp += (std::to_string(this->lcp));
			tmp += ("\t");

			INDEX begin = sa[this->i];
			for (INDEX j = 0; j < this->lcp; ++j)
			{
				if (text[begin + j] != 0)
				{
					tmp.push_back(text[begin + j]);
				}
				else
				{
					tmp += ("$(special end character)");
				}
			}

			return tmp;
		}

		/**
		 * @brief Compute the text represented as this LCP interval 
		 * @param text The input text T
		 * @param sa The suffix array of T
		 * @return The text string represented as this LCP interval
		 */
		template <typename SA>
		std::string get_text(std::vector<char> &text, SA &sa)
		{
			std::string intervalText = "";
			INDEX begin = sa[this->i];
			for (INDEX j = 0; j < this->lcp; ++j)
			{
				intervalText.push_back(text[begin + j]);
			}
			return intervalText;
		}

		/**
		 * @brief Checks if a text position p is contained within this LCP interval
		 * @param sa The suffix array of T
		 * @param pos a position p in T
		 * @return true if there exists an occurrence P[i'..j'] in T such that p in [i', j'], where P is the text represented as this LCP interval
		 */
		bool contains_position(std::vector<uint64_t> &sa, uint64_t pos) const
		{
			for (uint64_t x = this->i; x <= this->j; x++)
			{
				if (sa[x] <= pos && pos <= sa[x] + this->lcp - 1)
				{
					return true;
				}
			}
			return false;
		}

	
	

	public:
		/**
		 * @brief Computes all LCP intervals in suffix array
		 * @param lcpArray The LCP array of a string T
		 * @return LCP intervals
		 */
		static std::vector<LCPInterval> compute_lcp_intervals(const std::vector<INDEX> &lcpArray)
		{
			using TMP = std::pair<INDEX, INDEX>;
			std::stack<TMP> st;
			st.push(TMP(0, 0));
			std::vector<LCPInterval> r;

			for (INDEX i = 1; i < lcpArray.size(); i++)
			{
				INDEX _next_lcp_interval_i_candidate = i - 1;
				auto top = st.top();

				while (top.second > lcpArray[i])
				{
					r.push_back(LCPInterval<INDEX>(top.first, i - 1, top.second));
					st.pop();

					_next_lcp_interval_i_candidate = top.first;
					top = st.top();
				}
				if (top.second < lcpArray[i])
				{
					st.push(TMP(_next_lcp_interval_i_candidate, lcpArray[i]));
				}
			}
			while (st.size() > 0)
			{
				auto top = st.top();
				r.push_back(LCPInterval<INDEX>(top.first, lcpArray.size() - 1, top.second));
				st.pop();
			}

			return r;
		}

	};

	

	


}