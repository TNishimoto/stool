/**
 * @file rmq_small_sparse_table.hpp
 * @brief Implementation of a small sparse table for Range Minimum Query (RMQ).
 */

#pragma once
#include "./rmq_sparse_table.hpp"

namespace stool
{
    /**
     * @class RMQSmallSparseTable
     * @brief A class for performing Range Minimum Query (RMQ) using a small sparse table.
     * @tparam T The data type of the elements in the array, defaulting to uint64_t.
     */
    template <typename T = uint64_t>
    class RMQSmallSparseTable
    {
        std::vector<uint64_t> _sub_array; ///< Sub-array for storing minimum values.
        RMQSparseTable<T> _rmq; ///< Sparse table for RMQ operations.

    public:
        /**
         * @brief Default constructor.
         */
        RMQSmallSparseTable()
        {
        }

        /**
         * @brief Builds the small sparse table from the given array.
         * @param array The input array to build the sparse table from.
         */
        void build(const std::vector<T> &array)
        {
            _sub_array.clear();
            uint64_t logn = stool::Log::log2floor(array.size());
            uint64_t counter = 0;
            T current_minimum = std::numeric_limits<T>::max();

            for(auto p : array){
                if(current_minimum > p){
                    current_minimum = p;
                }
                counter++;
                if(counter == logn){
                    _sub_array.push_back(current_minimum);
                    current_minimum = std::numeric_limits<T>::max();
                    counter = 0;
                }
            }
            if(counter > 0){
                _sub_array.push_back(current_minimum);
                counter = 0;
            }

            _rmq.build(_sub_array);
        }

        /**
         * @brief Finds the index of the minimum element in the range [i, j] of the array.
         * @param i The start index of the range.
         * @param j The end index of the range.
         * @param array The input array.
         * @return The index of the minimum element in the specified range.
         * @throws std::out_of_range if j < i.
         */
        uint64_t rmq_index(uint64_t i, uint64_t j, const std::vector<T> &array) const
        {
            if(j < i){
                throw std::out_of_range("rmq_index error: j < i");
            }
            if(array.size() < 2){
                return RMQSparseTable<T>::naive_rmq_index(i, j, array);
            }
            uint64_t logn = stool::Log::log2floor(array.size());
            //uint64_t len = j - i + 1;

            uint64_t i_pos = i / logn;
            uint64_t j_pos = j / logn;

            if(i_pos == j_pos){
                return RMQSparseTable<T>::naive_rmq_index(i, j, array);
            }else{
                uint64_t left_rmq_index = RMQSparseTable<T>::naive_rmq_index(i, ((i_pos+1) * logn - 1), array);
                uint64_t right_rmq_index = RMQSparseTable<T>::naive_rmq_index(j_pos * logn, j, array);

                if(i_pos + 1 == j_pos){
                    return array[left_rmq_index] <= array[right_rmq_index] ? left_rmq_index : right_rmq_index;
                }else{
                    assert(i_pos+1 <= j_pos-1);
                    uint64_t center_sub_rmq_index = _rmq.rmq_index(i_pos+1, j_pos-1, _sub_array);
                    uint64_t center_rmq_index = RMQSparseTable<T>::naive_rmq_index(center_sub_rmq_index * logn, ((center_sub_rmq_index+1) * logn) - 1, array);

                    if(array[left_rmq_index] <= array[right_rmq_index]){
                        if(array[left_rmq_index] <= array[center_rmq_index]){
                            return left_rmq_index;
                        }else{
                            return center_rmq_index;
                        }
                    }else{
                        if(array[center_rmq_index] <= array[right_rmq_index]){
                            return center_rmq_index;
                        }else{
                            return right_rmq_index;
                        }
                    }
                }
            }
        }

        /**
         * @brief Returns the minimum element in the range [i, j] of the array.
         * @param i The start index of the range.
         * @param j The end index of the range.
         * @param array The input array.
         * @return The minimum element in the specified range.
         */
        uint64_t rmq(uint64_t i, uint64_t j, const std::vector<T> &array) const
        {
            return array[rmq_index(i, j, array)];
        }
    };
}
