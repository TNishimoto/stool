#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <chrono>
#include <random>
#include <bitset>
#include "../../../include/light_stool.hpp"

namespace stool
{
    class DynamicIntegerTest
    {
    public:
        static uint64_t compute_psum(const std::vector<uint64_t> &bv, uint64_t i)
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

        static int64_t compute_search(const std::vector<uint64_t> &bv, uint64_t value)
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

        template <typename INTEGER_CONTAINER>
        static void build_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
        {
            std::cout << "BUILD_TEST \t" << std::endl;
            uint64_t len = 1;

            while (len < max_len)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;
                for (uint64_t i = 0; i < number_of_trials; i++)
                {
                    std::cout << "+" << std::flush;

                    std::vector<uint64_t> values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
                    INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(values);
                    std::vector<uint64_t> test_values = test_container.to_vector();
                    stool::EqualChecker::equal_check(values, test_values, "BUILD_TEST");
                }
                std::cout << std::endl;
                len *= 2;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER>
        static void reverse_psum_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
        {

            std::cout << "REVERSE_PSUM_TEST \t" << std::endl;

            uint64_t len = 1;
            while (len < max_len)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;
                
                for (uint64_t i = 0; i < number_of_trials; i++)
                {
                    std::cout << "+" << std::flush;

                    std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
                    INTEGER_CONTAINER test_values = INTEGER_CONTAINER::build(naive_values);

                    uint64_t rpsum = 0;

                    for (uint64_t i = 0; i < len; i++)
                    {
                        rpsum += naive_values[len - i - 1];
                        if (test_values.reverse_psum(i) != rpsum)
                        {
                            std::cout << "reverse_psum error" << std::endl;
                            std::cout << "rpsum = " << rpsum << std::endl;
                            std::cout << "test_values.psum(i) = " << test_values.psum(i) << std::endl;
                            throw std::runtime_error("psum error");
                        }
                    }
                }
                std::cout << std::endl;

                len *= 2;
            }

            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER>
        static void psum_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
        {
            std::cout << "PSUM_TEST \t" << std::endl;

            if(UINT64_MAX / max_value <= max_len){
                throw std::runtime_error("max_value is too large");

            }


            uint64_t len = 1;
            while (len < max_len)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;
                for (uint64_t i = 0; i < number_of_trials; i++)
                {
                    std::cout << "+" << std::flush;

                    std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
                    INTEGER_CONTAINER test_values = INTEGER_CONTAINER::build(naive_values);

                    uint64_t psum = 0;

                    for (uint64_t i = 0; i < len; i++)
                    {
                        psum += naive_values[i];
                        if (test_values.psum(i) != psum)
                        {
                            std::cout << "psum error" << std::endl;
                            std::cout << "psum = " << psum << std::endl;
                            std::cout << "i = " << i << std::endl;
                            std::cout << "test_values.psum(i) = " << test_values.psum(i) << std::endl;

                            test_values.print_info();
                            throw std::runtime_error("psum error");
                        }
                    }
                }
                std::cout << std::endl;
                len *= 2;
            }

            std::cout << "[DONE]" << std::endl;
        }
        template <typename INTEGER_CONTAINER>
        static void search_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
        {
            std::cout << "SEARCH_TEST \t" << std::endl;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

            std::cout << "max_len = " << max_len << std::endl;
            std::cout << "max_value = " << max_value << std::endl;
            std::cout << "UINT64_MAX / max_value = " << UINT64_MAX / max_value << std::endl;

            if(UINT64_MAX / max_value <= max_len){
                throw std::runtime_error("max_value is too large");

            }


            uint64_t len = 1;
            while (len < max_len)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;
                for (uint64_t i = 0; i < number_of_trials; i++)
                {
                    std::cout << "+" << std::flush;

                    std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
                    INTEGER_CONTAINER test_values = INTEGER_CONTAINER::build(naive_values);

                    for (uint64_t i = 0; i < len; i++)
                    {
                        uint64_t value = get_rand_value(mt) % max_value;
                        int64_t search_result1 = test_values.search(value);
                        int64_t search_result2 = DynamicIntegerTest::compute_search(naive_values, value);

                        if (search_result1 != search_result2)
                        {
                            std::cout << "search error" << std::endl;
                            std::cout << "search_result[TEST   ] = " << search_result1 << std::endl;
                            std::cout << "search_result[CORRECT] = " << search_result2 << std::endl;

                            stool::DebugPrinter::print_integers(naive_values, "naive_values");
                            stool::DebugPrinter::print_integers(test_values.to_vector(), "test_values");


                            throw std::runtime_error("search error");
                        }
                    }
                }
                std::cout << std::endl;
                len *= 2;
            }

            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER>
        static void push_and_pop_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
        {
            std::cout << "PUSH_AND_POP_TEST \t" << std::flush;
            std::vector<uint64_t> naive_values;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT32_MAX);
            for (uint64_t i = 0; i < number_of_trials; i++)
            {
                naive_values.clear();
                INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(naive_values);

                std::cout << "+" << std::flush;
                while (naive_values.size() < max_len)
                {

                    uint64_t value = get_rand_value(mt) % max_value;
                    uint64_t type = get_rand_value(mt) % 5;

                    if (type == 0 || type == 1)
                    {
                        test_container.push_back(value);
                        naive_values.push_back(value);
                    }
                    else if (type == 2 || type == 3)
                    {
                        test_container.push_front(value);
                        naive_values.insert(naive_values.begin(), value);
                    }
                    else if (type == 4 && naive_values.size() > 0)
                    {
                        test_container.pop_back();
                        naive_values.pop_back();
                    }
                    else if (type == 5 && test_container.size() > 0)
                    {
                        test_container.pop_front();
                        naive_values.erase(naive_values.begin());
                    }

                    if (detail_check)
                    {
                        std::vector<uint64_t> test_values = test_container.to_vector();
                        stool::EqualChecker::equal_check(naive_values, test_values, "PUSH_AND_POP_TEST");
                    }
                }

                std::vector<uint64_t> test_values = test_container.to_vector();
                stool::EqualChecker::equal_check(naive_values, test_values, "PUSH_AND_POP_TEST");
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER>
        static void insert_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
        {
            std::cout << "INSERT_TEST \t" << std::endl;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);
            std::vector<uint64_t> naive_values;

            int64_t len = 1;

            while (len < (int64_t)max_len)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;

                for (uint64_t i = 0; i < number_of_trials; i++)
                {
                    std::cout << "+" << std::flush;
                    naive_values.clear();
                    INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(naive_values);

                    for (int64_t x = 0; x < len; x++)
                    {
                        uint64_t new_value = get_rand_value(mt) % max_value;
                        uint64_t pos = get_rand_value(mt) % (naive_values.size() + 1);

                        naive_values.insert(naive_values.begin() + pos, new_value);
                        test_container.insert(pos, new_value);

                        if (detail_check)
                        {
                            try
                            {
                                std::vector<uint64_t> test_values = test_container.to_vector();
                                stool::EqualChecker::equal_check(naive_values, test_values, "INSERT_TEST");
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
                    std::vector<uint64_t> test_values = test_container.to_vector();
                    stool::EqualChecker::equal_check(naive_values, test_values, "INSERT_TEST");
                }
                std::cout << std::endl;
                len *= 2;
            }

            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER>
        static void remove_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
        {

            std::cout << "REMOVE_TEST \t" << std::endl;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

            int64_t len = 1;

            while (len < (int64_t)max_len)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;

                for (uint64_t i = 0; i < number_of_trials; i++)
                {
                    std::cout << "+" << std::flush;
                    std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
                    INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(naive_values);

                    for (int64_t x = 0; x < len; x++)
                    {
                        uint64_t pos = get_rand_value(mt) % naive_values.size();
                        naive_values.erase(naive_values.begin() + pos);
                        test_container.remove(pos);

                        if (detail_check)
                        {
                            try
                            {
                                std::vector<uint64_t> test_values = test_container.to_vector();
                                stool::EqualChecker::equal_check(naive_values, test_values, "REMOVE_TEST");
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
                    std::vector<uint64_t> test_values = test_container.to_vector();
                    stool::EqualChecker::equal_check(naive_values, test_values, "REMOVE_TEST");
                }
                std::cout << std::endl;
                len *= 2;
            }

            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER>
        static void replace_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t seed, bool detail_check = true)
        {

            std::cout << "REPLACE_TEST \t" << std::endl;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

            int64_t len = 1;

            while (len < (int64_t)max_len)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;

                for (uint64_t i = 0; i < number_of_trials; i++)
                {
                    std::cout << "+" << std::flush;
                    std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
                    INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(naive_values);

                    for (int64_t x = 0; x < len; x++)
                    {

                        uint64_t new_value = get_rand_value(mt) % max_value;
                        naive_values[x] = new_value;
                        test_container.set_value(x, new_value);

                        if (detail_check)
                        {
                            try
                            {
                                std::vector<uint64_t> test_values = test_container.to_vector();
                                stool::EqualChecker::equal_check(naive_values, test_values, "REPLACE_TEST");
                            }
                            catch (const std::runtime_error &e)
                            {
                                std::cout << "Replace test error" << std::endl;
                                std::cout << "len = " << len << std::endl;
                                std::cout << "pos = " << x << std::endl;
                                throw e;
                            }
                        }
                    }
                    std::vector<uint64_t> test_values = test_container.to_vector();
                    stool::EqualChecker::equal_check(naive_values, test_values, "REPLACE_TEST");
                }
                std::cout << std::endl;
                len *= 2;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER>
        static void load_write_file_test(uint64_t max_element_count, uint64_t max_value, uint64_t trial_count, uint64_t seed, [[maybe_unused]] bool detailed_check)
        {
            std::cout << "LOAD_AND_WRITE_FILE_TEST: " << std::endl;
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

            for (uint64_t len = 16; len < max_element_count; len *= 2)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;


                for (uint64_t x = 0; x < trial_count; x++)
                {
                    std::cout << "+" << std::flush;

                    std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
                    INTEGER_CONTAINER test_container1 = INTEGER_CONTAINER::build(naive_values);

                    {
                        std::ofstream os;
                        os.open("flc_vector.bits", std::ios::binary);
                        if (!os)
                        {
                            std::cerr << "Error: Could not open file for writing." << std::endl;
                            throw std::runtime_error("File open error");
                        }
                        INTEGER_CONTAINER::save(test_container1, os);
                    }

                    INTEGER_CONTAINER test_container2;

                    {
                        std::ifstream ifs;
                        ifs.open("flc_vector.bits", std::ios::binary);
                        if (!ifs)
                        {
                            std::cerr << "Error: Could not open file for reading." << std::endl;
                            throw std::runtime_error("File open error");
                        }

                        auto tmp = INTEGER_CONTAINER::load(ifs);
                        test_container2.swap(tmp);
                    }
                    std::remove("flc_vector.bits");

                    if (naive_values.size() != test_container2.size())
                    {
                        test_container2.print_info();
                        assert(false);
                        throw -1;
                    }

                    std::vector<uint64_t> test_values = test_container2.to_vector();
                    stool::EqualChecker::equal_check(naive_values, test_values, "LOAD_WRITE_FILE_TEST");
                }
                std::cout << std::endl;
                len *= 2;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER>
        static void load_write_bits_test(uint64_t max_element_count, uint64_t max_value, uint64_t trial_count, uint64_t seed, [[maybe_unused]] bool detailed_check)
        {
            std::cout << "LOAD_AND_WRITE_BITS_TEST: " << std::endl;
            std::mt19937_64 mt64(seed);
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, UINT64_MAX);

            for (uint64_t len = 16; len < max_element_count; len *= 2)
            {
                std::cout << "\t" << "len: " << len << ": " << std::flush;

                for (uint64_t x = 0; x < trial_count; x++)
                {
                    std::cout << "+" << std::flush;

                    std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(len, max_value, seed++);
                    INTEGER_CONTAINER test_container1 = INTEGER_CONTAINER::build(naive_values);

                    std::vector<uint8_t> bytes;
                    uint64_t pos = 0;
                    INTEGER_CONTAINER::save(test_container1, bytes, pos);
                    pos = 0;

                    INTEGER_CONTAINER test_container2 = INTEGER_CONTAINER::load(bytes, pos);

                    if (naive_values.size() != test_container2.size())
                    {
                        test_container2.print_info();
                        assert(false);
                        throw -1;
                    }

                    std::vector<uint64_t> test_values = test_container2.to_vector();
                    stool::EqualChecker::equal_check(naive_values, test_values, "LOAD_WRITE_FILE_TEST");
                }
                std::cout << std::endl;
                len *= 2;
            }
            std::cout << "[DONE]" << std::endl;
        }

        template <typename INTEGER_CONTAINER, bool USE_QUERY = true>
        static void random_test(uint64_t max_len, uint64_t max_value, uint64_t number_of_trials, uint64_t max_counter, uint64_t seed, bool detail_check = false)
        {
            std::cout << "RANDOM_TEST: \t" << std::flush;
            std::mt19937 mt(seed);
            std::uniform_int_distribution<uint64_t> get_rand_value(0, UINT64_MAX);

            for (uint64_t i = 0; i < number_of_trials; i++)
            {
                uint64_t counter = 0;
                std::vector<uint64_t> naive_values = stool::StringGenerator::create_random_integer_sequence(max_len / 2, max_value, seed++);
                INTEGER_CONTAINER test_container = INTEGER_CONTAINER::build(naive_values);

                std::cout << "+" << std::flush;

                while (counter < max_counter)
                {

                    uint64_t type = get_rand_value(mt) % 10;
                    uint64_t random_pos = naive_values.size() > 0 ? (get_rand_value(mt) % naive_values.size()) : 0;
                    uint64_t random_value = get_rand_value(mt) % max_value;

                    if (type == 0 || type == 1)
                    {
                        test_container.push_back(random_value);
                        naive_values.push_back(random_value);
                    }
                    else if (type == 2 || type == 3)
                    {
                        test_container.push_front(random_value);
                        naive_values.insert(naive_values.begin(), random_value);
                    }
                    else if (type == 4 && naive_values.size() > 0)
                    {
                        test_container.pop_back();
                        naive_values.pop_back();
                    }
                    else if (type == 5 && test_container.size() > 0)
                    {
                        test_container.pop_front();
                        naive_values.erase(naive_values.begin());
                    }
                    else if (type == 6 && naive_values.size() < max_len)
                    {
                        test_container.insert(random_pos, random_value);
                        naive_values.insert(naive_values.begin() + random_pos, random_value);
                    }
                    else if (type == 7 && naive_values.size() > 0)
                    {
                        naive_values.erase(naive_values.begin() + random_pos);
                        test_container.remove(random_pos);
                    }
                    else if (type == 8)
                    {
                        if (random_pos < naive_values.size())
                        {
                            naive_values[random_pos] = random_value;
                            test_container.set_value(random_pos, random_value);
                        }
                    }
                    else
                    {
                        if constexpr(USE_QUERY){
                            if (naive_values.size() > 1)
                            {
                                uint64_t psum1A = compute_psum(naive_values, random_pos);
                                uint64_t psum1B = test_container.psum(random_pos);
                                if (psum1A != psum1B)
                                {
                                    std::cout << "psum_test error/" << psum1A << "/" << psum1B << std::endl;
                                    throw std::runtime_error("psum_test error");
                                }
    
                                int64_t search1A = compute_search(naive_values, random_pos);
                                int64_t search1B = test_container.search(random_pos);
                                if (search1A != search1B)
                                {
                                    std::cout << "search_test error/" << search1A << "/" << search1B << std::endl;
                                    throw std::runtime_error("search_test error");
                                }
                            }    
                        }
                    }
                    counter++;

                    if (detail_check)
                    {
                        try
                        {
                            std::vector<uint64_t> test_values = test_container.to_vector();
                            stool::EqualChecker::equal_check(naive_values, test_values, "RANDOM_TEST");
                        }
                        catch (const std::runtime_error &e)
                        {
                            std::cout << "random_test error" << std::endl;
                            std::cout << "type = " << type << std::endl;
                            throw e;
                        }
                    }
                }
                std::vector<uint64_t> test_values = test_container.to_vector();
                stool::EqualChecker::equal_check(naive_values, test_values, "RANDOM_TEST");
            }
            std::cout << "[DONE]" << std::endl;
        }
    };
}
