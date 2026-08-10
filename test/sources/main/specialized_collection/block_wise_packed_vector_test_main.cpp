#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../../../include/specialized_collection/block_wise_packed_vector.hpp"

namespace
{
    void require(bool condition, const char *message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    template <std::size_t BLOCK_SIZE>
    void verify_equal(const stool::BlockPackedVector<BLOCK_SIZE> &actual,
                      const std::vector<std::uint64_t> &expected)
    {
        require(actual.size() == expected.size(), "size mismatch");
        require(actual.empty() == expected.empty(), "empty mismatch");
        require(actual.num_blocks() ==
                    (expected.size() + BLOCK_SIZE - 1) / BLOCK_SIZE,
                "block count mismatch");

        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            require(actual[i] == expected[i], "value mismatch");
        }
    }

    template <std::size_t BLOCK_SIZE>
    void test_push_pop_boundaries()
    {
        stool::BlockPackedVector<BLOCK_SIZE> actual;
        std::vector<std::uint64_t> expected;
        const std::vector<std::uint64_t> values = {
            0,
            std::numeric_limits<std::uint8_t>::max(),
            std::numeric_limits<std::uint16_t>::max(),
            std::numeric_limits<std::uint32_t>::max(),
            std::numeric_limits<std::uint64_t>::max(),
        };

        const std::size_t count = BLOCK_SIZE * 3 + 1;
        for (std::size_t i = 0; i < count; ++i)
        {
            const std::uint64_t value = values[i % values.size()];
            actual.push_back(value);
            expected.push_back(value);
            verify_equal(actual, expected);
        }

        while (!expected.empty())
        {
            actual.pop_back();
            expected.pop_back();
            verify_equal(actual, expected);
        }

        bool threw = false;
        try
        {
            actual.pop_back();
        }
        catch (const std::out_of_range &)
        {
            threw = true;
        }
        require(threw, "pop_back on an empty vector did not throw");
    }

    void test_width_shrinks_after_pop()
    {
        stool::BlockPackedVector<4> actual;
        actual.push_back(1);
        actual.push_back(std::numeric_limits<std::uint64_t>::max());
        require(actual.block_width(0) == 64, "block did not widen to 64 bits");

        actual.pop_back();
        require(actual.size() == 1, "pop_back did not update size");
        require(actual[0] == 1, "pop_back changed the preceding value");
        require(actual.block_width(0) == 8, "block did not shrink to 8 bits");
    }

    template <std::size_t BLOCK_SIZE>
    void test_random_push_pop(std::uint64_t seed)
    {
        stool::BlockPackedVector<BLOCK_SIZE> actual;
        std::vector<std::uint64_t> expected;
        std::mt19937_64 random(seed);

        for (std::size_t operation = 0; operation < 5000; ++operation)
        {
            if (expected.empty() || (random() & 1) != 0)
            {
                const std::uint64_t raw_value = random();
                std::uint64_t value = raw_value;
                switch (random() % 4)
                {
                case 0:
                    value &= std::numeric_limits<std::uint8_t>::max();
                    break;
                case 1:
                    value &= std::numeric_limits<std::uint16_t>::max();
                    break;
                case 2:
                    value &= std::numeric_limits<std::uint32_t>::max();
                    break;
                default:
                    break;
                }
                actual.push_back(value);
                expected.push_back(value);
            }
            else
            {
                actual.pop_back();
                expected.pop_back();
            }
            verify_equal(actual, expected);
        }
    }
}

int main()
{
    test_push_pop_boundaries<1>();
    test_push_pop_boundaries<3>();
    test_push_pop_boundaries<16>();
    test_push_pop_boundaries<128>();
    test_width_shrinks_after_pop();
    test_random_push_pop<1>(1);
    test_random_push_pop<3>(2);
    test_random_push_pop<16>(3);
    test_random_push_pop<128>(4);
    std::cout << "BlockPackedVector push_back/pop_back tests passed\n";
    return 0;
}
