#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../include/specialized_collection/block_wise_packed_vector.hpp"

namespace performance_test
{

    constexpr size_t DEFAULT_OPERATIONS = 10'000;
    constexpr size_t DEFAULT_TRIALS = 10;
    constexpr uint64_t DEFAULT_SEED = 123456789;

    struct Config
    {
        size_t input_size = 0;
        size_t operations = DEFAULT_OPERATIONS;
        size_t trials = DEFAULT_TRIALS;
        uint64_t seed = DEFAULT_SEED;
        uint8_t value_bits = 32;
        std::string container = "all";
        std::string operation = "all";
    };

    bool should_run_container(const Config &config, const std::string &container)
    {
        return config.container == "all" || config.container == container;
    }

    bool should_run_operation(const Config &config, const std::string &operation)
    {
        return config.operation == "all" || config.operation == operation;
    }

    enum class TailOperation
    {
        PushBack,
        PopBack,
    };

    struct TimedResult
    {
        double nanoseconds_per_operation;
        uint64_t checksum;
    };

    struct Measurements
    {
        std::vector<double> nanoseconds_per_operation;
        std::vector<uint64_t> memory_bytes;
        std::vector<double> average_block_capacities;
        uint64_t checksum = 0;

        void add(const TimedResult &result,
                 uint64_t container_memory_bytes,
                 double container_average_block_capacity)
        {
            nanoseconds_per_operation.push_back(result.nanoseconds_per_operation);
            memory_bytes.push_back(container_memory_bytes);
            average_block_capacities.push_back(container_average_block_capacity);
            checksum += result.checksum;
        }
    };

    struct WriteMeasurements
    {
        Measurements vector;
        Measurements block_16;
        Measurements block_32;
        Measurements block_64;
        Measurements block_128;
        Measurements block_256;
        Measurements block_512;
        Measurements block_1024;
    };



    uint64_t hash_values(const std::vector<uint64_t> &values)
    {
        uint64_t hash = 1469598103934665603ULL;
        for (uint64_t value : values)
        {
            hash ^= value;
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    template <size_t BLOCK_SIZE>
    uint64_t hash_values(const stool::BlockPackedVector<BLOCK_SIZE> &values)
    {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < values.size(); ++i)
        {
            hash ^= values[i];
            hash *= 1099511628211ULL;
        }
        return hash;
    }


    template <typename Function>
    TimedResult measure(size_t operation_count, Function &&function)
    {
        const auto start = std::chrono::steady_clock::now();
        const uint64_t checksum = function();
        const auto finish = std::chrono::steady_clock::now();
        const double elapsed_ns =
            std::chrono::duration<double, std::nano>(finish - start).count();
        return {elapsed_ns / static_cast<double>(operation_count), checksum};
    }

    TimedResult measure_vector_read(const std::vector<uint64_t> &values,
                                    const std::vector<size_t> &indices)
    {
        return measure(indices.size(), [&]()
                       {
            uint64_t checksum = 0;
            for (size_t index : indices)
            {
                checksum += values[index];
            }
            return checksum; });
    }


    template <size_t BLOCK_SIZE>
    TimedResult measure_block_packed_read(const stool::BlockPackedVector<BLOCK_SIZE> &values,
                                          const std::vector<size_t> &indices)
    {
        return measure(indices.size(), [&]() {
            uint64_t checksum = 0;
            for (size_t index : indices)
            {
                checksum += values[index];
            }
            return checksum;
        });
    }

    TimedResult measure_vector_write(std::vector<uint64_t> &values,
                                     const std::vector<size_t> &indices,
                                     const std::vector<uint64_t> &new_values)
    {
        TimedResult result = measure(indices.size(), [&]() {
            for (size_t i = 0; i < indices.size(); ++i)
            {
                values[indices[i]] = new_values[i];
            }
            return uint64_t{0};
        });
        result.checksum = hash_values(values);
        return result;
    }

    template <size_t BLOCK_SIZE>
    TimedResult measure_block_packed_write(stool::BlockPackedVector<BLOCK_SIZE> &values,
                                           const std::vector<size_t> &indices,
                                           const std::vector<uint64_t> &new_values)
    {
        TimedResult result = measure(indices.size(), [&]() {
            for (size_t i = 0; i < indices.size(); ++i)
            {
                values[indices[i]] = new_values[i];
            }
            return uint64_t{0};
        });
        result.checksum = hash_values(values);
        return result;
    }

    template <typename Container>
    TimedResult measure_tail_operation(Container &values,
                                       const std::vector<uint64_t> &new_values,
                                       TailOperation operation)
    {
        TimedResult result = measure(new_values.size(), [&]() {
            if (operation == TailOperation::PushBack)
            {
                for (uint64_t value : new_values)
                {
                    values.push_back(value);
                }
            }
            else
            {
                for (size_t i = 0; i < new_values.size(); ++i)
                {
                    result.checksum ^= new_values[i];
                    values.pop_back();
                }
            }
            return uint64_t{0};
        });
        result.checksum = hash_values(values);
        return result;
    }
}
