#pragma once
#include <queue>
#include "../basic/log.hpp"
#include "../debug/print.hpp"
#include "../strings/lcp_interval.hpp"
#include "../rmq/rmq_sparse_table.hpp"

namespace stool
{
    class SimpleSuffixTree
    {
        const std::vector<uint8_t> *text;
        const std::vector<uint64_t> *suffix_array;

        std::vector<stool::LCPInterval<uint64_t>> sorted_lcp_intervals_with_leaves;
        std::vector<std::vector<uint64_t>> children_array;
        std::vector<uint64_t> parent_array;

    public:
        bool is_leaf(uint64_t node_index)
        {
            return children_array[node_index].size() == 0;
        }
        bool is_root(uint64_t node_index)
        {
            return parent_array[node_index] == UINT64_MAX;
        }
        bool is_internal(uint64_t node_index)
        {
            return !is_leaf(node_index) && !is_root(node_index);
        }

        static SimpleSuffixTree build(const std::vector<uint8_t> *text, const std::vector<uint64_t> *suffix_array, const std::vector<stool::LCPInterval<uint64_t>> &sorted_lcp_intervals_with_leaves)
        {
            SimpleSuffixTree st;
            st.text = text;
            st.suffix_array = suffix_array;
            st.sorted_lcp_intervals_with_leaves = build_sorted_lcp_intervals_with_leaves(sorted_lcp_intervals_with_leaves, *suffix_array);


            st.parent_array = build_parent_array(st.sorted_lcp_intervals_with_leaves);

            st.children_array = build_children_array(st.sorted_lcp_intervals_with_leaves, st.parent_array);

            return st;
        }
        static std::vector<stool::LCPInterval<uint64_t>> build_sorted_lcp_intervals_with_leaves(const std::vector<stool::LCPInterval<uint64_t>> &sorted_lcp_intervals, const std::vector<uint64_t> &suffix_array)
        {
            std::vector<stool::LCPInterval<uint64_t>> sorted_lcp_intervals_with_leaves = sorted_lcp_intervals;
            uint64_t n = suffix_array.size();
            for (uint64_t x = 0; x < n; x++)
            {
                sorted_lcp_intervals_with_leaves.push_back(stool::LCPInterval<uint64_t>(x, x, n - suffix_array[x]));
            }
            stool::LCPIntervalSort::sort_in_preorder(sorted_lcp_intervals_with_leaves);

            return sorted_lcp_intervals_with_leaves;
        }

        static std::vector<uint64_t> build_parent_array(const std::vector<stool::LCPInterval<uint64_t>> &sorted_lcp_intervals)
        {
            std::vector<uint64_t> parent_array;
            parent_array.resize(sorted_lcp_intervals.size(), UINT64_MAX);
            std::unordered_map<uint64_t, uint64_t> parent_map;
            parent_map[0] = UINT64_MAX;
            for (uint64_t x = 0; x < sorted_lcp_intervals.size(); x++)
            {
                auto f = parent_map.find(sorted_lcp_intervals[x].i);
                if (f == parent_map.end())
                {
                    std::cout << sorted_lcp_intervals[x].to_string() << std::endl;
                    throw std::runtime_error("The first interval must be the root interval.");
                }

                parent_array[x] = parent_map[sorted_lcp_intervals[x].i];
                if (parent_array[x] != UINT64_MAX)
                {

                    uint64_t parent_j = sorted_lcp_intervals[parent_array[x]].j;
                    if (sorted_lcp_intervals[x].j < parent_j)
                    {
                        parent_map[sorted_lcp_intervals[x].j + 1] = parent_array[x];
                    }
                }
                parent_map[sorted_lcp_intervals[x].i] = x;
            }
            return parent_array;
        }
        static std::vector<std::vector<uint64_t>> build_children_array(const std::vector<stool::LCPInterval<uint64_t>> &sorted_lcp_intervals, const std::vector<uint64_t> &parent_array)
        {
            std::vector<std::vector<uint64_t>> children_array;
            children_array.resize(sorted_lcp_intervals.size());
            for (uint64_t x = 0; x < parent_array.size(); x++)
            {
                if (parent_array[x] != UINT64_MAX)
                {
                    children_array[parent_array[x]].push_back(x);
                }
            }
            return children_array;
        }
    };

}