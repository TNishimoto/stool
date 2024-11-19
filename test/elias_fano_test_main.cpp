#include <cassert>
#include <chrono>
#include <algorithm>
#include <vector>
#include <list>
#include <iterator>
//#include "../../include/io.hpp"
//#include "../../include/cmdline.h"
#include "../include/debug.hpp"
#include "../include/specialized_collection/value_array.hpp"
#include "../include/specialized_collection/elias_fano_vector.hpp"

bool SHOW = false;

std::vector<uint64_t> create_random_integer_vector(uint64_t max, uint64_t ratio)
{
    std::vector<uint64_t> r;
    std::random_device rnd;
    for (uint64_t i = 0; i < max; ++i)
    {
        if (rnd() % ratio == 0)
        {
            r.push_back(i);
        }
    }
    return r;
}

uint64_t naive_rank(std::vector<uint64_t> &sorted_items, uint64_t value)
{
    uint64_t count = 0;
    for (uint64_t i = 0; i < sorted_items.size(); i++)
    {
        if (sorted_items[i] < value)
        {
            count++;
        }
        else
        {
            return count;
        }
    }
    return count;
}
uint64_t rank2(std::vector<uint64_t> &sorted_items, uint64_t value)
{
    auto p2 = std::lower_bound(sorted_items.begin(), sorted_items.end(), value);
    uint64_t pos2 = std::distance(sorted_items.begin(), p2);

    return pos2;
}

uint64_t rank3(std::vector<uint64_t> &sorted_items, uint64_t value)
{
    uint64_t x = rank2(sorted_items, value + 1) - 1;
    auto p = std::upper_bound(sorted_items.begin(), sorted_items.end(), value);
    uint64_t pos = std::distance(sorted_items.begin(), p) - 1;
    /*
    if(pos == UINT64_MAX){
        pos = 0;
    }
    */
    if (x != pos)
    {
        std::cout << "RANK ERRO" << x << ", " << pos << std::endl;
        throw -1;
    }
    assert(x == pos);
    return pos;
}

void rankTest()
{
    std::vector<uint64_t> r = create_random_integer_vector(30000, 3000);
    stool::Printer::print(r);

    stool::EliasFanoVector efs;
    efs.construct(&r);

    std::cout << "Rank Test" << std::endl;
    uint64_t value1 = 10000;

    auto p1 = std::upper_bound(r.begin(), r.end(), value1);
    uint64_t pos1 = std::distance(r.begin(), p1) - 1;
    std::cout << r[pos1] << " <= " << value1 << " <= " << r[pos1 + 1] << std::endl;

    auto p2 = std::upper_bound(efs.begin(), efs.end(), value1);
    uint64_t pos2 = std::distance(efs.begin(), p2) - 1;
    std::cout << r[pos2] << " <= " << value1 << " <= " << r[pos2 + 1] << std::endl;

    efs.print();
    /*
    efs2.rank(value1);
    efs2.rank(efs2.access(0)+1);
    */

    std::vector<uint64_t> test_r = create_random_integer_vector(50000, 3000);
    for (auto &i : test_r)
    {
        uint64_t collect_value = rank2(r, i);
        uint64_t test_value = efs.rank(i);
        rank3(r, i);
        std::cout << i << ", " << test_value << ", " << collect_value << std::endl;

        if (test_value != collect_value)
        {
            std::cout << "rank Error" << std::endl;
            throw -1;
        }
    }
}
void builderTest()
{
    std::vector<uint64_t> r = create_random_integer_vector(30000, 300);
    stool::Printer::print(r);


    uint64_t max_value = r[r.size()-1];
    stool::EliasFanoVectorBuilder builder;
    builder.initialize(max_value, r.size());

    for(auto &it: r){
        builder.push(it);
    }
    builder.finish();    

    std::vector<uint64_t> r2;    
    builder.to_vector(r2);

    stool::equal_check(r, r2);

    stool::EliasFanoVector efv;
    efv.build_from_builder(builder);

        

    std::vector<uint64_t> r3 = efv.to_vector();
    stool::equal_check(r, r3);

    std::cout << "OK!" << std::endl;

}

int main()
{
    /*
    std::vector<uint64_t> r = create_random_integer_vector(30000, 3000);
    stool::Printer::print(r);

    stool::ValueArray va;
    va.set(r, true);

    std::vector<uint64_t> r2;
    va.decode(r2);
    stool::Printer::print(r2);

    stool::EliasFanoVector efs;
    efs.construct(&r2);

    stool::EliasFanoVector efs2(std::move(efs));

    std::vector<uint64_t> r3 = efs2.to_vector();
    stool::Printer::print(r3);

    std::vector<uint64_t> r4;
    for (auto it : efs2)
        r4.push_back(it);
    stool::Printer::print(r4);

    */

    //rankTest();
    builderTest();
}
