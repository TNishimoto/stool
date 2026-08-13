#pragma once
#include <array>
#include <vector>
#include <stack>
#include <stdexcept>
#include <cstdint>
#include <unordered_set>
#include "./semi_succinct_2d_array.hpp"

namespace stool
{

    template <uint64_t COLUMN_SIZE, uint64_t BLOCK_SIZE = 128>
    class SemiSuccinct2DArrayWithUnorderedSet
    {
        SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE> succinct_2d_array;
        std::unordered_set<size_t, ArrayHash<COLUMN_SIZE, BLOCK_SIZE>, ArrayEqual<COLUMN_SIZE, BLOCK_SIZE>> used_row_indexes;

        public:
        int64_t find_row_index(std::array<uint64_t, COLUMN_SIZE> row){
            ArrayHash<COLUMN_SIZE, BLOCK_SIZE>::block_packed_vectors = &succinct_2d_array.block_packed_vectors;
            ArrayEqual<COLUMN_SIZE, BLOCK_SIZE>::block_packed_vectors = &succinct_2d_array.block_packed_vectors;

            uint64_t new_row_index = succinct_2d_array.insert(row);
            auto it = this->used_row_indexes.find(new_row_index);
            int64_t found_row_index = -1;
            if (it != this->used_row_indexes.end())
            {
                found_row_index = *it;
            }

            succinct_2d_array.erase(new_row_index);
            return found_row_index;
        }

        std::pair<size_t, bool> insert(std::array<uint64_t, COLUMN_SIZE> new_row)
        {
            ArrayHash<COLUMN_SIZE, BLOCK_SIZE>::block_packed_vectors = &succinct_2d_array.block_packed_vectors;
            ArrayEqual<COLUMN_SIZE, BLOCK_SIZE>::block_packed_vectors = &succinct_2d_array.block_packed_vectors;

            int64_t found_row_index = find_row_index(new_row);
            if (found_row_index != -1)
            {
                return std::make_pair(found_row_index, false);
            }
            else
            {
                size_t new_row_index = succinct_2d_array.insert(new_row);
                used_row_indexes.insert(new_row_index);
                return std::make_pair(new_row_index, true);
            }
        }
        void erase(size_t row_index)
        {
            ArrayHash<COLUMN_SIZE, BLOCK_SIZE>::block_packed_vectors = &succinct_2d_array.block_packed_vectors;
            ArrayEqual<COLUMN_SIZE, BLOCK_SIZE>::block_packed_vectors = &succinct_2d_array.block_packed_vectors;

            if (!succinct_2d_array.is_used_rows[row_index])
            {
                throw std::invalid_argument("index is not used");
            }
            used_row_indexes.erase(row_index);
            succinct_2d_array.erase(row_index);
        }

        uint64_t access(uint64_t row_index, uint64_t column_index)
        {
            return succinct_2d_array.access(row_index, column_index);
        }

        std::array<uint64_t, COLUMN_SIZE> access_row(uint64_t row_index)
        {
            return succinct_2d_array.access_row(row_index);
        }


        size_t size() const
        {
            return used_row_indexes.size();
        }
    };
}