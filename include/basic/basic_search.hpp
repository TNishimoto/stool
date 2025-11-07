#pragma once
#include <vector>
namespace stool
{
    /// \defgroup BasicClasses Classes for basic operations
    /// Basic functions
    /// @{
    /// @}



    /**
     * @brief Predecessors and successors on vectors.
     * \ingroup BasicClasses
     */
    class BasicSearch
    {
    public:

        /*! 
        * @brief Searches for the predecessor of an element p in an array X by linear search
        *
        * @param vec Array X
        * @param item Element p
        * @return Index of the predecessor of p in X, or -1 if not found
        */
        template <typename T>
        static int64_t predecessor_by_linear_search(const std::vector<T> &X, T p)
        {
            int64_t size = X.size();
            for (int64_t i = size - 1; i >= 0; i--)
            {
                if (X[i] <= p)
                {
                    return i;
                }
            }
            return -1;
        }


        /*! 
        * @brief Searches for the successor of an element p in an array X by linear search
        * 
        * @param vec Array X
        * @param item Element p
        * @return Index of the successor of p in X, or -1 if not found
        */
        template <typename T>
        static int64_t successor_by_linear_search(const std::vector<T> &X, T p)
        {
            int64_t size = X.size();
            for (int64_t i = 0; i < size; i++)
            {
                if (X[i] >= p)
                {
                    return i;
                }
            }
            return -1;
        }
    };

}
