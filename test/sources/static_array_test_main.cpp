#include <cassert>
#include <chrono>
#include "../../include/light_stool.hpp"


template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void random_shift(stool::StaticArrayDeque<SIZE, USE_PSUM_ARRAY> &bit_deque, uint64_t seed)
{
    /*
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);

    uint64_t shift_len = get_rand_value(mt) % bit_deque.capacity();
    bit_deque.get_bit_array_deque_pointer()->change_starting_position(shift_len);
    */
}

template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void equal_test(const stool::StaticArrayDeque<SIZE, USE_PSUM_ARRAY> &dequeA, const std::vector<uint64_t> &dequeB)
{
    if (dequeA.size() != dequeB.size())
    {
        std::cout << std::endl;

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

    if(USE_PSUM_ARRAY){
        uint64_t sum = 0;
        for (uint64_t i = 0; i < dequeB.size(); i++)
        {
            sum += dequeA[i];
            if (dequeA.psum(i) != sum)
            {
                std::cout << std::endl;
                dequeA.print_info();
                stool::DebugPrinter::print_integers(dequeB, "DequeB");
                throw std::runtime_error("equal_test is incorrect (psum is incorrect)");
            }
        }
    
    }

}
template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void access_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "access_test/" << SIZE << "/" << USE_PSUM_ARRAY << "/" << alphabet_size << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::StaticArrayDeque<SIZE, USE_PSUM_ARRAY> deque(seq);
            // random_shift(deque, seed++);

            equal_test(deque, seq);
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "access_test is done." << std::endl;
}
template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void replace_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{

    std::cout << "replace_test" << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::StaticArrayDeque<SIZE, USE_PSUM_ARRAY> deque(seq);

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
    std::cout << std::endl;
    std::cout << "replace_test is done." << std::endl;
}

template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void push_and_pop_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{
    stool::StaticArrayDeque<SIZE, USE_PSUM_ARRAY> deque;
    std::vector<uint64_t> seq;
    std::cout << "push_and_pop_test/" << SIZE << "/" << USE_PSUM_ARRAY << std::endl;
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
            equal_test(deque, seq);
        }
    }
    std::cout << std::endl;
    std::cout << "push_and_pop_test is done." << std::endl;
}

template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void insert_and_erase_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "insert_and_erase_test/" << SIZE << "/" << alphabet_size << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        int64_t len = 1;
        while (len < (int64_t)max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::StaticArrayDeque<SIZE, USE_PSUM_ARRAY> deque(seq);
            random_shift(deque, seed++);
            assert(seq.size() == deque.size());

            while ((int64_t)seq.size() < len)
            {
                uint64_t new_value = get_rand_value(mt) % alphabet_size;
                uint64_t pos = get_rand_value(mt) % (seq.size() + 1);

                seq.insert(seq.begin() + pos, new_value);
                deque.insert(pos, new_value);

                assert(pos <= deque.size());

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

            while (seq.size() > 0)
            {
                uint64_t pos = get_rand_value(mt) % seq.size();
                seq.erase(seq.begin() + pos);
                deque.erase(pos);

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

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "insert_and_erase_test is done." << std::endl;
}
template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void psum_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "psum_test" << std::endl;

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
            stool::StaticArrayDeque<SIZE, USE_PSUM_ARRAY> deque(seq);

            for (uint64_t j = 0; j < len; j++)
            {
                uint64_t psum1 = compute_psum(seq, j);
                uint64_t psum2 = deque.psum(j);
                if (psum1 != psum2)
                {
                    std::cout << "psum_test error/" << psum1 << "/" << psum2 << std::endl;
                    throw std::runtime_error("psum_test error");
                }
            }

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "psum_test is done." << std::endl;
}

template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void search_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "search_test/" << SIZE << "/" << USE_PSUM_ARRAY << std::endl;
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
            stool::StaticArrayDeque<SIZE, USE_PSUM_ARRAY> deque(seq);

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
    std::cout << std::endl;
    std::cout << "search_test is done." << std::endl;
}


template<uint64_t SIZE, bool USE_PSUM_ARRAY>
void all_test(uint64_t seq_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed){

    //std::mt19937_64 mt64(seed);



    access_test<SIZE, USE_PSUM_ARRAY>(seq_len, alphabet_size, number_of_trials, seed);
    replace_test<SIZE, USE_PSUM_ARRAY>(seq_len, alphabet_size, number_of_trials, seed);
    push_and_pop_test<SIZE, USE_PSUM_ARRAY>(seq_len, alphabet_size, number_of_trials, seed);    
    insert_and_erase_test<SIZE, USE_PSUM_ARRAY>(seq_len, alphabet_size, number_of_trials, seed);
    psum_test<SIZE, USE_PSUM_ARRAY>(seq_len, alphabet_size, number_of_trials, seed);
    search_test<SIZE, USE_PSUM_ARRAY>(seq_len, alphabet_size, number_of_trials, seed);


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

    uint64_t seq_len = 1000;
    uint64_t number_of_trials = 100;
    uint64_t alphabet_size8 = UINT8_MAX;
    uint64_t alphabet_size16 = UINT16_MAX;
    uint64_t alphabet_size32 = UINT32_MAX;
    uint64_t alphabet_size64 = (uint64_t)UINT32_MAX * 10000;

    
    /*
    all_test<256, true>(256, alphabet_size8, number_of_trials, seed);

    all_test<1024, false>(seq_len, alphabet_size8, number_of_trials, seed);
    all_test<1024, true>(seq_len, alphabet_size8, number_of_trials, seed);
    all_test<2048, false>(seq_len, alphabet_size16, number_of_trials, seed);
    all_test<2048, true>(seq_len, alphabet_size16, number_of_trials, seed);
    

    
    all_test<4096, false>(seq_len, alphabet_size32, number_of_trials, seed);
    */
    all_test<4096, true>(seq_len, alphabet_size32, number_of_trials, seed);
    all_test<8192, false>(seq_len, alphabet_size64, number_of_trials, seed);
    all_test<8192, true>(seq_len, alphabet_size64, number_of_trials, seed);
    
    


}
