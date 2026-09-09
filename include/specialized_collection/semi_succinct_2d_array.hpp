#pragma once
#include <array>
#include <vector>
#include <stack>
#include <stdexcept>
#include <cstdint>
#include <unordered_set>
#include "./block_wise_packed_vector.hpp"
#include "../debug/memory.hpp"
#include "../debug/message.hpp"
namespace stool
{
    template <size_t ARRAY_SIZE, uint64_t BLOCK_SIZE = 128>
    struct ArrayHash
    {
        inline static std::vector<stool::BlockPackedVector<BLOCK_SIZE>> *block_packed_vectors = nullptr;

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
        inline static std::vector<stool::BlockPackedVector<BLOCK_SIZE>> *block_packed_vectors = nullptr;
    
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
        std::vector<uint64_t> unused_indexes;

    public:
        SemiSuccinct2DArray()
        {
            block_packed_vectors.resize(COLUMN_SIZE);
            is_used_rows.resize(0);
        }

        const stool::BlockPackedVector<BLOCK_SIZE> &get_column(uint64_t column_index) const{
            return block_packed_vectors[column_index];
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
                uint64_t index = unused_indexes.back();
                unused_indexes.pop_back();
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
            unused_indexes.push_back(row_index);
        }
        void clear() {
            for (uint64_t i = 0; i < COLUMN_SIZE; i++){
                block_packed_vectors[i].clear();
            }
            is_used_rows.clear();
            unused_indexes.clear();
            
        }
        void swap(SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE> &other) {
            block_packed_vectors.swap(other.block_packed_vectors);
            is_used_rows.swap(other.is_used_rows);
            unused_indexes.swap(other.unused_indexes);
        }

        uint64_t access(uint64_t row_index, uint64_t column_index) const
        {
            return block_packed_vectors[column_index][row_index];
        }
        void set(uint64_t row_index, uint64_t column_index, uint64_t value) {
            block_packed_vectors[column_index][row_index] = value;
        }
        void set_row(uint64_t row_index, std::array<uint64_t, COLUMN_SIZE> new_row) {
            for (uint64_t i = 0; i < COLUMN_SIZE; i++) {
                block_packed_vectors[i][row_index] = new_row[i];
            }
        }


        std::array<uint64_t, COLUMN_SIZE> access_row(uint64_t row_index) const 
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
        size_t row_size() const
        {
            return is_used_rows.size();
        }

        uint64_t size_in_bytes() const
        {
			uint64_t total_size = 0;
            for (uint64_t i = 0; i < COLUMN_SIZE; i++)
            {
                total_size += block_packed_vectors[i].size_in_bytes();
            }
            total_size += stool::Memory::estimate_memory_usage(is_used_rows);
            total_size += stool::Memory::estimate_memory_usage(unused_indexes);
            return total_size;
        }

        void print_memory_breakdown(int64_t message_paragraph = stool::Message::SHOW_MESSAGE) const
		{
			std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Memory Breakdown (SemiSuccinct2DArray): " << this->size_in_bytes() << " bytes" << std::endl;
            for (uint64_t i = 0; i < COLUMN_SIZE; i++)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph+1) << "block_packed_vectors[" << i << "]: " << block_packed_vectors[i].size_in_bytes() << " bytes" << std::endl;
            }
            std::cout << stool::Message::get_paragraph_string(message_paragraph+1) << "is_used_rows: " << stool::Memory::estimate_memory_usage(is_used_rows) << " bytes" << std::endl;
            std::cout << stool::Message::get_paragraph_string(message_paragraph+1) << "unused_indexes: " << stool::Memory::estimate_memory_usage(unused_indexes) << " bytes" << std::endl;
			std::cout << stool::Message::get_paragraph_string(message_paragraph) << "[END]" << std::endl;

		}

        static void store_to_file(const SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE> &item, std::ofstream &os)
		{
            for (uint64_t i = 0; i < COLUMN_SIZE; i++)
            {
                stool::BlockPackedVector<BLOCK_SIZE>::store_to_file(item.block_packed_vectors[i], os);
            }

            uint64_t unused_indexes_size = item.unused_indexes.size();
            os.write(reinterpret_cast<const char *>(&unused_indexes_size), sizeof(uint64_t));
            os.write(reinterpret_cast<const char *>(item.unused_indexes.data()), sizeof(uint64_t) * unused_indexes_size);

		}

        static SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE> load_from_file(std::ifstream &ifs)
		{

			SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE> r;
            for (uint64_t i = 0; i < COLUMN_SIZE; i++)
            {
                r.block_packed_vectors[i] = stool::BlockPackedVector<BLOCK_SIZE>::load_from_file(ifs);
            }
            uint64_t unused_indexes_size;
			ifs.read(reinterpret_cast<char *>(&unused_indexes_size), sizeof(uint64_t));
			r.unused_indexes.resize(unused_indexes_size);
			ifs.read(reinterpret_cast<char *>(r.unused_indexes.data()), sizeof(uint64_t) * unused_indexes_size);

            uint64_t row_size = 0;
            if(COLUMN_SIZE > 0){
                row_size = r.block_packed_vectors[0].size();
            }

            r.is_used_rows.resize(row_size, true);
            for (uint64_t i = 0; i < unused_indexes_size; i++)
            {
                r.is_used_rows[r.unused_indexes[i]] = false;
            }

			return r;
		}



    };


}
