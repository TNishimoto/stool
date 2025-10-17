#include <cassert>
#include <chrono>
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
#include "../../include/light_stool.hpp"

template <std::size_t L, std::size_t R>
struct lower_bound_unrolled_range
{
    static std::size_t eval(const uint64_t *a, uint64_t v)
    {
        if constexpr (L == R)
        {
            return L;
        }
        else if constexpr (L + 1 == R)
        {
            // lower_bound: a[L] >= v ? L : R
            return (a[L] < v) ? R : L;
        }
        else
        {
            constexpr std::size_t M = (L + R) / 2;

            if (a[M] < v)
            {
                return lower_bound_unrolled_range<M + 1, R>::eval(a, v);
            }
            else
            {
                return lower_bound_unrolled_range<L, M>::eval(a, v);
            }
        }
    }
};

template <uint64_t N>
int64_t naive_successor(const std::array<uint64_t, N> &sequence, uint64_t v)
{
    for (int64_t i = 0; i < (int64_t)N; i++)
    {
        if (sequence[i] >= v)
        {
            return i;
        }
    }
    return -1;
}

template <uint64_t N>
int64_t unrolling_successor2(const std::array<uint64_t, N> &sequence, uint64_t v)
{
    std::array<uint8_t, N> bits;
    for (uint64_t i = 0; i < N; i++)
    {
        bits[i] = v >= sequence[i];
    }
    return std::accumulate(bits.begin(), bits.end(), 0);

    /*
    if(sequence.size() == 8){
        bool b1 = v >= sequence[0];
    bool b2 = v >= sequence[1];
    bool b3 = v >= sequence[2];
    bool b4 = v >= sequence[3];
    bool b5 = v >= sequence[4];
    bool b6 = v >= sequence[5];
    bool b7 = v >= sequence[6];
    bool b8 = v >= sequence[7];
    return b1 + b2 + b3 + b4 + b5 + b6 + b7 + b8;
    }else{
        return 0;
    }
    */
    /*
    uint8_t m =
    (v >= sequence[0]) |
    ((v >= sequence[1]) << 1) |
    ((v >= sequence[2]) << 2) |
    ((v >= sequence[3]) << 3) |
    ((v >= sequence[4]) << 4) |
    ((v >= sequence[5]) << 5) |
    ((v >= sequence[6]) << 6) |
    ((v >= sequence[7]) << 7);
    return __builtin_popcount((unsigned)m);
    */
}

template <uint64_t N>
int64_t unrolling_successor(const std::array<uint64_t, N> &sequence, uint64_t v)
{
    if (sequence.size() == 8)
    {
        bool b1 = v >= sequence[0];
        bool b2 = v >= sequence[1];
        bool b3 = v >= sequence[2];
        bool b4 = v >= sequence[3];
        bool b5 = v >= sequence[4];
        bool b6 = v >= sequence[5];
        bool b7 = v >= sequence[6];
        bool b8 = v >= sequence[7];
        return b1 + b2 + b3 + b4 + b5 + b6 + b7 + b8;
    }
    else
    {
        return 0;
    }
    /*
    uint8_t m =
    (v >= sequence[0]) |
    ((v >= sequence[1]) << 1) |
    ((v >= sequence[2]) << 2) |
    ((v >= sequence[3]) << 3) |
    ((v >= sequence[4]) << 4) |
    ((v >= sequence[5]) << 5) |
    ((v >= sequence[6]) << 6) |
    ((v >= sequence[7]) << 7);
    return __builtin_popcount((unsigned)m);
    */
}

std::vector<stool::IntegerSketch8> build_sketches(std::vector<std::vector<uint64_t>> &arrays)
{
    std::vector<stool::IntegerSketch8> sketches;
    sketches.resize(arrays.size());
    for (uint64_t i = 0; i < arrays.size(); i++)
    {
        sketches[i] = stool::IntegerSketch8::build(arrays[i]);
    }
    return sketches;
}

template <uint64_t N>
std::vector<std::array<uint64_t, N>> build_arrays(uint64_t array_count, uint64_t max_value, uint64_t seed)
{
    std::mt19937_64 mt64(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, max_value);

    std::vector<std::array<uint64_t, N>> arrays;

    arrays.resize(array_count);

    for (uint64_t i = 0; i < array_count; i++)
    {
        for (uint64_t j = 0; j < N; j++)
        {
            arrays[i][j] = get_rand_value(mt64);
        }
        std::sort(arrays[i].begin(), arrays[i].end());
    }
    return arrays;
}

template <uint64_t N>
uint64_t access_test(const std::vector<std::array<uint64_t, N>> &arrays, const std::vector<std::pair<uint64_t, uint64_t>> &queries)
{
    uint64_t hash = 0;
    #pragma clang loop vectorize(disable)
    for (uint64_t i = 0; i < queries.size(); i++)
    {
        uint64_t index = queries[i].first;
        hash += arrays[index][0];
    }
    return hash;
}

template <uint64_t N>
uint64_t access_vector_test(const std::vector<std::array<uint64_t, N>> &arrays, const std::vector<std::pair<uint64_t, uint64_t>> &queries)
{
    uint64_t hash = 0;
    uint64_t query_size = queries.size();
    #pragma clang loop vectorize(disable)
    for (uint64_t i = 0; i < query_size; i++)
    {

        /*
        std::array<uint64_t, N> tmp_array;
        for(uint64_t j = 0; j < N; j++){
            tmp_array[j] = arrays[queries[i].first][j];
        }
        hash += std::accumulate(tmp_array.begin(), tmp_array.end(), 0);
        */

        hash += std::accumulate(arrays[queries[i].first].begin(), arrays[queries[i].first].end(), 0);
    }

    return hash;
}

template <uint64_t N>
uint64_t naive_successor_test(const std::vector<std::array<uint64_t, N>> &arrays, const std::vector<std::pair<uint64_t, uint64_t>> &queries)
{
    uint64_t naive_successor_hash = 0;
    uint64_t query_size = queries.size();
    #pragma clang loop vectorize(disable)
    for (uint64_t i = 0; i < query_size; i++)
    {
        uint64_t index = queries[i].first;
        uint64_t v = queries[i].second;

        int64_t naive_successor_result = naive_successor<N>(arrays[index], v);
        naive_successor_hash += naive_successor_result;
    }
    return naive_successor_hash;
}

template <uint64_t N>
uint64_t naive_unrolled_binary_search_test(const std::vector<std::array<uint64_t, N>> &arrays, const std::vector<std::pair<uint64_t, uint64_t>> &queries)
{
    uint64_t naive_successor_hash = 0;
    uint64_t query_size = queries.size();
    #pragma clang loop vectorize(disable)
    for (uint64_t i = 0; i < query_size; i++)
    {
        uint64_t index = queries[i].first;
        uint64_t v = queries[i].second;

        int64_t naive_successor_result = lower_bound_unrolled_range<0, N-1>::eval(arrays[index].data(), v);
        naive_successor_hash += naive_successor_result;
    }
    return naive_successor_hash;
}

template <uint64_t N>
uint64_t unrolling_successor_test(const std::vector<std::array<uint64_t, N>> &arrays, const std::vector<std::pair<uint64_t, uint64_t>> &queries)
{
    uint64_t naive_successor_hash = 0;
    uint64_t query_size = queries.size();

//#pragma clang loop vectorize(enable) vectorize_width(4) interleave_count(2)
#pragma clang loop vectorize(disable)
    for (uint64_t i = 0; i < query_size; i++)
    {
        uint64_t index = queries[i].first;
        uint64_t v = queries[i].second;

        int64_t naive_successor_result = unrolling_successor2<N>(arrays[index], v);
        naive_successor_hash += naive_successor_result;
    }
    return naive_successor_hash;
}

uint64_t sketch_successor_test(const std::vector<std::vector<uint64_t>> &arrays, const std::vector<stool::IntegerSketch8> &sketches, const std::vector<std::pair<uint64_t, uint64_t>> &queries)
{
    uint64_t sketch_successor_hash = 0;
    #pragma clang loop vectorize(disable)
    for (uint64_t i = 0; i < queries.size(); i++)
    {
        uint64_t index = queries[i].first;
        uint64_t v = queries[i].second;

        int64_t sketch_successor_result = sketches[index].successor(v, arrays[index]);
        sketch_successor_hash += sketch_successor_result;
    }
    return sketch_successor_hash;
}

template <uint64_t N>
void successor_test(uint64_t array_count, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
{
    std::mt19937_64 mt64(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, max_value);
    std::uniform_int_distribution<uint64_t> get_rand_index(0, array_count - 1);

    std::vector<std::array<uint64_t, N>> arrays = build_arrays<N>(array_count, max_value, seed);
    // std::vector<stool::IntegerSketch8> sketches = build_sketches(arrays);
    std::vector<std::pair<uint64_t, uint64_t>> queries;

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        uint64_t index = get_rand_index(mt64);
        uint64_t v = get_rand_value(mt64);
        std::pair<uint64_t, uint64_t> query = std::make_pair(index, v);
        queries.push_back(query);
    }
    std::chrono::system_clock::time_point st1, st2;
    std::cout << "Naive successor test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t naive_successor_hash = naive_successor_test<N>(arrays, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t naive_successor_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();

    std::cout << "Access test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t access_hash = access_test<N>(arrays, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t access_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();

    std::cout << "Access Vector test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t access_vector_hash = access_vector_test<N>(arrays, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t access_vector_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();

    std::cout << "Unrolling successor test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t unrolling_successor_hash = unrolling_successor_test<N>(arrays, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t unrolling_successor_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();

    std::cout << "Naive unrolled binary search test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t naive_unrolled_binary_search_hash = naive_unrolled_binary_search_test<N>(arrays, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t naive_unrolled_binary_search_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();

    /*
    std::cout << "Sketch successor test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t sketch_successor_hash = sketch_successor_test(arrays, sketches, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t sketch_successor_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();
    */

    std::cout << "\033[36m";
    std::cout << "=============SETTING===============" << std::endl;
    std::cout << "array_count: " << array_count << std::endl;
    std::cout << "max_value: " << max_value << std::endl;
    std::cout << "number_of_trials: " << number_of_trials << std::endl;
    std::cout << "array_size: " << N << std::endl;
    std::cout << "seed: " << seed << std::endl;
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "naive_successor_hash: " << naive_successor_hash << std::endl;
    // std::cout << "sketch_successor_hash: " << sketch_successor_hash << std::endl;
    std::cout << "access_hash: " << access_hash << std::endl;
    std::cout << "access_vector_hash: " << access_vector_hash << std::endl;
    std::cout << "Unrolling successor hash: " << unrolling_successor_hash << std::endl;
    std::cout << "Naive unrolled binary search hash: " << naive_unrolled_binary_search_hash << std::endl;

    std::cout << "Naive time: " << (naive_successor_time / (1000 * 1000)) << " ms" << std::endl;
    std::cout << "Unrolling successor time: " << (unrolling_successor_time / (1000 * 1000)) << " ms" << std::endl;
    std::cout << "Access time: " << (access_time / (1000 * 1000)) << " ms" << std::endl;
    std::cout << "Access Vector time: " << (access_vector_time / (1000 * 1000)) << " ms" << std::endl;
    std::cout << "Naive unrolled binary search time: " << (naive_unrolled_binary_search_time / (1000 * 1000)) << " ms" << std::endl;
    // std::cout << "Sketch time: " << (sketch_successor_time / (1000 * 1000)) << " ms" << std::endl;

    stool::Memory::print_memory_usage();
    std::cout << "==================================" << std::endl;
    std::cout << "\033[39m" << std::endl;
}

int main(int argc, char *argv[])
{
    cmdline::parser p;

    p.add<uint64_t>("array_count", 'n', "array_count", false, 10000000);
    p.add<uint64_t>("max_value", 'v', "max_value", false, UINT64_MAX);
    p.add<uint64_t>("number_of_trials", 't', "number_of_trials", false, 1000000);
    p.add<uint64_t>("array_size", 'k', "array_size", false, 8);

    p.add<uint64_t>("seed", 's', "seed", false, 0);

    p.parse_check(argc, argv);
    uint64_t array_count = p.get<uint64_t>("array_count");
    uint64_t max_value = p.get<uint64_t>("max_value");
    uint64_t number_of_trials = p.get<uint64_t>("number_of_trials");
    uint64_t array_size = p.get<uint64_t>("array_size");
    uint64_t seed = p.get<uint64_t>("seed");

    if (array_size == 8)
    {
        successor_test<8>(array_count, max_value, number_of_trials, seed);
    }
    else if (array_size == 16)
    {
        successor_test<16>(array_count, max_value, number_of_trials, seed);
    }
    else if (array_size == 32)
    {
        successor_test<32>(array_count, max_value, number_of_trials, seed);
    }
    else if (array_size == 64)
    {
        successor_test<64>(array_count, max_value, number_of_trials, seed);
    }

    return 0;
}