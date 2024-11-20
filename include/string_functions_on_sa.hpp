#pragma once
#include <string>
#include <vector>
#include <cassert>
#include "./sa_bwt_lcp.hpp"

namespace stool
{

    class StringFunctionsOnSA
    {
    public:
        using Interval = std::pair<int64_t, int64_t>;
        static Interval compute_sa_interval(const std::vector<uint8_t> &text, const std::vector<uint8_t> &pattern, const std::vector<uint64_t> &sa)
        {
            std::vector<uint64_t> r;
            auto intv = stool::LCPInterval<uint64_t>::computeLCPInterval(text, pattern, sa);
            if (intv.lcp != pattern.size())
            {
                return Interval(-1, -1);
            }
            else
            {
                return Interval(intv.i, intv.j);
            }
        }

        static std::vector<uint64_t> locate_query(const std::vector<uint8_t> &text, const std::vector<uint8_t> &pattern, const std::vector<uint64_t> &sa)
        {
            std::vector<uint64_t> r;
            auto interval = compute_sa_interval(text, pattern, sa);
            if (interval.first != -1)
            {
                for (int64_t x = interval.first; x <= interval.second; x++)
                {
                    r.push_back(sa[x]);
                }
                std::sort(r.begin(), r.end());
            }
            return r;
        }
    };
} // namespace stool