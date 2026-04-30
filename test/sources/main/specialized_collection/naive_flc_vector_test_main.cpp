#include <cassert>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>

#include "../../../../include/specialized_collection/push_pop_arrays/naive_flc_vector.hpp"

using stool::NaiveFLCVector;

/// @brief Test the basic operations of NaiveFLCVector: push_back, at, operator[]
void test_naive_flc_vector_basic()
{
    std::cout << "[Test] NaiveFLCVector basic push_back/at/[] ..." << std::endl;
    NaiveFLCVector<> vec;
    std::vector<uint64_t> ref;

    const size_t N = 1024;
    for (size_t i = 0; i < N; ++i)
    {
        uint64_t v = i * 7 % 10000;
        vec.push_back(v);
        ref.push_back(v);
        assert(vec.at(i) == ref[i]);
        assert(vec[i] == ref[i]);
    }

    assert(vec.size() == ref.size());
    std::mt19937_64 mt(42);
    for (size_t t = 0; t < 1000; ++t)
    {
        size_t idx = mt() % N;
        assert(vec.at(idx) == ref[idx]);
        assert(vec[idx] == ref[idx]);
    }
    std::cout << "[OK] Basic test passed" << std::endl;
}

/// @brief Test the push_front, push_back, pop_front, pop_back operations
void test_naive_flc_vector_push_pop()
{
    std::cout << "[Test] NaiveFLCVector push_front/push_back/pop_front/pop_back ..." << std::endl;
    NaiveFLCVector<> vec;
    std::vector<uint64_t> ref;

    for (uint64_t i = 0; i < 100; ++i)
    {
        vec.push_back(i);
        ref.push_back(i);
    }
    for (int i = 0; i < 50; ++i)
    {
        vec.pop_back();
        ref.pop_back();
    }
    for (uint64_t i = 100; i < 110; ++i)
    {
        vec.push_front(i);
        ref.insert(ref.begin(), i);
    }
    for (int i = 0; i < 5; ++i)
    {
        vec.pop_front();
        ref.erase(ref.begin());
    }

    assert(vec.size() == ref.size());
    for (size_t i = 0; i < ref.size(); ++i)
    {
        assert(vec[i] == ref[i]);
    }
    std::cout << "[OK] Push/pop test passed" << std::endl;
}

/// @brief Test with a sequence of random operations (push/pop/set_value/increment/insert/remove/shift/psum search, etc.)
/// @param n_init Number of initial values
/// @param op_num Number of operations to execute
/// @param max_value Maximum value for vector elements
/// @param seed Random seed
void test_naive_flc_vector_random_ops(size_t n_init = 1000, size_t op_num = 10000, uint64_t max_value = (1ULL << 24), uint64_t seed = 2024)
{
    std::cout << "[Test] NaiveFLCVector random operations (with psum/reverse_psum/search/shift etc) ..." << std::endl;
    NaiveFLCVector<> vec;
    std::vector<uint64_t> ref;
    std::mt19937_64 mt(seed);

    // Initialize first
    for (size_t i = 0; i < n_init; ++i)
    {
        uint64_t v = mt() % max_value;
        vec.push_back(v);
        ref.push_back(v);
    }

    // Perform various random operations
    for (size_t op = 0; op < op_num; ++op)
    {
        if (op % 1000 == 0) {
            std::cout << "+" << std::flush;
        }
        // Select operation type (13 types)
        int op_type = mt() % 13;

        switch(op_type) {
        case 0: // push_back
        {
            if(vec.size() >= vec.max_size()) {
                continue;
            }
            uint64_t v = mt() % max_value;
            vec.push_back(v);
            ref.push_back(v);
            break;
        }
        case 1: // push_front
        {
            uint64_t v = mt() % max_value;
            vec.push_front(v);
            ref.insert(ref.begin(), v);
            break;
        }
        case 2: // pop_back
            if (!ref.empty()) {
                vec.pop_back();
                ref.pop_back();
            }
            break;
        case 3: // pop_front
            if (!ref.empty()) {
                vec.pop_front();
                ref.erase(ref.begin());
            }
            break;
        case 4: // set_value
            if (!ref.empty()) {
                size_t idx = mt() % ref.size();
                uint64_t v = mt() % max_value;
                vec.set_value(idx, v);
                ref[idx] = v;
            }
            break;
        case 5: // increment
            if (!ref.empty()) {
                size_t idx = mt() % ref.size();
                int64_t delta = int64_t(mt() % (max_value / 32 + 1));
                vec.increment(idx, delta);
                ref[idx] += delta;
            }
            break;
        case 6: // insert
        {
            size_t pos = ref.empty() ? 0 : (mt() % (ref.size() + 1));
            uint64_t v = mt() % max_value;
            vec.insert(pos, v);
            ref.insert(ref.begin() + pos, v);
            break;
        }
        case 7: // remove
            if (!ref.empty()) {
                size_t pos = mt() % ref.size();
                uint64_t removed = vec.remove(pos);
                assert(removed == ref[pos]);
                ref.erase(ref.begin() + pos);
            }
            break;
        case 8: // shift_left(position_p, len)
            if (!ref.empty()) {
                size_t len = 1 + (mt() % std::min<size_t>(8, ref.size()));
                size_t position_p = len + (mt() % (ref.size() - len + 1)); // len <= p <= size
                vec.shift_left(position_p, len);
                ref.erase(ref.begin() + (position_p - len), ref.begin() + position_p);
            }
            break;
        case 9: // shift_right(position_p, len, new_code_type)
        {
            size_t position_p = ref.empty() ? 0 : (mt() % (ref.size() + 1)); // 0 <= p <= size
            size_t len = 1 + (mt() % 8);
            uint64_t max_ref = 0;
            if (!ref.empty()) {
                max_ref = *std::max_element(ref.begin(), ref.end());
            }
            uint8_t new_code_type = static_cast<uint8_t>(stool::PackedPSum::get_code_type(max_ref));
            vec.shift_right(position_p, len, new_code_type);
            ref.insert(ref.begin() + position_p, len, 0ULL);
            break;
        }
        case 10: // psum
            if (!ref.empty()) {
                size_t idx = mt() % ref.size();
                uint64_t val1 = vec.psum(idx);
                uint64_t val2 = 0;
                for (size_t i = 0; i <= idx; ++i) val2 += ref[i];
                assert(val1 == val2);
            }
            break;
        case 11: // reverse_psum
            if (!ref.empty()) {
                size_t idx = mt() % ref.size();
                uint64_t val1 = vec.reverse_psum(idx);
                uint64_t val2 = 0;
                size_t begin = ref.size() - idx - 1;
                for (size_t i = begin; i < ref.size(); ++i) val2 += ref[i];
                assert(val1 == val2);
            }
            break;
        case 12: // search
            if (!ref.empty()) {
                uint64_t total = 0;
                for (auto v : ref) total += v;
                uint64_t tgt = mt() % (total + 1);
                // ref: smallest i s.t. psum(i) >= tgt
                uint64_t acc = 0;
                int64_t expected = -1;
                for (size_t i = 0; i < ref.size(); ++i) {
                    acc += ref[i];
                    if (acc >= tgt) {
                        expected = static_cast<int64_t>(i);
                        break;
                    }
                }
                int64_t found = vec.search(tgt);
                assert(found == expected);
            }
            break;
        }

        // Synchronization check at regular intervals
        if ((op + 1) % 250 == 0)
        {
            assert(vec.size() == ref.size());
            for (size_t i = 0; i < ref.size(); ++i)
            {
                assert(vec[i] == ref[i]);
            }
        }
    }
    std::cout << std::endl;
    assert(vec.size() == ref.size());
    for (size_t i = 0; i < ref.size(); ++i)
    {
        assert(vec[i] == ref[i]);
    }
    std::cout << "[OK] Random operation test (with psum/search/shift etc) passed" << std::endl;
}

/// @brief Test the iterator functionality. Traverse all elements using begin() and end()
void test_naive_flc_vector_iterator()
{
    std::cout << "[Test] NaiveFLCVector iterator ..." << std::endl;
    NaiveFLCVector<> vec;
    std::vector<uint64_t> ref;
    for (uint64_t i = 0; i < 128; ++i)
    {
        vec.push_back(i * 3);
        ref.push_back(i * 3);
    }
    size_t idx = 0;
    for (auto it = vec.begin(); !it.is_end(); ++it, ++idx)
    {
        assert(*it == ref[idx]);
    }
    assert(idx == ref.size());
    std::cout << "[OK] Iterator test passed" << std::endl;
}

/// @brief Test file I/O (save/load)
void test_naive_flc_vector_save_load()
{
    std::cout << "[Test] NaiveFLCVector file I/O ..." << std::endl;
    NaiveFLCVector<> vec;
    std::vector<uint64_t> ref;
    for (uint64_t i = 0; i < 100; ++i)
    {
        uint64_t v = i * 7 + 123;
        vec.push_back(v);
        ref.push_back(v);
    }

    {
        std::ofstream ofs("naiveflc_test.bin", std::ios::binary);
        NaiveFLCVector<>::store_to_file(vec, ofs);
    }

    NaiveFLCVector<> vec2;
    {
        std::ifstream ifs("naiveflc_test.bin", std::ios::binary);
        vec2 = NaiveFLCVector<>::load_from_file(ifs);
    }

    assert(vec2.size() == ref.size());
    for (size_t i = 0; i < ref.size(); ++i)
    {
        assert(vec2[i] == ref[i]);
    }
    std::remove("naiveflc_test.bin");
    std::cout << "[OK] File save/load test passed" << std::endl;
}

/// @brief Run all individual test functions to perform comprehensive testing
int main()
{
    std::cout << "\033[34mTest: NaiveFLCVector\033[0m" << std::endl;
    std::cout << "Running test_naive_flc_vector_basic() ..." << std::endl;
    test_naive_flc_vector_basic();

    std::cout << "Running test_naive_flc_vector_push_pop() ..." << std::endl;
    test_naive_flc_vector_push_pop();

    std::cout << "Running test_naive_flc_vector_iterator() ..." << std::endl;
    test_naive_flc_vector_iterator();

    std::cout << "Running test_naive_flc_vector_save_load() ..." << std::endl;
    test_naive_flc_vector_save_load();

    std::cout << "Running test_naive_flc_vector_random_ops(0, 50000, 1ULL << 24, 2) ..." << std::endl;
    test_naive_flc_vector_random_ops(0, 50000, 1ULL << 24, 2);

    std::cout << "Running test_naive_flc_vector_random_ops(512, 50000, 1ULL << 24, 2) ..." << std::endl;
    test_naive_flc_vector_random_ops(512, 50000, 1ULL << 24, 2);

    std::cout << "Running test_naive_flc_vector_random_ops(1024, 50000, 1ULL << 24, 3) ..." << std::endl;
    test_naive_flc_vector_random_ops(1024, 50000, 1ULL << 24, 3);

    std::cout << "Running test_naive_flc_vector_random_ops(2048, 50000, 1ULL << 24, 4) ..." << std::endl;
    test_naive_flc_vector_random_ops(2048, 50000, 1ULL << 24, 4);

    std::cout << "All NaiveFLCVector tests passed!" << std::endl;
    return 0;
}