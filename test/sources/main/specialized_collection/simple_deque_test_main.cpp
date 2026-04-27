#include <iostream>
#include <cassert>
#include <deque>
#include "../../../include/specialized_collection/simple_deque.hpp"
// Test for SimpleDeque using uint16_t and uint32_t as template arguments
#include <random>

// Randomly executes push_back, push_front, pop_back, pop_front, insert, erase operations x times for testing
void test_simple_deque_16(uint64_t initial_size, uint64_t operation_num) {
    using Deque16 = stool::SimpleDeque<uint16_t, uint16_t>;
    Deque16 d;
    std::deque<uint16_t> ref;

    std::mt19937 rng(202406); // Fixed seed for reproducibility
    std::uniform_int_distribution<uint16_t> val_dist(1, 10000);

    // Add elements for initial size
    for(uint64_t i = 0; i < initial_size; ++i) {
        uint16_t value = val_dist(rng);
        d.push_back(value);
        ref.push_back(value);
    }

    std::uniform_int_distribution<int> op_dist(0, 5);
 

    for (int i = 0; i < (int)operation_num; ++i) {
        if(i %1000 == 0){
            std::cout << "+" << std::flush;
        }
        int op = op_dist(rng);
        uint16_t value = val_dist(rng);

        if (op == 0) { // push_back
            d.push_back(value);
            ref.push_back(value);
        } else if (op == 1) { // push_front
            d.push_front(value);
            ref.push_front(value);
        } else if (op == 2) { // pop_back
            if (!ref.empty()) {
                d.pop_back();
                ref.pop_back();
            }
        } else if (op == 3) { // pop_front
            if (!ref.empty()) {
                d.pop_front();
                ref.pop_front();
            }
        } else if (op == 4) { // insert
            if (!ref.empty()) {
                std::uniform_int_distribution<size_t> idx_dist(0, ref.size());
                size_t pos = idx_dist(rng);
                d.insert(pos, value);
                ref.insert(ref.begin() + pos, value);
            } else {
                d.push_back(value);
                ref.push_back(value);
            }
        } else if (op == 5) { // erase
            if (!ref.empty()) {
                std::uniform_int_distribution<size_t> idx_dist(0, ref.size() - 1);
                size_t pos = idx_dist(rng);
                d.erase(pos);
                ref.erase(ref.begin() + pos);
            }
        }
        // Check
        assert(d.size() == ref.size());
        for (size_t j = 0; j < d.size(); ++j) {
            assert(d.at(j) == ref[j]);
        }
        // Check empty
        assert(d.empty() == ref.empty());
    }
    // shrink_to_fit and to_deque checks
    d.shrink_to_fit();
    std::deque<uint16_t> ref2 = d.to_deque();
    assert(ref2 == ref);
    std::cout << "[OK]" << std::endl;
}

// Version of test_simple_deque_16 for 32bit: full test including random operations
void test_simple_deque_32(uint64_t initial_size, uint64_t operation_num) {
    using Deque32 = stool::SimpleDeque<uint32_t, uint32_t>;
    Deque32 d;
    std::deque<uint32_t> ref;

    std::mt19937 rng(20240632); // Fixed seed for reproducibility
    std::uniform_int_distribution<uint32_t> val_dist(1, 1000000000U); // for 32bit range

    // Add elements for initial size
    for(uint64_t i = 0; i < initial_size; ++i) {
        uint32_t value = val_dist(rng);
        d.push_back(value);
        ref.push_back(value);
    }

    std::uniform_int_distribution<int> op_dist(0, 5);

    for (int i = 0; i < (int)operation_num; ++i) {
        if(i %1000 == 0){
            std::cout << "+" << std::flush;
        }
        int op = op_dist(rng);
        uint32_t value = val_dist(rng);

        if (op == 0) { // push_back
            d.push_back(value);
            ref.push_back(value);
        } else if (op == 1) { // push_front
            d.push_front(value);
            ref.push_front(value);
        } else if (op == 2) { // pop_back
            if (!ref.empty()) {
                d.pop_back();
                ref.pop_back();
            }
        } else if (op == 3) { // pop_front
            if (!ref.empty()) {
                d.pop_front();
                ref.pop_front();
            }
        } else if (op == 4) { // insert
            if (!ref.empty()) {
                std::uniform_int_distribution<size_t> idx_dist(0, ref.size());
                size_t pos = idx_dist(rng);
                d.insert(pos, value);
                ref.insert(ref.begin() + pos, value);
            } else {
                d.push_back(value);
                ref.push_back(value);
            }
        } else if (op == 5) { // erase
            if (!ref.empty()) {
                std::uniform_int_distribution<size_t> idx_dist(0, ref.size() - 1);
                size_t pos = idx_dist(rng);
                d.erase(pos);
                ref.erase(ref.begin() + pos);
            }
        }
        // Check
        assert(d.size() == ref.size());
        for (size_t j = 0; j < d.size(); ++j) {
            assert(d.at(j) == ref[j]);
        }
        // Check empty
        assert(d.empty() == ref.empty());
    }
    // shrink_to_fit and to_deque checks
    d.shrink_to_fit();
    std::deque<uint32_t> ref2 = d.to_deque();
    assert(ref2 == ref);
    std::cout << "[OK32]" << std::endl;
}

void test_simple_deque_64(uint64_t initial_size, uint64_t operation_num) {
    using Deque = stool::SimpleDeque<uint64_t, uint64_t>;
    Deque d;
    std::deque<uint64_t> ref;
    std::mt19937_64 rng(42);
    std::uniform_int_distribution<uint64_t> val_dist(1, 10000000000000000000ULL);

    // initial_size分push_back
    for (uint64_t i = 0; i < initial_size; ++i) {
        uint64_t value = val_dist(rng);
        d.push_back(value);
        ref.push_back(value);
    }

    for (uint64_t i = 0; i < operation_num; ++i) {
        if(i % 1000 == 0){
            std::cout << "+" << std::flush;
        }
        uint64_t value = val_dist(rng);
        size_t op = rng() % 6; // 0:push_back, 1:push_front, 2:pop_back, 3:pop_front, 4:insert, 5:erase

        if (op == 0) { // push_back
            d.push_back(value);
            ref.push_back(value);
        } else if (op == 1) { // push_front
            d.push_front(value);
            ref.push_front(value);
        } else if (op == 2) { // pop_back
            if (!ref.empty()) {
                d.pop_back();
                ref.pop_back();
            }
        } else if (op == 3) { // pop_front
            if (!ref.empty()) {
                d.pop_front();
                ref.pop_front();
            }
        } else if (op == 4) { // insert
            if (!ref.empty()) {
                std::uniform_int_distribution<size_t> idx_dist(0, ref.size());
                size_t pos = idx_dist(rng);
                d.insert(pos, value);
                ref.insert(ref.begin() + pos, value);
            } else {
                d.push_back(value);
                ref.push_back(value);
            }
        } else if (op == 5) { // erase
            if (!ref.empty()) {
                std::uniform_int_distribution<size_t> idx_dist(0, ref.size() - 1);
                size_t pos = idx_dist(rng);
                d.erase(pos);
                ref.erase(ref.begin() + pos);
            }
        }
        // Check
        assert(d.size() == ref.size());
        for (size_t j = 0; j < d.size(); ++j) {
            assert(d.at(j) == ref[j]);
        }
        // Check empty
        assert(d.empty() == ref.empty());
    }
    // shrink_to_fit and to_deque checks
    d.shrink_to_fit();
    std::deque<uint64_t> ref2 = d.to_deque();
    assert(ref2 == ref);
    std::cout << "[OK64]" << std::endl;
}

void test_simple_deque_large() {
    using Deque = stool::SimpleDeque<uint64_t, uint64_t>;
    Deque d;

    const size_t N = 10000;
    // Check that values are in correct order after push_back
    for (size_t i = 0; i < N; ++i) {
        d.push_back(i);
    }
    assert(d.size() == N);
    for (size_t i = 0; i < N; ++i) {
        assert(d.at(i) == i);
    }

    // Check decrease after pop_front
    for (size_t i = 0; i < N / 2; ++i) {
        d.pop_front();
    }
    assert(d.size() == N / 2);
    for (size_t i = 0; i < N / 2; ++i) {
        assert(d.at(i) == i + N / 2);
    }
}

int main() {
    std::cout << "\033[34mTest: SimpleDeque\033[0m" << std::endl;
    std::cout << "Testing SimpleDeque<uint16_t, uint16_t>..." << std::endl;
    std::cout << " - Verifying basic operations (push_back, push_front, pop_back, pop_front, insert, erase, shrink_to_fit, to_deque)." << std::endl;
    test_simple_deque_16(10, 10000);
    test_simple_deque_16(100, 10000);
    test_simple_deque_16(1000, 10000);

    std::cout << "Testing SimpleDeque<uint32_t, uint32_t>..." << std::endl;
    std::cout << " - Verifying basic operations (push_back, push_front, pop_back, pop_front, insert, erase, shrink_to_fit, to_deque)." << std::endl;
    test_simple_deque_32(10, 10000);
    test_simple_deque_32(100, 10000);
    test_simple_deque_32(1000, 10000);

    std::cout << "Testing SimpleDeque<uint64_t, uint64_t> (random operations & verification)..." << std::endl;
    std::cout << " - Verifying basic operations (push_back, push_front, pop_back, pop_front, insert, erase, shrink_to_fit, to_deque)." << std::endl;
    test_simple_deque_64(10, 10000);
    test_simple_deque_64(100, 10000);
    test_simple_deque_64(1000, 10000);
    test_simple_deque_64(10000, 10000);
    std::cout << "Basic test OK." << std::endl;

    std::cout << "SimpleDeque large data (push_back/push_front→pop_front, order verification) test..." << std::endl;
    std::cout << " - Verifying correct order with large data (N=10000) for push_back→pop_front." << std::endl;
    test_simple_deque_large();
    std::cout << "Large test OK." << std::endl;

    std::cout << "All SimpleDeque tests passed!" << std::endl;
    return 0;
}