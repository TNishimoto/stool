#include <cassert>
#include <chrono>
#include "../../../include/light_stool.hpp"
#include "../../../include/develop/short_integer_vector.hpp"

void equal_test(const stool::ShortIntegerVector &seq1, const std::vector<uint64_t> &seq2)
{
    if (seq1.size() != seq2.size())
    {
        std::cout << "[seq1.size() / seq2.size()] = " << seq1.size() << " != " << seq2.size() << std::endl;
        throw std::runtime_error("equal_test is incorrect (size is different)");
    }

    for (uint64_t i = 0; i < seq2.size(); i++)
    {
        if (seq1.at(i) != seq2[i])
        {
            std::cout << "seq1.access(" << i << ") = " << seq1.at(i) << " != " << seq2[i] << std::endl;

            /*
            auto vec1 = seq1.to_vector();
            stool::DebugPrinter::print_integers(vec1, "EF");
            stool::DebugPrinter::print_integers(seq2, "Seq");
            */

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
            std::vector<uint64_t> items = stool::RandomString::create_random_sequence<uint64_t>(len, max_value, seed++);
            stool::ShortIntegerVector short_ef(items);
            // std::cout << "short_ef.size() = " << short_ef.size() << std::endl;
            // short_ef.print_color_bits();

            equal_test(short_ef, items);

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "access_test is done." << std::endl;
}

void psum_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::cout << "access_test" << std::endl;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;

        auto psum_func = [](const std::vector<uint64_t> &items, int64_t i)
        {
            uint64_t sum = 0;
            for (int64_t k = 0; k <= i; k++)
            {
                sum += items[k];
            }
            return sum;
        };

        auto reverse_psum_func = [](const std::vector<uint64_t> &items, int64_t i)
        {
            uint64_t sum = 0;
            for (int64_t k = items.size() - 1; k >= i; k--)
            {
                sum += items[k];
            }
            return sum;
        };

        while (len < max_len)
        {
            std::vector<uint64_t> items = stool::RandomString::create_random_sequence<uint64_t>(len, max_value, seed++);
            stool::ShortIntegerVector short_ef(items);

            for (uint64_t j = 0; j < len; j++)
            {
                uint64_t psum1 = psum_func(items, j);
                uint64_t psum2 = short_ef.psum(j);
                if (psum1 != psum2)
                {
                    std::cout << "psum1: " << psum1 << " != " << psum2 << std::endl;
                    throw std::runtime_error("psum error");
                }
            }

            for (int64_t j = 0; j < (int64_t)len; j++)
            {
                uint64_t psum1 = reverse_psum_func(items, j);
                uint64_t psum2 = short_ef.reverse_psum(len - j - 1);
                if (psum1 != psum2)
                {
                    std::cout << "j: " << j << " , psum1: " << psum1 << " != " << psum2 << std::endl;
                    std::cout << "Seq1: " << stool::ConverterToString::to_integer_string(items) << std::endl;
                    std::cout << "Seq2: " << short_ef.to_string() << std::endl;

                    throw std::runtime_error("reverse psum error");
                }
            }

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "access_test is done." << std::endl;
}

void search_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::cout << "search_test" << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<> rand_value(0, UINT32_MAX);
    auto search_func = [](const std::vector<uint64_t> &items, int64_t i) -> int64_t
    {
        int64_t sum = 0;
        for (int64_t k = 0; k <= i; k++)
        {
            sum += items[k];
            if(sum >= i){
                return k;
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
            std::vector<uint64_t> items = stool::RandomString::create_random_sequence<uint64_t>(len, max_value, seed++);
            stool::ShortIntegerVector short_ef(items);
            uint64_t sum = short_ef.psum();

            for (uint64_t j = 0; j < max_len; j++)
            {
                uint64_t v = rand_value(mt) % sum;
                int64_t search1 = search_func(items, v);
                int64_t search2 = short_ef.search(v);
                if (search1 != search2)
                {
                    std::cout << "search1: " << search1 << " != " << search2 << " , j: " << v << std::endl;
                    std::cout << "Seq1: " << stool::ConverterToString::to_integer_string(items) << std::endl;
                    throw std::runtime_error("search error");
                }
            }

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "search_test is done." << std::endl;

}


void back_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::cout << "back_test" << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<> rand_len(0, UINT32_MAX);

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> items = stool::RandomString::create_random_sequence<uint64_t>(len, max_value, seed++);
            stool::ShortIntegerVector short_ef;

            for (auto it : items)
            {
                short_ef.push_back(it);
            }
            equal_test(short_ef, items);

            uint64_t sum1 = 0;
            for (auto it : items)
            {
                sum1 += it;
            }

            uint64_t sum2 = 0;

            while(short_ef.size() > 0){
                uint64_t remove_len = rand_len(mt) % (short_ef.size() + 1);
                std::vector<uint64_t> items2 = short_ef.pop_back(remove_len);
                for (auto it : items2)
                {
                    sum2 += it;
                }    
            }

            if (sum1 != sum2)
            {
                std::cout << "sum1: " << sum1 << " != " << sum2 << std::endl;
                throw std::runtime_error("back_test is incorrect");
            }
            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "back_test is done." << std::endl;
}

void front_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::cout << "front_test" << std::endl;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<> rand_len(0, UINT32_MAX);

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> items = stool::RandomString::create_random_sequence<uint64_t>(len, max_value, seed++);
            stool::ShortIntegerVector short_ef;

            for (int64_t i = items.size() - 1; i >= 0; i--)
            {
                short_ef.push_front(items[i]);
            }
            equal_test(short_ef, items);

            uint64_t sum1 = 0;
            for (auto it : items)
            {
                sum1 += it;
            }

            uint64_t sum2 = 0;
            while(short_ef.size() > 0){
                uint64_t remove_len = rand_len(mt) % (short_ef.size() + 1);
                std::vector<uint64_t> items2 = short_ef.pop_front(remove_len);
                for (auto it : items2)
                {
                    sum2 += it;
                }    
            }


            if (sum1 != sum2)
            {
                std::cout << "sum1: " << sum1 << " != " << sum2 << std::endl;
                throw std::runtime_error("front_test is incorrect");
            }

            len *= 2;
        }
    }
    std::cout << std::endl;
    std::cout << "front_test is done." << std::endl;
}

void insert_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::mt19937 mt(seed);
    std::uniform_int_distribution<> rand_value(0, max_value);
    std::uniform_int_distribution<> rand_pos(0, UINT32_MAX);

    std::cout << "I" << std::flush;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;

        std::vector<uint64_t> items;
        stool::ShortIntegerVector short_ef(items);

        for (int64_t k = max_len; k >= 0; k--)
        {
            uint64_t v = rand_value(mt);
            uint64_t pos = rand_pos(mt) % (items.size() + 1);

            short_ef.insert(pos, v);
            items.insert(items.begin() + pos, v);
            equal_test(short_ef, items);
        }
    }
    std::cout << std::endl;
}

void erase_test(uint64_t max_len, uint64_t number_of_trials, uint64_t max_value, uint64_t seed)
{
    std::mt19937 mt(seed);
    std::uniform_int_distribution<> rand_pos(0, UINT32_MAX);

    std::cout << "E" << std::flush;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;

        std::vector<uint64_t> items = stool::RandomString::create_random_sequence<uint64_t>(max_len, max_value, seed++);
        stool::ShortIntegerVector short_ef(items);

        while (short_ef.size() > 0)
        {
            uint64_t pos = rand_pos(mt) % items.size();
            short_ef.remove(pos);
            items.erase(items.begin() + pos);
            equal_test(short_ef, items);
        }
    }
    std::cout << std::endl;
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

    uint64_t seq_len = 256;
    uint64_t max_value = 1000000;
    uint64_t number_of_trials = 100;
    access_test(seq_len, number_of_trials, max_value, seed);
    psum_test(seq_len, number_of_trials, max_value, seed);

    //search_test(seq_len, number_of_trials, max_value, seed);


    back_test(seq_len, number_of_trials / 10, max_value, seed);
    front_test(seq_len, number_of_trials / 10, max_value, seed);

    insert_test(seq_len, number_of_trials / 10, max_value, seed);
    erase_test(seq_len, number_of_trials / 10, max_value, seed);
}
