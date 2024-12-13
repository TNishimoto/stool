#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
#include "../specialized_collection/elias_fano_vector.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"  // suppress specific warning
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>
#pragma GCC diagnostic pop

namespace stool
{

    using WT = sdsl::wt_huff<>;

    namespace rlbwt2
    {
        template <typename T>
        class RankSupportVectorWrapper
        {
            T &items;

        public:
            RankSupportVectorWrapper(T &_items) : items(_items)
            {
            }
            uint64_t operator[](uint64_t index) const
            {
                return this->items[index];
            }
            uint64_t rank(uint64_t value) const
            {
                auto p2 = std::lower_bound(this->items.begin(), this->items.end(), value);
                uint64_t pos2 = std::distance(this->items.begin(), p2);

                return pos2;
            }
            uint64_t size() const
            {
                return this->items.size();
            }
        };
    } // namespace rlbwt2
} // namespace stool