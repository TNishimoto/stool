#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <set>
#include <queue>

// #include "sa_lcp.hpp"
// using namespace std;
// using namespace sdsl;

namespace stool
{

    /*!
     * @brief A class representing character intervals in text sequences [Unchecked AI's Comment] 
     * 
     * CharInterval provides a data structure for storing and manipulating intervals
     * associated with specific characters in text sequences. It includes:
     * - Start and end positions of the interval
     * - The character associated with the interval
     * - Methods for interval manipulation and comparison
     * 
     * The class is particularly useful for:
     * - Pattern matching algorithms
     * - Text indexing data structures
     * - Suffix array interval computations
     * 
     * @tparam INDEX The type used for interval positions
     * @tparam CHAR The character type for the interval
     */
    template <typename INDEX, typename CHAR>
    class CharInterval
    {
    public:
        INDEX i;
        INDEX j;
        CHAR c;
        CharInterval()
        {
        }
        CharInterval(INDEX _i, INDEX _j, CHAR _c) : i(_i), j(_j), c(_c)
        {
        }

        std::string to_string() const
        {
            std::string s = "a";
            s[0] = c;
            return "[" + std::to_string(i) + ", " + std::to_string(j) + ", " + s + "]";
        }
    };

} // namespace stool
