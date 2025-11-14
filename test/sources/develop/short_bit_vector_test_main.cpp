#include <cassert>
#include <chrono>
#include "../../include/all.hpp"

std::string to_string(const std::vector<bool> &bv){
    std::string s;
    for(uint64_t i = 0; i < bv.size(); i++){
        s += bv[i] ? "1" : "0";
    }
    return s;
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

void access_test(uint64_t max_len, uint64_t number_of_trials, std::mt19937_64 &mt64)
{
    std::cout << "access_test" << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<bool> bv = stool::RandomString::create_random_bit_sequence(len, mt64);
            stool::ShortBitVector short_bv(bv);

            for (uint64_t j = 0; j < len; j++)
            {
                if (bv[j] != short_bv.at(j))
                {
                    std::cout << "access(" << j << ") = " << short_bv.at(j) << " != " << bv[j] << std::endl;
                    throw std::runtime_error("access is incorrect");
                }
            }
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "access_test is done." << std::endl;
}

void select_test(uint64_t max_len, uint64_t number_of_trials, std::mt19937_64 &mt64)
{
    std::cout << "select_test" << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> selecter_array;

            std::vector<bool> bv = stool::RandomString::create_random_bit_sequence(len, mt64);
            for (uint64_t j = 0; j < len; j++)
            {
                if (bv[j])
                {
                    selecter_array.push_back(j);
                }
            }
            stool::ShortBitVector short_bv(bv);

            for (uint64_t j = 0; j < selecter_array.size(); j++)
            {
                if (short_bv.select1(j) != (int64_t)selecter_array[j])
                {
                    std::cout << "select1(" << j << ") = " << short_bv.select1(j) << " != " << selecter_array[j] << std::endl;
                    throw std::runtime_error("select1 is incorrect");
                }
            }
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "select_test is done." << std::endl;
}

void rank_test(uint64_t max_len, uint64_t number_of_trials, std::mt19937_64 &mt64)
{
    std::cout << "rank_test" << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<bool> bv = stool::RandomString::create_random_bit_sequence(len, mt64);
            stool::ShortBitVector short_bv(bv);

            for (uint64_t j = 0; j < len; j++)
            {
                if (short_bv.rank1(j) != rank1(bv, j))
                {
                    std::cout << "rank1(" << j << ") = " << short_bv.rank1(j) << " != " << rank1(bv, j) << std::endl;
                    throw std::runtime_error("rank1 is incorrect");
                }
            }
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "rank_test is done." << std::endl;
}

void select_test(const stool::ShortBitVector &bv, const std::vector<bool> &naive_bv)
{
    for (uint64_t i = 0; i < bv.size(); i++)
    {
        if (bv.select1(i) != (int64_t)select1(naive_bv, i))
        {
            std::cout << "select1(" << i << ") = " << bv.select1(i) << " != " << select1(naive_bv, i) << std::endl;
            throw std::runtime_error("select1 is incorrect");
        }
    }
}

void equal_test(const stool::ShortBitVector &bv, const std::vector<bool> &naive_bv)
{
    if(naive_bv.size() != bv.size()){
        std::cout << "[naive_bv.size() / bv.size()] = " << naive_bv.size() << " != " << bv.size() << std::endl;
        throw std::runtime_error("equal_test is incorrect (size is different)");
    }

    for (uint64_t i = 0; i < naive_bv.size(); i++)
    {
        if (bv.at(i) != naive_bv[i])
        {
            std::cout << "bv.access(" << i << ") = " << bv.at(i) << " != " << naive_bv[i] << std::endl;
            throw std::runtime_error("equal_test is incorrect");
        }
    }
}

void push_back_and_pop_back_test(uint64_t max_len, uint64_t number_of_trials, std::mt19937_64 &mt64)
{
    stool::ShortBitVector bv;
    std::vector<bool> naive_bv;
    std::cout << "push_back_test" << std::endl;
    std::uniform_int_distribution<uint64_t> get_rand_value(0, 1);
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        bv.clear();
        naive_bv.clear();

        std::cout << "+" << std::flush;
        while (naive_bv.size() < max_len)
        {
            bool b = get_rand_value(mt64) == 1;
            bv.push_back(b);
            naive_bv.push_back(b);
        }
        equal_test(bv, naive_bv);
    }
    std::cout << std::endl;
    std::cout << "push_back_test is done." << std::endl;
}

void extend_right_test(uint64_t max_len, uint64_t number_of_trials, uint64_t extend_length, std::mt19937_64 &mt64)
{    
    std::cout << "ER" << extend_length << std::flush;
    for(uint64_t i = 0; i < number_of_trials; i++){
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < max_len; j++){
            std::vector<bool> bv = stool::RandomString::create_random_bit_sequence(max_len, mt64);
            stool::ShortBitVector short_bv(bv);

            for(uint64_t k=0;k<extend_length;k++){
                bv.insert(bv.begin() + j, false);
            }

            short_bv.extend_right(j, extend_length);

            //std::cout << "naive_bv: " << to_string(bv) << std::endl;
            //std::cout << "short_bv: " << short_bv.to_string() << std::endl;

            equal_test(short_bv, bv);
        }
    }
    std::cout << std::endl;
}
void extend_right_test(uint64_t max_len, uint64_t number_of_trials, std::mt19937_64 &mt64)
{
    std::cout << "extend_right_test" << std::endl;
    uint64_t extend_len = 1;
    while (extend_len <= 256)
    {
        extend_right_test(max_len, number_of_trials, extend_len, mt64);
        extend_len *= 2;
    }
    std::cout << std::endl;
    std::cout << "extend_right_test is done." << std::endl;
}

void insert_test(uint64_t max_len, uint64_t number_of_trials, uint64_t insert_length, std::mt19937_64 &mt64)
{    
    std::cout << "I" << insert_length << std::flush;
    for(uint64_t i = 0; i < number_of_trials; i++){
        std::cout << "+" << std::flush;
        for(uint64_t j = 0; j < max_len; j++){

            std::vector<bool> bv = stool::RandomString::create_random_bit_sequence(max_len, mt64);
            stool::ShortBitVector short_bv(bv);
            std::vector<bool> replace_bv = stool::RandomString::create_random_bit_sequence(insert_length, mt64);
            uint64_t insert_value = 0;

            if(j == 0 && insert_length == 128){
                std::cout << "naive_bv: " << to_string(bv) << std::endl;
                std::cout << "short_bv: " << short_bv.to_string() << std::endl;
            }

            for(uint64_t k=0;k<insert_length;k++){
                bv.insert(bv.begin() + j + k, replace_bv[k]);
                if (replace_bv[k])
                {
                    insert_value = insert_value | (1ULL << (63 - k));
                }
            }

            short_bv.insert(j, insert_length, insert_value);

            if(j == 0 && insert_length == 128){
                std::cout << "naive_bv: " << to_string(bv) << "[END]" << std::endl;
                std::cout << "short_bv: " << short_bv.to_string() << "[END]" << std::endl;
            }


            equal_test(short_bv, bv);
        }
    }
    std::cout << std::endl;
}





void insert_test(uint64_t max_len, uint64_t number_of_trials, std::mt19937_64 &mt64)
{
    std::cout << "insert_test" << std::endl;
    uint64_t insert_len = 1;
    while (insert_len <= 64)
    {
        insert_test(max_len, number_of_trials, insert_len, mt64);
        insert_len *= 2;
    }
    std::cout << std::endl;
    std::cout << "insert_test is done." << std::endl;
}

void erase_test(uint64_t max_len, uint64_t number_of_trials, uint64_t erase_length, std::mt19937_64 &mt64)
{ 
    std::cout << "E" << erase_length << std::flush;
    for(uint64_t i = 0; i < number_of_trials; i++){
        std::cout << "+" << std::flush;
        for(int64_t j = 0; j < (int64_t)max_len - (int64_t)erase_length; j++){
            std::vector<bool> bv = stool::RandomString::create_random_bit_sequence(max_len, mt64);
            stool::ShortBitVector short_bv(bv);
            std::vector<bool> replace_bv = stool::RandomString::create_random_bit_sequence(erase_length, mt64);
            uint64_t insert_value = 0;



            for(uint64_t k=0;k<erase_length;k++){
                bv.erase(bv.begin() + j);
            }

            short_bv.erase(j, erase_length);
            equal_test(short_bv, bv);
        }
    }
    std::cout << std::endl;   
}
void erase_test(uint64_t max_len, uint64_t number_of_trials, std::mt19937_64 &mt64)
{
    std::cout << "erase_test" << std::endl;
    uint64_t erase_len = 1;
    while (erase_len <= 256)
    {
        erase_test(max_len, number_of_trials, erase_len, mt64);
        erase_len *= 2;
    }
    std::cout << std::endl;
    std::cout << "erase_test is done." << std::endl;
}



void replace_test(uint64_t max_len, uint64_t number_of_trials, uint64_t replace_length, std::mt19937_64 &mt64)
{
    std::uniform_int_distribution<uint64_t> get_rand_value(0, 1);

    std::cout << "R" << replace_length << std::flush;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;

        std::vector<bool> bv = stool::RandomString::create_random_bit_sequence(max_len, mt64);
        stool::ShortBitVector short_bv(bv);

        for (uint64_t j = 0; j < short_bv.size() - replace_length; j++)
        {
            uint64_t replace_value = 0;

            //std::cout << "j = "<< j << std::endl;
            std::vector<bool> replace_bv = stool::RandomString::create_random_bit_sequence(replace_length, mt64);
            for (uint64_t k = 0; k < replace_length; k++)
            {
                //std::cout << (replace_bv[k] ? "1" : "0") << std::flush;
                bv[j + k] = replace_bv[k];
                if (replace_bv[k])
                {
                    replace_value = replace_value | (1ULL << (63 - k));
                }
            }
            //std::cout << std::endl;
            short_bv.replace(j, replace_length, replace_value);

            //std::cout << "NaiveBV: " << to_string(bv) << std::endl;
            //std::cout << "ShortBV: " << short_bv.to_string() << std::endl;

            equal_test(short_bv, bv);
        }
    }
    std::cout << std::endl;

}




void replace_test(uint64_t max_len, uint64_t number_of_trials, std::mt19937_64 &mt64)
{
    std::cout << "replace_test" << std::endl;
    uint64_t replace_len = 1;
    while (replace_len <= 64)
    {
        replace_test(max_len, number_of_trials, replace_len, mt64);
        replace_len *= 2;
    }
    std::cout << std::endl;
    std::cout << "replace_test is done." << std::endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{

    cmdline::parser p;

    p.add<uint>("mode", 'm', "mode", false, 0);
    p.add<uint>("seed", 's', "seed", false, 0);

    p.parse_check(argc, argv);
    uint64_t mode = p.get<uint>("mode");
    uint64_t seed = p.get<uint>("seed");

    std::mt19937_64 mt64(seed);

    uint64_t seq_len = 1000;
    uint64_t number_of_trials = 100;
    
    access_test(seq_len, number_of_trials, mt64);
    rank_test(seq_len, number_of_trials, mt64);
    select_test(seq_len, number_of_trials, mt64);
    push_back_and_pop_back_test(seq_len, number_of_trials, mt64);
    replace_test(seq_len, number_of_trials, mt64);
    extend_right_test(seq_len, number_of_trials/10, mt64);
    
    insert_test(seq_len, number_of_trials/10, mt64);    
    erase_test(seq_len, number_of_trials/10, mt64);
}
