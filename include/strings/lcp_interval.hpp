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
	 * @brief Represents an LCP (Longest Common Prefix) interval in a suffix array [Unchecked AI's Comment] 
	 * 
	 * An LCP interval represents a range of suffixes in a suffix array that share
	 * a common prefix of a specific length. This is a fundamental structure used
	 * in suffix array-based string algorithms.
	 * 
	 * @tparam index_type The type used for indexing (typically uint64_t or uint32_t)
	 */
	template <typename index_type>
	class LCPInterval
	{
	public:
		index_type i;  ///< Starting position in the suffix array
		index_type j;  ///< Ending position in the suffix array  
		index_type lcp; ///< Length of the longest common prefix

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
		LCPInterval(index_type _i, index_type _j, index_type _lcp)
		{
			this->i = _i;
			this->j = _j;
			this->lcp = _lcp;
		}

		/**
		 * @brief Creates a special end marker interval
		 * @return An LCP interval with maximum values for all fields
		 */
		static LCPInterval<index_type> create_end_marker()
		{
			return LCPInterval<index_type>(std::numeric_limits<index_type>::max(), std::numeric_limits<index_type>::max(), std::numeric_limits<index_type>::max());
		}

		/**
		 * @brief Less-than comparison operator
		 * @param right The right-hand side LCP interval to compare with
		 * @return true if this interval is less than the right interval
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
		 * @param rhs The right-hand side LCP interval to compare with
		 * @return true if the intervals are equal
		 */
		bool operator==(const LCPInterval &rhs) const
		{
			const LCPInterval &lhs = *this;
			bool b = lhs.i == rhs.i && lhs.j == rhs.j && lhs.lcp == rhs.lcp;
			// if(!b) std::cout << "b" << std::endl;
			return b;
		}

		/**
		 * @brief Inequality comparison operator
		 * @param rhs The right-hand side LCP interval to compare with
		 * @return true if the intervals are not equal
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
		 * @brief Checks if this interval is a special marker
		 * @return true if this is a special marker interval
		 */
		bool is_special_marker() const
		{
			return this->i == std::numeric_limits<index_type>::max() && this->j == std::numeric_limits<index_type>::max() && this->lcp == std::numeric_limits<index_type>::max();
		}

		/**
		 * @brief Generates a CSV line representation of the interval
		 * @param id The identifier for this interval
		 * @param text The input text
		 * @param sa The suffix array
		 * @return CSV formatted string with interval information
		 */
		template <typename sa_type>
		std::string get_CSV_line(uint64_t id, std::vector<char> &text, sa_type &sa)
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

			index_type begin = sa[this->i];
			for (index_type j = 0; j < this->lcp; ++j)
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

			/*
			std::string tmp = "";
			std::string intervalText = this->getText(text, sa);
			tmp = std::to_string(id) + "," + std::to_string(this->j - this->i + 1) + "," + std::to_string(this->i) + "," + std::to_string(this->j) + "," + intervalText;
			return tmp;
			*/
		}

		/**
		 * @brief Extracts the text corresponding to this LCP interval
		 * @param text The input text
		 * @param sa The suffix array
		 * @return The text string corresponding to this interval
		 */
		template <typename sa_type>
		std::string getText(std::vector<char> &text, sa_type &sa)
		{
			std::string intervalText = "";
			index_type begin = sa[this->i];
			for (index_type j = 0; j < this->lcp; ++j)
			{
				intervalText.push_back(text[begin + j]);
			}
			return intervalText;
		}

		/**
		 * @brief Checks if a position is contained within this interval
		 * @param sa The suffix array
		 * @param pos The position to check
		 * @return true if the position is contained in this interval
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

	private:
		/**
		 * @brief Compares a substring with a pattern
		 * @param T The text
		 * @param pattern The pattern to compare
		 * @param suf_pos The suffix position
		 * @param isBeg Whether this is a beginning comparison
		 * @param isComp The comparison type
		 * @return Comparison result
		 */
		template <typename CHAR = char>
		static bool compareSubstr(const std::vector<CHAR> &T, const std::vector<CHAR> &pattern, uint64_t suf_pos, bool isBeg, bool isComp)
		{
			uint64_t suf_len = T.size() - suf_pos;
			uint64_t min = pattern.size() < suf_len ? pattern.size() : suf_len;
			for (size_t i = 0; i < min; i++)
			{
				if (pattern[i] != T[suf_pos + i])
				{
					return isComp ? pattern[i] < T[suf_pos + i] : pattern[i] > T[suf_pos + i];
				}
			}
			uint64_t sufLastChar = suf_len > min ? 2 : 1;
			uint64_t patternLastChar = pattern.size() > min ? 2 : isBeg ? 0
																		: 3;

			return isComp ? patternLastChar < sufLastChar : patternLastChar > sufLastChar;
		}

	public:
		/**
		 * @brief Computes all LCP intervals from an LCP array
		 * @param lcpArray The LCP array
		 * @return Vector of LCP intervals
		 */
		static std::vector<LCPInterval> compute_lcp_intervals(const std::vector<index_type> &lcpArray)
		{
			using TMP = std::pair<index_type, index_type>;
			std::stack<TMP> st;
			st.push(TMP(0, 0));
			std::vector<LCPInterval> r;

			for (index_type i = 1; i < lcpArray.size(); i++)
			{
				index_type _next_lcp_interval_i_candidate = i - 1;
				auto top = st.top();

				while (top.second > lcpArray[i])
				{
					r.push_back(LCPInterval<index_type>(top.first, i - 1, top.second));
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
				r.push_back(LCPInterval<index_type>(top.first, lcpArray.size() - 1, top.second));
				st.pop();
			}

			return r;
		}

		/**
		 * @brief Computes the LCP interval for a specific pattern
		 * @param T The text
		 * @param pattern The pattern to search for
		 * @param sa The suffix array
		 * @return The LCP interval corresponding to the pattern
		 */
		template <typename CHAR = char>
		static stool::LCPInterval<index_type> compute_lcp_intervals(const std::vector<CHAR> &T, const std::vector<CHAR> &pattern, const std::vector<index_type> &sa)
		{
			uint64_t p = T.size();
			auto beg = std::upper_bound(
				sa.begin(),
				sa.end(),
				p,
				[&](const index_type &x, const index_type &y)
				{
					if (x == T.size())
					{
						bool b = LCPInterval::compareSubstr(T, pattern, y, true, true);
						return b;
					}
					else
					{
						bool b = LCPInterval::compareSubstr(T, pattern, x, true, false);
						return b;
					}
				});

			auto end = std::upper_bound(
				sa.begin(),
				sa.end(),
				p,
				[&](const index_type &x, const index_type &y)
				{
					if (x == T.size())
					{
						bool b = LCPInterval::compareSubstr(T, pattern, y, false, true);
						return b;
					}
					else
					{
						bool b = LCPInterval::compareSubstr(T, pattern, y, false, false);
						return b;
					}
				});

			std::size_t begi = std::distance(sa.begin(), beg);
			std::size_t endi = std::distance(sa.begin(), end) - 1;

			return stool::LCPInterval<index_type>(begi, endi, pattern.size());
		}
	};

	/**
	 * @brief Comparator for LCP intervals in preorder traversal
	 * @tparam INDEX The index type
	 */
	template <typename INDEX = uint64_t>
	struct LCPIntervalPreorderComp
	{
		/**
		 * @brief Compares two LCP intervals for preorder sorting
		 * @param x The first LCP interval
		 * @param y The second LCP interval
		 * @return true if x should come before y in preorder
		 */
		bool operator()(const LCPInterval<INDEX> &x, const LCPInterval<INDEX> &y)
		{
			if (x.i == y.i)
			{
				if (x.j == y.j)
				{
					return x.lcp < y.lcp;
				}
				else
				{
					return x.j > y.j;
				}
			}
			else
			{
				return x.i < y.i;
			}
		}
	};

	/**
	 * @brief Comparator for LCP intervals in depth-first order
	 * @tparam INDEX The index type
	 */
	template <typename INDEX = uint64_t>
	struct LCPIntervalDepthOrderComp
	{
		/**
		 * @brief Compares two LCP intervals for depth-first sorting
		 * @param x The first LCP interval
		 * @param y The second LCP interval
		 * @return true if x should come before y in depth-first order
		 */
		bool operator()(const LCPInterval<INDEX> &x, const LCPInterval<INDEX> &y)
		{
			if (x.lcp == y.lcp)
			{
				if (x.i == y.i)
				{
					return x.j < y.j;
				}
				else
				{
					return x.i < y.i;
				}
			}
			else
			{
				return x.lcp < y.lcp;
			}
		}
	};

	/**
	 * @brief Utility class for sorting LCP intervals
	 */
	class LCPIntervalSort
	{
		public: 
		/**
		 * @brief Sorts LCP intervals in preorder
		 * @param items Vector of LCP intervals to sort
		 */
		template <typename INDEX = uint64_t>
		static void sort_in_preorder(std::vector<stool::LCPInterval<INDEX>> &items)
		{
			std::sort(
				items.begin(),
				items.end(),
				stool::LCPIntervalPreorderComp<INDEX>());
		}
	};

}