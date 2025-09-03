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


    stool::DynamicIntegerTest::build_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed);
    stool::DynamicIntegerTest::psum_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed);
    stool::DynamicIntegerTest::reverse_psum_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed);
    stool::DynamicIntegerTest::search_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed);
    stool::DynamicIntegerTest::load_write_file_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed, false);
    stool::DynamicIntegerTest::load_write_bits_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed, false);


    stool::DynamicIntegerTest::push_and_pop_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed, false);    
    stool::DynamicIntegerTest::insert_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed, false);
    stool::DynamicIntegerTest::remove_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed, false);
    stool::DynamicIntegerTest::replace_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, seed, false);
    stool::DynamicIntegerTest::random_test<stool::NaiveFLCVector<>>(seq_len, max_value, number_of_trials, 100, seed, false);

  

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
