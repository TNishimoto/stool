#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace stool{

    struct BWPVBlock {
        using value_type = std::uint64_t;
        using size_type = std::size_t;
    
        size_type size = 0;
        unsigned width = 0;
        size_type max_width_count = 0;
        std::vector<std::uint64_t> words;

        private:

        /**
         * @brief Returns the number of bits required to represent a value.
         *
         * @param x Value whose bit width is requested.
         * @return Zero when @p x is zero; otherwise, the position of its most
         * significant set bit plus one.
         */
        static unsigned bits_required(std::uint64_t x) noexcept {
            if (x == 0) {
                return 0;
            }
    
    #if defined(__GNUC__) || defined(__clang__)
            return 64u - static_cast<unsigned>(__builtin_clzll(x));
    #else
            unsigned bits = 0;
            while (x != 0) {
                ++bits;
                x >>= 1;
            }
            return bits;
    #endif
        }

        /**
         * @brief Creates a mask with the requested number of low bits set.
         *
         * @param bits Number of low-order bits to set.
         * @return A 64-bit mask whose lowest @p bits bits are one.
         * @pre @p bits is at most 64.
         */
        static std::uint64_t low_mask(unsigned bits) noexcept {
            if (bits == 0) {
                return 0;
            }
    
            if (bits == 64) {
                return ~std::uint64_t{0};
            }
    
            return (std::uint64_t{1} << bits) - 1;
        }

        /**
         * @brief Calculates the number of 64-bit words needed for packed values.
         *
         * @param n Number of values to store.
         * @param width Bit width used for each value.
         * @return Number of 64-bit words required for the packed representation.
         * @throws std::length_error If the packed bit count cannot be represented
         * by size_type.
         */
        static size_type word_count(size_type n, unsigned width) {
            if (n == 0 || width == 0) {
                return 0;
            }
    
            if (n >
                (std::numeric_limits<size_type>::max() - 63)
                    / static_cast<size_type>(width)) {
                throw std::length_error("Block is too large");
            }
    
            return (
                n * static_cast<size_type>(width) + 63
            ) / 64;
        }

        /**
         * @brief Rebuilds the block after replacing one value.
         *
         * The block width and packed storage are recalculated from all values.
         *
         * @param changed_index Index of the value to replace.
         * @param new_value Replacement value.
         * @pre @p changed_index is less than size.
         */
        void repack_after_change(size_type changed_index, value_type new_value) {
            std::vector<value_type> values(size);
    
            for (size_type i = 0; i < size; ++i) {
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
         * @pre @p i is less than size.
         */
        value_type get(size_type i) const noexcept {
            assert(i < size);
    
            if (width == 0) {
                return 0;
            }
    
            const size_type bit_pos =
                i * static_cast<size_type>(width);
    
            const size_type word_index = bit_pos >> 6;
            const unsigned shift =
                static_cast<unsigned>(bit_pos & 63);
    
            if (shift + width <= 64) {
                return (words[word_index] >> shift)
                    & low_mask(width);
            }
    
            const unsigned low_bits = 64 - shift;
            const unsigned high_bits = width - low_bits;
    
            const std::uint64_t low =
                words[word_index] >> shift;
    
            const std::uint64_t high =
                words[word_index + 1] & low_mask(high_bits);
    
            return low | (high << low_bits);
        }

        /**
         * @brief Writes a value without changing the block width.
         *
         * @param i Zero-based index of the value to replace.
         * @param x Replacement value.
         * @pre @p i is less than size.
         * @pre The bit width of @p x does not exceed width.
         */
        void set_raw(size_type i, value_type x) noexcept {
            assert(i < size);
            assert(bits_required(x) <= width);
    
            if (width == 0) {
                return;
            }
    
            const size_type bit_pos =
                i * static_cast<size_type>(width);
    
            const size_type word_index = bit_pos >> 6;
            const unsigned shift =
                static_cast<unsigned>(bit_pos & 63);
    
            if (shift + width <= 64) {
                const std::uint64_t mask =
                    low_mask(width);
    
                const std::uint64_t shifted_mask =
                    mask << shift;
    
                words[word_index] =
                    (words[word_index] & ~shifted_mask)
                    | ((x & mask) << shift);
    
                return;
            }
    
            const unsigned low_bits = 64 - shift;
            const unsigned high_bits = width - low_bits;
    
            {
                const std::uint64_t mask =
                    low_mask(low_bits);
    
                const std::uint64_t shifted_mask =
                    mask << shift;
    
                words[word_index] =
                    (words[word_index] & ~shifted_mask)
                    | ((x & mask) << shift);
            }
    
            {
                const std::uint64_t mask =
                    low_mask(high_bits);
    
                words[word_index + 1] =
                    (words[word_index + 1] & ~mask)
                    | ((x >> low_bits) & mask);
            }
        }

        /**
         * @brief Replaces the block contents with the supplied values.
         *
         * Determines the minimum common width, allocates packed storage, and
         * records how many values require that width.
         *
         * @param values Pointer to the values to pack.
         * @param n Number of values in @p values.
         * @pre @p values is non-null when @p n is greater than zero.
         * @throws std::length_error If the packed bit count cannot be represented
         * by size_type.
         */
        void assign(const value_type* values, size_type n) {
            size = n;
            width = 0;
    
            for (size_type i = 0; i < n; ++i) {
                width = std::max(
                    width,
                    bits_required(values[i])
                );
            }
    
            words.assign(
                word_count(size, width),
                0
            );
    
            max_width_count = 0;
    
            for (size_type i = 0; i < n; ++i) {
                set_raw(i, values[i]);
    
                if (bits_required(values[i]) == width) {
                    ++max_width_count;
                }
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
         * @pre @p i is less than size.
         */
        void set(size_type i, value_type x) {
            assert(i < size);
    
            const value_type old_value = get(i);
    
            if (old_value == x) {
                return;
            }
    
            const unsigned old_bits =
                bits_required(old_value);
    
            const unsigned new_bits =
                bits_required(x);
    
            // Block width must increase.
            if (new_bits > width) {
                repack_after_change(i, x);
                return;
            }
    
            // This was the only value requiring the current width,
            // so the block may be shrinkable.
            if (
                old_bits == width &&
                new_bits < width &&
                max_width_count == 1
            ) {
                repack_after_change(i, x);
                return;
            }
    
            set_raw(i, x);
    
            if (old_bits == width) {
                --max_width_count;
            }
    
            if (new_bits == width) {
                ++max_width_count;
            }
        }
    };

    
}
