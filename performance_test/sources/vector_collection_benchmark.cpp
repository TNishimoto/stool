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

#include "cmdline/cmdline.h"

#include "../../include/specialized_collection/block_wise_packed_vector.hpp"

namespace
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
    };

    bool should_run_container(const Config &config, const std::string &container)
    {
        return config.container == "all" || config.container == container;
    }

    struct TimedResult
    {
        double nanoseconds_per_operation;
        uint64_t checksum;
    };

    struct Measurements
    {
        std::vector<double> nanoseconds_per_operation;
        std::vector<uint64_t> memory_bytes;
        uint64_t checksum = 0;

        void add(const TimedResult &result, uint64_t container_memory_bytes)
        {
            nanoseconds_per_operation.push_back(result.nanoseconds_per_operation);
            memory_bytes.push_back(container_memory_bytes);
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
    };

    enum class TailOperation
    {
        PushBack,
        PopBack,
    };

    Config parse_arguments(int argc, char **argv)
    {
        cmdline::parser parser;
        parser.add<size_t>("size", 'n', "integer sequence length", false, 1000000,
                           cmdline::range<size_t>(1, std::numeric_limits<size_t>::max()));
        parser.add<size_t>("operations", 'o', "operations per trial", false,
                           DEFAULT_OPERATIONS,
                           cmdline::range<size_t>(1, std::numeric_limits<size_t>::max()));
        parser.add<size_t>("trials", 't', "number of trials", false, DEFAULT_TRIALS,
                           cmdline::range<size_t>(1, std::numeric_limits<size_t>::max()));
        parser.add<uint64_t>("seed", 's', "random seed", false, DEFAULT_SEED);
        parser.add<uint32_t>("value-bits", 'b', "random value width", false, 32,
                             cmdline::oneof<uint32_t>(8, 16, 32));
        parser.add<std::string>(
            "container", 'c',
            "container (all|none|vector|block16|block32|block64|block128)", false, "all",
            cmdline::oneof<std::string>("all", "none", "vector", "block16", "block32",
                                       "block64", "block128"));
        parser.add("help", '?', "print this message");

        const bool valid = parser.parse(argc, argv);
        if (parser.exist("help"))
        {
            std::cout << parser.usage();
            std::exit(0);
        }
        if (!valid)
        {
            throw std::invalid_argument(parser.error() + "\n" + parser.usage());
        }

        return {
            parser.get<size_t>("size"),
            parser.get<size_t>("operations"),
            parser.get<size_t>("trials"),
            parser.get<uint64_t>("seed"),
            static_cast<uint8_t>(parser.get<uint32_t>("value-bits")),
            parser.get<std::string>("container"),
        };
    }

    template <size_t BLOCK_SIZE>
    void verify_equal(const stool::BlockPackedVector<BLOCK_SIZE> &packed,
                      const std::vector<uint64_t> &values,
                      size_t input_size,
                      size_t trial,
                      const std::string &phase)
    {
        if (packed.size() != values.size())
        {
            throw std::runtime_error(phase + " BlockPackedVector size mismatch for N=" +
                                     std::to_string(input_size) + ", trial=" +
                                     std::to_string(trial));
        }

        for (size_t i = 0; i < values.size(); ++i)
        {
            if (packed[i] != values[i])
            {
                throw std::runtime_error(phase + " BlockPackedVector value mismatch for N=" +
                                         std::to_string(input_size) + ", trial=" +
                                         std::to_string(trial) + ", index=" +
                                         std::to_string(i));
            }
        }
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

    uint64_t vector_size_in_bytes(const std::vector<uint64_t> &values)
    {
        return sizeof(values) + (values.capacity() * sizeof(uint64_t));
    }

    uint64_t container_size_in_bytes(const std::vector<uint64_t> &values)
    {
        return vector_size_in_bytes(values);
    }

    template <size_t BLOCK_SIZE>
    uint64_t container_size_in_bytes(const stool::BlockPackedVector<BLOCK_SIZE> &values)
    {
        return values.size_in_bytes();
    }

    TimedResult measure_vector_read(const std::vector<uint64_t> &values,
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
                    values.pop_back();
                }
            }
            return uint64_t{0};
        });
        result.checksum = hash_values(values);
        return result;
    }

    template <typename Container>
    void run_tail_container(const std::vector<uint64_t> &setup_values,
                            const std::vector<uint64_t> &expected_values,
                            const std::vector<uint64_t> &new_values,
                            TailOperation operation,
                            size_t input_size,
                            size_t trial,
                            const std::string &operation_name,
                            const std::string &container_name,
                            Measurements &measurements)
    {
        Container values(setup_values);
        const TimedResult result = measure_tail_operation(values, new_values, operation);

        if (values.size() != expected_values.size())
        {
            throw std::runtime_error(operation_name + " size mismatch for " + container_name +
                                     ", N=" + std::to_string(input_size) + ", trial=" +
                                     std::to_string(trial));
        }
        for (size_t i = 0; i < expected_values.size(); ++i)
        {
            if (values[i] != expected_values[i])
            {
                throw std::runtime_error(operation_name + " value mismatch for " +
                                         container_name + ", N=" +
                                         std::to_string(input_size) + ", trial=" +
                                         std::to_string(trial) + ", index=" +
                                         std::to_string(i));
            }
        }

        const uint64_t expected_checksum = hash_values(expected_values);
        if (result.checksum != expected_checksum)
        {
            throw std::runtime_error(operation_name + " checksum mismatch for " +
                                     container_name + ", N=" +
                                     std::to_string(input_size) + ", trial=" +
                                     std::to_string(trial));
        }
        measurements.add(result, container_size_in_bytes(values));
    }

    void run_write_trial(const std::vector<uint64_t> &initial_values,
                         const std::vector<size_t> &indices,
                         const std::vector<uint64_t> &new_values,
                         size_t input_size,
                         size_t trial,
                         const std::string &operation,
                         const Config &config,
                         WriteMeasurements &measurements)
    {
        std::vector<uint64_t> expected = initial_values;
        for (size_t i = 0; i < indices.size(); ++i)
        {
            expected[indices[i]] = new_values[i];
        }
        const uint64_t expected_checksum = hash_values(expected);

        for (size_t offset = 0; offset < 5; ++offset)
        {
            switch ((trial + offset) % 5)
            {
            case 0:
                if (should_run_container(config, "vector"))
                {
                    std::vector<uint64_t> values = initial_values;
                    const TimedResult result =
                        measure_vector_write(values, indices, new_values);
                    if (result.checksum != expected_checksum)
                    {
                        throw std::runtime_error(operation + " checksum mismatch for vector, N=" +
                                                 std::to_string(input_size) + ", trial=" +
                                                 std::to_string(trial));
                    }
                    measurements.vector.add(result, vector_size_in_bytes(values));
                }
                break;
            case 1:
                if (should_run_container(config, "block16"))
                {
                    stool::BlockPackedVector<16> values(initial_values);
                    const TimedResult result =
                        measure_block_packed_write(values, indices, new_values);
                    verify_equal(values, expected, input_size, trial, operation);
                    if (result.checksum != expected_checksum)
                    {
                        throw std::runtime_error(operation +
                                                 " checksum mismatch for BlockPackedVector<16>, N=" +
                                                 std::to_string(input_size) + ", trial=" +
                                                 std::to_string(trial));
                    }
                    measurements.block_16.add(result, values.size_in_bytes());
                }
                break;
            case 2:
                if (should_run_container(config, "block32"))
                {
                    stool::BlockPackedVector<32> values(initial_values);
                    const TimedResult result =
                        measure_block_packed_write(values, indices, new_values);
                    verify_equal(values, expected, input_size, trial, operation);
                    if (result.checksum != expected_checksum)
                    {
                        throw std::runtime_error(operation +
                                                 " checksum mismatch for BlockPackedVector<32>, N=" +
                                                 std::to_string(input_size) + ", trial=" +
                                                 std::to_string(trial));
                    }
                    measurements.block_32.add(result, values.size_in_bytes());
                }
                break;
            case 3:
                if (should_run_container(config, "block64"))
                {
                    stool::BlockPackedVector<64> values(initial_values);
                    const TimedResult result =
                        measure_block_packed_write(values, indices, new_values);
                    verify_equal(values, expected, input_size, trial, operation);
                    if (result.checksum != expected_checksum)
                    {
                        throw std::runtime_error(operation +
                                                 " checksum mismatch for BlockPackedVector<64>, N=" +
                                                 std::to_string(input_size) + ", trial=" +
                                                 std::to_string(trial));
                    }
                    measurements.block_64.add(result, values.size_in_bytes());
                }
                break;
            default:
                if (should_run_container(config, "block128"))
                {
                    stool::BlockPackedVector<128> values(initial_values);
                    const TimedResult result =
                        measure_block_packed_write(values, indices, new_values);
                    verify_equal(values, expected, input_size, trial, operation);
                    if (result.checksum != expected_checksum)
                    {
                        throw std::runtime_error(
                            operation +
                            " checksum mismatch for BlockPackedVector<128>, N=" +
                            std::to_string(input_size) + ", trial=" + std::to_string(trial));
                    }
                    measurements.block_128.add(result, values.size_in_bytes());
                }
                break;
            }
        }
    }

    void run_tail_trial(const std::vector<uint64_t> &initial_values,
                        const std::vector<uint64_t> &new_values,
                        TailOperation operation,
                        size_t input_size,
                        size_t trial,
                        const std::string &operation_name,
                        const Config &config,
                        WriteMeasurements &measurements)
    {
        std::vector<uint64_t> expanded_values = initial_values;
        expanded_values.insert(expanded_values.end(), new_values.begin(), new_values.end());

        const std::vector<uint64_t> &setup_values =
            operation == TailOperation::PushBack ? initial_values : expanded_values;
        const std::vector<uint64_t> &expected_values =
            operation == TailOperation::PushBack ? expanded_values : initial_values;

        for (size_t offset = 0; offset < 5; ++offset)
        {
            switch ((trial + offset) % 5)
            {
            case 0:
                if (should_run_container(config, "vector"))
                {
                    run_tail_container<std::vector<uint64_t>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "std::vector<uint64_t>", measurements.vector);
                }
                break;
            case 1:
                if (should_run_container(config, "block16"))
                {
                    run_tail_container<stool::BlockPackedVector<16>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "BlockPackedVector<16>", measurements.block_16);
                }
                break;
            case 2:
                if (should_run_container(config, "block32"))
                {
                    run_tail_container<stool::BlockPackedVector<32>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "BlockPackedVector<32>", measurements.block_32);
                }
                break;
            case 3:
                if (should_run_container(config, "block64"))
                {
                    run_tail_container<stool::BlockPackedVector<64>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "BlockPackedVector<64>", measurements.block_64);
                }
                break;
            default:
                if (should_run_container(config, "block128"))
                {
                    run_tail_container<stool::BlockPackedVector<128>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "BlockPackedVector<128>", measurements.block_128);
                }
                break;
            }
        }
    }

    void print_measurements(size_t input_size,
                            uint8_t value_bits,
                            const std::string &operation,
                            const std::string &container,
                            size_t operations,
                            const Measurements &measurements)
    {
        const auto minmax = std::minmax_element(
            measurements.nanoseconds_per_operation.begin(),
            measurements.nanoseconds_per_operation.end());
        const auto memory_minmax =
            std::minmax_element(measurements.memory_bytes.begin(), measurements.memory_bytes.end());

        double sum = 0;
        for (double sample : measurements.nanoseconds_per_operation)
        {
            sum += sample;
        }
        const double average =
            sum / static_cast<double>(measurements.nanoseconds_per_operation.size());

        long double memory_sum = 0;
        for (uint64_t memory_bytes : measurements.memory_bytes)
        {
            memory_sum += memory_bytes;
        }
        const long double average_memory =
            memory_sum / static_cast<long double>(measurements.memory_bytes.size());

        std::cout << input_size << ',' << static_cast<uint32_t>(value_bits) << ',' << operation
                  << ',' << container << ','
                  << measurements.nanoseconds_per_operation.size() << ',' << operations << ','
                  << std::fixed << std::setprecision(3) << average << ',' << *minmax.first << ','
                  << *minmax.second << ',' << average_memory << ',' << *memory_minmax.first << ','
                  << *memory_minmax.second << ',' << measurements.checksum << '\n';
    }

    void print_write_measurements(size_t input_size,
                                  const Config &config,
                                  const std::string &operation,
                                  const WriteMeasurements &measurements)
    {
        if (should_run_container(config, "vector"))
            print_measurements(input_size, config.value_bits, operation, "std::vector<uint64_t>",
                               config.operations, measurements.vector);
        if (should_run_container(config, "block16"))
            print_measurements(input_size, config.value_bits, operation, "BlockPackedVector<16>",
                               config.operations, measurements.block_16);
        if (should_run_container(config, "block32"))
            print_measurements(input_size, config.value_bits, operation, "BlockPackedVector<32>",
                               config.operations, measurements.block_32);
        if (should_run_container(config, "block64"))
            print_measurements(input_size, config.value_bits, operation, "BlockPackedVector<64>",
                               config.operations, measurements.block_64);
        if (should_run_container(config, "block128"))
            print_measurements(input_size, config.value_bits, operation,
                               "BlockPackedVector<128>", config.operations,
                               measurements.block_128);
    }

    void print_dummy_measurement(size_t input_size,
                                 const Config &config,
                                 const std::string &operation)
    {
        std::cout << input_size << ',' << static_cast<uint32_t>(config.value_bits) << ','
                  << operation << ",none," << config.trials << ',' << config.operations
                  << ",0.000,0.000,0.000,0.000,0,0,0\n";
    }

    void run_benchmark(size_t input_size, const Config &config, std::mt19937_64 &random)
    {
        const uint64_t maximum_value = (uint64_t{1} << config.value_bits) - 1;
        std::uniform_int_distribution<uint64_t> value_distribution(0, maximum_value);
        std::uniform_int_distribution<size_t> index_distribution(0, input_size - 1);

        std::vector<uint64_t> initial_values(input_size);
        for (uint64_t &value : initial_values)
        {
            value = value_distribution(random);
        }

        std::optional<stool::BlockPackedVector<16>> read_block_16;
        std::optional<stool::BlockPackedVector<32>> read_block_32;
        std::optional<stool::BlockPackedVector<64>> read_block_64;
        std::optional<stool::BlockPackedVector<128>> read_block_128;
        if (should_run_container(config, "block16"))
        {
            read_block_16.emplace(initial_values);
            verify_equal(*read_block_16, initial_values, input_size, 0, "initialization");
        }
        if (should_run_container(config, "block32"))
        {
            read_block_32.emplace(initial_values);
            verify_equal(*read_block_32, initial_values, input_size, 0, "initialization");
        }
        if (should_run_container(config, "block64"))
        {
            read_block_64.emplace(initial_values);
            verify_equal(*read_block_64, initial_values, input_size, 0, "initialization");
        }
        if (should_run_container(config, "block128"))
        {
            read_block_128.emplace(initial_values);
            verify_equal(*read_block_128, initial_values, input_size, 0, "initialization");
        }

        Measurements vector_reads;
        Measurements block_16_reads;
        Measurements block_32_reads;
        Measurements block_64_reads;
        Measurements block_128_reads;
        WriteMeasurements writes;
        WriteMeasurements write_zeroes;
        WriteMeasurements push_backs;
        WriteMeasurements pop_backs;
        const std::vector<uint64_t> zero_values(config.operations, 0);

        for (size_t trial = 0; trial < config.trials; ++trial)
        {
            std::vector<size_t> indices(config.operations);
            std::vector<uint64_t> new_values(config.operations);
            for (size_t i = 0; i < config.operations; ++i)
            {
                indices[i] = index_distribution(random);
                new_values[i] = value_distribution(random);
            }

            uint64_t expected_read_checksum = 0;
            for (size_t index : indices)
            {
                expected_read_checksum += initial_values[index];
            }

            for (size_t offset = 0; offset < 5; ++offset)
            {
                switch ((trial + offset) % 5)
                {
                case 0:
                    if (should_run_container(config, "vector"))
                    {
                        const TimedResult result = measure_vector_read(initial_values, indices);
                        if (result.checksum != expected_read_checksum)
                        {
                            throw std::runtime_error("read checksum mismatch for vector, N=" +
                                                     std::to_string(input_size) + ", trial=" +
                                                     std::to_string(trial));
                        }
                        vector_reads.add(result, vector_size_in_bytes(initial_values));
                    }
                    break;
                case 1:
                    if (read_block_16)
                    {
                        const TimedResult result =
                            measure_block_packed_read(*read_block_16, indices);
                        if (result.checksum != expected_read_checksum)
                        {
                            throw std::runtime_error(
                                "read checksum mismatch for BlockPackedVector<16>, N=" +
                                std::to_string(input_size) + ", trial=" + std::to_string(trial));
                        }
                        block_16_reads.add(result, read_block_16->size_in_bytes());
                    }
                    break;
                case 2:
                    if (read_block_32)
                    {
                        const TimedResult result =
                            measure_block_packed_read(*read_block_32, indices);
                        if (result.checksum != expected_read_checksum)
                        {
                            throw std::runtime_error(
                                "read checksum mismatch for BlockPackedVector<32>, N=" +
                                std::to_string(input_size) + ", trial=" + std::to_string(trial));
                        }
                        block_32_reads.add(result, read_block_32->size_in_bytes());
                    }
                    break;
                case 3:
                    if (read_block_64)
                    {
                        const TimedResult result =
                            measure_block_packed_read(*read_block_64, indices);
                        if (result.checksum != expected_read_checksum)
                        {
                            throw std::runtime_error(
                                "read checksum mismatch for BlockPackedVector<64>, N=" +
                                std::to_string(input_size) + ", trial=" + std::to_string(trial));
                        }
                        block_64_reads.add(result, read_block_64->size_in_bytes());
                    }
                    break;
                default:
                    if (read_block_128)
                    {
                        const TimedResult result =
                            measure_block_packed_read(*read_block_128, indices);
                        if (result.checksum != expected_read_checksum)
                        {
                            throw std::runtime_error(
                                "read checksum mismatch for BlockPackedVector<128>, N=" +
                                std::to_string(input_size) + ", trial=" + std::to_string(trial));
                        }
                        block_128_reads.add(result, read_block_128->size_in_bytes());
                    }
                    break;
                }
            }

            run_write_trial(initial_values, indices, new_values, input_size, trial, "write",
                            config, writes);
            run_write_trial(initial_values, indices, zero_values, input_size, trial, "write0",
                            config, write_zeroes);
            run_tail_trial(initial_values, new_values, TailOperation::PushBack, input_size, trial,
                           "push_back", config, push_backs);
            run_tail_trial(initial_values, new_values, TailOperation::PopBack, input_size, trial,
                           "pop_back", config, pop_backs);
        }

        if (config.container == "none")
        {
            print_dummy_measurement(input_size, config, "read");
            print_dummy_measurement(input_size, config, "write");
            print_dummy_measurement(input_size, config, "write0");
            print_dummy_measurement(input_size, config, "push_back");
            print_dummy_measurement(input_size, config, "pop_back");
            return;
        }

        if (should_run_container(config, "vector"))
            print_measurements(input_size, config.value_bits, "read", "std::vector<uint64_t>",
                               config.operations, vector_reads);
        if (should_run_container(config, "block16"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<16>",
                               config.operations, block_16_reads);
        if (should_run_container(config, "block32"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<32>",
                               config.operations, block_32_reads);
        if (should_run_container(config, "block64"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<64>",
                               config.operations, block_64_reads);
        if (should_run_container(config, "block128"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<128>",
                               config.operations, block_128_reads);
        print_write_measurements(input_size, config, "write", writes);
        print_write_measurements(input_size, config, "write0", write_zeroes);
        print_write_measurements(input_size, config, "push_back", push_backs);
        print_write_measurements(input_size, config, "pop_back", pop_backs);
    }
}

int main(int argc, char **argv)
{
    try
    {
        const Config config = parse_arguments(argc, argv);
        std::mt19937_64 random(config.seed);

        std::cout << "N,value_bits,operation,container,trials,operations,average_ns_per_op,"
                     "min_ns_per_op,max_ns_per_op,average_memory_bytes,min_memory_bytes,"
                     "max_memory_bytes,checksum\n";
        run_benchmark(config.input_size, config, random);
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
