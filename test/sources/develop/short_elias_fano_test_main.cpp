#include <cassert>
#include <chrono>
#include "../../../include/light_stool.hpp"
#include "../../../include/develop/short_elias_fano_vector.hpp"


void equal_test(const stool::ShortEliasFanoVector &seq1, const std::vector<uint64_t> &seq2)
{
    if(seq1.size() != seq2.size()){
        std::cout << "[seq1.size() / seq2.size()] = " << seq1.size() << " != " << seq2.size() << std::endl;
        throw std::runtime_error("equal_test is incorrect (size is different)");
    }

    for (uint64_t i = 0; i < seq2.size(); i++)
    {
        if (seq1.at(i) != seq2[i])
        {
            std::cout << "seq1.access(" << i << ") = " << seq1.at(i) << " != " << seq2[i] << std::endl;

            
            auto vec1 = seq1.to_vector();
            stool::DebugPrinter::print_integers(vec1, "EF");
            stool::DebugPrinter::print_integers(seq2, "Seq");
            

            throw std::runtime_error("equal_test is incorrect");
        }
    }
}
void access_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::cout << "access_test" << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> items = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
            std::sort(items.begin(), items.end());
            stool::ShortEliasFanoVector short_ef(items);
            //std::cout << "short_ef.size() = " << short_ef.size() << std::endl;
            //short_ef.print_color_bits();

            equal_test(short_ef, items);

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "access_test is done." << std::endl;
}





void insert_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::cout << "insert_test" << std::endl;

    for(uint64_t i = 0; i < number_of_trials; i++){
        std::cout << "+" << std::flush;

        std::vector<uint64_t> items = stool::StringGenerator::create_random_integer_sequence(max_len, max_value, seed++);
        stool::ShortEliasFanoVector short_ef;
        for(auto it: items){
            short_ef.insert(it);
        }
        std::sort(items.begin(), items.end());
        equal_test(short_ef, items);
    }
    std::cout << std::endl;
    std::cout << "insert_test is done." << std::endl;
}
void erase_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::cout << "erase_test" << std::endl;

    std::mt19937 mt(seed);
    std::uniform_int_distribution<> rand_value(0, max_len);


    for(uint64_t i = 0; i < number_of_trials; i++){
        std::cout << "+" << std::flush;

        std::vector<uint64_t> items = stool::StringGenerator::create_random_integer_sequence(max_len, max_value, seed++);
        std::sort(items.begin(), items.end());
        stool::ShortEliasFanoVector short_ef(items);

        while(items.size() > 0){
            uint64_t nth = rand_value(mt) % items.size();

            if(items[nth] != short_ef.at(nth)){
                std::cout << "Error in erase_test" << std::endl;
                std::cout << "nth: " << nth << "/" << items[nth] << "/" << short_ef.at(nth) << std::endl;
                throw std::runtime_error("Error in erase_test");
            }
            short_ef.erase(nth);
            items.erase(items.begin() + nth);
        }
    }
    std::cout << std::endl;
    std::cout << "erase_test is done." << std::endl;
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
    uint64_t max_value = 1000000;
    uint64_t number_of_trials = 100;
    access_test(seq_len, number_of_trials, max_value, seed);
    insert_test(seq_len, number_of_trials/10, max_value, seed);
    erase_test(seq_len, number_of_trials/10, max_value, seed);
}
