#include <cassert>
#include <chrono>
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
// #include "../../include/io.hpp"
// #include "../../include/cmdline.h"
#include "../../../include/light_stool.hpp"


/*
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy-with-user-provided-copy" // suppress specific warning
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/wt_algorithm.hpp>
#pragma GCC diagnostic pop
*/

int64_t compute_select1(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = 0; j < bv.size(); j++){
        if(bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}
int64_t compute_select0(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = 0; j < bv.size(); j++){
        if(!bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}

int64_t compute_rev_select1(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = bv.size()-1; j >= 0; j--){
        if(bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}
int64_t compute_rev_select0(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = bv.size()-1; j >= 0; j--){
        if(!bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}
int64_t compute_rank1(const std::vector<bool> &bv, uint64_t b, uint64_t e){
    uint64_t count = 0;
    for(uint64_t j = b; j <= e; j++){
        if(bv[j]){
            count++;
        }
    }
    return count;
}
int64_t compute_search(const std::vector<uint64_t> &values, uint64_t i){
    uint64_t sum = 0;

    for(uint64_t j = 0; j < values.size(); j++){
        if(sum + values[j] >= i){
            return j;
        }
        sum += values[j];
    }
    return -1;
}

void packed_psum1_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_psum1_test: " << std::flush;
    std::vector<bool> bits;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> bit_seq;
        std::vector<bool> bits;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 64)+1); j++){
            bit_seq.push_back(get_rand_value(mt));
        }
        for(auto x : bit_seq){
            for(uint64_t j = 0; j < 64; j++){
                bits.push_back(x & (1ULL << (63 - j)));
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t p = stool::PackedPSum::psum(bit_seq.data(), j, stool::PackedPSum::PackedBitType::BIT_1, bit_seq.size());
            uint64_t q = compute_rank1(bits, 0, j);
            if(p != q){
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("rank1 error(1)");
            }
        }

        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t x = get_rand_value(mt) % (j+1);
            uint64_t p = stool::PackedPSum::psum(bit_seq.data(), x, j, stool::PackedPSum::PackedBitType::BIT_1, bit_seq.size());
            uint64_t q = compute_rank1(bits, x, j);
            if(p != q){
                std::cout << x << "/" << j << "/p:" << p << "/q:" << q << std::endl;
                throw std::runtime_error("rank1 error(2)");
            }
        }

    }
    std::cout << "[DONE]" << std::endl;
}

void packed_psum2_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_psum2_test: " << std::flush;
    std::vector<bool> bits;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 32)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        for(auto x : packed_bits){
            uint64_t mask = UINT64_MAX << 62;
            for(uint64_t j = 0; j < 32; j++){
                uint64_t value = (x & (mask >> (2 * j))) >> (62 - (2*j));
                values.push_back(value);
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), j, stool::PackedPSum::PackedBitType::BIT_2, packed_bits.size());
            uint64_t q = std::accumulate(values.begin(), values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum2 error");
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t x = get_rand_value(mt) % (j+1);
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), x, j, stool::PackedPSum::PackedBitType::BIT_2, packed_bits.size());
            uint64_t q = std::accumulate(values.begin() + x, values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum2 error");
            }
        }

    }

    std::cout << "[DONE]" << std::endl;
}
void packed_psum4_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_psum4_test: " << std::flush;
    std::vector<bool> bits;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 16)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        for(auto x : packed_bits){
            uint64_t mask = UINT64_MAX << 60;
            for(uint64_t j = 0; j < 16; j++){
                uint64_t value = (x & (mask >> (4 * j))) >> (60 - (4*j));
                values.push_back(value);
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), j, stool::PackedPSum::PackedBitType::BIT_4, packed_bits.size());
            uint64_t q = std::accumulate(values.begin(), values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum4 error");
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t x = get_rand_value(mt) % (j+1);
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), x, j, stool::PackedPSum::PackedBitType::BIT_4, packed_bits.size());
            uint64_t q = std::accumulate(values.begin() + x, values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum4 error");
            }
        }

    }
    std::cout << "[DONE]" << std::endl;
}
void packed_psum8_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_psum8_test: " << std::flush;
    std::vector<bool> bits;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 8)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        for(auto x : packed_bits){
            uint64_t mask = UINT64_MAX << 56;
            for(uint64_t j = 0; j < 8; j++){
                uint64_t value = (x & (mask >> (8 * j))) >> (56 - (8*j));
                values.push_back(value);
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), j, stool::PackedPSum::PackedBitType::BIT_8, packed_bits.size());
            uint64_t q = std::accumulate(values.begin(), values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum8 error");
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t x = get_rand_value(mt) % (j+1);
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), x, j, stool::PackedPSum::PackedBitType::BIT_8, packed_bits.size());
            uint64_t q = std::accumulate(values.begin() + x, values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum8 error");
            }
        }        
    }
    std::cout << "[DONE]" << std::endl;
}
void packed_psum16_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_psum16_test: " << std::flush;
    std::vector<bool> bits;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 4)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        for(auto x : packed_bits){
            uint64_t mask = UINT64_MAX << 48;
            for(uint64_t j = 0; j < 4; j++){
                uint64_t value = (x & (mask >> (16 * j))) >> (48 - (16*j));
                values.push_back(value);
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), j, stool::PackedPSum::PackedBitType::BIT_16, packed_bits.size());
            uint64_t q = std::accumulate(values.begin(), values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum16 error");
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t x = get_rand_value(mt) % (j+1);
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), x, j, stool::PackedPSum::PackedBitType::BIT_16, packed_bits.size());
            uint64_t q = std::accumulate(values.begin() + x, values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum16 error");
            }
        }        
    }
    std::cout << "[DONE]" << std::endl;
}

void packed_psum32_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "1packed_psum32_test: " << std::flush;
    std::vector<bool> bits;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 2)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        for(auto x : packed_bits){
            uint64_t mask = UINT64_MAX << 32;
            for(uint64_t j = 0; j < 2; j++){
                uint64_t value = (x & (mask >> (32 * j))) >> (32 - (32*j));
                values.push_back(value);
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), j, stool::PackedPSum::PackedBitType::BIT_32, packed_bits.size());
            uint64_t q = std::accumulate(values.begin(), values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum32 error");
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t x = get_rand_value(mt) % (j+1);
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), x, j, stool::PackedPSum::PackedBitType::BIT_32, packed_bits.size());
            uint64_t q = std::accumulate(values.begin() + x, values.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum32 error");
            }
        }        
    }
    std::cout << "[DONE]" << std::endl;
}
void packed_psum64_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "1packed_psum64_test: " << std::flush;
    std::vector<bool> bits;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < seq_size; j++){
            packed_bits.push_back(get_rand_value(mt) % (1ULL << 48) );
        }
        for(uint64_t j = 0; j < seq_size; j++){

            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), j, stool::PackedPSum::PackedBitType::BIT_64, packed_bits.size());
            uint64_t q = std::accumulate(packed_bits.begin(), packed_bits.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(packed_bits, "packed_bits");
                std::cout << j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum64 error");
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){

            uint64_t x = get_rand_value(mt) % (j+1);
            uint64_t p = stool::PackedPSum::psum(packed_bits.data(), x, j, stool::PackedPSum::PackedBitType::BIT_64, packed_bits.size());
            uint64_t q = std::accumulate(packed_bits.begin() + x, packed_bits.begin() + (j+1), 0LL);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(packed_bits, "packed_bits");
                std::cout << x << "/"<< j << "/" << p << "/" << q << std::endl;
                throw std::runtime_error("psum64 error(2)");
            }
        }        
    }
    std::cout << "[DONE]" << std::endl;
}
void packed_search1_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_search1_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 64)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        uint64_t max = 0;
        for(auto x : packed_bits){
            uint64_t mask = UINT64_MAX << 63;
            for(uint64_t j = 0; j < 64; j++){
                uint64_t value = (x & (mask >> j)) >> (63 - j);
                values.push_back(value);
                max += value;
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t random_value = get_rand_value(mt) % max;
            int64_t p = stool::PackedPSum::search(packed_bits.data(), random_value, stool::PackedPSum::PackedBitType::BIT_1, max, packed_bits.size());
            int64_t q = compute_search(values, random_value);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << random_value << "/p: " << p << "/q: " << q << "/max: " << max << "/" << seq_size << std::endl;
                throw std::runtime_error("search1 error");
            }
        }

    }

    std::cout << "[DONE]" << std::endl;
}
void packed_search2_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_search2_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 32)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        uint64_t max = 0;
        for(auto x : packed_bits){
            uint64_t counter = 0;
            uint64_t mask = UINT64_MAX << 62;
            for(uint64_t j = 0; j < 32; j++){
                uint64_t value = (x & (mask >> (2*j))) >> (62 - (2*j));
                values.push_back(value);
                max += value;
                counter++;
                if(counter >= seq_size){
                    break;
                }
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t random_value = get_rand_value(mt) % max;
            int64_t p = stool::PackedPSum::search(packed_bits.data(), random_value, stool::PackedPSum::PackedBitType::BIT_2, max, packed_bits.size());
            int64_t q = compute_search(values, random_value);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << random_value << "/p: " << p << "/q: " << q << "/max: " << max << "/" << seq_size << std::endl;
                throw std::runtime_error("search2 error");
            }
        }

    }

    std::cout << "[DONE]" << std::endl;
}
void packed_search4_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_search4_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 16)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        uint64_t max = 0;
        for(auto x : packed_bits){
            uint64_t counter = 0;
            uint64_t mask = UINT64_MAX << 60;
            for(uint64_t j = 0; j < 16; j++){
                uint64_t value = (x & (mask >> (4*j))) >> (60 - (4*j));
                values.push_back(value);
                max += value;
                counter++;
                if(counter >= seq_size){
                    break;
                }
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t random_value = get_rand_value(mt) % max;
            int64_t p = stool::PackedPSum::search(packed_bits.data(), random_value, stool::PackedPSum::PackedBitType::BIT_4, max, packed_bits.size());
            int64_t q = compute_search(values, random_value);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << random_value << "/p: " << p << "/q: " << q << "/max: " << max << "/" << seq_size << std::endl;
                throw std::runtime_error("search4 error");
            }
        }

    }

    std::cout << "[DONE]" << std::endl;
}
void packed_search8_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_search8_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 8)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        uint64_t max = 0;
        for(auto x : packed_bits){
            uint64_t counter = 0;
            uint64_t mask = UINT64_MAX << 56;
            for(uint64_t j = 0; j < 8; j++){
                uint64_t value = (x & (mask >> (8*j))) >> (56 - (8*j));
                values.push_back(value);
                max += value;
                counter++;
                if(counter >= seq_size){
                    break;
                }
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t random_value = get_rand_value(mt) % max;
            int64_t p = stool::PackedPSum::search(packed_bits.data(), random_value, stool::PackedPSum::PackedBitType::BIT_8, max, packed_bits.size());
            int64_t q = compute_search(values, random_value);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << random_value << "/p: " << p << "/q: " << q << "/max: " << max << "/" << seq_size << std::endl;
                throw std::runtime_error("search8 error");
            }
        }

    }

    std::cout << "[DONE]" << std::endl;
}
void packed_search16_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_search16_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 4)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        uint64_t max = 0;
        for(auto x : packed_bits){
            uint64_t counter = 0;
            uint64_t mask = UINT64_MAX << 48;
            for(uint64_t j = 0; j < 4; j++){
                uint64_t value = (x & (mask >> (16*j))) >> (48 - (16*j));
                values.push_back(value);
                max += value;
                counter++;
                if(counter >= seq_size){
                    break;
                }
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t random_value = get_rand_value(mt) % max;
            int64_t p = stool::PackedPSum::search(packed_bits.data(), random_value, stool::PackedPSum::PackedBitType::BIT_16, max, packed_bits.size());
            int64_t q = compute_search(values, random_value);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << random_value << "/p: " << p << "/q: " << q << "/max: " << max << "/" << seq_size << std::endl;
                throw std::runtime_error("search16 error");
            }
        }

    }

    std::cout << "[DONE]" << std::endl;
}
void packed_search32_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_search32_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::vector<uint64_t> values;
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < ((seq_size / 2)+1); j++){
            packed_bits.push_back(get_rand_value(mt));
        }
        uint64_t max = 0;
        for(auto x : packed_bits){
            uint64_t counter = 0;
            uint64_t mask = UINT64_MAX << 32;
            for(uint64_t j = 0; j < 2; j++){
                uint64_t value = (x & (mask >> (32*j))) >> (32 - (32*j));
                values.push_back(value);
                max += value;
                counter++;
                if(counter >= seq_size){
                    break;
                }
            }
        }
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t random_value = get_rand_value(mt) % max;
            int64_t p = stool::PackedPSum::search(packed_bits.data(), random_value, stool::PackedPSum::PackedBitType::BIT_32, max, packed_bits.size());
            int64_t q = compute_search(values, random_value);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(values, "values");
                std::cout << random_value << "/p: " << p << "/q: " << q << "/max: " << max << "/" << seq_size << std::endl;
                throw std::runtime_error("search32 error");
            }
        }

    }

    std::cout << "[DONE]" << std::endl;
}
void packed_search64_test(uint64_t seq_size, uint64_t max_counter, uint64_t seed){
    std::cout << "packed_search64_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        std::vector<uint64_t> packed_bits;
        std::cout << "+" << std::flush;
        uint64_t max = 0;
        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t value = get_rand_value(mt) % (1ULL << 48);
            packed_bits.push_back(value);
            max += value;
        }

        for(uint64_t j = 0; j < seq_size; j++){
            uint64_t random_value = get_rand_value(mt) % max;
            int64_t p = stool::PackedPSum::search(packed_bits.data(), random_value, stool::PackedPSum::PackedBitType::BIT_64, max, packed_bits.size());
            int64_t q = compute_search(packed_bits, random_value);
            if(p != q){                
                std::cout << std::endl;
                std::cout << "packed_bits: " << stool::Byte::to_bit_string(packed_bits[0]) << std::endl;
                stool::DebugPrinter::print_integers(packed_bits, "packed_bits");
                std::cout << random_value << "/p: " << p << "/q: " << q << "/max: " << max << "/" << seq_size << std::endl;
                throw std::runtime_error("search64 error");
            }
        }

    }

    std::cout << "[DONE]" << std::endl;
}



/*
int64_t compute_rev_select1(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(uint64_t j = 0; j < bv.size(); j++){
        if(bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}
*/


/*
void rank_test(std::vector<uint8_t> &text, stool::WT &wt)
{
    uint64_t CHARMAX = UINT8_MAX + 1;
    std::vector<uint64_t> C_run_sum;
    C_run_sum.resize(CHARMAX, 1);
    uint64_t rle = text.size();
    for (uint64_t i = 0; i < rle; i++)
    {
        uint8_t c = text[i];
        uint64_t rank1 = wt.rank(i+1, c);
        std::cout << i << "/" << (int) c << "/" << C_run_sum[c] << "/" << rank1 << std::endl;
        if (C_run_sum[c] != rank1)
        {
            std::cout << "#" << (int)c << std::endl;
        }
        assert(C_run_sum[c] == rank1);
        C_run_sum[c]++;
    }
}

void c_run_sum_test(){

    uint64_t seed = 0;
    std::vector<uint8_t> alph{ 1, 2, 3, 4, 5, 6, 7, 8};    
    //std::vector<uint8_t> alph = stool::UInt8VectorGenerator::create_ACGT_alphabet();
    std::vector<uint8_t> seq = stool::UInt8VectorGenerator::create_random_sequence(100, alph, seed);
    stool::Printer::print_string(seq);
    sdsl::int_vector<8> text;
    //text.width(8);
    text.resize(seq.size());
    for (uint64_t i = 0; i < seq.size(); i++)
    {
        text[i] = seq[i];
    }
    stool::WT wt;
    construct_im(wt, text);

    rank_test(seq, wt);
}
*/

void bit_select_test(uint64_t seed, uint64_t max_counter){
    std::cout << "bit_select_test: " << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for(uint64_t i = 0; i < max_counter; i++){
        if(i % 100 == 0){
            std::cout << "+" << std::flush;
        }
        uint64_t random_value = get_rand_value(mt);
        uint64_t rank1 = stool::Byte::popcount(random_value);
        uint64_t rank0 = 64 - rank1;

        std::vector<bool> bv;
        for(uint64_t j = 0; j < 64; j++){
            bv.push_back(random_value & (1ULL << (63 - j)));
        }

        std::vector<uint64_t> select1_results;
        std::vector<uint64_t> select0_results;

        std::vector<uint64_t> select1_results_msb_test;
        std::vector<uint64_t> select0_results_msb_test;
        std::vector<uint64_t> select1_results_lsb_test;
        std::vector<uint64_t> select0_results_lsb_test;

        std::vector<uint64_t> rev_select1_results;
        std::vector<uint64_t> rev_select0_results;

        std::vector<uint64_t> rev_select1_results_test;
        std::vector<uint64_t> rev_select0_results_test;


        select1_results_lsb_test.resize(rank1);
        for(uint64_t j = 0; j < rank1; j++){
            select1_results.push_back(compute_select1(bv, j));
            select1_results_msb_test.push_back(stool::MSBByte::select1(random_value, j));
            select1_results_lsb_test[rank1 - 1 - j] = 63 - stool::LSBByte::select1(random_value, j);
        }
        try{
            stool::EqualChecker::equal_check(select1_results, select1_results_msb_test, "select1_resultsA");
        }
        catch(const std::exception& e){
            std::cout << "select1_resultsA: " << e.what() << std::endl;
            std::cout << "select1_results: " << stool::Byte::to_bit_string(random_value) << std::endl;
            std::cout << "select1_results: " << stool::DebugPrinter::to_integer_string(select1_results) << std::endl;
            std::cout << "select1_results_msb_test: " << stool::DebugPrinter::to_integer_string(select1_results_msb_test) << std::endl;

            throw e;
        }
        stool::EqualChecker::equal_check(select1_results, select1_results_lsb_test, "select1_resultsB");

        select0_results_lsb_test.resize(rank0);
        for(uint64_t j = 0; j < rank0; j++){
            select0_results.push_back(compute_select0(bv, j));
            select0_results_msb_test.push_back(stool::MSBByte::select0(random_value, j));
            select0_results_lsb_test[rank0 - 1 - j] = 63 - stool::LSBByte::select0(random_value, j);
        }
        stool::EqualChecker::equal_check(select0_results, select0_results_msb_test, "select0_resultsA");
        stool::EqualChecker::equal_check(select0_results, select0_results_lsb_test, "select0_resultsB");

        for(uint64_t j = 0; j < rank1; j++){
            rev_select1_results.push_back(compute_rev_select1(bv, j));
            int64_t p = stool::LSBByte::select1(random_value, j);
            if(p != -1){
                rev_select1_results_test.push_back(63 - p);
            }else{
                rev_select1_results_test.push_back(-1);
            }
        }
        stool::EqualChecker::equal_check(rev_select1_results, rev_select1_results_test, "rev_select1_results");
        
        for(uint64_t j = 0; j < rank0; j++){
            rev_select0_results.push_back(compute_rev_select0(bv, j));
            int64_t p = stool::LSBByte::select0(random_value, j);
            if(p != -1){
                rev_select0_results_test.push_back(63 - p);
            }else{
                rev_select0_results_test.push_back(-1);
            }
        }
        stool::EqualChecker::equal_check(rev_select0_results, rev_select0_results_test, "rev_select0_results");
        

    }
    std::cout << "[DONE]" << std::endl;
}


int main()
{
    /*
    bit_select_test(0, 10000);
    
    packed_psum1_test(1000, 100, 0);
    packed_psum2_test(1000, 100, 0);
    packed_psum4_test(1000, 100, 0);
    packed_psum8_test(1000, 100, 0);
    packed_psum16_test(1000, 100, 0);
    packed_psum32_test(1000, 100, 0);
    packed_psum64_test(1000, 100, 0);
    */

    packed_search1_test(1000, 100, 0);
    packed_search2_test(1000, 100, 0);
    packed_search4_test(1000, 100, 0);
    packed_search8_test(1000, 100, 0);
    packed_search16_test(1000, 100, 0);
    packed_search32_test(1000, 100, 0);
    packed_search64_test(1000, 100, 0);
    //c_run_sum_test();


    // rankTest();
}
