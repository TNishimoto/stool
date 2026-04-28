#include <iostream>
#include <deque>
#include <random>
#include <cassert>
#include "../../../include/specialized_collection/vlc_deque.hpp"

// 簡単なユーティリティ：std::deque<uint64_t> <-> stool::VLCDeque変換
void check_equal(const std::deque<uint64_t>& a, const stool::VLCDeque& b) {
    auto bv = b.to_deque();
    if (!(a == bv)) {
        std::cerr << "Deque contents do not match!\n";
        std::cerr << "correct: ";
        for (const auto& v : a) std::cerr << v << " ";
        std::cerr << "\n";
        std::cerr << "test   : ";
        for (const auto& v : bv) std::cerr << v << " ";
        std::cerr << "\n";
        assert(false && "Deque contents do not match!");
    }
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

void test_randomized(uint64_t N, uint64_t max_val, uint64_t num_ops, uint64_t seed, bool verbose = false) {
    std::mt19937_64 mt(seed);

    std::deque<uint64_t> stddeq;
    stool::VLCDeque vdeq;

    for (uint64_t i = 0; i < N; ++i) {
        auto v = mt() % max_val;
        stddeq.push_back(v);
        vdeq.push_back(v);
    }

    auto safe_rand_index = [&](size_t sz) -> size_t {
        return sz == 0 ? 0 : (mt() % sz);
    };

    for (uint64_t op = 0; op < num_ops; ++op) {
        int act = mt() % 9;
        if (act == 0 && !stddeq.empty()) { // pop_back
            if (verbose) std::cout << "pop_back" << std::endl;
            stddeq.pop_back();
            vdeq.pop_back();
        } else if (act == 1 && !stddeq.empty()) { // pop_front
            if (verbose) std::cout << "pop_front" << std::endl;
            stddeq.pop_front();
            vdeq.pop_front();
        } else if (act == 2) { // push_back
            uint64_t v = mt() % max_val;
            if (verbose) std::cout << "push_back" << std::endl;
            stddeq.push_back(v);
            vdeq.push_back(v);
        } else if (act == 3) { // push_front
            uint64_t v = mt() % max_val;
            if (verbose) std::cout << "push_front" << std::endl;
            stddeq.push_front(v);
            vdeq.push_front(v);
        } else if (act == 4) { // insert
            uint64_t v = mt() % max_val;
            size_t pos = safe_rand_index(stddeq.size() + 1);
            if (verbose) std::cout << "insert" << std::endl;
            stddeq.insert(stddeq.begin() + pos, v);
            vdeq.insert(pos, v);
        } else if (act == 5 && !stddeq.empty()) { // remove
            size_t pos = safe_rand_index(stddeq.size());
            if (verbose) std::cout << "remove, pos: " << pos << std::endl;
            stddeq.erase(stddeq.begin() + pos);
            vdeq.remove(pos);
        } else if (act == 6 && !stddeq.empty()) { // set_value
            size_t pos = safe_rand_index(stddeq.size());
            uint64_t w = stddeq[pos];
            uint64_t v = mt() % w;
            if (verbose) std::cout << "set_value," << pos << ", " << v << ", " << w << ", " << stddeq[pos] << ", " << vdeq.at(pos) << std::endl;
            assert(stddeq[pos] == vdeq.at(pos));
            stddeq[pos] = v;
            vdeq.set_value(pos, v);
        } else if (act == 7 && stddeq.size() > 1) { // shift_left
            size_t k = 1 + (mt() % (stddeq.size() - 1));
            if (verbose) std::cout << "shift_left, pos: " << k << std::endl;
            stddeq.erase(stddeq.begin() + k);
            vdeq.shift_left(k);
        } else if (act == 8 && stddeq.size() > 1) { // shift_right
            size_t k = 1 + (mt() % (stddeq.size() - 1));
            if (verbose) std::cout << "shift_right" << std::endl;            
            stddeq.insert(stddeq.begin() + k, 0);
            vdeq.shift_right(k, 1);
        }
   
   

        // increment
        if (!stddeq.empty() && (mt() % 5 == 0)) {
            size_t pos = safe_rand_index(stddeq.size());
            stddeq[pos]++;
            vdeq.increment(pos, 1);
        }

        check_equal(stddeq, vdeq);
    }
    for (size_t i = 0; i < stddeq.size(); ++i) {
        assert(stddeq[i] == vdeq.at(i));
    }
    assert(stddeq.size() == vdeq.size());
}

int main() {
    std::cout << "Starting VLCDeque basic tests ..." << std::endl;
    std::cout << "Test 1: Sequential push_back and push_front followed by pop operations. [N=1000, max_val=256, seed=1]" << std::endl;
    test_push_pop(1000, 256, 10000);

    std::cout << "Test 2: Sequential push_back and push_front followed by pop operations. [N=5000, max_val=2^32, seed=42]" << std::endl;
    test_push_pop(5000, 1ULL << 32, 10000);

    std::cout << "Push/pop basic tests passed." << std::endl;

    std::cout << "Starting VLCDeque randomized tests ..." << std::endl;
    std::cout << "Test 3: Randomized push/pop operations. [Initial elements=5, max_val=65536, num_ops=10000, seed=12345]" << std::endl;
    test_randomized(5, 65536, 10000, 12345, false);

    std::cout << "Test 4: Randomized push/pop operations with large values. [Initial elements=500, max_val=2^45, num_ops=10000, seed=246810]" << std::endl;
    test_randomized(500, 1ULL << 45, 10000, 246810);

    std::cout << "All VLCDeque tests passed!" << std::endl;
    return 0;
}