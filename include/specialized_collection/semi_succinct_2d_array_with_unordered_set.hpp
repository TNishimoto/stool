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

        uint64_t access(uint64_t row_index, uint64_t column_index) const
        {
            return succinct_2d_array.access(row_index, column_index);
        }

        const stool::BlockPackedVector<BLOCK_SIZE> &get_column(uint64_t column_index) const
        {
            return succinct_2d_array.get_column(column_index);
        }

        std::array<uint64_t, COLUMN_SIZE> access_row(uint64_t row_index) const
        {
            return succinct_2d_array.access_row(row_index);
        }
        uint64_t row_size() const
        {
            return succinct_2d_array.row_size();
        }

        void clear()
        {
            succinct_2d_array.clear();
            used_row_indexes.clear();
        }
        void swap(SemiSuccinct2DArrayWithUnorderedSet<COLUMN_SIZE, BLOCK_SIZE> &other)
        {
            succinct_2d_array.swap(other.succinct_2d_array);
            used_row_indexes.swap(other.used_row_indexes);
        }


        size_t size() const
        {
            return used_row_indexes.size();
        }

        void print_memory_breakdown(int64_t message_paragraph = stool::Message::SHOW_MESSAGE) const
        {
            std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Memory Breakdown (SemiSuccinct2DArrayWithUnorderedSet): " << this->size_in_bytes() << " bytes" << std::endl;
            succinct_2d_array.print_memory_breakdown(message_paragraph);
        }

        uint64_t size_in_bytes() const
        {
            return succinct_2d_array.size_in_bytes() + used_row_indexes.size() * sizeof(size_t);
        }

        static void store_to_file(const SemiSuccinct2DArrayWithUnorderedSet<COLUMN_SIZE, BLOCK_SIZE> &item, std::ofstream &os)
        {
            SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE>::store_to_file(item.succinct_2d_array, os);
        }
        static SemiSuccinct2DArrayWithUnorderedSet<COLUMN_SIZE, BLOCK_SIZE> load_from_file(std::ifstream &ifs)
        {
            SemiSuccinct2DArrayWithUnorderedSet<COLUMN_SIZE, BLOCK_SIZE> item;
            item.succinct_2d_array = SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE>::load_from_file(ifs);
            ArrayHash<COLUMN_SIZE, BLOCK_SIZE>::block_packed_vectors = &item.succinct_2d_array.block_packed_vectors;
            ArrayEqual<COLUMN_SIZE, BLOCK_SIZE>::block_packed_vectors = &item.succinct_2d_array.block_packed_vectors;
            for(uint64_t i = 0; i < item.succinct_2d_array.row_size(); i++){
                if(item.succinct_2d_array.is_used_rows[i]){
                    item.used_row_indexes.insert(i);
                }
            }
            return item;
        }
    };
}