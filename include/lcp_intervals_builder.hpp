
#pragma once
#include <cassert>
#include <chrono>
#include <stack>
#include "./sa_bwt_lcp.hpp"

namespace stool
{

  template <typename INDEX = uint64_t>
  std::vector<LCPInterval<INDEX>> compute_complete_lcp_intervals(std::vector<INDEX> &lcpArray)
  {
    using TMP = std::pair<INDEX, INDEX>;
    std::stack<TMP> st;
    std::vector<stool::LCPInterval<INDEX>> r;
    lcpArray.push_back(0);

    st.push(TMP(0, 0));
    for (INDEX i = 1; i < lcpArray.size(); i++)
    {
      std::cout << "look: LCParray[" << i << "]=" << lcpArray[i] << std::endl;
      INDEX lb = i - 1;
      auto top = st.top();
      while (lcpArray[i] < top.second)
      {
        INDEX rb = i - 1;
        r.push_back(stool::LCPInterval<INDEX>(top.first, rb, top.second));
        std::cout << "report: " << r[r.size() - 1].to_string() << std::endl;
        st.pop();
        top = st.top();
        lb = top.first;
      }
      if (lcpArray[i] > top.second)
      {
        std::cout << "push: [" << lb << ", " << lcpArray[i] << "]"
                  << "top = " << top.first << ", " << top.second << std::endl;
        if (lb == top.first && lb != 0)
        {
          st.pop();
        }
        st.push(TMP(lb, lcpArray[i]));
      }
    }
    r.push_back(stool::LCPInterval<INDEX>(0, lcpArray.size() - 2, 0));

    return r;
  }

  template <typename INDEX = uint64_t>
  INDEX getBottomIndex(stool::LCPInterval<INDEX> interval, std::vector<INDEX> &lcpArray)
  {
    uint64_t i = interval.i + 1;
    for (uint64_t x = interval.i + 1; x <= interval.j; x++)
    {
      if (lcpArray[x] < lcpArray[i])
      {
        i = x;
      }
    }
    return i;
  }
  template <typename INDEX = uint64_t>
  std::vector<stool::LCPInterval<INDEX>> get_bottom_lcp_intervals(std::vector<stool::LCPInterval<INDEX>> &lcpIntervals, uint64_t i, std::vector<INDEX> &lcpArray)
  {
    std::vector<stool::LCPInterval<INDEX>> r;
    for (auto it : lcpIntervals)
    {
      if (it.i <= i && i <= it.j)
      {
        INDEX bottom = getBottomIndex(it, lcpArray);
        if (bottom <= i)
        {
          r.push_back(it);
        }
      }
    }

    std::sort(
        r.begin(),
        r.end(),
        [](auto const &lhs, auto const &rhs)
        {
          if (lhs.j != rhs.j)
          {
            return lhs.j < rhs.j;
          }
          else
          {
            return lhs.lcp > rhs.lcp;
          }
        });
    return r;
  }
  template <typename INDEX = uint64_t>
  void print_bottom_lcp_intervals(std::vector<stool::LCPInterval<INDEX>> &lcpIntervals, std::vector<INDEX> &lcpArray)
  {
    for (INDEX i = 0; i < lcpArray.size(); i++)
    {
      auto r = get_bottom_lcp_intervals(lcpIntervals, i, lcpArray);
      std::cout << "i = " << i << " # " << std::flush;
      for (auto it : r)
      {
        std::cout << it.to_string() << std::flush;
      }
      std::cout << std::endl;
    }
  }
  template <typename INDEX = uint64_t>
  void print_bottom_lcp_intervals(std::stack<std::pair<INDEX, INDEX>> &stack, uint64_t i)
  {
    std::vector<std::pair<INDEX, INDEX>> r;
    while (stack.size() > 0)
    {
      auto top = stack.top();
      r.push_back(top);
      stack.pop();
    }
    std::cout << "i = " << i << " # " << std::flush;
    for (auto it : r)
    {
      std::cout << "[" << it.first << ", _ ," << it.second << "]" << std::flush;
    }
    std::cout << std::endl;
    for (int64_t i = r.size() - 1; i >= 0; --i)
    {
      stack.push(r[i]);
    }
  }

  template <typename INDEX = uint64_t>
  std::vector<stool::LCPInterval<INDEX>> compute_complete_lcp_intervals2(std::vector<INDEX> &lcpArray)
  {
    using TMP = std::pair<INDEX, INDEX>;
    std::stack<TMP> st;
    st.push(TMP(0, 0));
    std::vector<stool::LCPInterval<INDEX>> r;
    // lcpArray.push_back(0);

    for (INDEX i = 1; i < lcpArray.size(); i++)
    {
      // std::cout << "look: LCParray[" << i << "]=" << lcpArray[i] << std::endl;
      INDEX _next_lcp_interval_i_candidate = i - 1;
      auto top = st.top();

      while (top.second > lcpArray[i])
      {
        r.push_back(LCPInterval<INDEX>(top.first, i - 1, top.second));
        st.pop();
        // std::cout << "update: " << top.first << std::endl;
        // std::cout << "report: " << LCPInterval<INDEX>(top.first, i - 1, top.second).to_string() << std::endl;

        _next_lcp_interval_i_candidate = top.first;
        top = st.top();
      }
      if (top.second < lcpArray[i])
      {
        // std::cout << "push: " << _next_lcp_interval_i_candidate << ", " << lcpArray[i] << std::endl;

        st.push(TMP(_next_lcp_interval_i_candidate, lcpArray[i]));
      }
      print_bottom_lcp_intervals(st, i);
    }
    while (st.size() > 0)
    {
      auto top = st.top();
      r.push_back(LCPInterval<INDEX>(top.first, lcpArray.size() - 1, top.second));
      st.pop();
    }

    // r.push_back(stool::LCPInterval<INDEX>(0, lcpArray.size() - 1, 0));

    return r;
  }
}
