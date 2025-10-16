#pragma once
#include <string>
#include <vector>
#include <cassert>
#include "./array_constructor.hpp"
#include "./lcp_interval.hpp"
namespace stool
{

    /**
     * @brief A utility class providing string functions that operate on suffix arrays [Unchecked AI's Comment] 
     * 
     * This class contains static methods for performing various string operations
     * using suffix arrays, including pattern matching, LCP interval computation,
     * and query location.
     */
    class StringFunctionsOnSA
    {
    private: 
    static int cmp_suffix_with_pat(const std::vector<uint8_t> &T, size_t pos, const std::vector<uint8_t> &P) {
        size_t n = T.size();
        size_t m = P.size();
        if (pos > n) return -1;
    
        size_t avail = n - pos;
        size_t len = std::min(avail, m);
    
        int c = memcmp(T.data() + pos, P.data(), len);
        if (c != 0) return (c < 0) ? -1 : 1;
    
        if (m == len) return 0; 
        return -1;
    }
    static size_t lower_bound_sa(const std::vector<uint8_t> &T, const std::vector<uint64_t> &SA, const std::vector<uint8_t> &P) {
        size_t lo = 0, hi = SA.size();
        while (lo < hi) {
            size_t mid = (lo + hi) / 2;
            int cmp = cmp_suffix_with_pat(T, static_cast<size_t>(SA[mid]), P);
            if (cmp < 0) lo = mid + 1;
            else hi = mid;
        }
        return lo;
    }

    public:
        using Interval = std::pair<int64_t, int64_t>;
        
        
        static Interval compute_sa_interval([[maybe_unused]] const std::vector<uint8_t> &T, [[maybe_unused]] const std::vector<uint8_t> &P, [[maybe_unused]] const std::vector<uint64_t> &sa)
        {
            //vector<int> res;
            size_t n = T.size();
        
            if (P.empty()) { 
                return Interval(0, n - 1);
            }
        
            std::vector<uint8_t> P_hi = P;
            P_hi.push_back(0xFF);
        
            size_t L = lower_bound_sa(T, sa, P);
            size_t R = lower_bound_sa(T, sa, P_hi);

            if(L <= R)
            {
                return Interval(L, R - 1);
            }
            else
            {
                return Interval(-1, -1);
            }


            /*
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
            */
        }
        

        /**
         * @brief Locates all occurrences of a pattern in the text using suffix array
         * 
         * This function finds all positions in the text where the pattern occurs.
         * It first computes the suffix array interval for the pattern, then
         * retrieves all positions within that interval and sorts them.
         * 
         * @param text The input text as a vector of bytes
         * @param pattern The pattern to search for as a vector of bytes
         * @param sa The suffix array of the text
         * @return A vector containing all positions where the pattern occurs in the text
         */
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

        /**
         * @brief Computes LCP intervals using a naive approach
         * 
         * This function computes all LCP intervals in the suffix array using a
         * straightforward algorithm. It iterates through the suffix array and
         * constructs intervals based on the LCP array values.
         * 
         * @tparam CHAR The character type used in the text
         * @tparam INDEX The index type for suffix array positions
         * @param text The input text as a vector of characters
         * @param sa The suffix array of the text
         * @return A vector of LCP intervals sorted in preorder
         */
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

        /**
         * @brief Checks if an LCP interval is valid based on minimum character positions
         * 
         * This function validates an LCP interval by ensuring that it doesn't contain
         * any positions where the minimum character occurs in the text. It also
         * checks that the interval has a valid range (i < j).
         * 
         * @tparam INDEX The index type for suffix array positions
         * @param intv The LCP interval to check
         * @param sa The suffix array
         * @param charMinIndexes Vector of positions where the minimum character occurs
         * @return true if the interval is valid, false otherwise
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
    };
} // namespace stool