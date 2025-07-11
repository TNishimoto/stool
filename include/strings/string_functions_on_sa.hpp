#pragma once
#include <string>
#include <vector>
#include <cassert>
#include "./sa_bwt_lcp.hpp"
#include "./lcp_interval.hpp"
namespace stool
{

    class StringFunctionsOnSA
    {
    public:
        using Interval = std::pair<int64_t, int64_t>;
        static Interval compute_sa_interval(const std::vector<uint8_t> &text, const std::vector<uint8_t> &pattern, const std::vector<uint64_t> &sa)
        {
            std::vector<uint64_t> r;
            auto intv = stool::LCPInterval<uint64_t>::compute_lcp_intervals(text, pattern, sa);
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

        template <typename CHAR, typename INDEX = uint64_t>
        static std::vector<stool::LCPInterval<INDEX>> naive_compute_lcp_intervals(const std::vector<CHAR> &text, const std::vector<INDEX> &sa)
        {
            uint64_t minChar = *std::min_element(text.begin(), text.end());
            std::vector<uint64_t> minCharIndexes;
            for (uint64_t i = 0; i < text.size(); i++)
            {
                if ((uint64_t)text[i] == minChar)
                {
                    minCharIndexes.push_back(i);
                }
            }

            std::vector<stool::LCPInterval<INDEX>> r;
            std::vector<INDEX> lcpArray = stool::ArrayConstructor::construct_LCP_array<CHAR, INDEX>(text, sa, stool::Message::NO_MESSAGE);
            for (uint64_t i = 0; i < sa.size(); i++)
            {
                uint64_t limit_lcp = i == 0 ? 0 : lcpArray[i];
                uint64_t current_lcp = sa.size() - sa[i];
                for (uint64_t x = i + 1; x <= sa.size(); x++)
                {
                    uint64_t lcp = x == sa.size() ? 0 : lcpArray[x];
                    // std::cout << i << "/" << x << "/" << current_lcp << "/"<< lcp << "/" << limit_lcp<< std::endl;

                    if (current_lcp > lcp)
                    {
                        auto newIntv = stool::LCPInterval<INDEX>(i, x - 1, current_lcp);
                        if (check_lcp_interval(newIntv, sa, minCharIndexes))
                        {
                            r.push_back(newIntv);
                        }

                        current_lcp = lcp;
                    }

                    if (current_lcp <= limit_lcp)
                    {
                        break;
                    }
                }
            }
            r.push_back(stool::LCPInterval<INDEX>(0, sa.size() - 1, 0));

            std::sort(
                r.begin(),
                r.end(),
                LCPIntervalPreorderComp<INDEX>());

            return r;
        }

        template <typename INDEX = uint64_t>
        static bool check_lcp_interval(const stool::LCPInterval<INDEX> &intv, const std::vector<INDEX> &sa, const std::vector<INDEX> &charMinIndexes)
        {
            uint64_t left = sa[intv.i];
            uint64_t right = sa[intv.i] + intv.lcp - 1;
            bool b = true;
            for (auto &it : charMinIndexes)
            {
                if (left <= it && it <= right)
                {
                    b = false;
                }
            }

            return b && (intv.i < intv.j);
        }
    };
} // namespace stool