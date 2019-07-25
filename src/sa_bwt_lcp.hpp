#pragma once
#include <unordered_set>
#include <cassert>
#include "divsufsort.h"
#include "divsufsort64.h"


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
		//if(!b) std::cout << "b" << std::endl;
		return b;
	}
	std::string to_string()
	{
		return "[" + std::to_string(i) + ", " + std::to_string(j) + ", " + std::to_string(lcp) + "]";
	}
	bool is_special_marker()
	{
		return this->i == std::numeric_limits<index_type>::max() && this->j == std::numeric_limits<index_type>::max() && this->lcp == std::numeric_limits<index_type>::max();
	}

	template <typename sa_type>
	void print(uint64_t id, std::vector<char> &text, sa_type &sa)
	{
		std::cout << id << "\t\t" << (this->j - this->i + 1) << "\t\t" << this->i << ".." << this->j << "\t\t" << this->lcp << "\t\t";
		int64_t begin = sa[this->i];
		for (int64_t j = 0; j < this->lcp; ++j)
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
	template <typename sa_type>
	std::string getCSVLine(uint64_t id, std::vector<char> &text, sa_type &sa)
	{
		std::string tmp = "";
		std::string intervalText = this->getText(text, sa);
		tmp = std::to_string(id) + "," + std::to_string(this->j - this->i + 1) + "," + std::to_string(this->i) + "," + std::to_string(this->j) + "," + intervalText;
		return tmp;
	}

	template <typename sa_type>
	std::string getText(std::vector<char> &text, sa_type &sa)
	{
		std::string intervalText = "";
		int64_t begin = sa[this->i];
		for (int64_t j = 0; j < this->lcp; ++j)
		{
			intervalText.push_back(text[begin + j]);
		}
		return intervalText;
	}
	bool containsPosition(std::vector<uint64_t> &sa, uint64_t pos) const
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
};

template <typename CHAR = uint8_t,typename INDEX = uint64_t>
std::vector<INDEX> constructSA(std::vector<CHAR> &text);
//template std::vector<uint64_t> constructSA<char,uint64_t>(std::vector<char>&);


template <typename CHAR = char,typename INDEX = uint64_t>
std::vector<INDEX> constructISA(std::vector<CHAR> &text, std::vector<INDEX> &sa);

template <typename CHAR = char,typename INDEX = uint64_t>
std::vector<INDEX> constructLCP(std::vector<CHAR> &text, std::vector<INDEX> &sa, std::vector<INDEX> &isa);
template <typename CHAR = char,typename INDEX = uint64_t>
std::vector<INDEX> constructLCP(std::vector<CHAR> &text, std::vector<INDEX> &sa);

template <typename CHAR,typename INDEX>
std::vector<CHAR> constructBWT(std::vector<CHAR> &text, std::vector<INDEX> &sa);


} // namespace stool