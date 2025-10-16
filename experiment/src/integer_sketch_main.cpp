#include <cassert>
#include <chrono>
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
#include "../../include/light_stool.hpp"

int64_t naive_successor(const std::vector<uint64_t> &sequence, uint64_t v)
{
    for (int64_t i = 0; i < (int64_t)sequence.size(); i++)
    {
        if (sequence[i] >= v)
        {
            return i;
        }
    }
    return -1;
}

std::vector<stool::IntegerSketch8> build_sketches(std::vector<std::vector<uint64_t>> &arrays){
    std::vector<stool::IntegerSketch8> sketches;
    sketches.resize(arrays.size());
    for(uint64_t i = 0; i < arrays.size(); i++)
    {
        sketches[i] = stool::IntegerSketch8::build(arrays[i]);
    }
    return sketches;
}


std::vector<std::vector<uint64_t>> build_arrays(uint64_t array_count, uint64_t max_value, uint64_t seed)
{
    std::mt19937_64 mt64(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, max_value);

    std::vector<std::vector<uint64_t>> arrays;

    arrays.resize(array_count);

    for(uint64_t i = 0; i < array_count; i++)
    {
        for(uint64_t j = 0; j < 8; j++)
        {
            arrays[i].push_back(get_rand_value(mt64));
        }
        std::sort(arrays[i].begin(), arrays[i].end());
    }
    return arrays;
}

uint64_t access_test(const std::vector<std::vector<uint64_t>> &arrays, const std::vector<std::pair<uint64_t, uint64_t>> &queries){
    uint64_t hash = 0;
    for(uint64_t i = 0; i < queries.size(); i++)
    {
        uint64_t index = queries[i].first;
        hash += arrays[index][0];
    }
    return hash;
}

uint64_t access_vector_test(const std::vector<std::vector<uint64_t>> &arrays, const std::vector<std::pair<uint64_t, uint64_t>> &queries){
    uint64_t hash = 0;
    uint64_t query_size = queries.size();
    for(uint64_t i = 0; i < query_size; i++)
    {
        uint64_t index = queries[i].first;
        hash += arrays[index][0] + arrays[index][1] + arrays[index][2] + arrays[index][3];
        /*
        for(uint64_t x: arrays[index]){
            hash += x;
        }
        */
    }
    return hash;
}


uint64_t naive_successor_test(const std::vector<std::vector<uint64_t>> &arrays, const std::vector<std::pair<uint64_t, uint64_t>> &queries){
    uint64_t naive_successor_hash = 0;
    for(uint64_t i = 0; i < queries.size(); i++)
    {
        uint64_t index = queries[i].first;
        uint64_t v = queries[i].second;

        int64_t naive_successor_result = naive_successor(arrays[index], v);
        naive_successor_hash += naive_successor_result;
    }
    return naive_successor_hash;
}

uint64_t sketch_successor_test(const std::vector<std::vector<uint64_t>> &arrays, const std::vector<stool::IntegerSketch8> &sketches, const std::vector<std::pair<uint64_t, uint64_t>> &queries){
    uint64_t sketch_successor_hash = 0;
    for(uint64_t i = 0; i < queries.size(); i++)
    {
        uint64_t index = queries[i].first;
        uint64_t v = queries[i].second;

        int64_t sketch_successor_result = sketches[index].successor(v, arrays[index]);
        sketch_successor_hash += sketch_successor_result;
    }
    return sketch_successor_hash;
}




void successor_test(uint64_t array_count, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
{
    std::mt19937_64 mt64(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, max_value);
    std::uniform_int_distribution<uint64_t> get_rand_index(0, array_count-1);

    std::vector<std::vector<uint64_t>> arrays = build_arrays(array_count, max_value, seed);
    std::vector<stool::IntegerSketch8> sketches = build_sketches(arrays);
    std::vector<std::pair<uint64_t, uint64_t>> queries;

    for(uint64_t i = 0; i < number_of_trials; i++)
    {
        uint64_t index = get_rand_index(mt64);
        uint64_t v = get_rand_value(mt64);
        std::pair<uint64_t, uint64_t> query = std::make_pair(index, v);
        queries.push_back(query);
    }
    std::chrono::system_clock::time_point st1, st2;
    std::cout << "Naive successor test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t naive_successor_hash = naive_successor_test(arrays, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t naive_successor_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();

    std::cout << "Access test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t access_hash = access_test(arrays, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t access_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();

    std::cout << "Access Vector test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t access_vector_hash = access_vector_test(arrays, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t access_vector_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();


    std::cout << "Sketch successor test" << std::endl;
    st1 = std::chrono::system_clock::now();
    uint64_t sketch_successor_hash = sketch_successor_test(arrays, sketches, queries);
    st2 = std::chrono::system_clock::now();
    uint64_t sketch_successor_time = std::chrono::duration_cast<std::chrono::nanoseconds>(st2 - st1).count();

    std::cout << "\033[36m";
    std::cout << "=============SETTING===============" << std::endl;
    std::cout << "array_count: " << array_count << std::endl;
    std::cout << "max_value: " << max_value << std::endl;
    std::cout << "number_of_trials: " << number_of_trials << std::endl;
    std::cout << "seed: " << seed << std::endl;
    std::cout << "=============RESULT===============" << std::endl;
    std::cout << "naive_successor_hash: " << naive_successor_hash << std::endl;
    std::cout << "sketch_successor_hash: " << sketch_successor_hash << std::endl;
    std::cout << "access_hash: " << access_hash << std::endl;
    std::cout << "access_vector_hash: " << access_vector_hash << std::endl;
    std::cout << "Naive time: " << (naive_successor_time / (1000 * 1000)) << " ms" << std::endl;
    std::cout << "Access time: " << (access_time / (1000 * 1000)) << " ms" << std::endl;
    std::cout << "Access Vector time: " << (access_vector_time / (1000 * 1000)) << " ms" << std::endl;
    std::cout << "Sketch time: " << (sketch_successor_time / (1000 * 1000)) << " ms" << std::endl;

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
    p.add<uint64_t>("seed", 's', "seed", false, 0);

    p.parse_check(argc, argv);
    uint64_t array_count = p.get<uint64_t>("array_count");
    uint64_t max_value = p.get<uint64_t>("max_value");
    uint64_t number_of_trials = p.get<uint64_t>("number_of_trials");
    uint64_t seed = p.get<uint64_t>("seed");

    successor_test(array_count, max_value, number_of_trials, seed);





    return 0;
}