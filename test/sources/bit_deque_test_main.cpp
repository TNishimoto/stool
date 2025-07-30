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

uint64_t rank1(const std::vector<bool> &bv, uint64_t i)
{
    uint64_t rank = 0;
    for (uint64_t j = 0; j <= i; j++)
    {
        if (bv[j])
        {
            rank++;
        }
    }
    return rank;
}

uint64_t select1(const std::vector<bool> &bv, uint64_t x)
{
    uint64_t count = 0;
    for (uint64_t j = 0; j < bv.size(); j++)
    {
        if (bv[j])
        {
            count++;
        }
        if (count == x)
        {
            return j;
        }
    }
    throw std::runtime_error("select1 is incorrect");
}

void random_shift(stool::BitArrayDeque &bit_deque, uint64_t seed){
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);

    uint64_t shift_len = get_rand_value(mt) % bit_deque.capacity();
    bit_deque.change_starting_position(shift_len);
}


void equal_test(const stool::BitArrayDeque &bv, const std::vector<bool> &naive_bv)
{
    if(naive_bv.size() != bv.size()){
        std::cout << std::endl;
        std::cout << "[naive_bv.size() / bv.size()] = " << naive_bv.size() << " != " << bv.size() << std::endl;
        std::cout << "naive_bv = " << to_string(naive_bv, true) << std::endl;
        std::cout << "      bv = " << bv.to_string(true) << std::endl;

        bv.print_info();

        throw std::runtime_error("equal_test is incorrect (size is different)");
    }

    for (uint64_t i = 0; i < naive_bv.size(); i++)
    {

        if (bv[i] != naive_bv[i])
        {
            std::cout << std::endl;

            std::cout << "naive_bv = " << to_string(naive_bv, true) << std::endl;
            std::cout << "      bv = " << bv.to_string(true) << std::endl;
    
            std::cout << "bv.access(" << i << ") = " << bv[i] << " != " << naive_bv[i] << std::endl;

            //bv.print_info();

            throw std::runtime_error("equal_test is incorrect");
        }
    }
}
void access_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "access_test" << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::BitArrayDeque bit_deque(bv);

            random_shift(bit_deque, seed++);

            equal_test(bit_deque, bv);
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "access_test is done." << std::endl;
}
void select_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "select_test" << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> selecter_array;

            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            for (uint64_t j = 0; j < len; j++)
            {
                if (bv[j])
                {
                    selecter_array.push_back(j);
                }
            }
            stool::BitArrayDeque bit_deque(bv);

            for (uint64_t j = 0; j < selecter_array.size(); j++)
            {
                if(j % 10 == 0){
                    random_shift(bit_deque, seed++);
                }
                int64_t bit_deque_select = bit_deque.select1(j);

                if (bit_deque_select != (int64_t)selecter_array[j])
                {
                    std::cout << "select1(" << j << ") = " << bit_deque_select << " != " << selecter_array[j] << std::endl;
                    std::cout << "naive_bv = " << to_string(bv) << std::endl;
                    bit_deque.print_info();
        
                    auto pair = bit_deque.get_block_index_and_bit_index(i);
                    std::cout << "block_index = " << pair.first << ", bit_index = " << (int)pair.second << std::endl;

                    throw std::runtime_error("select1 is incorrect");
                }
            }
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "select_test is done." << std::endl;
}
void rank_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "rank_test" << std::endl;

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::BitArrayDeque bit_deque(bv);

            for (uint64_t j = 0; j < len; j++)
            {
                if(i % 10 == 0){
                    random_shift(bit_deque, seed++);
                }
                
                if (bit_deque.rank1(j) != rank1(bv, j))
                {
                    std::cout << "rank1(" << j << ") = " << bit_deque.rank1(j) << " != " << rank1(bv, j) << std::endl;

                    std::cout << "naive_bv = " << to_string(bv) << std::endl;
                    bit_deque.print_info();
        
                    auto pair = bit_deque.get_block_index_and_bit_index(i);
                    std::cout << "block_index = " << pair.first << ", bit_index = " << (int)pair.second << std::endl;

                    throw std::runtime_error("rank1 is incorrect");
                }
            }
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "rank_test is done." << std::endl;
}

void push_and_pop_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    stool::BitArrayDeque bit_deque;
    std::vector<bool> naive_bv;
    std::cout << "push_and_pop_test" << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);
    for (uint64_t i = 0; i < number_of_trials; i++)
    {

        
        bit_deque.clear();
        naive_bv.clear();

        std::cout << "+" << std::flush;
        while (naive_bv.size() < max_len)
        {
        
            bool b = get_rand_value(mt) % 2 == 1;
            uint64_t type = get_rand_value(mt) % 5;

            if(type == 0 || type == 1){
                bit_deque.push_back(b);
                naive_bv.push_back(b);    
            }else if(type == 2 || type == 3){
                bit_deque.push_front(b);
                naive_bv.insert(naive_bv.begin(), b);
            }else if(type == 4 && naive_bv.size() > 0){
                bit_deque.pop_back();
                naive_bv.pop_back();
            }else if(type == 5 && bit_deque.size() > 0){
                bit_deque.pop_front();
                naive_bv.erase(naive_bv.begin());
            }

        }
        equal_test(bit_deque, naive_bv);

    }
    std::cout << std::endl;
    std::cout << "push_and_pop_test is done." << std::endl;
}

void push64_and_pop64_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    stool::BitArrayDeque bit_deque;
    std::vector<bool> naive_bv;
    std::cout << "push64_and_pop64_test" << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    for (uint64_t i = 0; i < number_of_trials; i++)
    {

        
        bit_deque.clear();
        naive_bv.clear();

        std::cout << "+" << std::flush;
        while (naive_bv.size() < max_len)
        {
            uint64_t new_value = get_rand_value(mt);
            int64_t len = get_rand_value(mt) % 64;
            uint64_t type = get_rand_value(mt) % 5;
            std::bitset<64> bs(new_value);
            std::string s = bs.to_string();

            if(type == 0 || type == 1){
                bit_deque.push_back64(new_value, len);

                for(int64_t j = 0; j < len; j++){
                    naive_bv.push_back(s[j] == '1');
                }
            }else if(type == 2 || type == 3){
                bit_deque.push_front64(new_value, len);
                for(int64_t j = len-1; j >= 0; j--){
                    naive_bv.insert(naive_bv.begin(), s[j] == '1');
                }
            }else if(type == 4 && (int64_t)naive_bv.size() >= len){

                bit_deque.pop_back(len);
                for(int64_t j = 0; j < len; j++){
                    naive_bv.pop_back();
                }
            }else if(type == 5 && (int64_t)bit_deque.size() >= len){
                bit_deque.pop_front(len);
                for(int64_t j = 0; j < len; j++){
                    naive_bv.erase(naive_bv.begin());
                }
            }
            equal_test(bit_deque, naive_bv);

        }

    }
    std::cout << std::endl;
    std::cout << "push64_and_pop64_test is done." << std::endl;
}
void replace_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
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
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::BitArrayDeque bit_deque(bv);

            random_shift(bit_deque, seed++);

            for(uint64_t j = 0; j < len; j++){
                uint64_t new_value = get_rand_value(mt);
                int64_t plen = (get_rand_value(mt) % (std::min((int64_t)64, (int64_t)len - (int64_t)j))) + 1;
                std::bitset<64> bs(new_value);
                std::string s = bs.to_string();
                while((int64_t)s.size() > plen){
                    s.pop_back();
                }

                for(int64_t k = 0; k < plen; k++){
                    bv[j + k] = s[k] == '1';
                }
                
                bit_deque.replace_64bit_string(j, new_value, plen);
            }

            equal_test(bit_deque, bv);
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "replace_test is done." << std::endl;
}

void insert_and_erase_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "insert_and_erase_test" << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        int64_t len = 1;
        while (len < (int64_t)max_len)
        {
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::BitArrayDeque bit_deque(bv);
            random_shift(bit_deque, seed++);
            assert(bv.size() == bit_deque.size());

            while((int64_t)bv.size() < len){
                uint64_t new_value = get_rand_value(mt);
                int64_t plen = (get_rand_value(mt) % 64) + 1;
                uint64_t pos = get_rand_value(mt) % (bv.size() + 1);

                std::string bv_str = to_string(bv);
                std::bitset<64> bs(new_value);

                std::string s = bs.to_string();
                while((int64_t)s.size() > plen){
                    s.pop_back();
                }

                for(int64_t k = plen-1; k >= 0; k--){
                    bv.insert(bv.begin() + pos, s[k] == '1');
                }


                assert(pos <= bit_deque.size());
                bit_deque.insert_64bit_string(pos, new_value, plen);



            }
            equal_test(bit_deque, bv);

            
            while(bv.size() > 0){
                uint64_t pos = get_rand_value(mt) % bv.size();
                bv.erase(bv.begin() + pos);
                bit_deque.erase(pos);

                try{
                    equal_test(bit_deque, bv);
                }
                catch(const std::runtime_error& e){
                    std::cout << "Erase test error" << std::endl;
                    std::cout << "len = " << len << std::endl;
                    std::cout << "pos = " << pos << std::endl;
                    std::cout << "bv size = " << bv.size() << std::endl;
                    throw e;
                }
            }
            
            

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "insert_and_erase_test is done." << std::endl;
}

void insert64_and_erase64_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "insert64_and_erase64_test" << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        int64_t len = 1;
        while (len < (int64_t)max_len)
        {
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::BitArrayDeque bit_deque(bv);
            random_shift(bit_deque, seed++);
            assert(bv.size() == bit_deque.size());

            while((int64_t)bv.size() < len){

                uint64_t new_pattern_size = get_rand_value(mt) % 257;
                std::vector<uint64_t> new_pattern;
                std::bitset<256> new_pattern_bs;

                random_bit_string256(new_pattern_size, new_pattern, new_pattern_bs, seed++);

                uint64_t pos = get_rand_value(mt) % (bv.size() + 1);

                for(int64_t k = new_pattern_size-1; k >= 0; k--){
                    bv.insert(bv.begin() + pos, new_pattern_bs[k]);
                }

                assert(pos <= bit_deque.size());
                bit_deque.insert_64bit_string(pos, new_pattern, new_pattern_size);
            }

            try{
                equal_test(bit_deque, bv);
            }
            catch(const std::runtime_error& e){
                std::cout << "Erase test error" << std::endl;
                std::cout << "len = " << len << std::endl;
                //std::cout << "pos = " << pos << std::endl;
                std::cout << "bv size = " << bv.size() << std::endl;
                throw e;
            }

            while(bv.size() > 0){
                int64_t pos = get_rand_value(mt) % bv.size();
                int64_t new_pattern_size = get_rand_value(mt) % 257;
                if(pos + new_pattern_size > (int64_t)bv.size()){
                    new_pattern_size = bv.size() - pos;
                }

                for(int64_t j = 0; j < new_pattern_size; j++){
                    bv.erase(bv.begin() + pos);
                }

                bit_deque.erase(pos, new_pattern_size);

            }
            
            

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "insert64_and_erase64_test is done." << std::endl;
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
    
    
    access_test(seq_len, number_of_trials, seed);
    rank_test(seq_len, number_of_trials, seed);
    select_test(seq_len, number_of_trials, seed);
    push_and_pop_test(seq_len, number_of_trials, seed);
    push64_and_pop64_test(seq_len, number_of_trials, seed);
    replace_test(seq_len, number_of_trials, seed);
    
    
    insert_and_erase_test(seq_len*3, number_of_trials, seed);
    insert64_and_erase64_test(seq_len*3, number_of_trials, seed);
    
}
