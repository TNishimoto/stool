#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <bitset>
#include <cassert>
#include <chrono>
#include <random>
#include "../include/specialized_collection/integer_deque.hpp"
#include "../include/debug.hpp"
#include "./vlc_deque_test.hpp"

namespace stool
{
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

            stool::equal_check(deq1, vlc_deque.to_deque());

            while (deq1.size() > 0)
            {
                deq1.pop_back();
                vlc_deque.pop_back();
                stool::equal_check(deq1, vlc_deque.to_deque());

            }
            stool::equal_check(deq1, vlc_deque.to_deque());
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

                // stool::equal_check(deq1, deq2);
            }

            std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            stool::equal_check(deq1, deq2);
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

                // stool::equal_check(deq1, deq2);
            }

            std::deque<uint64_t> deq2 = vlc_deque.to_deque();
            stool::equal_check(deq1, deq2);
        }
    };

}
