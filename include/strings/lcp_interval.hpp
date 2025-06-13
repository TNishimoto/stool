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
	template <typename index_type>
	class LCPInterval
	{
	public:
		index_type i;
		index_type j;
		index_type lcp;
		LCPInterval() {}
		LCPInterval(index_type _i, index_type _j, index_type _lcp)
		{
			this->i = _i;
			this->j = _j;
			this->lcp = _lcp;
		}

		static LCPInterval<index_type> create_end_marker()
		{
			return LCPInterval<index_type>(std::numeric_limits<index_type>::max(), std::numeric_limits<index_type>::max(), std::numeric_limits<index_type>::max());
		}
		/*
		bool operator<(const LCPInterval<index_type> &right) const
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
		bool operator==(const LCPInterval &rhs) const
		{
			const LCPInterval &lhs = *this;
			bool b = lhs.i == rhs.i && lhs.j == rhs.j && lhs.lcp == rhs.lcp;
			// if(!b) std::cout << "b" << std::endl;
			return b;
		}
		bool operator!=(const LCPInterval &rhs) const
		{
			return !(*this == rhs);
		}

		std::string to_string() const
		{
			return "[" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(lcp) + "]";
		}
		bool is_special_marker() const
		{
			return this->i == std::numeric_limits<index_type>::max() && this->j == std::numeric_limits<index_type>::max() && this->lcp == std::numeric_limits<index_type>::max();
		}
		/*
		template <typename sa_type>
		void print(uint64_t id, std::vector<char> &text, sa_type &sa)
		{
			std::cout << id << "\t" << (this->j - this->i + 1) << "\t" << this->i << ".." << this->j << "\t" << this->lcp << "\t";
			index_type begin = sa[this->i];
			for (index_type j = 0; j < this->lcp; ++j)
			{
				if (text[begin + j] != 0)
				{
					std::cout << text[begin + j];
				}
				else
				{
					std::cout << "$(special end character)";
				}
			}
			std::cout << std::endl;
		}
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

		template <typename CHAR = char>
		static stool::LCPInterval<index_type> compute_LCP_interval(const std::vector<CHAR> &T, const std::vector<CHAR> &pattern, const std::vector<index_type> &sa)
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

	template <typename INDEX = uint64_t>
	struct LCPIntervalPreorderComp
	{
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

	template <typename INDEX = uint64_t>
	struct LCPIntervalDepthOrderComp
	{
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

	class LCPIntervalSort
	{
		public: 
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