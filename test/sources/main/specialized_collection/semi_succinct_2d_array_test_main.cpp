#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../../../include/specialized_collection/naive_2d_array.hpp"
#include "../../../../include/specialized_collection/semi_succinct_2d_array.hpp"

namespace
{
    void require(bool condition, const std::string &message)
    {
        if (!condition)
        {
            throw std::runtime_error(message);
        }
    }

    std::uint64_t random_value(std::mt19937_64 &random)
    {
        const std::uint64_t value = random();
        switch (random() % 5)
        {
        case 0:
            return 0;
        case 1:
            return value & std::numeric_limits<std::uint8_t>::max();
        case 2:
            return value & std::numeric_limits<std::uint16_t>::max();
        case 3:
            return value & std::numeric_limits<std::uint32_t>::max();
        default:
            return value;
        }
    }

    template <std::uint64_t COLUMN_SIZE>
    std::array<std::uint64_t, COLUMN_SIZE> random_row(std::mt19937_64 &random)
    {
        std::array<std::uint64_t, COLUMN_SIZE> row{};
        for (std::uint64_t &value : row)
        {
            value = random_value(random);
        }
        return row;
    }

    template <std::uint64_t COLUMN_SIZE, std::uint64_t BLOCK_SIZE>
    void verify_equal(stool::Naive2DArray<COLUMN_SIZE> &expected,
                      stool::SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE> &actual,
                      const std::vector<bool> &used_rows,
                      std::size_t operation)
    {
        const std::string context = " after operation " + std::to_string(operation);
        require(actual.size() == expected.size(), "size mismatch" + context);
        require(actual.size() ==
                    static_cast<std::size_t>(std::count(used_rows.begin(), used_rows.end(), true)),
                "active row count mismatch" + context);
        require(actual.is_used_rows == expected.is_used_rows,
                "row usage flags mismatch" + context);
        require(actual.is_used_rows == used_rows, "reference row usage flags mismatch" + context);

        for (std::size_t row_index = 0; row_index < used_rows.size(); ++row_index)
        {
            require(actual.access_row(row_index) == expected.access_row(row_index),
                    "row mismatch at row " + std::to_string(row_index) + context);
            for (std::size_t column_index = 0; column_index < COLUMN_SIZE; ++column_index)
            {
                require(actual.access(row_index, column_index) ==
                            expected.access(row_index, column_index),
                        "value mismatch at row " + std::to_string(row_index) + ", column " +
                            std::to_string(column_index) + context);
            }
        }
    }

    template <std::uint64_t COLUMN_SIZE, std::uint64_t BLOCK_SIZE>
    void test_random_operations(std::uint64_t seed, std::size_t operation_count)
    {
        stool::Naive2DArray<COLUMN_SIZE> expected;
        stool::SemiSuccinct2DArray<COLUMN_SIZE, BLOCK_SIZE> actual;
        std::vector<bool> used_rows;
        std::vector<std::size_t> active_rows;
        std::mt19937_64 random(seed);

        verify_equal(expected, actual, used_rows, 0);

        for (std::size_t operation = 1; operation <= operation_count; ++operation)
        {
            const std::uint64_t action = random() % 100;
            if (active_rows.empty() || (action < 55 && active_rows.size() < 512))
            {
                const auto row = random_row<COLUMN_SIZE>(random);
                const std::size_t expected_index = expected.insert(row);
                const std::size_t actual_index = actual.insert(row);
                require(actual_index == expected_index,
                        "insert returned a different row index after operation " +
                            std::to_string(operation));

                if (actual_index == used_rows.size())
                {
                    used_rows.push_back(true);
                }
                else
                {
                    require(actual_index < used_rows.size(),
                            "insert returned an invalid row index after operation " +
                                std::to_string(operation));
                    used_rows[actual_index] = true;
                }
                active_rows.push_back(actual_index);
            }
            else if (action < 80)
            {
                const std::size_t position = random() % active_rows.size();
                const std::size_t row_index = active_rows[position];
                expected.erase(row_index);
                actual.erase(row_index);
                used_rows[row_index] = false;
                active_rows[position] = active_rows.back();
                active_rows.pop_back();

                bool expected_threw = false;
                bool actual_threw = false;
                try
                {
                    expected.erase(row_index);
                }
                catch (const std::invalid_argument &)
                {
                    expected_threw = true;
                }
                try
                {
                    actual.erase(row_index);
                }
                catch (const std::invalid_argument &)
                {
                    actual_threw = true;
                }
                require(actual_threw == expected_threw && actual_threw,
                        "double erase behavior mismatch after operation " +
                            std::to_string(operation));
            }
            else
            {
                const std::size_t row_index = active_rows[random() % active_rows.size()];
                const std::size_t column_index = random() % COLUMN_SIZE;
                require(actual.access(row_index, column_index) ==
                            expected.access(row_index, column_index),
                        "random access mismatch after operation " + std::to_string(operation));
                require(actual.access_row(row_index) == expected.access_row(row_index),
                        "random row access mismatch after operation " +
                            std::to_string(operation));
            }

            verify_equal(expected, actual, used_rows, operation);
        }
    }
}

int main()
{
    test_random_operations<1, 1>(1, 3000);
    test_random_operations<3, 7>(2, 3000);
    test_random_operations<5, 16>(3, 3000);
    test_random_operations<8, 128>(4, 3000);
    std::cout << "SemiSuccinct2DArray randomized differential tests passed\n";
    return 0;
}
