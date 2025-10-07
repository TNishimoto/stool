#include <cassert>
#include <chrono>
#include "../../../include/light_stool.hpp"
#include "../template/dynamic_integer_test.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{

    cmdline::parser p;

    p.add<uint>("mode", 'm', "mode", false, 0);
    p.add<uint>("seed", 's', "seed", false, 0);

    p.parse_check(argc, argv);
    // uint64_t mode = p.get<uint>("mode");
    uint64_t seed = p.get<uint>("seed");

    std::mt19937_64 mt64(seed);

    uint64_t seq_len = 1000;
    uint64_t number_of_trials = 100;
    uint64_t max_value = 1000000;

    stool::DynamicIntegerTest<stool::NaiveFLCVector<>, true, true> test;

    test.build_test(seq_len, max_value, number_of_trials, seed);
    test.psum_test(seq_len, max_value, number_of_trials, seed);
    test.reverse_psum_test(seq_len, max_value, number_of_trials, seed);
    test.search_test(seq_len, max_value, number_of_trials, seed);
    test.load_and_save_file_test(seq_len, max_value, number_of_trials, false, seed);
    test.load_and_save_bytes_test(seq_len, max_value, number_of_trials, false, seed);

    test.push_back_test(seq_len, max_value, number_of_trials, false, seed);
    test.pop_back_test(seq_len, max_value, number_of_trials, false, seed);
    test.insert_test(seq_len, max_value, number_of_trials, false, seed);
    test.remove_test(seq_len, max_value, number_of_trials, false, seed);
    test.replace_test(seq_len, max_value, number_of_trials, false, seed);
    test.random_test<true>(seq_len, max_value, number_of_trials, 100, false, seed);

    //stool::DynamicIntegerTest::random_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, 100, seed, false);

  

    //access_test(seq_len, max_value, number_of_trials, seed);
    //psum_test(seq_len, max_value, number_of_trials, seed);
    //search_test(seq_len, max_value, number_of_trials, seed);
    //insert_and_erase_test(seq_len, max_value, number_of_trials, seed, false);
    //push_and_pop_test(seq_len, max_value, number_of_trials, seed, false);
    //replace_test(seq_len, max_value, number_of_trials, seed, false);
    //load_write_file_test(seq_len, number_of_trials, false, seed);
    //load_write_bits_test(seq_len, number_of_trials, false, seed);
    //random_test(seq_len, max_value, number_of_trials, 100, seed, false);
}
