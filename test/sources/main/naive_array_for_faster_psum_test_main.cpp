#include <cassert>
#include <chrono>
#include "../../../include/light_stool.hpp"
#include "../template/dynamic_integer_test.hpp"




template<uint64_t SIZE>
void all_test(uint64_t seq_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE){
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "TEST: len = " << seq_len <<  ", SIZE =" << SIZE  <<  ", alphabet_size = " << alphabet_size << ", number_of_trials = " << number_of_trials << ", seed = " << seed << std::endl;

    stool::DynamicIntegerTest<stool::NaiveArrayForFasterPsum<SIZE>, true, true> test;

    test.build_test(seq_len, alphabet_size, number_of_trials, seed);
    test.psum_test(seq_len, alphabet_size, number_of_trials, seed);
    test.search_test(seq_len, alphabet_size, number_of_trials, seed);
    test.push_back_test(seq_len, alphabet_size, number_of_trials, false, seed);    
    test.pop_back_test(seq_len, alphabet_size, number_of_trials, false, seed);

    test.insert_test(seq_len, alphabet_size, number_of_trials, false, seed);
    test.remove_test(seq_len, alphabet_size, number_of_trials, false, seed);
    test.replace_test(seq_len, alphabet_size, number_of_trials, false, seed);
    test.template random_test<false>(seq_len, alphabet_size, number_of_trials, 100, false, seed);

    /*
    stool::DynamicIntegerTest::build_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed);
    stool::DynamicIntegerTest::psum_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed);
    stool::DynamicIntegerTest::search_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed);

    stool::DynamicIntegerTest::push_and_pop_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);    
    stool::DynamicIntegerTest::insert_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);
    stool::DynamicIntegerTest::remove_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);
    stool::DynamicIntegerTest::replace_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);
    stool::DynamicIntegerTest::random_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);
    */



    /*
    access_test<SIZE>(seq_len, alphabet_size, number_of_trials, seed, message_paragraph+1);
    replace_test<SIZE>(seq_len, alphabet_size, number_of_trials, seed, message_paragraph+1);
    push_and_pop_test<SIZE>(seq_len, alphabet_size, number_of_trials, seed, false, message_paragraph+1);    
    insert_and_erase_test<SIZE>(seq_len, alphabet_size, number_of_trials, seed, true,message_paragraph+1);
    psum_test<SIZE>(seq_len, alphabet_size, number_of_trials, seed, message_paragraph+1);
    search_test<SIZE>(seq_len, alphabet_size, number_of_trials, seed, message_paragraph+1);
    random_test<SIZE>(seq_len, alphabet_size, number_of_trials, seed, false, message_paragraph+1);
    */

}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{

    cmdline::parser p;

    p.add<uint>("mode", 'm', "mode", false, 0);
    p.add<uint>("seed", 's', "seed", false, 0);

    p.parse_check(argc, argv);
    // uint64_t mode = p.get<uint>("mode");
    uint64_t seed = p.get<uint>("seed");

    std::mt19937_64 mt64(seed);

    //uint64_t seq_len = 1000;
    uint64_t number_of_trials = 100;
    //uint64_t alphabet_size8 = UINT8_MAX;
    //uint64_t alphabet_size16 = UINT16_MAX;
    //uint64_t alphabet_size32 = UINT32_MAX;

    uint64_t alphabet_size64 = (uint64_t)UINT32_MAX * 10000;

    
    

    /*
    all_test<256>(256, alphabet_size8, number_of_trials, seed, stool::Message::SHOW_MESSAGE);
    all_test<1024>(1000, alphabet_size8, number_of_trials, seed, stool::Message::SHOW_MESSAGE);
    all_test<2048>(2000, alphabet_size16, number_of_trials, seed, stool::Message::SHOW_MESSAGE);    
    all_test<4096>(4000, alphabet_size32, number_of_trials, seed, stool::Message::SHOW_MESSAGE);
    */

    all_test<8192>(8000, alphabet_size64, number_of_trials, seed, stool::Message::SHOW_MESSAGE);
    
    


}
