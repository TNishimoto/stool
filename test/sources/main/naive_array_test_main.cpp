#include <cassert>
#include <chrono>
#include "../../../include/light_stool.hpp"
#include "../template/dynamic_integer_test.hpp"



template<uint64_t SIZE>
void equal_test(const stool::NaiveArray<SIZE> &dequeA, const std::vector<uint64_t> &dequeB)
{
    if (dequeA.size() != dequeB.size())
    {
        std::cout << std::endl;
        std::cout << "dequeA.size() = " << dequeA.size() << std::endl;
        std::cout << "dequeB.size() = " << dequeB.size() << std::endl;

        throw std::runtime_error("equal_test is incorrect (size is different)");
    }

    for (uint64_t i = 0; i < dequeB.size(); i++)
    {

        if (dequeA[i] != dequeB[i])
        {
            std::cout << std::endl;
            dequeA.print_info();
            stool::DebugPrinter::print_integers(dequeB, "DequeB");
            throw std::runtime_error("equal_test is incorrect");
        }
    }


}
template<uint64_t SIZE>
void access_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
{
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "ACCESS_TEST: \t" << std::flush;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::NaiveArray<SIZE> deque(seq);
            // random_shift(deque, seed++);

            equal_test(deque, seq);
            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}
template<uint64_t SIZE>
void replace_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
{
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "REPLACE_TEST: \t" << std::flush;

    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::NaiveArray<SIZE> deque(seq);

            // random_shift(deque, seed++);

            for (uint64_t j = 0; j < len; j++)
            {
                uint64_t new_value = get_rand_value(mt) % alphabet_size;
                if(new_value < deque.value_capacity()){
                    seq[j] = new_value;
                    deque.set_value(j, new_value);
                }
            }

            equal_test(deque, seq);
            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

template<uint64_t SIZE>
void push_and_pop_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, bool detail_check = false, int message_paragraph = stool::Message::SHOW_MESSAGE)
{
    stool::NaiveArray<SIZE> deque;
    std::vector<uint64_t> seq;
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "PUSH_AND_POP_TEST: \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);
    for (uint64_t i = 0; i < number_of_trials; i++)
    {

        deque.clear();
        seq.clear();

        std::cout << "+" << std::flush;
        while (seq.size() < max_len)
        {

            uint64_t b = get_rand_value(mt) % alphabet_size;
            uint64_t type = get_rand_value(mt) % 5;

            if (type == 0 || type == 1)
            {
                if(seq.size() < deque.max_size()){
                    deque.push_back(b);
                    seq.push_back(b);    
                }
            }
            else if (type == 2 || type == 3)
            {
                if(seq.size() < deque.max_size()){

                    deque.push_front(b);
                    seq.insert(seq.begin(), b);    
                }

            }
            else if (type == 4 && seq.size() > 0)
            {
                deque.pop_back();
                seq.pop_back();
            }
            else if (type == 5 && deque.size() > 0)
            {
                deque.pop_front();
                seq.erase(seq.begin());
            }
            if(detail_check){
                equal_test(deque, seq);
            }
        }
        equal_test(deque, seq);

    }
    std::cout << "[DONE]" << std::endl;
}

template<uint64_t SIZE>
void insert_and_erase_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, bool detail_check = false, int message_paragraph = stool::Message::SHOW_MESSAGE)
{
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "INSERT_AND_ERASE_TEST: \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        int64_t len = 1;
        while (len < (int64_t)max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::NaiveArray<SIZE> deque(seq);
            assert(seq.size() == deque.size());

            while ((int64_t)seq.size() < len)
            {
                uint64_t new_value = get_rand_value(mt) % alphabet_size;
                uint64_t pos = get_rand_value(mt) % (seq.size() + 1);

                seq.insert(seq.begin() + pos, new_value);
                deque.insert(pos, new_value);

                assert(pos <= deque.size());

                if(detail_check){
                    try
                    {
                        equal_test(deque, seq);
                    }
                    catch (const std::runtime_error &e)
                    {
                        std::cout << "Insert test error/" << pos << "/" << new_value << std::endl;
                        throw e;
                    }    
                }

            }

            try
            {
                equal_test(deque, seq);
            }
            catch (const std::runtime_error &e)
            {
                std::cout << "Insert test error/"  << std::endl;
                throw e;
            }    


            while (seq.size() > 0)
            {
                uint64_t pos = get_rand_value(mt) % seq.size();


                seq.erase(seq.begin() + pos);
                deque.erase(pos);

                if(detail_check){
                    try
                    {
                        equal_test(deque, seq);
                    }
                    catch (const std::runtime_error &e)
                    {
                        std::cout << "Erase test error" << std::endl;
                        throw e;
                    }    
                }

            }
            try
            {
                equal_test(deque, seq);
            }
            catch (const std::runtime_error &e)
            {
                std::cout << "Erase test error" << std::endl;
                throw e;
            }    

            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}
template<uint64_t SIZE>
void psum_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
{
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "PSUM_TEST: \t" << std::flush;

    auto compute_psum = [](const std::vector<uint64_t> &seq, uint64_t i) -> uint64_t
    {
        uint64_t sum = 0;
        for (uint64_t x = 0; x <= i; x++)
        {
            sum += seq[x];
        }
        return sum;
    };

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::NaiveArray<SIZE> deque(seq);

            for (uint64_t j = 0; j < len; j++)
            {
                uint64_t psum1 = compute_psum(seq, j);
                uint64_t psum2 = deque.psum(j);
                if (psum1 != psum2)
                {
                    std::cout << "psum_test error/" << psum1 << "/" << psum2 << std::endl;

                    deque.print_info();
                    throw std::runtime_error("psum_test error");
                }
            }

            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

template<uint64_t SIZE>
void search_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE)
{
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "SEARCH_TEST: \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    auto compute_search = [](const std::vector<uint64_t> &seq, uint64_t value) -> int64_t
    {
        uint64_t sum = 0;
        for (uint64_t x = 0; x < seq.size(); x++)
        {
            sum += seq[x];
            if (sum >= value)
            {
                return x;
            }
        }
        return -1;
    };

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::NaiveArray<SIZE> deque(seq);

            for (uint64_t j = 0; j < len; j++)
            {
                uint64_t value = get_rand_value(mt) % alphabet_size;
                uint64_t psum1 = compute_search(seq, value);
                uint64_t psum2 = deque.search(value);
                if (psum1 != psum2)
                {
                    std::cout << "search_test error" << std::endl;
                    throw std::runtime_error("search_test error");
                }
            }

            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

template<uint64_t SIZE>
void random_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, bool detail_check = false, int message_paragraph = stool::Message::SHOW_MESSAGE)
{
    stool::NaiveArray<SIZE> deque;
    std::vector<uint64_t> seq;
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "RANDOM_TEST: \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);

    auto compute_psum = [](const std::vector<uint64_t> &seq, uint64_t i) -> uint64_t
    {
        uint64_t sum = 0;
        for (uint64_t x = 0; x <= i; x++)
        {
            sum += seq[x];
        }
        return sum;
    };


    auto compute_search = [](const std::vector<uint64_t> &seq, uint64_t value) -> int64_t
    {
        uint64_t sum = 0;
        for (uint64_t x = 0; x < seq.size(); x++)
        {
            sum += seq[x];
            if (sum >= value)
            {
                return x;
            }
        }
        return -1;
    };

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        uint64_t counter = 0;
        std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(max_len/2, alphabet_size, seed++);
        stool::NaiveArray<SIZE> deque(seq);

        std::cout << "+" << std::flush;

        while (counter < 10000)
        {

            uint64_t type = get_rand_value(mt) % 10;
            uint64_t random_pos = get_rand_value(mt) % seq.size();
            uint64_t random_value = get_rand_value(mt) % alphabet_size;

            if (type == 0 || type == 1)
            {
                if(seq.size() < deque.max_size()){
                    deque.push_back(random_value);
                    seq.push_back(random_value);    
                }
            }
            else if (type == 2 || type == 3)
            {
                if(seq.size() < deque.max_size()){

                    deque.push_front(random_value);
                    seq.insert(seq.begin(), random_value);    
                }

            }
            else if (type == 4 && seq.size() > 0)
            {
                deque.pop_back();
                seq.pop_back();
            }
            else if (type == 5 && deque.size() > 0)
            {
                deque.pop_front();
                seq.erase(seq.begin());
            }
            else if (type == 6 && seq.size() < max_len)
            {
                seq.insert(seq.begin() + random_pos, random_value);
                deque.insert(random_pos, random_value);
            }
            else if (type == 7 && seq.size() > 0)
            {
                seq.erase(seq.begin() + random_pos);
                deque.erase(random_pos);
            }
            else if (type == 8){
                seq[random_pos] = random_value;
                deque.set_value(random_pos, random_value);
            }else{

                uint64_t psum1 = compute_psum(seq, random_pos);
                uint64_t psum2 = deque.psum(random_pos);
                if (psum1 != psum2)
                {
                    std::cout << "psum_test error/" << psum1 << "/" << psum2 << std::endl;
                    throw std::runtime_error("psum_test error");
                }

                uint64_t search1 = compute_search(seq, random_value);
                uint64_t search2 = deque.search(random_value);
                if (search1 != search2)
                {
                    std::cout << "search_test error/" << search1 << "/" << search2 << std::endl;
                    throw std::runtime_error("search_test error");
                }
            }
            counter++;

            if(detail_check){
                equal_test(deque, seq);
            }
        }
        equal_test(deque, seq);

    }
    std::cout << "[DONE]" << std::endl;
}


template<uint64_t SIZE>
void all_test(uint64_t seq_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed, int message_paragraph = stool::Message::SHOW_MESSAGE){
    std::cout << stool::Message::get_paragraph_string(message_paragraph) << "TEST: len = " << seq_len <<  ", SIZE =" << SIZE  <<  ", alphabet_size = " << alphabet_size << ", number_of_trials = " << number_of_trials << ", seed = " << seed << std::endl;

    stool::DynamicIntegerTest::build_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed);
    stool::DynamicIntegerTest::psum_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed);
    stool::DynamicIntegerTest::search_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed);

    stool::DynamicIntegerTest::push_and_pop_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);    
    stool::DynamicIntegerTest::insert_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);
    stool::DynamicIntegerTest::remove_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);
    stool::DynamicIntegerTest::replace_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);
    stool::DynamicIntegerTest::random_test<stool::NaiveArray<SIZE>>(seq_len, alphabet_size, number_of_trials, seed, false);



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
