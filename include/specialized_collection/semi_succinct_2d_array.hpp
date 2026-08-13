#pragma once
#include <array>
#include <vector>
#include <stack>
#include <stdexcept>
#include <cstdint>
#include <unordered_set>
#include "./block_wise_packed_vector.hpp"

namespace stool
{
    template <size_t ARRAY_SIZE, uint64_t BLOCK_SIZE = 128>
    struct ArrayHash
    {
        static constexpr std::vector<stool::BlockPackedVector<BLOCK_SIZE>> *block_packed_vectors = nullptr;

        static uint64_t splitmix64(uint64_t x) noexcept
        {
            x += 0x9e3779b97f4a7c15ULL;
            x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
            x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
            return x ^ (x >> 31);
        }
    
        std::size_t operator()(size_t row_index) const noexcept
        {
            uint64_t h = 0x9e3779b97f4a7c15ULL;
    
            for (uint64_t i = 0; i < ARRAY_SIZE; i++)
            {
                h = splitmix64(h ^ splitmix64((*block_packed_vectors)[i][row_index]));
            }
    
            return static_cast<std::size_t>(h);
        }
    };

    template <size_t ARRAY_SIZE, uint64_t BLOCK_SIZE = 128>
    struct ArrayEqual
    {
        static constexpr std::vector<stool::BlockPackedVector<BLOCK_SIZE>> *block_packed_vectors = nullptr;
    
        bool operator()(size_t row_index1, size_t row_index2) const noexcept
        {
            for (uint64_t i = 0; i < ARRAY_SIZE; i++)
            {
                if ((*block_packed_vectors)[i][row_index1] != (*block_packed_vectors)[i][row_index2])
                {
                    return false;
                }
            }
            return true;
        }
    };




    template <uint64_t COLUMN_SIZE, uint64_t BLOCK_SIZE = 128>
    class SemiSuccinct2DArray
    {
    public:
        std::vector<stool::BlockPackedVector<BLOCK_SIZE>> block_packed_vectors;
        std::vector<bool> is_used_rows;
        std::stack<uint64_t> unused_indexes;

    public:
        SemiSuccinct2DArray()
        {
            block_packed_vectors.resize(COLUMN_SIZE);
            is_used_rows.resize(0);
        }

        size_t insert(std::array<uint64_t, COLUMN_SIZE> new_row)
        {
            if (unused_indexes.empty())
            {
                const size_t row_index = is_used_rows.size();
                for (uint64_t i = 0; i < COLUMN_SIZE; i++)
                {
                    block_packed_vectors[i].push_back(new_row[i]);
                }
                is_used_rows.push_back(true);
                return row_index;
            }
            else
            {
                uint64_t index = unused_indexes.top();
                unused_indexes.pop();
                for (uint64_t i = 0; i < COLUMN_SIZE; i++)
                {
                    block_packed_vectors[i][index] = new_row[i];
                }
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
                block_packed_vectors[i][row_index] = 0;
            }
            is_used_rows[row_index] = false;
            unused_indexes.push(row_index);
        }

        uint64_t access(uint64_t row_index, uint64_t column_index)
        {
            return block_packed_vectors[column_index][row_index];
        }

        std::array<uint64_t, COLUMN_SIZE> access_row(uint64_t row_index)
        {
            std::array<uint64_t, COLUMN_SIZE> row;
            for (uint64_t i = 0; i < COLUMN_SIZE; i++)
            {
                row[i] = block_packed_vectors[i][row_index];
            }
            return row;
        }

        size_t size() const
        {
            return is_used_rows.size() - unused_indexes.size();
        }
    };


}
