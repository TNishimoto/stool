#pragma once
#include <array>
#include <vector>
#include <stack>
#include <stdexcept>
#include <cstdint>
#include <unordered_set>

namespace stool
{

    template <uint64_t COLUMN_SIZE>
    class Naive2DArray
    {
    public:
        std::vector<std::array<uint64_t, COLUMN_SIZE>> block_packed_vectors;
        std::vector<bool> is_used_rows;
        std::stack<uint64_t> unused_indexes;

    public:
        Naive2DArray()
        {
            block_packed_vectors.resize(0);
            is_used_rows.resize(0);
        }

        size_t insert(std::array<uint64_t, COLUMN_SIZE> new_row)
        {
            if (unused_indexes.empty())
            {
                block_packed_vectors.push_back(new_row);
                is_used_rows.push_back(true);
                return block_packed_vectors.size() - 1;
            }
            else
            {
                uint64_t index = unused_indexes.top();
                unused_indexes.pop();
                block_packed_vectors[index] = new_row;
                is_used_rows[index] = true;
                return index;
            }
        }

        void erase(uint64_t row_index)
        {
            if (!is_used_rows[row_index])
            {
                throw std::invalid_argument("index is not used");
            }
            for (uint64_t i = 0; i < COLUMN_SIZE; i++)
            {
                block_packed_vectors[row_index][i] = 0;
            }
            is_used_rows[row_index] = false;
            unused_indexes.push(row_index);
        }

        uint64_t access(uint64_t row_index, uint64_t column_index)
        {
            return block_packed_vectors[row_index][column_index];
        }

        std::array<uint64_t, COLUMN_SIZE> access_row(uint64_t row_index)
        {
            return block_packed_vectors[row_index];
        }

        size_t size() const
        {
            return is_used_rows.size() - unused_indexes.size();
        }
    };
}
