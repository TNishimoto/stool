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

#include "vector_collection_benchmark_measure.hpp"

namespace performance_test
{

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
            "container (all|none|vector|block16|block32|block64|block128|block256|block512|block1024)", false, "all",
            cmdline::oneof<std::string>("all", "none", "vector", "block16", "block32",
                                       "block64", "block128", "block256", "block512",
                                       "block1024"));
        parser.add<std::string>(
            "operation", 'a',
            "operation (all|read|write|write0|push_back|pop_back)", false, "all",
            cmdline::oneof<std::string>("all", "read", "write", "write0", "push_back",
                                       "pop_back"));
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
            parser.get<std::string>("operation"),
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

    double container_average_block_capacity(const std::vector<uint64_t> &)
    {
        return 0.0;
    }

    template <size_t BLOCK_SIZE>
    double container_average_block_capacity(const stool::BlockPackedVector<BLOCK_SIZE> &values)
    {
        return values.average_block_capacity();
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
        measurements.add(result, container_size_in_bytes(values),
                         container_average_block_capacity(values));
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

        for (size_t offset = 0; offset < 8; ++offset)
        {
            switch ((trial + offset) % 8)
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
                    measurements.vector.add(result, vector_size_in_bytes(values), 0.0);
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
                    measurements.block_16.add(result, values.size_in_bytes(),
                                              values.average_block_capacity());
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
                    measurements.block_32.add(result, values.size_in_bytes(),
                                              values.average_block_capacity());
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
                    measurements.block_64.add(result, values.size_in_bytes(),
                                              values.average_block_capacity());
                }
                break;
            case 4:
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
                    measurements.block_128.add(result, values.size_in_bytes(),
                                               values.average_block_capacity());
                }
                break;
            case 5:
                if (should_run_container(config, "block256"))
                {
                    stool::BlockPackedVector<256> values(initial_values);
                    const TimedResult result =
                        measure_block_packed_write(values, indices, new_values);
                    verify_equal(values, expected, input_size, trial, operation);
                    if (result.checksum != expected_checksum)
                    {
                        throw std::runtime_error(
                            operation +
                            " checksum mismatch for BlockPackedVector<256>, N=" +
                            std::to_string(input_size) + ", trial=" + std::to_string(trial));
                    }
                    measurements.block_256.add(result, values.size_in_bytes(),
                                               values.average_block_capacity());
                }
                break;
            case 6:
                if (should_run_container(config, "block512"))
                {
                    stool::BlockPackedVector<512> values(initial_values);
                    const TimedResult result =
                        measure_block_packed_write(values, indices, new_values);
                    verify_equal(values, expected, input_size, trial, operation);
                    if (result.checksum != expected_checksum)
                    {
                        throw std::runtime_error(
                            operation +
                            " checksum mismatch for BlockPackedVector<512>, N=" +
                            std::to_string(input_size) + ", trial=" + std::to_string(trial));
                    }
                    measurements.block_512.add(result, values.size_in_bytes(),
                                               values.average_block_capacity());
                }
                break;
            default:
                if (should_run_container(config, "block1024"))
                {
                    stool::BlockPackedVector<1024> values(initial_values);
                    const TimedResult result =
                        measure_block_packed_write(values, indices, new_values);
                    verify_equal(values, expected, input_size, trial, operation);
                    if (result.checksum != expected_checksum)
                    {
                        throw std::runtime_error(
                            operation +
                            " checksum mismatch for BlockPackedVector<1024>, N=" +
                            std::to_string(input_size) + ", trial=" + std::to_string(trial));
                    }
                    measurements.block_1024.add(result, values.size_in_bytes(),
                                                values.average_block_capacity());
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

        for (size_t offset = 0; offset < 8; ++offset)
        {
            switch ((trial + offset) % 8)
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
            case 4:
                if (should_run_container(config, "block128"))
                {
                    run_tail_container<stool::BlockPackedVector<128>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "BlockPackedVector<128>", measurements.block_128);
                }
                break;
            case 5:
                if (should_run_container(config, "block256"))
                {
                    run_tail_container<stool::BlockPackedVector<256>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "BlockPackedVector<256>", measurements.block_256);
                }
                break;
            case 6:
                if (should_run_container(config, "block512"))
                {
                    run_tail_container<stool::BlockPackedVector<512>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "BlockPackedVector<512>", measurements.block_512);
                }
                break;
            default:
                if (should_run_container(config, "block1024"))
                {
                    run_tail_container<stool::BlockPackedVector<1024>>(
                        setup_values, expected_values, new_values, operation, input_size, trial,
                        operation_name, "BlockPackedVector<1024>", measurements.block_1024);
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

        double block_capacity_sum = 0;
        for (double average_block_capacity : measurements.average_block_capacities)
        {
            block_capacity_sum += average_block_capacity;
        }
        const double average_block_capacity =
            block_capacity_sum / static_cast<double>(measurements.average_block_capacities.size());

        std::cout << input_size << ',' << static_cast<uint32_t>(value_bits) << ',' << operation
                  << ',' << container << ','
                  << measurements.nanoseconds_per_operation.size() << ',' << operations << ','
                  << std::fixed << std::setprecision(3) << average << ',' << *minmax.first << ','
                  << *minmax.second << ',' << average_memory << ',' << average_block_capacity << ','
                  << measurements.checksum << '\n';
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
        if (should_run_container(config, "block256"))
            print_measurements(input_size, config.value_bits, operation,
                               "BlockPackedVector<256>", config.operations,
                               measurements.block_256);
        if (should_run_container(config, "block512"))
            print_measurements(input_size, config.value_bits, operation,
                               "BlockPackedVector<512>", config.operations,
                               measurements.block_512);
        if (should_run_container(config, "block1024"))
            print_measurements(input_size, config.value_bits, operation,
                               "BlockPackedVector<1024>", config.operations,
                               measurements.block_1024);
    }

    void print_dummy_measurement(size_t input_size,
                                 const Config &config,
                                 const std::string &operation)
    {
        std::cout << input_size << ',' << static_cast<uint32_t>(config.value_bits) << ','
                  << operation << ",none," << config.trials << ',' << config.operations
                  << ",0.000,0.000,0.000,0.000,0.000,0\n";
    }

    void run_benchmark(size_t input_size, const Config &config, std::mt19937_64 &random)
    {
        const uint64_t maximum_value = (uint64_t{1} << config.value_bits) - 1;
        std::uniform_int_distribution<uint64_t> value_distribution(0, maximum_value);
        std::uniform_int_distribution<size_t> index_distribution(0, input_size - 1);
        const bool run_read = should_run_operation(config, "read");
        const bool run_write = should_run_operation(config, "write");
        const bool run_write_zeroes = should_run_operation(config, "write0");
        const bool run_push_back = should_run_operation(config, "push_back");
        const bool run_pop_back = should_run_operation(config, "pop_back");
        const bool needs_indices = run_read || run_write || run_write_zeroes;
        const bool needs_new_values = run_write || run_push_back || run_pop_back;

        std::vector<uint64_t> initial_values(input_size);
        for (uint64_t &value : initial_values)
        {
            value = value_distribution(random);
        }

        std::optional<stool::BlockPackedVector<16>> read_block_16;
        std::optional<stool::BlockPackedVector<32>> read_block_32;
        std::optional<stool::BlockPackedVector<64>> read_block_64;
        std::optional<stool::BlockPackedVector<128>> read_block_128;
        std::optional<stool::BlockPackedVector<256>> read_block_256;
        std::optional<stool::BlockPackedVector<512>> read_block_512;
        std::optional<stool::BlockPackedVector<1024>> read_block_1024;
        if (run_read && should_run_container(config, "block16"))
        {
            read_block_16.emplace(initial_values);
            verify_equal(*read_block_16, initial_values, input_size, 0, "initialization");
        }
        if (run_read && should_run_container(config, "block32"))
        {
            read_block_32.emplace(initial_values);
            verify_equal(*read_block_32, initial_values, input_size, 0, "initialization");
        }
        if (run_read && should_run_container(config, "block64"))
        {
            read_block_64.emplace(initial_values);
            verify_equal(*read_block_64, initial_values, input_size, 0, "initialization");
        }
        if (run_read && should_run_container(config, "block128"))
        {
            read_block_128.emplace(initial_values);
            verify_equal(*read_block_128, initial_values, input_size, 0, "initialization");
        }
        if (run_read && should_run_container(config, "block256"))
        {
            read_block_256.emplace(initial_values);
            verify_equal(*read_block_256, initial_values, input_size, 0, "initialization");
        }
        if (run_read && should_run_container(config, "block512"))
        {
            read_block_512.emplace(initial_values);
            verify_equal(*read_block_512, initial_values, input_size, 0, "initialization");
        }
        if (run_read && should_run_container(config, "block1024"))
        {
            read_block_1024.emplace(initial_values);
            verify_equal(*read_block_1024, initial_values, input_size, 0, "initialization");
        }

        Measurements vector_reads;
        Measurements block_16_reads;
        Measurements block_32_reads;
        Measurements block_64_reads;
        Measurements block_128_reads;
        Measurements block_256_reads;
        Measurements block_512_reads;
        Measurements block_1024_reads;
        WriteMeasurements writes;
        WriteMeasurements write_zeroes;
        WriteMeasurements push_backs;
        WriteMeasurements pop_backs;
        std::vector<uint64_t> zero_values;
        if (run_write_zeroes)
        {
            zero_values.assign(config.operations, 0);
        }

        for (size_t trial = 0; trial < config.trials; ++trial)
        {
            std::vector<size_t> indices(needs_indices ? config.operations : 0);
            std::vector<uint64_t> new_values(needs_new_values ? config.operations : 0);
            for (size_t i = 0; i < config.operations; ++i)
            {
                if (needs_indices)
                {
                    indices[i] = index_distribution(random);
                }
                if (needs_new_values)
                {
                    new_values[i] = value_distribution(random);
                }
            }

            uint64_t expected_read_checksum = 0;
            if (run_read)
            {
                for (size_t index : indices)
                {
                    expected_read_checksum += initial_values[index];
                }
            }

            if (run_read)
            {
                for (size_t offset = 0; offset < 8; ++offset)
                {
                    switch ((trial + offset) % 8)
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
                            vector_reads.add(result, vector_size_in_bytes(initial_values), 0.0);
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
                                    std::to_string(input_size) + ", trial=" +
                                    std::to_string(trial));
                            }
                            block_16_reads.add(result, read_block_16->size_in_bytes(),
                                               read_block_16->average_block_capacity());
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
                                    std::to_string(input_size) + ", trial=" +
                                    std::to_string(trial));
                            }
                            block_32_reads.add(result, read_block_32->size_in_bytes(),
                                               read_block_32->average_block_capacity());
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
                                    std::to_string(input_size) + ", trial=" +
                                    std::to_string(trial));
                            }
                            block_64_reads.add(result, read_block_64->size_in_bytes(),
                                               read_block_64->average_block_capacity());
                        }
                        break;
                    case 4:
                        if (read_block_128)
                        {
                            const TimedResult result =
                                measure_block_packed_read(*read_block_128, indices);
                            if (result.checksum != expected_read_checksum)
                            {
                                throw std::runtime_error(
                                    "read checksum mismatch for BlockPackedVector<128>, N=" +
                                    std::to_string(input_size) + ", trial=" +
                                    std::to_string(trial));
                            }
                            block_128_reads.add(result, read_block_128->size_in_bytes(),
                                                read_block_128->average_block_capacity());
                        }
                        break;
                    case 5:
                        if (read_block_256)
                        {
                            const TimedResult result =
                                measure_block_packed_read(*read_block_256, indices);
                            if (result.checksum != expected_read_checksum)
                            {
                                throw std::runtime_error(
                                    "read checksum mismatch for BlockPackedVector<256>, N=" +
                                    std::to_string(input_size) + ", trial=" +
                                    std::to_string(trial));
                            }
                            block_256_reads.add(result, read_block_256->size_in_bytes(),
                                                read_block_256->average_block_capacity());
                        }
                        break;
                    case 6:
                        if (read_block_512)
                        {
                            const TimedResult result =
                                measure_block_packed_read(*read_block_512, indices);
                            if (result.checksum != expected_read_checksum)
                            {
                                throw std::runtime_error(
                                    "read checksum mismatch for BlockPackedVector<512>, N=" +
                                    std::to_string(input_size) + ", trial=" +
                                    std::to_string(trial));
                            }
                            block_512_reads.add(result, read_block_512->size_in_bytes(),
                                                read_block_512->average_block_capacity());
                        }
                        break;
                    default:
                        if (read_block_1024)
                        {
                            const TimedResult result =
                                measure_block_packed_read(*read_block_1024, indices);
                            if (result.checksum != expected_read_checksum)
                            {
                                throw std::runtime_error(
                                    "read checksum mismatch for BlockPackedVector<1024>, N=" +
                                    std::to_string(input_size) + ", trial=" +
                                    std::to_string(trial));
                            }
                            block_1024_reads.add(result, read_block_1024->size_in_bytes(),
                                                 read_block_1024->average_block_capacity());
                        }
                        break;
                    }
                }
            }

            if (run_write)
            {
                run_write_trial(initial_values, indices, new_values, input_size, trial, "write",
                                config, writes);
            }
            if (run_write_zeroes)
            {
                run_write_trial(initial_values, indices, zero_values, input_size, trial, "write0",
                                config, write_zeroes);
            }
            if (run_push_back)
            {
                run_tail_trial(initial_values, new_values, TailOperation::PushBack, input_size,
                               trial, "push_back", config, push_backs);
            }
            if (run_pop_back)
            {
                run_tail_trial(initial_values, new_values, TailOperation::PopBack, input_size, trial,
                               "pop_back", config, pop_backs);
            }
        }

        if (config.container == "none")
        {
            if (run_read)
                print_dummy_measurement(input_size, config, "read");
            if (run_write)
                print_dummy_measurement(input_size, config, "write");
            if (run_write_zeroes)
                print_dummy_measurement(input_size, config, "write0");
            if (run_push_back)
                print_dummy_measurement(input_size, config, "push_back");
            if (run_pop_back)
                print_dummy_measurement(input_size, config, "pop_back");
            return;
        }

        if (run_read && should_run_container(config, "vector"))
            print_measurements(input_size, config.value_bits, "read", "std::vector<uint64_t>",
                               config.operations, vector_reads);
        if (run_read && should_run_container(config, "block16"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<16>",
                               config.operations, block_16_reads);
        if (run_read && should_run_container(config, "block32"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<32>",
                               config.operations, block_32_reads);
        if (run_read && should_run_container(config, "block64"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<64>",
                               config.operations, block_64_reads);
        if (run_read && should_run_container(config, "block128"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<128>",
                               config.operations, block_128_reads);
        if (run_read && should_run_container(config, "block256"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<256>",
                               config.operations, block_256_reads);
        if (run_read && should_run_container(config, "block512"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<512>",
                               config.operations, block_512_reads);
        if (run_read && should_run_container(config, "block1024"))
            print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<1024>",
                               config.operations, block_1024_reads);
        if (run_write)
            print_write_measurements(input_size, config, "write", writes);
        if (run_write_zeroes)
            print_write_measurements(input_size, config, "write0", write_zeroes);
        if (run_push_back)
            print_write_measurements(input_size, config, "push_back", push_backs);
        if (run_pop_back)
            print_write_measurements(input_size, config, "pop_back", pop_backs);
    }
}

int main(int argc, char **argv)
{
    try
    {
        const performance_test::Config config = performance_test::parse_arguments(argc, argv);
        std::mt19937_64 random(config.seed);

        std::cout << "N,value_bits,operation,container,trials,operations,average_ns_per_op,"
                     "min_ns_per_op,max_ns_per_op,average_memory_bytes,average_block_capacity,"
                     "checksum\n";
        performance_test::run_benchmark(config.input_size, config, random);
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
