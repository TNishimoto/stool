#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace stool{


class BlockPackedVector {
public:
    using value_type = std::uint64_t;
    using size_type = std::size_t;

private:

struct Block {
    size_type size = 0;
    unsigned width = 0;
    size_type max_width_count = 0;
    std::vector<std::uint64_t> words;

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

    static std::uint64_t low_mask(unsigned bits) noexcept {
        if (bits == 0) {
            return 0;
        }

        if (bits == 64) {
            return ~std::uint64_t{0};
        }

        return (std::uint64_t{1} << bits) - 1;
    }

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

    void assign(
        const value_type* values,
        size_type n
    ) {
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

    void repack_after_change(
        size_type changed_index,
        value_type new_value
    ) {
        std::vector<value_type> values(size);

        for (size_type i = 0; i < size; ++i) {
            values[i] = get(i);
        }

        values[changed_index] = new_value;

        assign(values.data(), values.size());
    }

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

    size_type size_ = 0;
    size_type block_size_ = 128;
    std::vector<Block> blocks_;

    std::pair<size_type, size_type>
    locate(size_type i) const {
        if (i >= size_) {
            throw std::out_of_range(
                "BlockPackedVector index out of range"
            );
        }

        return {
            i / block_size_,
            i % block_size_
        };
    }

public:
class Reference {
    private:
        BlockPackedVector* owner_;
        size_type index_;

    public:
        Reference(
            BlockPackedVector* owner,
            size_type index
        ) noexcept
            : owner_(owner),
              index_(index)
        {
        }

        Reference& operator=(value_type x) {
            owner_->set(index_, x);
            return *this;
        }

        Reference& operator=(const Reference& other) {
            return *this =
                static_cast<value_type>(other);
        }

        operator value_type() const {
            return owner_->get(index_);
        }
    };

    explicit BlockPackedVector(
        size_type n = 0,
        value_type initial_value = 0,
        size_type block_size = 128
    )
        : size_(n),
          block_size_(block_size)
    {
        if (block_size_ == 0) {
            throw std::invalid_argument(
                "block_size must be > 0"
            );
        }

        if (n == 0) {
            return;
        }

        const size_type num_blocks =
            (n + block_size_ - 1) / block_size_;

        blocks_.resize(num_blocks);

        std::vector<value_type> tmp(
            block_size_,
            initial_value
        );

        for (size_type b = 0; b < num_blocks; ++b) {
            const size_type begin =
                b * block_size_;

            const size_type length =
                std::min(
                    block_size_,
                    n - begin
                );

            blocks_[b].assign(
                tmp.data(),
                length
            );
        }
    }

    explicit BlockPackedVector(
        const std::vector<value_type>& values,
        size_type block_size = 128
    )
        : size_(values.size()),
          block_size_(block_size)
    {
        if (block_size_ == 0) {
            throw std::invalid_argument(
                "block_size must be > 0"
            );
        }

        if (values.empty()) {
            return;
        }

        const size_type num_blocks =
            (size_ + block_size_ - 1) / block_size_;

        blocks_.resize(num_blocks);

        for (size_type b = 0; b < num_blocks; ++b) {
            const size_type begin =
                b * block_size_;

            const size_type length =
                std::min(
                    block_size_,
                    size_ - begin
                );

            blocks_[b].assign(
                values.data() + begin,
                length
            );
        }
    }

    size_type size() const noexcept {
        return size_;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type block_size() const noexcept {
        return block_size_;
    }

    size_type num_blocks() const noexcept {
        return blocks_.size();
    }

    value_type get(size_type i) const {
        const auto location = locate(i);

        return blocks_[location.first]
            .get(location.second);
    }

    void set(size_type i, value_type x) {
        const auto location = locate(i);

        blocks_[location.first]
            .set(location.second, x);
    }

    value_type operator[](size_type i) const {
        return get(i);
    }

    Reference operator[](size_type i) {
        if (i >= size_) {
            throw std::out_of_range(
                "BlockPackedVector index out of range"
            );
        }

        return Reference(this, i);
    }

    unsigned block_width(size_type block) const {
        if (block >= blocks_.size()) {
            throw std::out_of_range(
                "Block index out of range"
            );
        }

        return blocks_[block].width;
    }

    size_type packed_bytes() const noexcept {
        size_type total = 0;

        for (const auto& block : blocks_) {
            total +=
                block.words.size()
                * sizeof(std::uint64_t);
        }

        return total;
    }

    size_type size_in_bytes() const noexcept {
        size_type total =
            sizeof(BlockPackedVector)
            + blocks_.capacity() * sizeof(Block);

        for (const auto& block : blocks_) {
            total +=
                block.words.capacity()
                * sizeof(std::uint64_t);
        }

        return total;
    }
};

}
