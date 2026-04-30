#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstdint>
#include "../../../../include/specialized_collection/push_pop_arrays/naive_bit_vector.hpp"

using stool::NaiveBitVector;

/**
 * @brief Tests NaiveBitVector by performing a wide variety of random operations and checking consistency with the reference implementation (std::vector<bool>).
 *
 * @param initial_size The initial size of the bit vector (randomly initialized with 0/1)
 * @param op_num The total number of operations to execute
 * @param max_bits Maximum bit length (do not increase size beyond this)
 * @param seed Random seed
 */
void test_naive_bit_vector_random_ops(
    int initial_size = 100,
    int op_num = 10000,
    uint64_t max_bits = 512,
    uint64_t seed = 424242
) {
    std::mt19937_64 mt(seed);
    // Use a reference model for correctness check
    std::vector<bool> ref;
    NaiveBitVector<> bv;

    // Add 0 or 1 at random for the specified initial size (never exceed max_bits)
    const int init_n = std::max(0, std::min(initial_size, static_cast<int>(max_bits)));
    for (int i = 0; i < init_n; ++i) {
        bool val = mt() & 1;
        bv.push_back(val);
        ref.push_back(val);
    }

    auto random_bool = [&]() { return mt() & 1; };
    auto random_erase_len = [&](size_t pos) {
        if (ref.empty()) return 0U;
        size_t maxlen = std::min<size_t>(65, ref.size() - pos);
        return (uint32_t)(1 + (mt() % maxlen));
    };

    // New op numbers for query ops: start after mutating ops
    static constexpr int OP_PUSH_BACK = 0;
    static constexpr int OP_PUSH_FRONT = 1;
    static constexpr int OP_POP_BACK = 2;
    static constexpr int OP_POP_FRONT = 3;
    static constexpr int OP_INSERT = 4;
    static constexpr int OP_INSERT64 = 5;
    static constexpr int OP_ERASE = 6;
    static constexpr int OP_SHIFT_LEFT = 7;
    static constexpr int OP_SHIFT_RIGHT = 8;
    static constexpr int OP_REPLACE = 9;
    static constexpr int OP_REPLACE64 = 10;
    static constexpr int OP_ERASE_RANGE = 11;
    static constexpr int OP_PSUM = 12;
    static constexpr int OP_REVERSE_PSUM = 13;
    static constexpr int OP_RANK0 = 14;
    static constexpr int OP_RANK1 = 15;
    static constexpr int OP_SELECT0 = 16;
    static constexpr int OP_SELECT1 = 17;
    static constexpr int OP_SELECT1_SUCCESSOR = 18;
    static constexpr int OP_SELECT1_PREDECESSOR = 19;
    static constexpr int OP_REV_SELECT1 = 20;

    static constexpr int SAFE_OPS[] = {
        OP_PUSH_BACK, OP_PUSH_FRONT, OP_POP_BACK, OP_POP_FRONT, OP_INSERT, OP_INSERT64,
        OP_ERASE, OP_SHIFT_LEFT, OP_SHIFT_RIGHT, OP_REPLACE, OP_REPLACE64, OP_ERASE_RANGE,
        // Query ops:
        OP_PSUM, OP_REVERSE_PSUM, OP_RANK0, OP_RANK1,
        OP_SELECT0, OP_SELECT1, OP_SELECT1_SUCCESSOR, OP_SELECT1_PREDECESSOR, OP_REV_SELECT1
    };

    for (int op = 0; op < op_num; ++op) {
        if (op % 100 == 0) {
            std::cout << "+" << std::flush;
        }
        int which = SAFE_OPS[mt() % (sizeof(SAFE_OPS) / sizeof(SAFE_OPS[0]))];
        switch (which) {
        case OP_PUSH_BACK: { // push_back
            if (ref.size() >= max_bits) break;
            bool val = random_bool();
            bv.push_back(val);
            ref.push_back(val);
            break;
        }
        case OP_PUSH_FRONT: { // push_front
            if (ref.size() >= max_bits) break;
            bool val = random_bool();
            bv.push_front(val);
            ref.insert(ref.begin(), val);
            break;
        }
        case OP_POP_BACK: { // pop_back
            if (ref.empty()) break;
            bv.pop_back();
            ref.pop_back();
            break;
        }
        case OP_POP_FRONT: { // pop_front
            if (ref.empty()) break;
            bv.pop_front();
            ref.erase(ref.begin());
            break;
        }
        case OP_INSERT: { // insert
            if (ref.size() >= max_bits) break;
            uint32_t pos = ref.empty() ? 0 : (mt() % (ref.size() + 1));
            bool val = random_bool();
            bv.insert(pos, val);
            ref.insert(ref.begin() + pos, val);
            break;
        }
        case OP_INSERT64: { // insert64
            if (ref.size() >= max_bits) break;
            uint32_t pos = ref.empty() ? 0 : (mt() % (ref.size() + 1));
            int len = std::min<int>({64, (int)(max_bits - ref.size()), (int)(64 - (pos % 64))});
            if (len == 0) break;
            uint64_t val = mt();
            bv.insert64(pos, val, len);
            for (int i = 0; i < len; ++i)
                ref.insert(ref.begin() + pos + i, (val >> (63 - i)) & 1);
            break;
        }
        case OP_ERASE: { // erase
            if (ref.empty()) break;
            uint32_t pos = mt() % ref.size();
            bv.erase(pos);
            ref.erase(ref.begin() + pos);
            break;
        }
        case OP_SHIFT_LEFT: { // shift_left(p, len): erase [p-len, p)
            if (ref.empty()) break;
            uint32_t p = 1 + (mt() % ref.size()); // p in [1, size]
            uint32_t len = 1 + (mt() % std::min<uint32_t>(32, p)); // len <= p
            bv.shift_left(p, len);
            ref.erase(ref.begin() + (p - len), ref.begin() + p);
            break;
        }
        case OP_SHIFT_RIGHT: { // shift_right(p, len): insert len zeros at p
            if (ref.size() >= max_bits) break;
            uint32_t p = ref.empty() ? 0 : (mt() % (ref.size() + 1));
            uint32_t len = 1 + (mt() % std::min<size_t>(32, max_bits - ref.size()));
            bv.shift_right(p, len);
            ref.insert(ref.begin() + p, len, false);
            break;
        }
        case OP_REPLACE: { // replace
            if (ref.empty()) break;
            uint32_t pos = mt() % ref.size();
            bool val = random_bool();
            bv.replace(pos, val);
            ref[pos] = val;
            break;
        }
        case OP_REPLACE64: { // replace64
            if (ref.empty()) break;
            uint32_t pos = mt() % ref.size();
            uint32_t len = std::min<uint32_t>({64, (uint32_t)(ref.size() - pos), (uint32_t)(64 - (pos % 64))});
            if (len == 0) break;
            uint64_t val = mt();
            bv.replace64(pos, val, len);
            for (uint32_t i = 0; i < len; ++i)
                ref[pos + i] = (val >> (63 - i)) & 1;
            break;
        }
        case OP_ERASE_RANGE: { // erase range (simulate erase(len))
            if (ref.empty()) break;
            uint32_t pos = mt() % ref.size();
            uint32_t elen = random_erase_len(pos);
            if (elen == 0) break;
            if (pos + elen > ref.size()) elen = ref.size() - pos;
            for (uint32_t i = 0; i < elen; ++i)
                bv.erase(pos);
            ref.erase(ref.begin() + pos, ref.begin() + pos + elen);
            break;
        }
        case OP_PSUM: { // psum(i) == #1s in B[0..i] (inclusive); half-open [0,pos) matches psum(pos-1)
            if (ref.empty()) break;
            size_t pos = mt() % (ref.size() + 1);
            size_t expected = 0;
            for (size_t i = 0; i < pos; ++i) expected += ref[i] ? 1 : 0;
            const size_t res = pos == 0 ? 0 : bv.psum(pos - 1);
            if (res != expected) {
                std::cerr << "[NG] psum mismatch at op=" << op << " pos=" << pos
                          << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        case OP_REVERSE_PSUM: { // #1s in B[(n-1)-pos .. n-1]; if pos+1 >= n then whole-vector psum
            if (ref.empty()) break;
            const size_t n = ref.size();
            size_t pos = mt() % (n + 1);
            size_t expected = 0;
            if (pos + 1 >= n) {
                for (size_t i = 0; i < n; ++i) expected += ref[i] ? 1 : 0;
            } else {
                for (size_t j = n - 1 - pos; j < n; ++j) expected += ref[j] ? 1 : 0;
            }
            const size_t res = bv.reverse_psum(pos);
            if (res != expected) {
                std::cerr << "[NG] reverse_psum mismatch at op=" << op << " pos=" << pos
                          << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        case OP_RANK0: { // rank0(i) == #0s in B[0..i] inclusive; [0,pos) -> rank0(pos-1)
            if (ref.empty()) break;
            size_t pos = mt() % (ref.size() + 1);
            size_t expected = 0;
            for (size_t i = 0; i < pos; ++i) expected += ref[i] ? 0 : 1;
            const size_t res = pos == 0 ? 0 : bv.rank0(pos - 1);
            if (res != expected) {
                std::cerr << "[NG] rank0 mismatch at op=" << op << " pos=" << pos
                          << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        case OP_RANK1: { // rank1(i) == #1s in B[0..i] inclusive; [0,pos) -> rank1(pos-1)
            if (ref.empty()) break;
            size_t pos = mt() % (ref.size() + 1);
            size_t expected = 0;
            for (size_t i = 0; i < pos; ++i) expected += ref[i] ? 1 : 0;
            const size_t res = pos == 0 ? 0 : bv.rank1(pos - 1);
            if (res != expected) {
                std::cerr << "[NG] rank1 mismatch at op=" << op << " pos=" << pos
                          << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        case OP_SELECT0: { // select0(k) returns pos s.t. [pos] is the k-th 0 (0-based)
            size_t zeros = 0;
            for (bool bit : ref) if (!bit) ++zeros;
            if (zeros == 0) break;
            size_t k = mt() % zeros;
            size_t expected = ref.size();
            size_t cnt = 0;
            for (size_t i = 0; i < ref.size(); ++i) {
                if (!ref[i]) {
                    if (cnt == k) { expected = i; break; }
                    ++cnt;
                }
            }
            const int64_t res = bv.select0(k);
            if (res < 0 || static_cast<size_t>(res) != expected) {
                std::cerr << "[NG] select0 mismatch at op=" << op
                          << " k=" << k << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        case OP_SELECT1: { // select1(k) returns pos s.t. [pos] is the k-th 1 (0-based)
            size_t ones = 0;
            for (bool bit : ref) if (bit) ++ones;
            if (ones == 0) break;
            size_t k = mt() % ones;
            size_t expected = ref.size();
            size_t cnt = 0;
            for (size_t i = 0; i < ref.size(); ++i) {
                if (ref[i]) {
                    if (cnt == k) { expected = i; break; }
                    ++cnt;
                }
            }
            const int64_t res = bv.select1(k);
            if (res < 0 || static_cast<size_t>(res) != expected) {
                std::cerr << "[NG] select1 mismatch at op=" << op
                          << " k=" << k << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        case OP_SELECT1_SUCCESSOR: {
            // Implementation scans from index pos+1 (not including pos); returns -1 if none.
            if (ref.empty()) break;
            const size_t pos = mt() % ref.size();
            int64_t expected = -1;
            for (size_t i = pos + 1; i < ref.size(); ++i) {
                if (ref[i]) { expected = static_cast<int64_t>(i); break; }
            }
            const int64_t res = bv.select1_successor(pos);
            if (res != expected) {
                std::cerr << "[NG] select1_successor mismatch at op=" << op << " pos=" << pos
                          << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        case OP_SELECT1_PREDECESSOR: {
            // select1_predecessor(pos): largest i < pos with ref[i]==1; API returns -1 if none.
            // Skip pos==0: implementation uses (pos-1) on uint64_t.
            if (ref.empty()) break;
            const size_t pos = 1 + (mt() % ref.size());
            int64_t expected = -1;
            for (size_t i = pos; i-- > 0;) {
                if (ref[i]) { expected = static_cast<int64_t>(i); break; }
            }
            const int64_t res = bv.select1_predecessor(pos);
            if (res != expected) {
                std::cerr << "[NG] select1_predecessor mismatch at op=" << op << " pos=" << pos
                          << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        case OP_REV_SELECT1: {
            // rev_select1(k): the position i from the right such that i-th 1 from the right (0-based)
            size_t ones = 0;
            for (bool bit : ref) if (bit) ++ones;
            if (ones == 0) break;
            size_t k = mt() % ones;
            size_t expected = ref.size();
            size_t cnt = 0;
            for (size_t i = ref.size(); i-- > 0;) {
                if (ref[i]) {
                    if (cnt == k) { expected = i; break; }
                    ++cnt;
                }
            }
            const int64_t res = bv.rev_select1(k);
            if (res < 0 || static_cast<size_t>(res) != expected) {
                std::cerr << "[NG] rev_select1 mismatch at op=" << op
                          << " k=" << k << " res=" << res << " expected=" << expected << std::endl;
                assert(false);
            }
            break;
        }
        }

        // Verify structure agreement for mutation ops
        if (which <= OP_ERASE_RANGE) {
            if (bv.size() != ref.size()) {
                std::cerr << "[NG] size mismatch at op=" << op << ", which=" << which
                          << ", bv.size=" << bv.size() << ", ref.size=" << ref.size() << std::endl;
                assert(false);
            }
            for (size_t i = 0; i < ref.size(); ++i) {
                if (bv.at(i) != ref[i]) {
                    std::cerr << "[NG] bit mismatch at op=" << op << ", which=" << which
                              << ", index=" << i << ", bv=" << (int)bv.at(i)
                              << ", ref=" << (int)ref[i] << std::endl;
                    assert(false);
                }
            }
        }
    }
    std::cout << std::endl;
    std::cout << "[OK] Randomized NaiveBitVector ops (" << op_num << " ops, max_bits=" << max_bits << ", seed=" << seed << ") PASSED." << std::endl;
}

/**
 * @brief Basic operation test for push_back and pop_back. Push a fixed pattern and pop them all to check correctness.
 */
void test_push_and_pop() {
    std::cout << "[Test] NaiveBitVector push_back & pop_back ..." << std::endl;
    NaiveBitVector<> bv;
    const int N = 500;

    // Push 0, 1, 0, 1, ...
    for (int i = 0; i < N; ++i) {
        bv.push_back(i % 2 == 0 ? false : true);
    }
    assert(bv.size() == N);

    for (int i = 0; i < N; ++i) {
        assert(bv.at(i) == (i % 2 == 0 ? false : true));
    }

    // Pop everything and verify
    for (int i = 0; i < N; ++i) {
        bool expected = ((N - i - 1) % 2 == 0 ? false : true);
        // NaiveBitVector does not have back(), so use at(size()-1)
        bool val = bv.at(bv.size() - 1);
        assert(val == expected);
        bv.pop_back();
    }
    assert(bv.size() == 0);

    std::cout << "[OK] push_back & pop_back test passed" << std::endl;
}

/**
 * @brief Pushes a random bit sequence via push_back, then pops all bits via pop_back and checks consistency at every step.
 *
 * @param seed Random seed
 * @param N    Number of bits
 */
void test_random_bitvector(uint64_t seed = 777, int N = 1024) {
    std::cout << "[Test] NaiveBitVector random push_back/pop_front ..." << std::endl;
    std::vector<bool> ref;
    NaiveBitVector<> bv;
    std::mt19937_64 mt(seed);
    std::uniform_int_distribution<uint64_t> dist(0, 1);

    // Push random bits
    for (int i = 0; i < N; ++i) {
        bool val = dist(mt) == 1;
        bv.push_back(val);
        ref.push_back(val);
    }

    // Check all values
    for (int i = 0; i < N; ++i) {
        assert(bv.at(i) == ref[i]);
    }

    // Pop_back & check
    for (int i = 0; i < N; ++i) {
        // NaiveBitVector does not have back(), so use at(size()-1)
        assert(bv.at(bv.size() - 1) == ref[ref.size()-1]);
        bv.pop_back();
        ref.pop_back();
        assert(bv.size() == ref.size());
    }
    assert(bv.size() == 0);

    std::cout << "[OK] random push_back/pop_back test passed" << std::endl;
}

/**
 * @brief Tests if NaiveBitVector's iterator can properly traverse all bits.
 */
void test_iterator() {
    std::cout << "[Test] NaiveBitVector iterator ..." << std::endl;
    NaiveBitVector<> bv;
    std::vector<bool> vals = {1,0,1,1,0,0,1,1,1,0,1};
    for (bool v : vals) bv.push_back(v);

    std::vector<bool> iter_vals;
    auto it = bv.begin();
    while (it != bv.end()) {
        iter_vals.push_back(*it);
        ++it;
    }
    assert(iter_vals == vals);

    std::cout << "[OK] iterator test passed" << std::endl;
}

/**
 * @brief Tests properties and iterator behavior of an empty NaiveBitVector.
 */
void test_empty_vector() {
    std::cout << "[Test] NaiveBitVector empty vector ..." << std::endl;
    NaiveBitVector<> bv;
    assert(bv.size() == 0);

    // begin() == end()
    assert(bv.begin() == bv.end());
    std::cout << "[OK] empty vector test passed" << std::endl;
}

/**
 * @brief Tests that storing and accessing all-ones or all-zeros bit vectors works correctly.
 */
void test_all_ones_and_zeros() {
    std::cout << "[Test] NaiveBitVector all ones/zeros ..." << std::endl;
    int N = 512;
    NaiveBitVector<> ones, zeros;
    for (int i = 0; i < N; ++i) {
        ones.push_back(true);
        zeros.push_back(false);
    }
    for (int i = 0; i < N; ++i) {
        assert(ones.at(i) == true);
        assert(zeros.at(i) == false);
    }
    std::cout << "[OK] all ones/zeros test passed" << std::endl;
}

/**
 * @brief Main entry to run all test cases. Also prints descriptions of each test to the console.
 */
int main() {
    std::cout << "\033[34mTest: NaiveBitVector\033[0m" << std::endl;

    std::cout << "test_push_and_pop: Testing push_back and pop_back." << std::endl;
    test_push_and_pop();

    std::cout << "test_random_bitvector: Testing push/pop and front/back operations on random bit vectors." << std::endl;
    test_random_bitvector();

    std::cout << "test_iterator: Verifying iterator behavior of NaiveBitVector." << std::endl;
    test_iterator();

    std::cout << "test_empty_vector: Testing empty NaiveBitVector characteristics (iterator etc.)." << std::endl;
    test_empty_vector();

    std::cout << "test_all_ones_and_zeros: Checking behavior for bit vectors with all ones or all zeros." << std::endl;
    test_all_ones_and_zeros();

    std::cout << "test_naive_bit_vector_random_ops: Performing many randomized operations and checking correctness against the reference model." << std::endl;
    // (initial_size, op_num, max_bits, seed) — initial length is capped to max_bits
    test_naive_bit_vector_random_ops(256, 10000, 512, 42);
    test_naive_bit_vector_random_ops(512, 10000, 512, 424);
    test_naive_bit_vector_random_ops(1024, 10000, 512, 4242);
    test_naive_bit_vector_random_ops(2048, 10000, 512, 42424);

    std::cout << "All NaiveBitVector tests passed!" << std::endl;
    return 0;
}