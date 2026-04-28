#include <iostream>
#include <vector>
#include <cassert>
#include <sstream>
#include <random>

#include "../../../include/specialized_collection/value_array.hpp"

using stool::ValueArray;

// Helper: Generate a vector of given type and values
template <typename T>
std::vector<T> make_test_vector(size_t n, T base = 0, T diff = 1) {
    std::vector<T> v(n);
    for (size_t i = 0; i < n; ++i) {
        v[i] = static_cast<T>(base + i * diff);
    }
    return v;
}

void test_value_array_basic() {
    std::cout << "[Test] ValueArray basic operation..." << std::endl;

    // Test for uint8_t, uint16_t, uint32_t, uint64_t
    {
        std::vector<uint8_t> data = {1, 3, 5, 7, 9, 250, 255};
        ValueArray arr;
        arr.set(data);
        assert(arr.size() == data.size());

        std::vector<uint8_t> dec;
        arr.decode(dec);
        assert(dec == data);

        std::vector<uint16_t> dec16;
        arr.decode(dec16);
        for (size_t i = 0; i < data.size(); ++i) {
            assert(dec16[i] == data[i]);
        }
    }
    {
        std::vector<uint16_t> data = {10, 100, 200, 1000, 65000};
        ValueArray arr;
        arr.set(data);
        assert(arr.size() == data.size());

        std::vector<uint16_t> dec;
        arr.decode(dec);
        assert(dec == data);

        std::vector<uint32_t> dec32;
        arr.decode(dec32);
        for (size_t i = 0; i < data.size(); ++i) {
            assert(dec32[i] == data[i]);
        }
    }
    {
        std::vector<uint32_t> data = {1, 255, 65535, 100000, 3000000000U};
        ValueArray arr;
        arr.set(data);
        assert(arr.size() == data.size());

        std::vector<uint32_t> dec;
        arr.decode(dec);
        assert(dec == data);

        std::vector<uint64_t> dec64;
        arr.decode(dec64);
        for (size_t i = 0; i < data.size(); ++i) {
            assert(dec64[i] == data[i]);
        }
    }
    {
        std::vector<uint64_t> data = {0, 1, 2, 3, 4, 0xFFFFFFFFFFFFFFFLLU, 0xFFFFFFFFFFFFFFFFLLU};
        ValueArray arr;
        arr.set(data);
        assert(arr.size() == data.size());

        std::vector<uint64_t> dec;
        arr.decode(dec);
        assert(dec == data);

        std::vector<uint8_t> dec8;
        arr.decode(dec8);
        // When using 8bit type, only the lowest 8 bits are stored for each element
        for (size_t i = 0; i < data.size(); ++i) {
            assert(dec8[i] == static_cast<uint8_t>(data[i]));
        }
    }

    std::cout << "[OK] Basic set/decode test" << std::endl;
}
void test_value_array_change_and_access(size_t X) {
    // Random resize, change, operator[] test
    std::cout << "[Test] ValueArray resize()/change()/operator[] random operation test" << std::endl;

    std::mt19937_64 mt(12345);
    std::uniform_int_distribution<size_t> opdist(0, 2); // 0:resize, 1:change, 2:operator[]
    std::uniform_int_distribution<size_t> sizedist(1, 100);
    std::uniform_int_distribution<uint64_t> vald8(0, 0xFF);
    std::uniform_int_distribution<uint64_t> vald16(0, 0xFFFF);
    std::uniform_int_distribution<uint64_t> vald32(0, 0xFFFFFFFFULL);
    std::uniform_int_distribution<uint64_t> vald64(0, 0xFFFFFFFFFFFFFFFFULL);

    // Data to be tested
    std::vector<uint64_t> ref;
    size_t n = 50;
    ref.resize(n, 0);
    for(size_t i=0;i<n;++i) ref[i] = vald64(mt);

    ValueArray arr;
    arr.set(ref, true);

    for(size_t t=0; t<X; ++t){
        int op = opdist(mt);
        if(op == 0) {
            // resize
            size_t newn = sizedist(mt);
            ref.resize(newn);
            for(size_t i=0;i<newn;++i) {
                if (i >= arr.size()) {
                    ref[i] = vald64(mt);
                }
            }
            // ValueArray: emulate resize (reset with set())
            arr.set(ref, true);
        } else if(op == 1) {
            if(arr.size() == 0) continue;
            // change operation
            std::uniform_int_distribution<size_t> idist(0, arr.size()-1);
            size_t idx = idist(mt);
            // Choose value by element type
            uint64_t val = 0;
            switch(arr.get_byte_size()) {
            case 1: val = vald8(mt); break;
            case 2: val = vald16(mt); break;
            case 4: val = vald32(mt); break;
            case 8: val = vald64(mt); break;
            }
            arr.change(idx, val);
            ref[idx] = val;
        } else {
            if(arr.size() == 0) continue;
            std::uniform_int_distribution<size_t> idist(0, arr.size()-1);
            size_t idx = idist(mt);
            // Check value obtained by operator[]
            uint64_t arrval = arr[idx];
            assert(arrval == ref[idx]);
        }
        // Occasional full check for all elements match
        if(t % 231 == 0) {
            assert(arr.size() == ref.size());
            for(size_t i=0;i<arr.size();++i){
                assert(arr[i] == ref[i]);
            }
        }
    }
    // Final check: all elements should match
    assert(arr.size() == ref.size());
    for(size_t i=0;i<arr.size();++i){
        assert(arr[i] == ref[i]);
    }

    std::cout << "[OK] Random resize/change/operator[] test" << std::endl;
}



void test_value_array_random(size_t n) {
    std::cout << "[Test] ValueArray random data test (" << n << " items)" << std::endl;
    // Generate random data
    std::mt19937_64 mt(31415);
    std::uniform_int_distribution<uint32_t> dist32(0, UINT32_MAX);

    std::vector<uint32_t> data(n);
    for (size_t i = 0; i < n; ++i) data[i] = dist32(mt);

    ValueArray arr;
    arr.set(data);

    std::vector<uint32_t> dec;
    arr.decode(dec);
    assert(dec == data);

    std::cout << "[OK] Random 32bit set/decode test" << std::endl;
}

void test_value_array_save_load() {
    std::cout << "[Test] ValueArray file I/O..." << std::endl;
    std::vector<uint16_t> data = {0, 42, 1000, 65535};
    ValueArray arr;
    arr.set(data);

    std::ofstream ofs("va_test.bin", std::ios::binary);
    arr.write(ofs);
    ofs.close();

    std::ifstream ifs("va_test.bin", std::ios::binary);
    ValueArray arr2;
    arr2.load(ifs);
    ifs.close();
    assert(arr2.size() == data.size());

    std::vector<uint16_t> dec;
    arr2.decode(dec);
    assert(dec == data);

    std::remove("va_test.bin");
    std::cout << "[OK] File save/load test" << std::endl;
}

void test_value_array_memory() {
    std::cout << "[Test] ValueArray memory usage..." << std::endl;
    std::vector<uint8_t> d8(100, 1);
    std::vector<uint16_t> d16(100, 0x8000);
    std::vector<uint32_t> d32(100, 0xFFFFFFFE);
    std::vector<uint64_t> d64(100, 0xFF'FF'FF'FF'FF'FF'FF'FE);

    ValueArray v8, v16, v32, v64;
    v8.set(d8); v16.set(d16); v32.set(d32); v64.set(d64);

    std::cout << "100 x uint8_t:  " << v8.get_using_memory() << " bytes" << std::endl;
    std::cout << "100 x uint16_t:" << v16.get_using_memory() << " bytes" << std::endl;
    std::cout << "100 x uint32_t:" << v32.get_using_memory() << " bytes" << std::endl;
    std::cout << "100 x uint64_t:" << v64.get_using_memory() << " bytes" << std::endl;
    std::cout << "[OK] Memory usage print" << std::endl;
}

int main() {
    std::cout << "\033[34mTest: ValueArray\033[0m" << std::endl;

    test_value_array_basic();
    test_value_array_random(10000);
    test_value_array_save_load();
    test_value_array_memory();
    test_value_array_change_and_access(3000);

    std::cout << "All ValueArray tests passed!" << std::endl;
    return 0;
}