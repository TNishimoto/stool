#pragma once
#include <vector>
namespace stool
{

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief      Predecessors and successors on vectors.
    ///
    ////////////////////////////////////////////////////////////////////////////////
    class BasicSearch
    {
    public:

        /*! 
        * @brief Searches for the predecessor of an element in an array [Unchecked AI's Comment]
        * 
        * This function returns the index of the largest element less than or equal to 
        * the given value in the provided array. It uses linear search by scanning
        * the array from back to front. Returns -1 if no such element is found.
        *
        * @param vec The array to search in
        * @param item The value to search for
        * @return Index of the predecessor, or -1 if not found
        */


        template <typename T>
        static int64_t predecessor_by_linear_search(const std::vector<T> &vec, T item)
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


        /*! 
        * @brief Searches for the successor of an element in an array [Unchecked AI's Comment]
        * 
        * This function returns the index of the smallest element greater than or equal to
        * the given value in the provided array. It uses linear search by scanning
        * the array from front to back. Returns -1 if no such element is found.
        *
        * @param vec The array to search in
        * @param item The value to search for
        * @return Index of the successor, or -1 if not found
        */
        template <typename T>
        static int64_t successor_by_linear_search(const std::vector<T> &vec, T item)
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
