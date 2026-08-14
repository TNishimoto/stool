#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>
#include <fstream>

namespace stool{

    template <std::size_t ITEM_COUNT>
    struct BWPVBlock {
        using value_type = std::uint64_t;
        using size_type = std::size_t;

        static_assert(ITEM_COUNT > 0, "ITEM_COUNT must be greater than zero");
        static constexpr size_type item_count = ITEM_COUNT;

        unsigned item_bit_width = 8;
        size_type max_width_count = 0;
        std::vector<std::uint64_t> words;

        private:

        /**
         * @brief Selects the smallest supported width that can store a value.
         *
         * @param value Value whose storage width is requested.
         * @return The smallest supported width among 8, 16, 32, and 64 that can
         * store @p value.
         */
        static unsigned select_item_bit_width(value_type value) noexcept {
            if (value <= std::numeric_limits<std::uint8_t>::max()) {
                return 8;
            }
            if (value <= std::numeric_limits<std::uint16_t>::max()) {
                return 16;
            }
            if (value <= std::numeric_limits<std::uint32_t>::max()) {
                return 32;
            }
            return 64;
        }

        /**
         * @brief Creates a mask with the requested number of low bits set.
         *
         * @param bits Number of low-order bits to set.
         * @return A 64-bit mask whose lowest @p bits bits are one.
         * @pre @p bits is one of 8, 16, 32, or 64.
         */
        static std::uint64_t low_mask(unsigned bits) noexcept {
            return ~std::uint64_t{0} >> (64 - bits);
        }

        /**
         * @brief Calculates the number of 64-bit words needed for packed values.
         *
         * @param width Bit width used for each value.
         * @return Number of 64-bit words required for the packed representation.
         * @pre @p width is one of 8, 16, 32, or 64.
         */
        static size_type word_count(unsigned width) noexcept {
            const size_type values_per_word = 64 / width;
            return (item_count / values_per_word)
                + (item_count % values_per_word != 0);
        }

        /**
         * @brief Rebuilds the block after replacing one value.
         *
         * The block width and packed storage are recalculated from all values.
         *
         * @param changed_index Index of the value to replace.
         * @param new_value Replacement value.
         * @pre @p changed_index is less than item_count.
         */
        void repack_after_change(size_type changed_index, value_type new_value) {
            std::vector<value_type> values(item_count);
    
            for (size_type i = 0; i < item_count; ++i) {
                values[i] = get(i);
            }
    
            values[changed_index] = new_value;
    
            assign(values.data(), values.size());
        }
    
        public:
        /**
         * @brief Reads a value from the packed block.
         *
         * @param i Zero-based index of the value to read.
         * @return Decoded value at index @p i.
         * @pre @p i is less than item_count.
         */
        value_type get(size_type i) const noexcept {
            assert(i < item_count);

            const size_type bit_pos =
                i * static_cast<size_type>(item_bit_width);

            const size_type word_index = bit_pos >> 6;
            const unsigned shift =
                static_cast<unsigned>(bit_pos & 63);

            return (words[word_index] >> shift)
                & low_mask(item_bit_width);
        }

        /**
         * @brief Writes a value without changing the block width.
         *
         * @param i Zero-based index of the value to replace.
         * @param x Replacement value.
         * @pre @p i is less than item_count.
         * @pre The bit width of @p x does not exceed item_bit_width.
         */
        void set_raw(size_type i, value_type x) noexcept {
            assert(i < item_count);
            assert(select_item_bit_width(x) <= item_bit_width);

            const size_type bit_pos =
                i * static_cast<size_type>(item_bit_width);

            const size_type word_index = bit_pos >> 6;
            const unsigned shift =
                static_cast<unsigned>(bit_pos & 63);

            const std::uint64_t mask = low_mask(item_bit_width);
            const std::uint64_t shifted_mask = mask << shift;

            words[word_index] =
                (words[word_index] & ~shifted_mask)
                | (x << shift);
        }

        /**
         * @brief Replaces the block contents with the supplied values.
         *
         * Determines the minimum supported common width among 8, 16, 32, and 64,
         * allocates packed storage, and records how many values require that width.
         *
         * @param values Pointer to the values to pack.
         * Values after @p n are initialized to zero.
         *
         * @param n Number of supplied values in @p values.
         * @pre @p values is non-null when @p n is greater than zero.
         * @pre @p n is at most item_count.
         */
        void assign(const value_type* values, size_type n) {
            assert(n <= item_count);
            item_bit_width = 8;
            max_width_count = item_count - n;
    
            for (size_type i = 0; i < n; ++i) {
                const unsigned value_width = select_item_bit_width(values[i]);
                if (value_width > item_bit_width) {
                    item_bit_width = value_width;
                    max_width_count = 1;
                } else if (value_width == item_bit_width) {
                    ++max_width_count;
                }
            }
    
            words.assign(
                word_count(item_bit_width),
                0
            );
    
            for (size_type i = 0; i < n; ++i) {
                set_raw(i, values[i]);
            }
        }

        /**
         * @brief Replaces one value and adjusts the packed width when necessary.
         *
         * Repacking occurs when the new value requires a wider representation or
         * when replacing the sole widest value allows the block to shrink.
         *
         * @param i Zero-based index of the value to replace.
         * @param x Replacement value.
         * @pre @p i is less than item_count.
         */
        void set(size_type i, value_type x) {
            assert(i < item_count);
    
            const value_type old_value = get(i);
    
            if (old_value == x) {
                return;
            }
    
            const unsigned old_item_bit_width =
                select_item_bit_width(old_value);
    
            const unsigned new_item_bit_width =
                select_item_bit_width(x);
    
            // Block width must increase.
            if (new_item_bit_width > item_bit_width) {
                repack_after_change(i, x);
                return;
            }
    
            // This was the only value requiring the current width,
            // so the block may be shrinkable.
            if (
                old_item_bit_width == item_bit_width &&
                new_item_bit_width < item_bit_width &&
                max_width_count == 1
            ) {
                repack_after_change(i, x);
                return;
            }
    
            set_raw(i, x);

            if (old_item_bit_width != new_item_bit_width) {
                if (old_item_bit_width == item_bit_width) {
                    --max_width_count;
                }
                if (new_item_bit_width == item_bit_width) {
                    ++max_width_count;
                }
            }
        }

        static BWPVBlock load_from_file(std::ifstream &ifs)
		{

			BWPVBlock r;

			ifs.read(reinterpret_cast<char *>(&r.item_bit_width), sizeof(unsigned));
			ifs.read(reinterpret_cast<char *>(&r.max_width_count), sizeof(size_type));

            uint64_t words_vec_size;
			ifs.read(reinterpret_cast<char *>(&words_vec_size), sizeof(uint64_t));
			r.words.resize(words_vec_size);
			ifs.read(reinterpret_cast<char *>(r.words.data()), sizeof(std::uint64_t) * words_vec_size);

			return r;
		}

        static void store_to_file(const BWPVBlock &item, std::ofstream &os)
		{
            unsigned __item_bit_width = item.item_bit_width;
			os.write(reinterpret_cast<const char *>(&__item_bit_width), sizeof(unsigned));

            size_type __max_width_count = item.max_width_count;
			os.write(reinterpret_cast<const char *>(&__max_width_count), sizeof(size_type));

            uint64_t words_vec_size = item.words.size();
			os.write(reinterpret_cast<const char *>(&words_vec_size), sizeof(uint64_t));
            os.write(reinterpret_cast<const char *>(item.words.data()), sizeof(std::uint64_t) * words_vec_size);
		}



        
    };

    
}
