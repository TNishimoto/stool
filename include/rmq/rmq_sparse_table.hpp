#pragma once
#include "../basic/log.hpp"
#include "../debug/print.hpp"

namespace stool
{
    template <typename T = uint64_t>
    class RMQSparseTable
    {
        std::vector<std::vector<uint64_t>> _sparse_table;

    public:
        RMQSparseTable()
        {
        }

        void clear(){
            _sparse_table.clear();
        }

        void build(const std::vector<T> &array)
        {
            this->clear();
            _sparse_table.resize(array.size());

            uint64_t logn = stool::Log::log2floor(array.size());
            for (uint64_t i = 0; i < array.size(); i++)
            {
                _sparse_table[i].resize(logn, std::numeric_limits<T>::max());
            }
            /*
            for (uint64_t j = 0; j < array.size(); j++)
            {
                _sparse_table[j][0] = array[j];
            }
            */

            uint64_t len = 1;
            for (uint64_t y = 0; y < logn; y++)
            {
                std::cout << "y: " << y << "/" << logn << std::endl;
                for (uint64_t j = 0; j < array.size(); j++)
                {
                    if (j + len < array.size())
                    {
                        if (y > 0)
                        {
                            uint64_t left = _sparse_table[j][y - 1];
                            uint64_t right = _sparse_table[j + len][y - 1];

                            _sparse_table[j][y] = array[left] <= array[right] ? left : right;
                        }
                        else
                        {
                            _sparse_table[j][y] = array[j] <= array[j + len] ? j : j + len;
                        }
                    }
                    else
                    {
                        if (y > 0)
                        {
                            _sparse_table[j][y] = _sparse_table[j][y - 1];
                        }
                        else
                        {
                            _sparse_table[j][y] = j;
                        }
                    }
                }
                len *= 2;
            }
        }
        uint64_t rmq_index(uint64_t i, uint64_t j, const std::vector<T> &array) const
        {
            if(j < i){
                throw std::out_of_range("rmq_index error: j < i");
            }

            uint64_t len = j - i + 1;
            uint64_t k = stool::Log::log2floor(len);



            if (j >= array.size())
            {
                throw std::out_of_range("rmq_index error: j >= array.size()");
            }
            if (k == 0)
            {
                return i;
            }
            else
            {
                uint64_t mlen = 1 << k;
                //std::cout << "i: " << i << ", j: " << j << ", k: " << k << ", mlen: " << mlen  << std::endl;

                assert(_sparse_table[i].size() > k-1);
                uint64_t left = _sparse_table[i][k-1];
                uint64_t right = _sparse_table[j - mlen + 1][k-1];
                //std::cout << ", left: " << left << ", right: " << right << std::endl;
                assert(i <= left && left <= j);
                assert(i <= right && right <= j);
                return array[left] <= array[right] ? left : right;
            }
        }
        uint64_t rmq(uint64_t i, uint64_t j, const std::vector<T> &array) const
        {
            return array[rmq_index(i, j, array)];
        }

        static uint64_t naive_rmq_index(uint64_t i, uint64_t j, const std::vector<T> &array)
        {
            if(j < i){
                throw std::out_of_range("rmq_index error: j < i");
            }

            uint64_t min_index = i;

            uint64_t M = std::min(j+1, (uint64_t)array.size());
            for (uint64_t k = i; k < M; k++)
            {
                if (array[k] < array[min_index])
                {
                    min_index = k;
                }
            }
            return min_index;
        }
        static uint64_t naive_rmq(uint64_t i, uint64_t j, const std::vector<T> &array)
        {
            return array[naive_rmq_index(i, j, array)];
        }

        void print() const{
            std::cout << "sparse table" << std::endl;
            for(uint64_t i = 0; i < _sparse_table.size(); i++){
                stool::DebugPrinter::print_integers(this->_sparse_table[i], "sparse table");
            }
        }
    };

}
