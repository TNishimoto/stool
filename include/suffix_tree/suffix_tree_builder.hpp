#pragma once
#include <queue>
#include "../basic/log.hpp"
#include "../debug/debug_printer.hpp"
#include "../strings/lcp_interval.hpp"
#include "../rmq/rmq_sparse_table.hpp"

namespace stool
{
    /**
     * @brief A simple suffix tree implementation using LCP intervals [Unchecked AI's Comment] 
     * 
     * This class represents a suffix tree built from LCP (Longest Common Prefix) intervals.
     * It provides methods to determine node types (leaf, root, internal) and construction
     * utilities for building the tree structure from sorted LCP intervals.
     */
    class SimpleSuffixTree
    {
        const std::vector<uint8_t> *text;
        const std::vector<uint64_t> *suffix_array;

        std::vector<stool::LCPInterval<uint64_t>> sorted_lcp_intervals_with_leaves;
        std::vector<std::vector<uint64_t>> children_array;
        std::vector<uint64_t> parent_array;

    public:
        /**
         * @brief Check if a node is a leaf node
         * 
         * @param node_index The index of the node to check
         * @return true if the node is a leaf (has no children), false otherwise
         */
        bool is_leaf(uint64_t node_index)
        {
            return children_array[node_index].size() == 0;
        }
        
        /**
         * @brief Check if a node is the root node
         * 
         * @param node_index The index of the node to check
         * @return true if the node is the root (has no parent), false otherwise
         */
        bool is_root(uint64_t node_index)
        {
            return parent_array[node_index] == UINT64_MAX;
        }
        
        /**
         * @brief Check if a node is an internal node
         * 
         * @param node_index The index of the node to check
         * @return true if the node is internal (has both parent and children), false otherwise
         */
        bool is_internal(uint64_t node_index)
        {
            return !is_leaf(node_index) && !is_root(node_index);
        }

        /**
         * @brief Build a SimpleSuffixTree from text, suffix array, and LCP intervals
         * 
         * @param text Pointer to the input text
         * @param suffix_array Pointer to the suffix array
         * @param sorted_lcp_intervals_with_leaves Vector of sorted LCP intervals
         * @return A constructed SimpleSuffixTree object
         */
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
        
        /**
         * @brief Build sorted LCP intervals with leaf nodes included
         * 
         * This function adds leaf nodes (representing individual suffixes) to the sorted LCP intervals
         * and sorts them in preorder traversal order.
         * 
         * @param sorted_lcp_intervals The original sorted LCP intervals
         * @param suffix_array The suffix array
         * @return Vector of LCP intervals including leaf nodes, sorted in preorder
         */
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

        /**
         * @brief Build the parent array for the suffix tree nodes
         * 
         * This function constructs the parent-child relationships between nodes
         * based on the sorted LCP intervals. Each node's parent is determined
         * by finding the smallest interval that contains it.
         * 
         * @param sorted_lcp_intervals Vector of LCP intervals sorted in preorder
         * @return Vector where each element contains the parent index of the corresponding node
         */
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
        
        /**
         * @brief Build the children array for the suffix tree nodes
         * 
         * This function constructs the children relationships for each node
         * based on the parent array. Each node's children are all nodes that
         * have this node as their parent.
         * 
         * @param sorted_lcp_intervals Vector of LCP intervals
         * @param parent_array Vector containing parent indices for each node
         * @return Vector of vectors where each inner vector contains the child indices of the corresponding node
         */
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