#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "specialized_collection/block_wise_packed_vector.hpp"
#include "specialized_collection/vlc_deque.hpp"

namespace
{
    constexpr std::array<size_t, 4> INPUT_SIZES = {64, 128, 256, 512};
    constexpr size_t DEFAULT_OPERATIONS = 1'000'000;
    constexpr size_t DEFAULT_TRIALS = 10;
    constexpr uint64_t DEFAULT_SEED = 123456789;

    struct Config
    {
        size_t operations = DEFAULT_OPERATIONS;
        size_t trials = DEFAULT_TRIALS;
        uint64_t seed = DEFAULT_SEED;
        uint8_t value_bits = 32;
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
        uint64_t checksum = 0;

        void add(const TimedResult &result, uint64_t container_memory_bytes)
        {
            nanoseconds_per_operation.push_back(result.nanoseconds_per_operation);
            memory_bytes.push_back(container_memory_bytes);
            checksum += result.checksum;
        }
    };

    void print_usage(std::ostream &out, const char *program_name)
    {
        out << "Usage: " << program_name
            << " [--operations COUNT] [--trials COUNT] [--seed VALUE]"
               " [--value-bits 8|16|32]\n";
    }

    uint64_t parse_uint64(const std::string &text, const std::string &option)
    {
        if (text.empty() || text.front() == '-')
        {
            throw std::invalid_argument(option + " requires a non-negative integer");
        }

        uint64_t value = 0;
        const char *begin = text.data();
        const char *end = begin + text.size();
        const auto result = std::from_chars(begin, end, value);
        if (result.ec != std::errc() || result.ptr != end)
        {
            throw std::invalid_argument("invalid value for " + option + ": " + text);
        }
        return value;
    }

    size_t parse_positive_size(const std::string &text, const std::string &option)
    {
        const uint64_t value = parse_uint64(text, option);
        if (value == 0 || value > std::numeric_limits<size_t>::max())
        {
            throw std::invalid_argument(option + " requires a positive size_t value");
        }
        return static_cast<size_t>(value);
    }

    Config parse_arguments(int argc, char **argv)
    {
        Config config;
        for (int i = 1; i < argc; ++i)
        {
            const std::string option = argv[i];
            if (option == "--help")
            {
                print_usage(std::cout, argv[0]);
                std::exit(0);
            }
            if (i + 1 >= argc)
            {
                throw std::invalid_argument("missing value for " + option);
            }

            const std::string value = argv[++i];
            if (option == "--operations")
            {
                config.operations = parse_positive_size(value, option);
            }
            else if (option == "--trials")
            {
                config.trials = parse_positive_size(value, option);
            }
            else if (option == "--seed")
            {
                config.seed = parse_uint64(value, option);
            }
            else if (option == "--value-bits")
            {
                const uint64_t value_bits = parse_uint64(value, option);
                if (value_bits != 8 && value_bits != 16 && value_bits != 32)
                {
                    throw std::invalid_argument("--value-bits must be 8, 16, or 32");
                }
                config.value_bits = static_cast<uint8_t>(value_bits);
            }
            else
            {
                throw std::invalid_argument("unknown option: " + option);
            }
        }
        return config;
    }

    stool::VLCDeque make_vlc_deque(const std::vector<uint64_t> &values)
    {
        stool::VLCDeque result;
        for (uint64_t value : values)
        {
            result.push_back(value);
        }
        return result;
    }

    void verify_equal(const stool::VLCDeque &vlc,
                      const std::vector<uint64_t> &values,
                      size_t input_size,
                      size_t trial,
                      const std::string &phase)
    {
        if (vlc.size() != values.size())
        {
            throw std::runtime_error(phase + " size mismatch for N=" +
                                     std::to_string(input_size) + ", trial=" +
                                     std::to_string(trial));
        }

        for (size_t i = 0; i < values.size(); ++i)
        {
            if (vlc.at(i) != values[i])
            {
                throw std::runtime_error(phase + " value mismatch for N=" +
                                         std::to_string(input_size) + ", trial=" +
                                         std::to_string(trial) + ", index=" +
                                         std::to_string(i));
            }
        }
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

    uint64_t hash_values(const stool::VLCDeque &values)
    {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < values.size(); ++i)
        {
            hash ^= values.at(i);
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

    TimedResult measure_vlc_read(const stool::VLCDeque &values,
                                 const std::vector<size_t> &indices)
    {
        return measure(indices.size(), [&]() {
            uint64_t checksum = 0;
            for (size_t index : indices)
            {
                checksum += values.at(index);
            }
            return checksum;
        });
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

    TimedResult measure_vlc_write(stool::VLCDeque &values,
                                  const std::vector<size_t> &indices,
                                  const std::vector<uint64_t> &new_values)
    {
        TimedResult result = measure(indices.size(), [&]() {
            for (size_t i = 0; i < indices.size(); ++i)
            {
                values.set_value(indices[i], new_values[i]);
            }
            return uint64_t{0};
        });
        result.checksum = hash_values(values);
        return result;
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

        stool::VLCDeque read_vlc = make_vlc_deque(initial_values);
        stool::BlockPackedVector<16> read_block_16(initial_values);
        stool::BlockPackedVector<32> read_block_32(initial_values);
        stool::BlockPackedVector<64> read_block_64(initial_values);
        stool::BlockPackedVector<128> read_block_128(initial_values);
        verify_equal(read_vlc, initial_values, input_size, 0, "initialization");
        verify_equal(read_block_16, initial_values, input_size, 0, "initialization");
        verify_equal(read_block_32, initial_values, input_size, 0, "initialization");
        verify_equal(read_block_64, initial_values, input_size, 0, "initialization");
        verify_equal(read_block_128, initial_values, input_size, 0, "initialization");

        Measurements vlc_reads;
        Measurements vector_reads;
        Measurements block_16_reads;
        Measurements block_32_reads;
        Measurements block_64_reads;
        Measurements block_128_reads;
        Measurements vlc_writes;
        Measurements vector_writes;
        Measurements block_16_writes;
        Measurements block_32_writes;
        Measurements block_64_writes;
        Measurements block_128_writes;

        for (size_t trial = 0; trial < config.trials; ++trial)
        {
            std::vector<size_t> indices(config.operations);
            std::vector<uint64_t> new_values(config.operations);
            for (size_t i = 0; i < config.operations; ++i)
            {
                indices[i] = index_distribution(random);
                new_values[i] = value_distribution(random);
            }

            TimedResult vlc_read_result{};
            TimedResult vector_read_result{};
            TimedResult block_16_read_result{};
            TimedResult block_32_read_result{};
            TimedResult block_64_read_result{};
            TimedResult block_128_read_result{};
            for (size_t offset = 0; offset < 6; ++offset)
            {
                switch ((trial + offset) % 6)
                {
                case 0:
                    vlc_read_result = measure_vlc_read(read_vlc, indices);
                    break;
                case 1:
                    vector_read_result = measure_vector_read(initial_values, indices);
                    break;
                case 2:
                    block_16_read_result = measure_block_packed_read(read_block_16, indices);
                    break;
                case 3:
                    block_32_read_result = measure_block_packed_read(read_block_32, indices);
                    break;
                case 4:
                    block_64_read_result = measure_block_packed_read(read_block_64, indices);
                    break;
                default:
                    block_128_read_result = measure_block_packed_read(read_block_128, indices);
                    break;
                }
            }
            if (vlc_read_result.checksum != vector_read_result.checksum ||
                block_16_read_result.checksum != vector_read_result.checksum ||
                block_32_read_result.checksum != vector_read_result.checksum ||
                block_64_read_result.checksum != vector_read_result.checksum ||
                block_128_read_result.checksum != vector_read_result.checksum)
            {
                throw std::runtime_error("read checksum mismatch for N=" +
                                         std::to_string(input_size) + ", trial=" +
                                         std::to_string(trial));
            }
            vlc_reads.add(vlc_read_result, read_vlc.size_in_bytes());
            vector_reads.add(vector_read_result, vector_size_in_bytes(initial_values));
            block_16_reads.add(block_16_read_result, read_block_16.size_in_bytes());
            block_32_reads.add(block_32_read_result, read_block_32.size_in_bytes());
            block_64_reads.add(block_64_read_result, read_block_64.size_in_bytes());
            block_128_reads.add(block_128_read_result, read_block_128.size_in_bytes());

            stool::VLCDeque write_vlc = make_vlc_deque(initial_values);
            std::vector<uint64_t> write_vector = initial_values;
            stool::BlockPackedVector<16> write_block_16(initial_values);
            stool::BlockPackedVector<32> write_block_32(initial_values);
            stool::BlockPackedVector<64> write_block_64(initial_values);
            stool::BlockPackedVector<128> write_block_128(initial_values);
            TimedResult vlc_write_result{};
            TimedResult vector_write_result{};
            TimedResult block_16_write_result{};
            TimedResult block_32_write_result{};
            TimedResult block_64_write_result{};
            TimedResult block_128_write_result{};
            for (size_t offset = 0; offset < 6; ++offset)
            {
                switch ((trial + offset) % 6)
                {
                case 0:
                    vlc_write_result = measure_vlc_write(write_vlc, indices, new_values);
                    break;
                case 1:
                    vector_write_result = measure_vector_write(write_vector, indices, new_values);
                    break;
                case 2:
                    block_16_write_result =
                        measure_block_packed_write(write_block_16, indices, new_values);
                    break;
                case 3:
                    block_32_write_result =
                        measure_block_packed_write(write_block_32, indices, new_values);
                    break;
                case 4:
                    block_64_write_result =
                        measure_block_packed_write(write_block_64, indices, new_values);
                    break;
                default:
                    block_128_write_result =
                        measure_block_packed_write(write_block_128, indices, new_values);
                    break;
                }
            }

            verify_equal(write_vlc, write_vector, input_size, trial, "write");
            verify_equal(write_block_16, write_vector, input_size, trial, "write");
            verify_equal(write_block_32, write_vector, input_size, trial, "write");
            verify_equal(write_block_64, write_vector, input_size, trial, "write");
            verify_equal(write_block_128, write_vector, input_size, trial, "write");
            if (vlc_write_result.checksum != vector_write_result.checksum ||
                block_16_write_result.checksum != vector_write_result.checksum ||
                block_32_write_result.checksum != vector_write_result.checksum ||
                block_64_write_result.checksum != vector_write_result.checksum ||
                block_128_write_result.checksum != vector_write_result.checksum)
            {
                throw std::runtime_error("write checksum mismatch for N=" +
                                         std::to_string(input_size) + ", trial=" +
                                         std::to_string(trial));
            }
            vlc_writes.add(vlc_write_result, write_vlc.size_in_bytes());
            vector_writes.add(vector_write_result, vector_size_in_bytes(write_vector));
            block_16_writes.add(block_16_write_result, write_block_16.size_in_bytes());
            block_32_writes.add(block_32_write_result, write_block_32.size_in_bytes());
            block_64_writes.add(block_64_write_result, write_block_64.size_in_bytes());
            block_128_writes.add(block_128_write_result, write_block_128.size_in_bytes());
        }

        print_measurements(input_size, config.value_bits, "read", "VLCDeque", config.operations,
                           vlc_reads);
        print_measurements(input_size, config.value_bits, "read", "std::vector<uint64_t>",
                           config.operations, vector_reads);
        print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<16>",
                           config.operations, block_16_reads);
        print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<32>",
                           config.operations, block_32_reads);
        print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<64>",
                           config.operations, block_64_reads);
        print_measurements(input_size, config.value_bits, "read", "BlockPackedVector<128>",
                           config.operations, block_128_reads);
        print_measurements(input_size, config.value_bits, "write", "VLCDeque", config.operations,
                           vlc_writes);
        print_measurements(input_size, config.value_bits, "write", "std::vector<uint64_t>",
                           config.operations, vector_writes);
        print_measurements(input_size, config.value_bits, "write", "BlockPackedVector<16>",
                           config.operations, block_16_writes);
        print_measurements(input_size, config.value_bits, "write", "BlockPackedVector<32>",
                           config.operations, block_32_writes);
        print_measurements(input_size, config.value_bits, "write", "BlockPackedVector<64>",
                           config.operations, block_64_writes);
        print_measurements(input_size, config.value_bits, "write", "BlockPackedVector<128>",
                           config.operations, block_128_writes);
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
        for (size_t input_size : INPUT_SIZES)
        {
            run_benchmark(input_size, config, random);
        }
        return 0;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        print_usage(std::cerr, argv[0]);
        return 1;
    }
}
