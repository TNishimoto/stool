#include <cassert>
#include <chrono>
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
#include "../../../include/light_stool.hpp"
#include "../../../include/develop/integer_sketch8.hpp"

/*
int64_t naive_predecessor(const std::vector<uint64_t> &sequence, uint64_t v)
{
    for (int64_t i = sequence.size() - 1; i >= 0; i--)
    {
        if (sequence[i] < v)
        {
            return i;
        }
    }
    return -1;
}
*/
int64_t naive_successor(const std::vector<uint64_t> &sequence, uint64_t v)
{
    for (int64_t i = 0; i < (int64_t)sequence.size(); i++)
    {
        if (sequence[i] >= v)
        {
            return i;
        }
    }
    return -1;
}


void successor_test(uint64_t sequence_length, uint64_t max_value, uint64_t number_of_trials, uint64_t seed)
{
    std::mt19937_64 mt64(seed);
    std::uniform_int_distribution<uint64_t> get_rand_value(0, max_value);

    std::cout << "successor_test: " << "sequence_length = " << sequence_length << ", max_value = " << max_value << ", number_of_trials = " << number_of_trials << ", seed = " << seed << std::flush;

    for (uint64_t i = 0; i < number_of_trials; i++)
    {
        std::vector<uint64_t> sequence = stool::StringGenerator::create_random_integer_sequence(sequence_length, max_value, seed++);
        std::sort(sequence.begin(), sequence.end());

        stool::IntegerSketch8 sketch = stool::IntegerSketch8::build(sequence);
        uint64_t v = get_rand_value(mt64);
        int64_t successor = sketch.successor(v, sequence);
        int64_t correct_successor = naive_successor(sequence, v);
        if (successor != correct_successor)
        {
            sketch.print(sequence);

            std::cout << "error" << std::endl;
            stool::DebugPrinter::print_integers(sequence, "sequence");

            std::cout << "v = " << v << ", successor = " << successor << ", correct_successor = " << correct_successor << std::endl;
            throw std::runtime_error("error");
        }
    }
    std::cout << "[DONE]" << std::endl;
}

int main()
{
    uint64_t seed = 0;
    for(int64_t i = 0; i <= 8; i++){
        successor_test(i, 10, 10000, seed++);
        successor_test(i, 100, 10000, seed++);
        successor_test(i, UINT64_MAX, 1000000, seed++);
    }
    return 0;
}