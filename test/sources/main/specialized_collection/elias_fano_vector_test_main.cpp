#include <iostream>
#include <vector>
#include <cassert>
#include "../../../include/specialized_collection/elias_fano_vector.hpp"
#include "../../../include/debug/debug_printer.hpp"

using stool::EliasFanoVector;

// Helper function: Generate a monotonically increasing sequence
std::vector<uint64_t> make_increasing_sequence(size_t n, uint64_t base = 0, uint64_t step = 3, int seed = 0)
{
    // Generate a monotonically increasing sequence using the seed
    std::mt19937_64 eng(seed);
    std::uniform_int_distribution<uint64_t> dist(0, step);
    std::vector<uint64_t> v(n);
    uint64_t current = base;
    for (size_t i = 0; i < n; ++i)
    {
        uint64_t random_step = step + dist(eng); // Ensure step or higher
        v[i] = current;
        current += random_step;
    }
    return v;
}

#include <random>

void test_elias_fano_rank_random(uint64_t test_num, uint64_t n, uint64_t base, uint64_t step, int rank_num, int seed)
{
    std::cout << "[Test] EliasFanoVector rank (random)..." << std::endl;

    for (uint64_t i = 0; i < test_num; ++i)
    {
        // Test EliasFanoVector's rank query with random data
        std::vector<uint64_t> data = make_increasing_sequence(n, base, step, i);

        EliasFanoVector efv;
        efv.construct(&data);

        uint64_t max_value = data.empty() ? 0 : data.back();
    
        // Set up random number generator (using seed)
        std::mt19937_64 eng(seed);
        std::uniform_int_distribution<uint64_t> dist(
            data.empty() ? 0 : (data.front() > 0 ? data.front() - 100 : 0),
            data.empty() ? max_value : data.back() + 100);
    
        for (int i = 0; i < rank_num; ++i)
        {
            uint64_t value = dist(eng);
            size_t expect = 0;
            for(size_t j=0;j<data.size();++j){
                if(data[j] < value){
                    expect++;
                }
            }
    
            size_t got = efv.rank(value);
            assert(expect == got);
        }

        std::cout << "+" << std::flush;
    }

    // Random tests are performed using only the provided EliasFanoVector, rank_num, and seed.
    // The data sequence is restored from the EliasFanoVector.
    
    std::cout << "[OK] rank random test passed" << std::endl;
}

void test_elias_fano_random_access(uint64_t seq_size, uint64_t max_value, uint64_t step, int seed)
{
    std::cout << "[Test] EliasFanoVector random access..." << std::endl;
    auto data = make_increasing_sequence(seq_size, max_value, step, seed);

    EliasFanoVector efv;
    efv.construct(&data);
    auto data2 = efv.to_vector();

    assert(data == data2);

    std::cout << "[OK] random access test passed" << std::endl;
}

void test_elias_fano_basic()
{
    std::cout << "[Test] EliasFanoVector basic functionality..." << std::endl;
    std::vector<uint64_t> data = {1, 3, 7, 15, 31, 63, 127};
    EliasFanoVector efv;
    efv.construct(&data);

    // Size check
    assert(efv.size() == data.size());

    // Value access check
    for (size_t i = 0; i < data.size(); ++i)
    {
        assert(efv[i] == data[i]);
    }

    // Iterator begin/end test
    size_t idx = 0;
    for (auto it = efv.begin(); it != efv.end(); ++it, ++idx)
    {
        assert(*it == data[idx]);
    }

    std::cout << "[OK] basic functionality test passed" << std::endl;
}

void test_elias_fano_empty()
{
    std::cout << "[Test] EliasFanoVector empty sequence..." << std::endl;
    std::vector<uint64_t> empty;
    EliasFanoVector efv;
    efv.construct(&empty);

    assert(efv.size() == 0);
    assert(efv.begin() == efv.end());
    std::cout << "[OK] empty sequence test passed" << std::endl;
}

void test_elias_fano_large_values()
{
    std::cout << "[Test] EliasFanoVector large values..." << std::endl;
    std::vector<uint64_t> data;
    uint64_t val = 1ULL << 48;
    for (int i = 0; i < 100; ++i)
    {
        data.push_back(val + i * 123456);
    }
    EliasFanoVector efv;
    efv.construct(&data);
    for (size_t i = 0; i < data.size(); ++i)
    {
        assert(efv[i] == data[i]);
    }
    std::cout << "[OK] large values test passed" << std::endl;
}

int main()
{
    std::cout << "\033[34mTest: EliasFanoVector\033[0m" << std::endl;
    test_elias_fano_basic();
    test_elias_fano_random_access(1000, 100, 10, 0);
    test_elias_fano_empty();
    test_elias_fano_large_values();

    test_elias_fano_rank_random(100, 10, 0, 10, 1000, 0);
    test_elias_fano_rank_random(100, 1000, 5, 10, 1000, 0);
    test_elias_fano_rank_random(100, 10000, 500, 10, 1000, 0);

    std::cout << "[DONE]" << std::endl;

    std::cout << "All EliasFanoVector tests passed!" << std::endl;
    return 0;
}