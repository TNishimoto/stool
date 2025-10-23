#pragma once
#include <string>
#include <vector>
#include <cassert>
#include "./array_constructor.hpp"
#include "./lcp_interval.hpp"
#include "./lcp_interval_comparator_in_preorder.hpp"
namespace stool
{

    /**
     * @brief A utility class providing string functions on suffix arrays
     */
    class StringFunctionsOnSA
    {
    private:
        /**
         * @brief Compares a suffix \p T[pos..] of a text with a given pattern \p P
         * @return 0 if the suffix is equal to the pattern, -1 if the suffix is lexicographically less than the pattern, 1 if the suffix is lexicographically greater than the pattern
         */
        static int compare_suffix_with_pattern(const std::vector<uint8_t> &T, size_t pos, const std::vector<uint8_t> &P)
        {
            size_t n = T.size();
            size_t m = P.size();
            if (pos > n)
                return -1;

            size_t avail = n - pos;
            size_t len = std::min(avail, m);

            int c = std::memcmp(T.data() + pos, P.data(), len);
            if (c != 0)
                return (c < 0) ? -1 : 1;

            if (m == len)
                return 0;
            return -1;
        }

        /**
         * @brief Finds the lower bound of a pattern in a suffix array using binary search
         */
        static size_t lower_bound_on_suffix_array(const std::vector<uint8_t> &T, const std::vector<uint64_t> &SA, const std::vector<uint8_t> &P)
        {
            size_t lo = 0, hi = SA.size();
            while (lo < hi)
            {
                size_t mid = (lo + hi) / 2;
                int cmp = compare_suffix_with_pattern(T, static_cast<size_t>(SA[mid]), P);
                if (cmp < 0)
                    lo = mid + 1;
                else
                    hi = mid;
            }
            return lo;
        }

         /**
         * @brief This function is used only by \p naive_compute_lcp_intervals function.
         */
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

    public:
        using Interval = std::pair<int64_t, int64_t>;

        /**
         * @brief Computes the suffix array interval \p [L..R] (sa-interval) of a given pattern \p P on the suffix array \p SA of a string \p T.
         * 
         * @return The sa-interval \p [L..R] if it exists, otherwise \p [-1,-1].
         */
        static Interval compute_sa_interval(const std::vector<uint8_t> &T, const std::vector<uint8_t> &P, const std::vector<uint64_t> &SA)
        {
            // vector<int> res;
            size_t n = T.size();

            if (P.empty())
            {
                return Interval(0, n - 1);
            }

            std::vector<uint8_t> P_hi = P;
            P_hi.push_back(0xFF);

            size_t L = lower_bound_on_suffix_array(T, SA, P);
            size_t R = lower_bound_on_suffix_array(T, SA, P_hi);

            if (L <= R)
            {
                return Interval(L, R - 1);
            }
            else
            {
                return Interval(-1, -1);
            }
        }

        /**
         * @brief Locates all occurrences of a pattern \p P in the text \p T using the binary search on the suffix array \p SA.
         */
        static std::vector<uint64_t> locate_query(const std::vector<uint8_t> &T, const std::vector<uint8_t> &P, const std::vector<uint64_t> &SA)
        {
            std::vector<uint64_t> r;
            auto interval = compute_sa_interval(T, P, SA);
            if (interval.first != -1)
            {
                for (int64_t x = interval.first; x <= interval.second; x++)
                {
                    r.push_back(SA[x]);
                }
                std::sort(r.begin(), r.end());
            }
            return r;
        }

        /**
         * @brief Computes all LCP intervals in the suffix array \p SA of a string \p T using a naive approach.
         */
        template <typename CHAR, typename INDEX = uint64_t>
        static std::vector<stool::LCPInterval<INDEX>> naive_compute_lcp_intervals(const std::vector<CHAR> &T, const std::vector<INDEX> &SA)
        {
            uint64_t minChar = *std::min_element(T.begin(), T.end());
            std::vector<uint64_t> minCharIndexes;
            for (uint64_t i = 0; i < T.size(); i++)
            {
                if ((uint64_t)T[i] == minChar)
                {
                    minCharIndexes.push_back(i);
                }
            }

            std::vector<stool::LCPInterval<INDEX>> r;
            std::vector<INDEX> lcpArray = stool::ArrayConstructor::construct_LCP_array<CHAR, INDEX>(T, SA, stool::Message::NO_MESSAGE);
            for (uint64_t i = 0; i < SA.size(); i++)
            {
                uint64_t limit_lcp = i == 0 ? 0 : lcpArray[i];
                uint64_t current_lcp = SA.size() - SA[i];
                for (uint64_t x = i + 1; x <= SA.size(); x++)
                {
                    uint64_t lcp = x == SA.size() ? 0 : lcpArray[x];
                    // std::cout << i << "/" << x << "/" << current_lcp << "/"<< lcp << "/" << limit_lcp<< std::endl;

                    if (current_lcp > lcp)
                    {
                        auto newIntv = stool::LCPInterval<INDEX>(i, x - 1, current_lcp);
                        if (stool::StringFunctionsOnSA::check_lcp_interval(newIntv, SA, minCharIndexes))
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
            r.push_back(stool::LCPInterval<INDEX>(0, SA.size() - 1, 0));

            std::sort(
                r.begin(),
                r.end(),
                stool::LCPIntervalComparatorInPreorder<INDEX>());

            return r;
        }

        
    };
} // namespace stool