#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>

#include "../../include/stool.hpp"

namespace stool{

    class VLCDequeTest
    {
    public:
        static std::deque<uint64_t> create_sequence(uint64_t num, int64_t max_value, std::mt19937_64 &mt64)
        {
            std::deque<uint64_t> r;
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, max_value - 1);
            while (r.size() < (size_t)num)
            {
                r.push_back(get_rand_uni_int(mt64));
            }
            return r;
        }

        static void test1(uint64_t num, int64_t max_value, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
            stool::VLCDeque vlc_deque;
            for (uint64_t it : deq1)
            {
                vlc_deque.push_back(it);
                // vlc_deque.print();

                //stool::Printer::print("DEQ2", vlc_deque.to_vector());
                //vlc_deque.print();
                
            }
            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());

            while (deq1.size() > 0)
            {
                deq1.pop_back();
                vlc_deque.pop_back();
            }


            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());
        }
        static void test2(uint64_t num, int64_t max_value, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
            stool::VLCDeque vlc_deque;
            for (int64_t i = deq1.size() - 1; i >= 0; i--)
            {
                vlc_deque.push_front(deq1[i]);
            }
            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());

            while (deq1.size() > 0)
            {
                deq1.pop_front();
                vlc_deque.pop_front();
            }
            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());
        }

        static void test3(uint64_t num, int64_t max_value, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::deque<uint64_t> deq1;
            stool::VLCDeque vlc_deque;
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, max_value - 1);
            std::uniform_int_distribution<uint64_t> get_rand_uni_pos(0, num);

            // std::cout << "TEST: " << std::endl;

            while (deq1.size() < (size_t)num)
            {
                uint64_t pos = get_rand_uni_pos(mt64) % (deq1.size() + 1);
                uint64_t value = get_rand_uni_int(mt64);
                deq1.insert(deq1.begin() + pos, value);
                vlc_deque.insert(pos, value);

                // std::deque<uint64_t> deq2 = vlc_deque.to_deque();
                // stool::Printer::print("Deq1", deq1);
                // stool::Printer::print("Deq2", deq2);

                // stool::EqualChecker::equal_check(deq1, deq2);
            }

            std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            stool::EqualChecker::equal_check(deq1, deq2);
        }
        static void test4(uint64_t num, int64_t max_value, uint64_t seed)
        {

            std::mt19937_64 mt64(seed);
            std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
            stool::VLCDeque vlc_deque;
            for (uint64_t it : deq1)
            {
                vlc_deque.push_back(it);
            }
            std::uniform_int_distribution<uint64_t> get_rand_uni_pos(0, num);

            // std::cout << "TEST: " << std::endl;

                //vlc_deque.print();

            while (deq1.size() > 0)
            {
                uint64_t pos = get_rand_uni_pos(mt64) % deq1.size();
                deq1.erase(deq1.begin() + pos);
                vlc_deque.remove(pos);

                 //std::deque<uint64_t> deq2 = vlc_deque.to_deque();
                //stool::Printer::print("Deq1", deq1);
                //stool::Printer::print("Deq2", deq2);

                 //stool::EqualChecker::equal_check(deq1, deq2);
            }

            std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            stool::EqualChecker::equal_check(deq1, deq2);
        }
    };

class IntegerDequeTest
{
public:
    static void test1(uint64_t num, int64_t max_value, uint64_t seed)
    {
        std::cout << "test1" << std::endl;
        std::mt19937_64 mt64(seed);
        std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
        stool::IntegerDeque<uint64_t> vlc_deque;
            stool::Printer::print(deq1);

        for (uint64_t it : deq1)
        {
            //vlc_deque.push_back(0);
            //vlc_deque[vlc_deque.size()-1] = it;
            
            vlc_deque.push_back(it);
            //vlc_deque.print_info();
            //stool::Printer::print(vlc_deque.to_deque());

        }

        stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());

        while (deq1.size() > 0)
        {
            deq1.pop_back();
            vlc_deque.pop_back();
            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());

        }
        stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());
    }
    static void test2(uint64_t num, int64_t max_value, uint64_t seed)
    {
        std::mt19937_64 mt64(seed);
        std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
        stool::IntegerDeque<uint64_t> vlc_deque;
        for (int64_t i = deq1.size() - 1; i >= 0; i--)
        {
            vlc_deque.push_front(deq1[i]);

        }

        stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());

        while (deq1.size() > 0)
        {
            deq1.pop_front();
            vlc_deque.pop_front();

        }
        stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());
    }

    static void test3(uint64_t num, int64_t max_value, uint64_t seed)
    {
        std::mt19937_64 mt64(seed);
        std::deque<uint64_t> deq1;
        stool::IntegerDeque<uint64_t> vlc_deque;
        std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, max_value - 1);
        std::uniform_int_distribution<uint64_t> get_rand_uni_pos(0, num);

        // std::cout << "TEST: " << std::endl;

        while (deq1.size() < (size_t)num)
        {
            uint64_t pos = get_rand_uni_pos(mt64) % (deq1.size() + 1);
            uint64_t value = get_rand_uni_int(mt64);
            deq1.insert(deq1.begin() + pos, value);
            vlc_deque.insert(pos, value);

            // std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            // stool::Printer::print("Deq1", deq1);
            // stool::Printer::print("Deq2", deq2);

            // stool::EqualChecker::equal_check(deq1, deq2);
        }

        std::deque<uint64_t> deq2 = vlc_deque.to_deque();
        stool::EqualChecker::equal_check(deq1, deq2);
    }
    static void test4(uint64_t num, int64_t max_value, uint64_t seed)
    {

        std::mt19937_64 mt64(seed);
        std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
        stool::IntegerDeque<uint64_t> vlc_deque;
        for (uint64_t it : deq1)
        {
            vlc_deque.push_back(it);
        }
        std::uniform_int_distribution<uint64_t> get_rand_uni_pos(0, num);

        // std::cout << "TEST: " << std::endl;

        // vlc_deque.print();

        while (deq1.size() > 0)
        {
            uint64_t pos = get_rand_uni_pos(mt64) % deq1.size();
            deq1.erase(deq1.begin() + pos);
            vlc_deque.erase(pos);

            // std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            // stool::Printer::print("Deq1", deq1);
            // stool::Printer::print("Deq2", deq2);

            // stool::EqualChecker::equal_check(deq1, deq2);
        }

        std::deque<uint64_t> deq2 = vlc_deque.to_deque();
        stool::EqualChecker::equal_check(deq1, deq2);
    }
};

class SimpleDequeTest
    {
    public:
        static void test1(uint64_t num, int64_t max_value, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
            stool::SimpleDeque<uint64_t> vlc_deque;
            for (uint64_t it : deq1)
            {
                //vlc_deque.push_back(0);
                //vlc_deque[vlc_deque.size()-1] = it;
                //stool::Printer::print(vlc_deque.to_deque());
                
                vlc_deque.push_back(it);
            }

            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());

            while (deq1.size() > 0)
            {
                deq1.pop_back();
                vlc_deque.pop_back();
            }
            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());
        }
        static void test2(uint64_t num, int64_t max_value, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
            stool::SimpleDeque<uint64_t> vlc_deque;
            for (int64_t i = deq1.size() - 1; i >= 0; i--)
            {
                vlc_deque.push_front(deq1[i]);

            }

            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());

            while (deq1.size() > 0)
            {
                deq1.pop_front();
                vlc_deque.pop_front();

            }
            stool::EqualChecker::equal_check(deq1, vlc_deque.to_deque());
        }

        static void test3(uint64_t num, int64_t max_value, uint64_t seed)
        {
            std::mt19937_64 mt64(seed);
            std::deque<uint64_t> deq1;
            stool::SimpleDeque<uint64_t> vlc_deque;
            std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, max_value - 1);
            std::uniform_int_distribution<uint64_t> get_rand_uni_pos(0, num);

            // std::cout << "TEST: " << std::endl;

            while (deq1.size() < (size_t)num)
            {
                uint64_t pos = get_rand_uni_pos(mt64) % (deq1.size() + 1);
                uint64_t value = get_rand_uni_int(mt64);
                deq1.insert(deq1.begin() + pos, value);
                vlc_deque.insert(pos, value);

                // std::deque<uint64_t> deq2 = vlc_deque.to_deque();
                // stool::Printer::print("Deq1", deq1);
                // stool::Printer::print("Deq2", deq2);

                // stool::EqualChecker::equal_check(deq1, deq2);
            }

            std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            stool::EqualChecker::equal_check(deq1, deq2);
        }
        static void test4(uint64_t num, int64_t max_value, uint64_t seed)
        {

            std::mt19937_64 mt64(seed);
            std::deque<uint64_t> deq1 = VLCDequeTest::create_sequence(num, max_value, mt64);
            stool::SimpleDeque<uint64_t> vlc_deque;
            for (uint64_t it : deq1)
            {
                vlc_deque.push_back(it);
            }
            std::uniform_int_distribution<uint64_t> get_rand_uni_pos(0, num);

            // std::cout << "TEST: " << std::endl;

            // vlc_deque.print();

            while (deq1.size() > 0)
            {
                uint64_t pos = get_rand_uni_pos(mt64) % deq1.size();
                deq1.erase(deq1.begin() + pos);
                vlc_deque.erase(pos);

                // std::deque<uint64_t> deq2 = vlc_deque.to_deque();
                // stool::Printer::print("Deq1", deq1);
                // stool::Printer::print("Deq2", deq2);

                // stool::EqualChecker::equal_check(deq1, deq2);
            }

            std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            stool::EqualChecker::equal_check(deq1, deq2);
        }
    };
}
int main(int argc, char *argv[])
{
#ifdef DEBUG
    std::cout << "\033[41m";
    std::cout << "DEBUG MODE!" << std::endl;
    std::cout << "\e[m" << std::endl;
    // std::cout << "\033[30m" << std::endl;
#endif
#ifdef SLOWDEBUG
    std::cout << "\033[41m";
    std::cout << "SLOWDEBUG MODE!" << std::endl;
    std::cout << "\e[m" << std::endl;
    // std::cout << "\033[30m" << std::endl;
#endif

    cmdline::parser p;

    // p.add<std::string>("input_file", 'i', "input file name", true);
    p.add<uint>("mode", 'm', "mode", true);

    p.parse_check(argc, argv);
    uint64_t mode = p.get<uint>("mode");


    if (mode == 0)
    {
    }    
    else if (mode == 5)
    {
        uint64_t num = 16;
        uint64_t seed = 0;
        for (uint64_t i = 0; i < 10; i++)
        {
            std::cout << "Num: " << num << std::flush;
            uint64_t max_value = 2;
            for (uint64_t j = 0; j < 10; j++)
            {
                for (uint64_t x = 0; x < 10; x++)
                {
                    stool::VLCDequeTest::test1(num, max_value, seed);

                    stool::VLCDequeTest::test2(num, max_value, seed);
                    stool::VLCDequeTest::test3(num, max_value, seed);
                    stool::VLCDequeTest::test4(num, max_value, seed);

                    std::cout << "+" << std::flush;
                    ++seed;
                }
                std::cout << "/" << std::flush;
                max_value *= 2;
            }
            std::cout << std::endl;

            num *= 2;
        }
    }
    else if (mode == 6)
    {
        uint64_t num = 16;
        uint64_t seed = 0;
        for (uint64_t i = 0; i < 10; i++)
        {
            std::cout << "Num: " << num << std::flush;
            uint64_t max_value = 2;
            if (num > 10000)
            {
                break;
            }
            for (uint64_t j = 0; j < 10; j++)
            {

                for (uint64_t x = 0; x < 10; x++)
                {

                    stool::SimpleDequeTest::test1(num, max_value, seed);
                    stool::SimpleDequeTest::test2(num, max_value, seed);
                    stool::SimpleDequeTest::test3(num, max_value, seed);
                    stool::SimpleDequeTest::test4(num, max_value, seed);

                    std::cout << "+" << std::flush;
                    ++seed;
                }
                std::cout << "/" << std::flush;
                max_value *= 2;
            }
            std::cout << std::endl;

            num *= 2;
        }
    }
    else if (mode == 9)
    {
        uint64_t num = 16;
        uint64_t seed = 0;
        for (uint64_t i = 0; i < 5; i++)
        {
            std::cout << "Num: " << num << std::flush;
            uint64_t max_value = 2;
            if (num > 10000)
            {
                break;
            }
            for (uint64_t j = 0; j < 3; j++)
            {

                for (uint64_t x = 0; x < 5; x++)
                {

                    stool::IntegerDequeTest::test1(num, max_value, seed);
                    //stool::IntegerDequeTest::test2(num, max_value, seed);
                    //stool::IntegerDequeTest::test3(num, max_value, seed);
                    //stool::IntegerDequeTest::test4(num, max_value, seed);

                    std::cout << "+" << std::flush;
                    ++seed;
                }
                std::cout << "/" << std::flush;
                max_value = max_value << 8;
            }
            std::cout << std::endl;

            num *= 2;
        }
    }
    else if (mode == 7)
    {
        uint64_t length = 10000;
        using T = stool::VLCDeque;
        std::vector<T *> vec;
        vec.resize(length);

        for (uint64_t i = 0; i < length; i++)
        {
            vec[i] = new T();
            for (uint64_t j = 0; j < 100; j++)
            {
                vec[i]->push_back(length);
            }
        }

        std::cout << "Memory: " << (vec.capacity() * sizeof(T *) + (vec.size() * vec[0]->size_in_bytes())) << " bytes" << std::endl;

        stool::Memory::print_memory_usage();
        /*
        for(uint64_t i = 0; i < length;i++){
            delete vec[i];
        }
        */
    }
}
