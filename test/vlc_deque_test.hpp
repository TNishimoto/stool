#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
#include <random>
#include "../include/light_stool.hpp"


namespace stool
{
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
            stool::equal_check(deq1, vlc_deque.to_deque());

            while (deq1.size() > 0)
            {
                deq1.pop_back();
                vlc_deque.pop_back();
            }


            stool::equal_check(deq1, vlc_deque.to_deque());
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
            stool::equal_check(deq1, vlc_deque.to_deque());

            while (deq1.size() > 0)
            {
                deq1.pop_front();
                vlc_deque.pop_front();
            }
            stool::equal_check(deq1, vlc_deque.to_deque());
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

                // stool::equal_check(deq1, deq2);
            }

            std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            stool::equal_check(deq1, deq2);
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

                 //stool::equal_check(deq1, deq2);
            }

            std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            stool::equal_check(deq1, deq2);
        }
    };

}
