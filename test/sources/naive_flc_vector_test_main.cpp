#include <cassert>
#include <chrono>
#include "../../include/light_stool.hpp"

std::string to_string(const std::vector<bool> &bv, bool use_partition = false)
{
    std::string s;
    for (uint64_t i = 0; i < bv.size(); i++)
    {
        s += bv[i] ? "1" : "0";
        if (use_partition && i % 64 == 63)
        {
            s += " ";
        }
    }
    return s;
}

uint64_t compute_psum(const std::vector<uint64_t> &bv, uint64_t i)
{
    uint64_t sum = 0;

    if (i >= bv.size())
    {
        std::cout << "compute_psum error" << std::endl;
        std::cout << "i = " << i << std::endl;
        std::cout << "bv.size() = " << bv.size() << std::endl;
        throw std::runtime_error("compute_psum error");
    }

    for (uint64_t j = 0; j <= i; j++)
    {
        sum += bv[j];
    }
    return sum;
}

int64_t compute_search(const std::vector<uint64_t> &bv, uint64_t value)
{
    uint64_t sum = 0;
    for (uint64_t j = 0; j < bv.size(); j++)
    {
        sum += bv[j];
        if (sum >= value)
        {
            return j;
        }
    }
    return -1;
}

template <uint64_t N>
void print_bitset(const std::bitset<N> &bs)
{
    for (uint64_t i = 0; i < N; i++)
    {
        std::cout << bs[i];
    }
    std::cout << std::endl;
}

void equal_test(const stool::NaiveFLCVector<> &bv, const std::vector<uint64_t> &naive_bv)
{
    if (naive_bv.size() != bv.size())
    {
        std::cout << std::endl;
        std::cout << "[naive_bv.size() / bv.size()] = " << naive_bv.size() << " != " << bv.size() << std::endl;

        std::cout << "naive_bv = " << stool::DebugPrinter::to_integer_string(naive_bv) << std::endl;
        std::cout << "      bv = " << stool::DebugPrinter::to_integer_string(bv.to_vector()) << std::endl;

        bv.print_info();

        throw std::runtime_error("equal_test is incorrect (size is different)");
    }

    for (uint64_t i = 0; i < naive_bv.size(); i++)
    {

        if (bv.at(i) != naive_bv[i])
        {
            std::cout << std::endl;
            std::cout << "naive_bv = " << stool::DebugPrinter::to_integer_string(naive_bv) << std::endl;
            std::cout << "bv.at(" << i << ") = " << bv.at(i) << " != " << naive_bv[i] << std::endl;

            bv.print_info();

            throw std::runtime_error("equal_test is incorrect");
        }
    }

    /*
    std::vector<uint64_t> bv_values;
    for(auto v : bv){
        bv_values.push_back(v);
    }
    stool::EqualChecker::equal_check(naive_bv, bv_values, "EQUAL_TEST2");
    */
}
void access_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "ACCESS_TEST \t" << std::flush;
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);

            stool::NaiveFLCVector<> value_deque(values);

            equal_test(value_deque, values);
            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

void psum_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "PSUM_TEST \t" << std::flush;

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
            stool::NaiveFLCVector<> value_deque(values);

            uint64_t psum = 0;

            for (uint64_t i = 0; i < len; i++)
            {
                psum += values[i];
                if (value_deque.psum(i) != psum)
                {
                    std::cout << "psum error" << std::endl;
                    std::cout << "psum = " << psum << std::endl;
                    std::cout << "i = " << i << std::endl;
                    std::cout << "value_deque.psum(i) = " << value_deque.psum(i) << std::endl;

                    value_deque.print_info();
                    throw std::runtime_error("psum error");
                }
            }

            uint64_t rpsum = 0;

            for (uint64_t i = 0; i < len; i++)
            {
                rpsum += values[len - i - 1];
                if (value_deque.reverse_psum(i) != rpsum)
                {
                    std::cout << "reverse_psum error" << std::endl;
                    std::cout << "rpsum = " << rpsum << std::endl;
                    std::cout << "value_deque.psum(i) = " << value_deque.psum(i) << std::endl;
                    throw std::runtime_error("psum error");
                }
            }

            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

void search_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
{
    std::cout << "SEARCH_TEST \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::cout << "+" << std::flush;
        uint64_t len = 1;
        while (len < max_len)
        {
            std::vector<uint64_t> values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
            stool::NaiveFLCVector<> value_deque(values);

            for (uint64_t i = 0; i < len; i++)
            {
                uint64_t value = get_rand_value(mt) % max_value;
                int64_t search_result1 = value_deque.search(value);
                int64_t search_result2 = compute_search(values, value);

                if (search_result1 != search_result2)
                {
                    std::cout << "search error" << std::endl;
                    std::cout << "search_result1 = " << search_result1 << std::endl;
                    std::cout << "search_result2 = " << search_result2 << std::endl;
                    throw std::runtime_error("search error");
                }
            }

            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

void push_and_pop_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
{
    std::cout << "PUSH_AND_POP_TEST \t" << std::flush;
    stool::NaiveFLCVector<> value_deque;
    std::vector<uint64_t> naive_bv;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);
    for (uint64_t i = 0; i < number_of_trials; i++)
    {

        value_deque.clear();
        naive_bv.clear();

        std::cout << "+" << std::flush;
        while (naive_bv.size() < max_len)
        {

            uint64_t value = get_rand_value(mt) % max_value;
            uint64_t type = get_rand_value(mt) % 5;

            if (type == 0 || type == 1)
            {
                value_deque.push_back(value);
                naive_bv.push_back(value);
            }
            else if (type == 2 || type == 3)
            {
                value_deque.push_front(value);
                naive_bv.insert(naive_bv.begin(), value);
            }
            else if (type == 4 && naive_bv.size() > 0)
            {
                value_deque.pop_back();
                naive_bv.pop_back();
            }
            else if (type == 5 && value_deque.size() > 0)
            {
                value_deque.pop_front();
                naive_bv.erase(naive_bv.begin());
            }

            if (detail_check)
            {
                equal_test(value_deque, naive_bv);
            }
        }
        equal_test(value_deque, naive_bv);
    }
    std::cout << "[DONE]" << std::endl;
}

void insert_and_erase_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
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

            std::vector<uint64_t> values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
            stool::NaiveFLCVector<> value_deque(values);

            while ((int64_t)values.size() < (int64_t)(len * 2))
            {
                uint64_t new_value = get_rand_value(mt) % max_value;
                uint64_t pos = get_rand_value(mt) % (values.size() + 1);

                values.insert(values.begin() + pos, new_value);
                value_deque.insert(pos, new_value);

                if (detail_check)
                {
                    try
                    {
                        equal_test(value_deque, values);
                    }
                    catch (const std::runtime_error &e)
                    {
                        std::cout << "Insert test error" << std::endl;
                        std::cout << "len = " << len << std::endl;
                        std::cout << "pos = " << pos << std::endl;
                        throw e;
                    }
                }
            }

            equal_test(value_deque, values);

            while (values.size() > 0)
            {

                uint64_t pos = get_rand_value(mt) % values.size();
                values.erase(values.begin() + pos);
                value_deque.remove(pos);

                if (detail_check)
                {
                    try
                    {
                        equal_test(value_deque, values);
                    }
                    catch (const std::runtime_error &e)
                    {
                        std::cout << "Erase test error" << std::endl;
                        std::cout << "len = " << len << std::endl;
                        std::cout << "pos = " << pos << std::endl;
                        throw e;
                    }
                }
            }

            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

void replace_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
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
            std::vector<uint64_t> values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
            stool::NaiveFLCVector<> value_deque(values);

            for (uint64_t j = 0; j < len; j++)
            {
                uint64_t new_value = get_rand_value(mt) % max_value;
                values[j] = new_value;
                value_deque.set_value(j, new_value);

                if (detail_check)
                {
                    equal_test(value_deque, values);
                }
            }

            equal_test(value_deque, values);
            len *= 2;
        }
    }
    std::cout << "[DONE]" << std::endl;
}

void random_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t max_counter, uint64_t seed, bool detail_check = false)
{
    stool::NaiveFLCVector<> value_deque;
    std::vector<uint64_t> seq;
    std::cout << "RANDOM_TEST: \t" << std::flush;
    std::mt19937 mt(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        uint64_t counter = 0;
        std::vector<uint64_t> seq = stool::StringGenerator::create_random_integer_sequence(max_len / 2, max_value, seed++);
        stool::NaiveFLCVector<> value_deque(seq);

        std::cout << "+" << std::flush;

        while (counter < max_counter)
        {

            uint64_t type = get_rand_value(mt) % 10;
            uint64_t random_pos = seq.size() > 0 ? (get_rand_value(mt) % seq.size()) : 0;
            uint64_t random_value = get_rand_value(mt) % max_value;

            if (type == 0 || type == 1)
            {
                value_deque.push_back(random_value);
                seq.push_back(random_value);
            }
            else if (type == 2 || type == 3)
            {
                value_deque.push_front(random_value);
                seq.insert(seq.begin(), random_value);
            }
            else if (type == 4 && seq.size() > 0)
            {
                value_deque.pop_back();
                seq.pop_back();
            }
            else if (type == 5 && value_deque.size() > 0)
            {
                value_deque.pop_front();
                seq.erase(seq.begin());
            }
            else if (type == 6 && seq.size() < max_len)
            {
                value_deque.insert(random_pos, random_value);
                seq.insert(seq.begin() + random_pos, random_value);
            }
            else if (type == 7 && seq.size() > 0)
            {
                seq.erase(seq.begin() + random_pos);
                value_deque.remove(random_pos);
            }
            else if (type == 8)
            {
                if (random_pos < seq.size())
                {
                    seq[random_pos] = random_value;
                    value_deque.set_value(random_pos, random_value);
                }
            }
            else
            {
                if (seq.size() > 1)
                {
                    uint64_t psum1A = compute_psum(seq, random_pos);
                    uint64_t psum1B = value_deque.psum(random_pos);
                    if (psum1A != psum1B)
                    {
                        std::cout << "psum_test error/" << psum1A << "/" << psum1B << std::endl;
                        throw std::runtime_error("psum_test error");
                    }

                    int64_t search1A = compute_search(seq, random_pos);
                    int64_t search1B = value_deque.search(random_pos);
                    if (search1A != search1B)
                    {
                        std::cout << "search_test error/" << search1A << "/" << search1B << std::endl;
                        throw std::runtime_error("search_test error");
                    }
                }
            }
            counter++;

            if (detail_check)
            {
                try
                {
                    equal_test(value_deque, seq);
                }
                catch (const std::runtime_error &e)
                {
                    std::cout << "random_test error" << std::endl;
                    std::cout << "type = " << type << std::endl;
                    throw e;
                }
            }
        }
        equal_test(value_deque, seq);
    }
    std::cout << "[DONE]" << std::endl;
}
void load_write_file_test(uint64_t max_element_count, uint64_t trial_count, [[maybe_unused]] bool detailed_check, uint64_t seed)
{
    std::cout << "load_write_file_test: " << std::flush;
    std::mt19937_64 mt64(seed);
    std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

    for (uint64_t num = 16; num < max_element_count; num *= 2)
    {
        std::cout << "+" << std::flush;

        for (uint64_t x = 0; x < trial_count; x++)
        {
            stool::NaiveFLCVector<> value_deque;
            for(uint64_t i = 0; i < num; i++){
                value_deque.push_back(get_rand_uni_int(mt64) % (1ULL << 32));
            }

            {
                std::ofstream os;
                os.open("flc_vector.bits", std::ios::binary);
                if (!os)
                {
                    std::cerr << "Error: Could not open file for writing." << std::endl;
                    throw std::runtime_error("File open error");
                }
                stool::NaiveFLCVector<>::save(value_deque, os);
            }

            stool::NaiveFLCVector<> value_deque2;

            {
                std::ifstream ifs;
                ifs.open("flc_vector.bits", std::ios::binary);
                if (!ifs)
                {
                    std::cerr << "Error: Could not open file for reading." << std::endl;
                    throw std::runtime_error("File open error");
                }

                auto tmp = stool::NaiveFLCVector<>::load(ifs);
                value_deque2.swap(tmp);
            }
            std::remove("flc_vector.bits");

            if (value_deque.size() != value_deque2.size())
            {
                value_deque.print_info();
                value_deque2.print_info();
                assert(false);
                throw -1;
            }

            for (uint64_t i = 0; i < value_deque.size(); i++)
            {
                if (value_deque.at(i) != value_deque2.at(i))
                {
                    assert(false);
                    throw -1;
                }
            }
        }
    }
    std::cout << "[DONE]" << std::endl;
}
void load_write_bits_test(uint64_t max_element_count, uint64_t trial_count, [[maybe_unused]] bool detailed_check, uint64_t seed)
{
    std::cout << "load_write_bits_test: " << std::flush;
    std::mt19937_64 mt64(seed);
    std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

    for (uint64_t num = 16; num < max_element_count; num *= 2)
    {
        std::cout << "+" << std::flush;

        for (uint64_t x = 0; x < trial_count; x++)
        {
            stool::NaiveFLCVector<> value_deque;
            for(uint64_t i = 0; i < num; i++){
                value_deque.push_back(get_rand_uni_int(mt64) % (1ULL << 32));
            }

            std::vector<uint8_t> bytes;
            uint64_t pos = 0;

            stool::NaiveFLCVector<>::save(value_deque, bytes, pos);

            pos = 0;

            stool::NaiveFLCVector<> value_deque2 = stool::NaiveFLCVector<>::load(bytes, pos);

            if (value_deque.size() != value_deque2.size())
            {
                assert(false);
                throw -1;
            }

            for (uint64_t i = 0; i < value_deque.size(); i++)
            {
                if (value_deque.at(i) != value_deque2.at(i))
                {
                    assert(false);
                    throw -1;
                }
            }
        }
    }
    std::cout << "[DONE]" << std::endl;
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
    uint64_t max_value = 1000000;

    access_test(seq_len, max_value, number_of_trials, seed);
    psum_test(seq_len, max_value, number_of_trials, seed);
    search_test(seq_len, max_value, number_of_trials, seed);
    insert_and_erase_test(seq_len, max_value, number_of_trials, seed, false);
    push_and_pop_test(seq_len, max_value, number_of_trials, seed, false);
    replace_test(seq_len, max_value, number_of_trials, seed, false);
    load_write_file_test(seq_len, number_of_trials, false, seed);
    load_write_bits_test(seq_len, number_of_trials, false, seed);
    random_test(seq_len, max_value, number_of_trials, 100, seed, false);
}
