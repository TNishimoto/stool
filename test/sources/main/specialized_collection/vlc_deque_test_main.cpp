#include <iostream>
#include <deque>
#include <random>
#include <cassert>
#include "../../../include/specialized_collection/vlc_deque.hpp"

// 簡単なユーティリティ：std::deque<uint64_t> <-> stool::VLCDeque変換
void check_equal(const std::deque<uint64_t>& a, const stool::VLCDeque& b) {
    auto bv = b.to_deque();
    assert(a == bv && "Deque contents do not match!");
}

std::deque<uint64_t> random_deque(size_t n, uint64_t max_value, std::mt19937_64& mt) {
    std::uniform_int_distribution<uint64_t> dist(0, max_value);
    std::deque<uint64_t> res;
    for (size_t i = 0; i < n; ++i) {
        res.push_back(dist(mt));
    }
    return res;
}

void test_push_pop(uint64_t N, uint64_t max_val, uint64_t seed) {
    std::mt19937_64 mt(seed);

    std::deque<uint64_t> stddeq;
    stool::VLCDeque vdeq;

    // push_back
    for (uint64_t i = 0; i < N; ++i) {
        auto v = mt() % max_val;
        stddeq.push_back(v);
        vdeq.push_back(v);
        check_equal(stddeq, vdeq);
    }
    // pop_back
    for (uint64_t i = 0; i < N/2; ++i) {
        stddeq.pop_back();
        vdeq.pop_back();
        check_equal(stddeq, vdeq);
    }

    // push_front
    for (uint64_t i = 0; i < N/3; ++i) {
        auto v = mt() % max_val;
        stddeq.push_front(v);
        vdeq.push_front(v);
        check_equal(stddeq, vdeq);
    }
    // pop_front
    for (uint64_t i = 0; i < N/4; ++i) {
        stddeq.pop_front();
        vdeq.pop_front();
        check_equal(stddeq, vdeq);
    }

    // ランダムアクセス
    for (size_t i = 0; i < stddeq.size(); ++i) {
        assert(stddeq[i] == vdeq.at(i));
    }
    assert(stddeq.size() == vdeq.size());
}

void test_randomized(uint64_t N, uint64_t max_val, uint64_t num_ops, uint64_t seed) {
    std::mt19937_64 mt(seed);

    std::deque<uint64_t> stddeq;
    stool::VLCDeque vdeq;

    for (uint64_t i = 0; i < N; ++i) {
        auto v = mt() % max_val;
        stddeq.push_back(v);
        vdeq.push_back(v);
    }

    for (uint64_t op = 0; op < num_ops; ++op) {
        int act = mt() % 4;
        if (act == 0 && !stddeq.empty()) {
            stddeq.pop_back();
            vdeq.pop_back();
        } else if (act == 1 && !stddeq.empty()) {
            stddeq.pop_front();
            vdeq.pop_front();
        } else if (act == 2) {
            uint64_t v = mt() % max_val;
            stddeq.push_back(v);
            vdeq.push_back(v);
        } else if (act == 3) {
            uint64_t v = mt() % max_val;
            stddeq.push_front(v);
            vdeq.push_front(v);
        }
        check_equal(stddeq, vdeq);
    }
    for (size_t i = 0; i < stddeq.size(); ++i) {
        assert(stddeq[i] == vdeq.at(i));
    }
    assert(stddeq.size() == vdeq.size());
}

int main() {
    std::cout << "VLCDeque basic test ..." << std::endl;
    std::cout << "Test 1: Sequential push_back and push_front, then pop operations. [N=1000, max_val=256, seed=1]" << std::endl;
    test_push_pop(1000, 256, 1);

    std::cout << "Test 2: Sequential push_back and push_front, then pop operations. [N=50000, max_val=2^32, seed=42]" << std::endl;
    test_push_pop(50000, 1ULL << 32, 42);

    std::cout << "Passed push/pop basic tests" << std::endl;

    std::cout << "VLCDeque randomized test ..." << std::endl;
    std::cout << "Test 3: Randomized sequence of push/pop operations. [N=1000, max_val=65536, num_ops=3000, seed=12345]" << std::endl;
    test_randomized(1000, 65536, 3000, 12345);

    std::cout << "Test 4: Randomized sequence of push/pop operations with large values. [N=500, max_val=2^45, num_ops=2500, seed=246810]" << std::endl;
    test_randomized(500, 1ULL << 45, 2500, 246810);

    std::cout << "All VLCDeque tests passed!" << std::endl;
    return 0;
}