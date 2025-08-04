#include <cassert>
#include <chrono>
#include "../../include/light_stool.hpp"

std::string to_string(const std::vector<bool> &bv, bool use_partition = false){
    std::string s;
    for(uint64_t i = 0; i < bv.size(); i++){
        s += bv[i] ? "1" : "0";
        if(use_partition && i % 64 == 63){
            s += " ";
        }
    }
    return s;
}

void random_bit_string256(int64_t bit_length, std::vector<uint64_t> &new_pattern, std::bitset<256> &bs, uint64_t seed){
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    bs.reset();
    uint64_t i = 0;

    while(bit_length > 0){
        new_pattern.push_back(get_rand_value(mt));
        uint64_t len = std::min((uint64_t)bit_length, 64ULL);
        for(uint64_t j = 0; j < len; j++){
            bs[i] = (new_pattern[i] >> (63 - j)) & 1;
            i++;
            bit_length--;
        }
    }

    
}


template<typename T>
void random_shift(stool::IntegerArrayDeque<T> &bit_deque, uint64_t seed){
    /*
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);

    uint64_t shift_len = get_rand_value(mt) % bit_deque.capacity();
    bit_deque.get_bit_array_deque_pointer()->change_starting_position(shift_len);
    */
}


template<typename T>
void equal_test(const stool::IntegerArrayDeque<T> &dequeA, const std::vector<T> &dequeB)
{
    if(dequeA.size() != dequeB.size()){
        std::cout << std::endl;

        throw std::runtime_error("equal_test is incorrect (size is different)");
    }

    for (uint64_t i = 0; i < dequeB.size(); i++)
    {

        if (dequeA[i] != dequeB[i])
        {
            std::cout << std::endl;
            std::cout << "dequeA = " << dequeA.to_string() << std::endl;
            std::cout << "dequeB = " << stool::DebugPrinter::to_integer_string(dequeB) << std::endl;
            throw std::runtime_error("equal_test is incorrect");
        }
    }
}
//template<typename T>
void access_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "access_test"  << std::to_string(sizeof(T))  << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> seq = stool::StringGenerator::create_random_sequence<uint64_t>(len, alphabet_size, seed++);
            stool::IntegerArrayDeque<uint64_t> deque(seq);

            //random_shift(deque, seed++);

            equal_test(deque, seq);
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "access_test is done." << std::endl;
}

template<typename T>
void push_and_pop_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{
    stool::IntegerArrayDeque<T> deque;
    std::vector<T> seq;
    std::cout << "push_and_pop_test" << std::to_string(sizeof(T)) << std::endl;
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

            if(type == 0 || type == 1){
                deque.push_back(b);
                seq.push_back(b);    
            }else if(type == 2 || type == 3){
                deque.push_front(b);
                seq.insert(seq.begin(), b);
            }else if(type == 4 && seq.size() > 0){
                deque.pop_back();
                seq.pop_back();
            }else if(type == 5 && deque.size() > 0){
                deque.pop_front();
                seq.erase(seq.begin());
            }

        }
        equal_test(deque, seq);

    }
    std::cout << std::endl;
    std::cout << "push_and_pop_test is done." << std::endl;
}

template<typename T>
void replace_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{

    std::cout << "replace_test" << std::to_string(sizeof(T)) << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<T> seq = stool::StringGenerator::create_random_sequence<T>(len, alphabet_size, seed++);
            stool::IntegerArrayDeque<T> deque(seq);

            //random_shift(deque, seed++);

            for(uint64_t j = 0; j < len; j++){
                uint64_t new_value = get_rand_value(mt) % alphabet_size;
                seq[j] = new_value;
                deque.set_value(j, new_value);
            }

            equal_test(deque, seq);
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "replace_test is done." << std::endl;
}

template<typename T>
void insert_and_erase_test(uint64_t max_len, uint64_t alphabet_size, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "insert_and_erase_test"  << std::to_string(sizeof(T))<< std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        int64_t len = 1;
        while (len < (int64_t)max_len)
        {
            std::vector<T> seq = stool::StringGenerator::create_random_sequence<T>(len, alphabet_size, seed++);
            stool::IntegerArrayDeque<T> deque(seq);
            random_shift(deque, seed++);
            assert(seq.size() == deque.size());

            while((int64_t)seq.size() < len * 2){
                uint64_t new_value = get_rand_value(mt) % alphabet_size;
                uint64_t pos = get_rand_value(mt) % (seq.size() + 1);

                seq.insert(seq.begin() + pos, new_value);
                deque.insert(pos, new_value);

                assert(pos <= deque.size());



            }
            equal_test(deque, seq);

            
            while(seq.size() > 0){
                uint64_t pos = get_rand_value(mt) % seq.size();
                seq.erase(seq.begin() + pos);
                deque.erase(pos);

                try{
                    equal_test(deque, seq);
                }
                catch(const std::runtime_error& e){
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



int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{

    cmdline::parser p;

    p.add<uint>("mode", 'm', "mode", false, 0);
    p.add<uint>("seed", 's', "seed", false, 0);

    p.parse_check(argc, argv);
    //uint64_t mode = p.get<uint>("mode");
    uint64_t seed = p.get<uint>("seed");

    std::mt19937_64 mt64(seed);

    uint64_t seq_len = 1000;
    uint64_t number_of_trials = 100;
    uint64_t alphabet_size8 = UINT8_MAX;
    uint64_t alphabet_size16 = UINT16_MAX;
    uint64_t alphabet_size32 = UINT32_MAX;
    uint64_t alphabet_size64 = UINT64_MAX;

    access_test<uint8_t>(seq_len, alphabet_size8, number_of_trials, seed);
    access_test<uint16_t>(seq_len, alphabet_size16, number_of_trials, seed);
    access_test<uint32_t>(seq_len, alphabet_size32, number_of_trials, seed);
    access_test<uint64_t>(seq_len, alphabet_size64, number_of_trials, seed);

    push_and_pop_test<uint8_t>(seq_len, alphabet_size8, number_of_trials, seed);
    push_and_pop_test<uint16_t>(seq_len, alphabet_size16, number_of_trials, seed);
    push_and_pop_test<uint32_t>(seq_len, alphabet_size32, number_of_trials, seed);
    push_and_pop_test<uint64_t>(seq_len, alphabet_size64, number_of_trials, seed);

    replace_test<uint8_t>(seq_len, alphabet_size8, number_of_trials, seed);    
    replace_test<uint16_t>(seq_len, alphabet_size16, number_of_trials, seed);    
    replace_test<uint32_t>(seq_len, alphabet_size32, number_of_trials, seed);
    replace_test<uint64_t>(seq_len, alphabet_size64, number_of_trials, seed);    

    insert_and_erase_test<uint8_t>(seq_len, alphabet_size8, number_of_trials, seed);
    insert_and_erase_test<uint16_t>(seq_len, alphabet_size16, number_of_trials, seed);
    insert_and_erase_test<uint32_t>(seq_len, alphabet_size32, number_of_trials, seed);
    insert_and_erase_test<uint64_t>(seq_len, alphabet_size64, number_of_trials, seed);
    
}
