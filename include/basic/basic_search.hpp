#pragma once
#include <vector>
namespace stool
{

    ////////////////////////////////////////////////////////////////////////////////
    /// @class      BasicSearch
    /// @brief      Predecessors and successors on vectors.
    ///
    ////////////////////////////////////////////////////////////////////////////////
    class BasicSearch
    {
    public:
        template <typename T>
        static int64_t predecessor(const std::vector<T> &vec, T item)
        {
            int64_t size = vec.size();
            for (int64_t i = size - 1; i >= 0; i--)
            {
                if (vec[i] <= item)
                {
                    return i;
                }
            }
            return -1;
        }

        template <typename T>
        static int64_t successor(const std::vector<T> &vec, T item)
        {
            int64_t size = vec.size();
            for (int64_t i = 0; i < size; i++)
            {
                if (vec[i] >= item)
                {
                    return i;
                }
            }
            return -1;
        }
    };

}
