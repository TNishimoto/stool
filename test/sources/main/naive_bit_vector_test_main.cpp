#include <cassert>
#include <chrono>
#include "../../../include/light_stool.hpp"

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

uint64_t compute_rank1(const std::vector<bool> &bv, uint64_t i){
    uint64_t rank = 0;
    for(uint64_t j = 0; j <= i; j++){
        if(bv[j]){
            rank++;
        }
    }
    return rank;
}

void shift_right(std::vector<bool> &bv, uint64_t shift_pos, uint64_t shift_bitsize){
    bv.resize(bv.size() + shift_bitsize);
    uint64_t suffix_len = bv.size() - shift_pos;
    uint64_t move_len = suffix_len - shift_bitsize;
    for(int64_t i = move_len-1; i >= 0; i--){
        bv[shift_pos + i + shift_bitsize] = bv[shift_pos + i];
    }
    for(uint64_t i = 0; i < shift_bitsize; i++){
        bv[shift_pos + i] = false;
    }
}
void shift_left(std::vector<bool> &bv, uint64_t shift_pos, uint64_t shift_bitsize){

    uint64_t suffix_len = bv.size() - shift_pos;
    for(uint64_t i = 0; i < suffix_len; i++){
        bv[shift_pos + i - shift_bitsize] = bv[shift_pos + i];
    }
    for(uint64_t i = 0; i < shift_bitsize; i++){
        bv[bv.size() - i - 1] = false;
    }
    bv.resize(bv.size() - shift_bitsize);

}


uint64_t compute_rank0(const std::vector<bool> &bv, uint64_t i){

    uint64_t rank = 0;
    for(uint64_t j = 0; j <= i; j++){
        if(!bv[j]){
            rank++;
        }
    }
    return rank;
}

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

int64_t compute_rev_select1(const std::vector<bool> &bv, uint64_t i){
    uint64_t count = 0;
    uint64_t e = i+1;
    for(int64_t j = bv.size()-1; j >= 0; j--){
        if(bv[j]){
            count++;
        }
        if(count == e){
            return j;
        }
    }
    return -1;
}


template<uint64_t N>
void print_bitset(const std::bitset<N> &bs){
    for(uint64_t i = 0; i < N; i++){
        std::cout << bs[i];
    }
    std::cout << std::endl;
}

void random_bit_string256(int64_t bit_length, std::vector<uint64_t> &new_pattern, std::bitset<256> &bs, uint64_t seed){
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

    bs.reset();
    uint64_t i = 0;



    while(bit_length > 0){
        uint64_t new_value = get_rand_value(mt);
        new_pattern.push_back(new_value);
        uint64_t len = std::min((uint64_t)bit_length, 64ULL);
        for(uint64_t j = 0; j < len; j++){
            bs[i] = (new_value >> (63 - j)) & 1;
            i++;
            bit_length--;
        }
    }    
}

void equal_test(const stool::NaiveBitVector<> &bv, const std::vector<bool> &naive_bv)
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

    
    if(naive_bv.size() > 0){
        uint64_t num1_naive = compute_rank1(naive_bv, naive_bv.size() - 1);
        uint64_t num1 = bv.rank1();
        if(num1 != num1_naive){
            std::cout << "num1 = " << num1 << " != " << num1_naive << std::endl;
            std::cout << "naive_bv = " << to_string(naive_bv, true) << std::endl;
            std::cout << "      bv = " << bv.to_string(true) << std::endl;
            throw std::runtime_error("equal_test is incorrect (num1 is different)");
        }    
    }
    
}
void access_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "ACCESS_TEST \t" << std::flush;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);

            stool::NaiveBitVector<> bit_deque(bv);




            equal_test(bit_deque, bv);
            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

void select_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "SELECT_TEST \t" << std::flush;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> selecter_array1;
            std::vector<uint64_t> selecter_array0;
            std::vector<uint64_t> rev_selecter_array1;

            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);

            for (uint64_t j = 0; j < len; j++)
            {
                if (bv[j])
                {
                    selecter_array1.push_back(j);
                }else{
                    selecter_array0.push_back(j);
                }
            }
            for(int64_t j = len-1; j >= 0; j--){
                if(bv[j]){
                    rev_selecter_array1.push_back(j);
                }
            }



            stool::NaiveBitVector<> bit_deque(bv);

            for (uint64_t j = 0; j < selecter_array1.size(); j++)
            {
                int64_t bit_deque_select = bit_deque.select1(j);

                if (bit_deque_select != (int64_t)selecter_array1[j])
                {
                    std::cout << "select1(" << j << ") = " << bit_deque_select << " != " << selecter_array1[j] << std::endl;
                    std::cout << "naive_bv = " << to_string(bv) << std::endl;
                    bit_deque.print_info();
        

                    throw std::runtime_error("select1 is incorrect");
                }
            }
            for (uint64_t j = 0; j < selecter_array0.size(); j++)
            {
                int64_t bit_deque_select = bit_deque.select0(j);
 
                if (bit_deque_select != (int64_t)selecter_array0[j])
                {
                    std::cout << "select0(" << j << ") = " << bit_deque_select << " != " << selecter_array0[j] << std::endl;
                    std::cout << "naive_bv = " << to_string(bv) << std::endl;
                    bit_deque.print_info();
        

                    throw std::runtime_error("select0 is incorrect");
                }
            }


            for(int64_t j = rev_selecter_array1.size()-1; j >= 0; j--){
                int64_t bit_deque_select = bit_deque.rev_select1(j);
                if(bit_deque_select != (int64_t)rev_selecter_array1[j]){
                    std::cout << "rev_select1(" << j << ") = " << bit_deque_select << " != " << rev_selecter_array1[j] << std::endl;
                    std::cout << "naive_bv = " << to_string(bv) << std::endl;
                    bit_deque.print_info();
                    throw std::runtime_error("rev_select1 is incorrect");
                }
            }


            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}
    
   
void rank_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "RANK_TEST \t" << std::flush;

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::NaiveBitVector<> bit_deque(bv);

            for (uint64_t j = 0; j < len; j++)
            {
                
                if (bit_deque.rank1(j) != compute_rank1(bv, j))
                {
                    std::cout << "rank1(" << j << ") = " << bit_deque.rank1(j) << " != " << compute_rank1(bv, j) << std::endl;

                    std::cout << "naive_bv = " << to_string(bv) << std::endl;
                    bit_deque.print_info();
        

                    throw std::runtime_error("rank1 is incorrect");
                }

                if (bit_deque.rank0(j) != compute_rank0(bv, j))
                {
                    std::cout << "rank0(" << j << ") = " << bit_deque.rank0(j) << " != " << compute_rank0(bv, j) << std::endl;

                    std::cout << "naive_bv = " << to_string(bv) << std::endl;
                    bit_deque.print_info();
        

                    throw std::runtime_error("rank0 is incorrect");
                }

            }
            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}
    
void shift_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed, [[maybe_unused]] bool detail_check = true){
    std::cout << "SHIFT_TEST \t" << std::flush;
    stool::NaiveBitVector<> bit_deque;
    std::vector<bool> naive_bv;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        for(uint64_t len = 4; len < max_len; len *= 2){
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::NaiveBitVector<> bit_deque(bv);

            uint64_t shift_pos = get_rand_value(mt) % len;
            uint64_t shift_bitsize = get_rand_value(mt) % (len - shift_pos);

            
            bit_deque.shift_right(shift_pos, shift_bitsize);
            shift_right(bv, shift_pos, shift_bitsize);

            equal_test(bit_deque, bv);


            
            uint64_t shift_pos2 = get_rand_value(mt) % bv.size();
            if(shift_pos2 > 0){
                uint64_t shift_bitsize2 = get_rand_value(mt) % shift_pos2;


                bit_deque.shift_left(shift_pos2, shift_bitsize2);
                shift_left(bv, shift_pos2, shift_bitsize2);
    
                equal_test(bit_deque, bv);
            }
            




        }

        

    }
    std::cout << "[DONE]" << std::endl;
}


    

void push_and_pop_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
{
    std::cout << "PUSH_AND_POP_TEST \t" << std::flush;
    stool::NaiveBitVector<> bit_deque;
    std::vector<bool> naive_bv;
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

            if(detail_check){
                equal_test(bit_deque, naive_bv);
            }

        }
        equal_test(bit_deque, naive_bv);

    }
    std::cout << "[DONE]" << std::endl;
}
    

    

void push64_and_pop64_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
{
    std::cout << "PUSH64_AND_POP64_TEST \t" << std::flush;
    stool::NaiveBitVector<> bit_deque;
    std::vector<bool> naive_bv;
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
            if(detail_check){
                equal_test(bit_deque, naive_bv);
            }
        }
        equal_test(bit_deque, naive_bv);

    }
    std::cout << "[DONE]" << std::endl;
}




void replace_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
{

    std::cout << "REPLACE_TEST \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::NaiveBitVector<> bit_deque(bv);


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

                if(detail_check){
                    equal_test(bit_deque, bv);
                }
            }

            equal_test(bit_deque, bv);
            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}
    

    

void insert_and_erase_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
{
    std::cout << "INSERT_AND_ERASE_TEST \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        int64_t len = 1;
        while (len < (int64_t)max_len)
        {

            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::NaiveBitVector<> bit_deque(bv);

            equal_test(bit_deque, bv);

            assert(bv.size() == bit_deque.size());
            
            //while((int64_t)bv.size() < 50){
            while((int64_t)bv.size() < (int64_t)(len*2)){
                uint64_t new_value = get_rand_value(mt);
                int64_t plen = (get_rand_value(mt) % 64) + 1;
                uint64_t pos = get_rand_value(mt) % (bv.size() + 1);

                std::string new_value_str = stool::Byte::to_bit_string(new_value, true);




                assert(pos <= bit_deque.size());


                for(int64_t k = 0; k < plen; k++){
                    bv.insert(bv.begin() + pos + k, new_value_str[k] == '1');
                }




                bit_deque.insert_64bit_string(pos, new_value, plen);

                if(detail_check){
                    try{
                        equal_test(bit_deque, bv);                        
                    }catch(const std::runtime_error& e){
                        std::cout << "Insert test error" << std::endl;
                        std::cout << "len = " << len << std::endl;
                        std::cout << "pos = " << pos << std::endl;
                        std::cout << "plen = " << plen << std::endl;
                        std::cout << "bv size = " << bv.size() << std::endl;
                        std::cout << "pattern = " << stool::Byte::to_bit_string(new_value, true).substr(0, plen) << std::endl;
                        throw e;
                    }

                }


            }

            try{
                equal_test(bit_deque, bv);
            }catch(const std::runtime_error& e){
                std::cout << "Insert test error(1)" << std::endl;
                std::cout << "len = " << len << std::endl;
                std::cout << "bv size = " << bv.size() << std::endl;
                throw e;
            }

            
            while(bv.size() > 0){

                uint64_t pos = get_rand_value(mt) % bv.size();
                bv.erase(bv.begin() + pos);


                bit_deque.erase(pos);


                if(detail_check){
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
            }
            
            

            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}
    

    

void insert64_and_erase64_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
{
    std::cout << "INSERT64_AND_ERASE64_TEST \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        int64_t len = 1;
        while (len < (int64_t)max_len)
        {
            std::vector<bool> bv = stool::StringGenerator::create_random_bit_vector(len, seed++);
            stool::NaiveBitVector<> bit_deque(bv);
            assert(bv.size() == bit_deque.size());


            while((int64_t)bv.size() < (int64_t) len * 2){

                uint64_t new_pattern_size = get_rand_value(mt) % 257;
                std::vector<uint64_t> new_pattern;
                std::bitset<256> new_pattern_bs;

                random_bit_string256(new_pattern_size, new_pattern, new_pattern_bs, seed++);


                uint64_t pos = get_rand_value(mt) % (bv.size() + 1);

                for(uint64_t k = 0; k < new_pattern_size; k++){
                    bv.insert(bv.begin() + pos + k, new_pattern_bs[k]);
                }

                assert(pos <= bit_deque.size());
                bit_deque.insert_64bit_string(pos, new_pattern, new_pattern_size, new_pattern.size());

                if(detail_check){
                    try{
                        equal_test(bit_deque, bv);
                    }
                    catch(const std::runtime_error& e){
                        std::cout << "Insert64 test error" << std::endl;
                        std::cout << "\t len = " << len << std::endl;
                        std::cout << "\t pos = " << pos << std::endl;
                        std::cout << "\t new_pattern_size = " << new_pattern_size << std::endl;
                        std::cout << "\t new_pattern_bs = ";
                        print_bitset<256>(new_pattern_bs);
                        std::cout << "\t bv size = " << bv.size() << std::endl;
                        throw e;
                    }
                        
                }    
            }


            try{
                equal_test(bit_deque, bv);
            }
            catch(const std::runtime_error& e){
                std::cout << "Insert64 test error(1)" << std::endl;
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
                auto copy_bv = bv;

                for(int64_t j = 0; j < new_pattern_size; j++){
                    bv.erase(bv.begin() + pos);
                }

                bit_deque.erase(pos, new_pattern_size);

                if(detail_check){
                    if(detail_check){
                        try{
                            equal_test(bit_deque, bv);
                        }
                        catch(const std::runtime_error& e){
                            std::cout << "bv before update \t " << to_string(copy_bv, true) << std::endl;
                            std::cout << "pos: " << pos << std::endl;
                            std::cout << "new_pattern_size: " << new_pattern_size << std::endl;
                            std::cout << "Erase test error" << std::endl;
                            std::cout << "bv size = " << bv.size() << std::endl;
                            throw e;
                        }                            
                    }
        
                }

            }
            
            

            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}
    

    
void random_test(uint64_t max_len, uint64_t number_of_trials, uint64_t seed, bool detail_check = false)
{
    stool::NaiveBitVector<> bit_deque;
    std::vector<bool> seq;
    std::cout << "RANDOM_TEST: \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);


    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        uint64_t counter = 0;
        std::vector<bool> seq = stool::StringGenerator::create_random_bit_vector(max_len/2, seed++);
        stool::NaiveBitVector<> bit_deque(seq);

        std::cout << "+" << std::flush;

        while (counter < 10000)
        {

            uint64_t type = get_rand_value(mt) % 10;
            uint64_t random_pos = get_rand_value(mt) % seq.size();
            bool random_bit = get_rand_value(mt) % 2;

            if (type == 0 || type == 1)
            {
                bit_deque.push_back(random_bit);
                seq.push_back(random_bit);    
            }
            else if (type == 2 || type == 3)
            {
                bit_deque.push_front(random_bit);
                seq.insert(seq.begin(), random_bit);    
            }
            else if (type == 4 && seq.size() > 0)
            {
                bit_deque.pop_back();
                seq.pop_back();
            }
            else if (type == 5 && bit_deque.size() > 0)
            {
                bit_deque.pop_front();
                seq.erase(seq.begin());
            }
            else if (type == 6 && seq.size() < max_len)
            {
                bit_deque.insert(random_pos, random_bit);
                seq.insert(seq.begin() + random_pos, random_bit);
            }
            else if (type == 7 && seq.size() > 0)
            {
                seq.erase(seq.begin() + random_pos);
                bit_deque.erase(random_pos);
            }
            else if (type == 8){
                seq[random_pos] = random_bit;
                bit_deque.replace(random_pos, random_bit);
            }else{

                uint64_t rank1A = compute_rank1(seq, random_pos);
                uint64_t rank1B = bit_deque.rank1(random_pos);
                if (rank1A != rank1B)
                {
                    std::cout << "rank_test error/" << rank1A << "/" << rank1B << std::endl;
                    throw std::runtime_error("rank_test error");
                }

                int64_t select1A = compute_select1(seq, random_pos);
                int64_t select1B = bit_deque.select1(random_pos);
                if (select1A != select1B)
                {
                    std::cout << "select_test error/" << select1A << "/" << select1B << std::endl;
                    throw std::runtime_error("select_test error");
                }
            }
            counter++;

            if(detail_check){
                try{
                    equal_test(bit_deque, seq);
                }catch(const std::runtime_error& e){
                    std::cout << "random_test error" << std::endl;
                    std::cout << "type = " << type << std::endl;
                    std::cout << "seq = " << to_string(seq, true) << std::endl;
                    std::cout << "bit_deque = " << bit_deque.to_string(true) << std::endl;
                    throw e;
                }
            }
        }
        equal_test(bit_deque, seq);

    }
    std::cout << "[DONE]" << std::endl;
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
    
    
    
    
    
    shift_test(seq_len, number_of_trials*10, seed, false);    
    access_test(seq_len, number_of_trials, seed);
    rank_test(seq_len, number_of_trials, seed);
    select_test(seq_len, number_of_trials, seed);
    insert_and_erase_test(seq_len*3, number_of_trials, seed, false);
    replace_test(seq_len, number_of_trials, seed, false);     
    push_and_pop_test(seq_len, number_of_trials, seed, false);
    push64_and_pop64_test(seq_len, number_of_trials, seed, false);
    insert64_and_erase64_test(seq_len*3, number_of_trials, seed, true);

    random_test(seq_len, number_of_trials, seed, false);
    
        

    
    
}
