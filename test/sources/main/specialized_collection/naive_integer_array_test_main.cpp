#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <string>
#include <chrono>
#include "../../../../include/specialized_collection/push_pop_arrays/naive_integer_array.hpp"

// Alias for easier testing
using stool::NaiveIntegerArray;

// Test that performs a sequence of random operations on NaiveIntegerArray and verifies that its contents always match std::vector.
// - N: initial number of elements
// - max_val: maximum value per element
// - num_op: number of operations to perform
// - seed: random seed
// - verbose: verbose output flag
void test_randomized_ops(uint64_t N, uint64_t max_val, uint64_t num_op, uint64_t seed, bool verbose = false)
{
    std::mt19937_64 mt(seed);
    // Setup underlying vector and NaiveIntegerArray
    std::vector<uint64_t> stdvec;
    for (uint64_t i = 0; i < N; ++i)
        stdvec.push_back(mt() % (max_val + 1));
    constexpr uint64_t SIZE = 2048;
    NaiveIntegerArray<SIZE> arr(stdvec);

    // Check if NaiveIntegerArray and std::vector always have equivalent contents
    auto check_integrity = [&]() {
        assert(arr.size() == stdvec.size());
        for (uint64_t i = 0; i < stdvec.size(); ++i)
            assert(arr.at(i) == stdvec[i]);
        assert(arr.to_vector() == stdvec);
        assert(arr.verify());
    };

    for (uint64_t op = 0; op < num_op; ++op)
    {
        if(op % 100 == 0)
        {
            std::cout << "+" << std::flush;
        }
        int r = mt() % 12;
        if (verbose) std::cout << "[" << op << "] ";
        if (r == 0 && arr.size() > 0)
        {
            // Check correctness of psum
            uint64_t idx = mt() % arr.size();
            uint64_t arr_sum = arr.psum(idx);
            uint64_t std_sum = 0;
            for (uint64_t j = 0; j <= idx; ++j) std_sum += stdvec[j];
            if (verbose) std::cout << "psum(" << idx << ") = " << arr_sum << "/" << std_sum << std::endl;
            assert(arr_sum == std_sum);
        }
        else if (r == 1 && arr.size() > 0)
        {
            // Check correctness of reverse_psum
            uint64_t idx = mt() % arr.size();
            uint64_t arr_sum = arr.reverse_psum(idx);
            uint64_t std_sum = 0;
            // reverse_psum(i) = sum of S[(n-1)-i..n-1]
            uint64_t begin = arr.size() - idx - 1;
            for (uint64_t j = begin; j < arr.size(); ++j) std_sum += stdvec[j];
            if (verbose) std::cout << "reverse_psum(" << idx << ") = " << arr_sum << "/" << std_sum << std::endl;
            assert(arr_sum == std_sum);
        }
        else if (r == 2)
        {
            // Check correctness of search
            uint64_t target = 0;
            for (auto val : stdvec) target += val;
            if (target > 0)
            {
                uint64_t x = mt() % (target + 1);
                // First i where psum(i) >= x (returns -1 if not found)
                uint64_t acc = 0;
                int64_t found = -1;
                for (uint64_t i = 0; i < stdvec.size(); ++i)
                {
                    acc += stdvec[i];
                    if (acc >= x)
                    {
                        found = static_cast<int64_t>(i);
                        break;
                    }
                }
                int64_t arr_found = arr.search(x);
                if (verbose) std::cout << "search(" << x << ") = " << arr_found << "/" << found << std::endl;
                assert(arr_found == found);
            }
        }
        else if (r == 3 && arr.size() > 0)
        {
            // Verify set_value is correctly reflected
            uint64_t idx = mt() % arr.size();
            uint64_t val = mt() % (max_val + 1);
            if (verbose) std::cout << "set_value(" << idx << "," << val << ")\n";
            arr.set_value(idx, val);
            stdvec[idx] = val;
        }
        else if (r == 4 && arr.size() > 0)
        {
            // Verify increment effect
            uint64_t idx = mt() % arr.size();
            int64_t delta = (mt() % 21) - 10;
            if (verbose) std::cout << "increment(" << idx << "," << delta << ")\n";
            arr.increment(idx, delta);
            stdvec[idx] += delta;
        }
        else if (r == 5 && arr.size() > 0)
        {
            // Verify decrement effect
            uint64_t idx = mt() % arr.size();
            int64_t delta = (mt() % 21) - 10;
            if (verbose) std::cout << "decrement(" << idx << "," << delta << ")\n";
            arr.decrement(idx, delta);
            stdvec[idx] -= delta;
        }
        else if (r == 6)
        {
            // Verify push_back functionality
            if (arr.size() >= SIZE)
            {
                continue;
            }
            uint64_t val = mt() % (max_val + 1);
            if (verbose) std::cout << "push_back(" << val << ")\n";
            arr.push_back(val);
            stdvec.push_back(val);
        }
        else if (r == 7)
        {
            // Verify push_front functionality
            if (arr.size() >= SIZE)
            {
                continue;
            }
            uint64_t val = mt() % (max_val + 1);
            if (verbose) std::cout << "push_front(" << val << ")\n";
            arr.push_front(val);
            stdvec.insert(stdvec.begin(), val);
        }
        else if (r == 8 && arr.size() > 0)
        {
            // Verify pop_front functionality
            if (verbose) std::cout << "pop_front()\n";
            arr.pop_front();
            stdvec.erase(stdvec.begin());
        }
        else if (r == 9 && arr.size() > 0)
        {
            // Verify remove (erase at arbitrary position)
            uint64_t idx = mt() % arr.size();
            if (verbose) std::cout << "remove(" << idx << ")\n";
            arr.remove(idx);
            stdvec.erase(stdvec.begin() + idx);
        }
        else if (r == 10)
        {
            // Verify insert at arbitrary position
            if (arr.size() >= SIZE)
            {
                continue;
            }
            uint64_t pos = 0;
            if (arr.size() == 0)
                pos = 0;
            else
                pos = mt() % (arr.size() + 1);
            uint64_t val = mt() % (max_val + 1);
            if (verbose) std::cout << "insert(" << pos << "," << val << ")\n";
            arr.insert(pos, val);
            stdvec.insert(stdvec.begin() + pos, val);
        }
        else if (r == 11 && arr.size() > 0)
        {
            // Verify pop_back functionality
            if (verbose) std::cout << "pop_back()\n";
            arr.pop_back();
            stdvec.pop_back();
        }
        check_integrity();
    }
    std::cout << std::endl;
    if (verbose) std::cout << "Randomized ops test finished." << std::endl;
}

// Test that checks basic functionality (access, value change, increment/decrement, vector conversion, verify)
void test_basic()
{
    std::cout << "Basic functionality test (small array)..." << std::endl;
    NaiveIntegerArray<16> arr({1, 2, 3, 4, 5});
    assert(arr.size() == 5);
    assert(arr.at(0) == 1);
    assert(arr.at(4) == 5);
    arr.set_value(3, 10);
    assert(arr.at(3) == 10);
    arr.increment(2, 7);
    assert(arr.at(2) == 10);
    arr.decrement(2, 5);
    assert(arr.at(2) == 5);

    std::vector<uint64_t> v = arr.to_vector();
    assert(v.size() == arr.size());
    assert(v[0] == 1);
    assert(v[1] == 2);
    assert(v[2] == 5);
    assert(v[3] == 10);
    assert(v[4] == 5);

    // Verify after increment/decrement
    assert(arr.verify());
    std::cout << "Basic functionality test passed." << std::endl;
}

// Test that generates a random array and checks behavior of increment/decrement, to_vector and verify.
// - N: number of elements
// - max_val: maximum value per element
// - seed: random seed
void test_random(uint64_t N, uint64_t max_val, uint64_t seed)
{
    std::cout << "Random test (N=" << N << ", max_val=" << max_val << ", seed=" << seed << ")..." << std::endl;
    std::mt19937_64 mt(seed);
    std::vector<uint64_t> vals(N);
    for (uint64_t i = 0; i < N; ++i)
    {
        vals[i] = mt() % (max_val + 1);
    }

    NaiveIntegerArray<1024> arr(vals);
    assert(arr.size() == N);

    // Random increments and decrements
    for (int iter = 0; iter < 100; ++iter)
    {
        uint64_t idx = mt() % N;
        int op = mt() % 2;
        int delta = (mt() % 11) - 5; // -5 to +5
        if (op == 0)
        {
            arr.increment(idx, delta);
            vals[idx] += delta;
        }
        else
        {
            arr.decrement(idx, delta);
            vals[idx] -= delta;
        }
        assert(arr.at(idx) == vals[idx]);
        assert(arr.verify());
    }

    // Verify all elements are equal
    for (uint64_t i = 0; i < N; ++i)
    {
        assert(arr.at(i) == vals[i]);
    }

    // to_vector must match
    assert(arr.to_vector() == vals);

    std::cout << "Random test passed." << std::endl;
}

// Test that size_in_bytes() equals actual struct size (to confirm wrapper implementation correctness)
void test_size_in_bytes()
{
    std::cout << "Testing size_in_bytes()..." << std::endl;
    NaiveIntegerArray<8> arr({0,1,2,3,4});
    uint64_t byte_size = arr.size_in_bytes();
    assert(byte_size == sizeof(NaiveIntegerArray<8>));
    std::cout << "Reported: " << byte_size << " bytes, Actual: " << sizeof(NaiveIntegerArray<8>) << " bytes." << std::endl;
    std::cout << "size_in_bytes() test passed." << std::endl;
}

// Simple test that reverse_psum() works as defined
void test_reverse_psum()
{
    std::cout << "Testing reverse_psum()..." << std::endl;
    NaiveIntegerArray<8> arr({1, 2, 3, 4});
    // reverse_psum(i) = sum of S[(n-1)-i .. n-1]
    assert(arr.reverse_psum(0) == 4);      // [3..3]
    assert(arr.reverse_psum(1) == 3 + 4);  // [2..3]
    assert(arr.reverse_psum(2) == 2 + 3 + 4); // [1..3]
    assert(arr.reverse_psum(3) == 1 + 2 + 3 + 4); // [0..3]
    std::cout << "reverse_psum() test passed." << std::endl;
}

// Main function that calls each test function and verifies overall NaiveIntegerArray behavior
int main()
{
    std::cout << "Running test_basic(): Basic functionality test" << std::endl;
    test_basic();

    std::cout << "Running test_size_in_bytes(): Memory usage test" << std::endl;
    test_size_in_bytes();

    std::cout << "Running test_random(32, 1000, 42): Random array (32 elements, max 1000) test" << std::endl;
    test_random(32, 1000, 42);

    std::cout << "Running test_random(900, 1 << 20, 123): Random array (900 elements, max 1048576) test" << std::endl;
    test_random(900, 1 << 20, 123);

    std::cout << "Running test_reverse_psum(): reverse_psum test" << std::endl;
    test_reverse_psum();

    std::cout << "Running test_randomized_ops(10, 10000, 10000, 421): Small-scale randomized ops test (10 elements, 10000 operations)" << std::endl;
    test_randomized_ops(10, 10000, 10000, 421);

    std::cout << "Running test_randomized_ops(100, 10000, 10000, 4211): Medium-scale randomized ops test (100 elements, 10000 operations)" << std::endl;
    test_randomized_ops(100, 10000, 10000, 4211);

    std::cout << "Running test_randomized_ops(1000, 10000, 10000, 42111): Large-scale randomized ops test (1000 elements, 10000 operations)" << std::endl;
    test_randomized_ops(1000, 10000, 10000, 42111);

    std::cout << "All NaiveIntegerArray tests passed!" << std::endl;
    return 0;
}