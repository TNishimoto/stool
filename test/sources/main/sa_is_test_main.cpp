#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../../include/strings/sa_is.hpp"

// Reference implementation: sort all suffixes with plain comparison.
template <class C>
std::vector<uint64_t> naive_suffix_array(const std::vector<C> &text)
{
    std::vector<uint64_t> sa(text.size());
    for (size_t i = 0; i < text.size(); ++i)
    {
        sa[i] = i;
    }
    std::sort(sa.begin(), sa.end(), [&](uint64_t a, uint64_t b) {
        return std::lexicographical_compare(
            text.begin() + a, text.end(),
            text.begin() + b, text.end());
    });
    return sa;
}

template <class C>
void check_against_naive(const std::vector<C> &text, bool signed_version, const std::string &label)
{
    std::vector<uint64_t> sa = signed_version
                                   ? stool::sais_suffix_array_for_signed_characters(text)
                                   : stool::sais_suffix_array(text);
    std::vector<uint64_t> expected = naive_suffix_array(text);

    if (sa != expected)
    {
        std::cerr << "[NG] " << label << ": suffix array mismatch (n=" << text.size() << ")" << std::endl;
        assert(false);
    }
}

void test_banana()
{
    std::cout << "[Test] sais_suffix_array on \"banana\" ..." << std::endl;
    std::vector<char> s = {'b', 'a', 'n', 'a', 'n', 'a'};
    std::vector<uint64_t> sa = stool::sais_suffix_array(s);
    // Suffixes sorted: a(5), ana(3), anana(1), banana(0), na(4), nana(2)
    std::vector<uint64_t> expected = {5, 3, 1, 0, 4, 2};
    assert(sa == expected);
    std::cout << "[OK] banana test passed" << std::endl;
}

void test_edge_cases()
{
    std::cout << "[Test] edge cases (empty / single / repeated) ..." << std::endl;

    std::vector<uint8_t> empty;
    assert(stool::sais_suffix_array(empty).empty());
    assert(stool::sais_suffix_array_for_signed_characters(std::vector<int64_t>{}).empty());

    std::vector<uint8_t> one = {42};
    assert(stool::sais_suffix_array(one) == std::vector<uint64_t>{0});
    assert(stool::sais_suffix_array_for_signed_characters(std::vector<int64_t>{-42}) == std::vector<uint64_t>{0});

    // aaaa...a: suffix array must be n-1, n-2, ..., 0
    std::vector<uint8_t> rep(100, 'a');
    std::vector<uint64_t> sa = stool::sais_suffix_array(rep);
    for (size_t i = 0; i < rep.size(); ++i)
    {
        assert(sa[i] == rep.size() - 1 - i);
    }

    std::cout << "[OK] edge case test passed" << std::endl;
}

void test_unsigned_random(uint64_t trials, uint64_t max_len, uint64_t seed)
{
    std::cout << "[Test] sais_suffix_array: random texts vs naive ..." << std::endl;
    std::mt19937_64 mt(seed);

    for (uint64_t t = 0; t < trials; ++t)
    {
        uint64_t n = mt() % (max_len + 1);
        // Mix small and large alphabets.
        uint64_t sigma = 1 + (mt() % ((t % 2 == 0) ? 4 : 1000));

        std::vector<uint64_t> text(n);
        for (auto &c : text)
        {
            c = mt() % sigma;
        }
        check_against_naive(text, false, "unsigned random");
    }
    std::cout << "[OK] unsigned random test passed (" << trials << " trials)" << std::endl;
}

void test_signed_random(uint64_t trials, uint64_t max_len, uint64_t seed)
{
    std::cout << "[Test] sais_suffix_array_for_signed_characters: random texts with negatives vs naive ..." << std::endl;
    std::mt19937_64 mt(seed);

    for (uint64_t t = 0; t < trials; ++t)
    {
        uint64_t n = mt() % (max_len + 1);
        int64_t range = 1 + (int64_t)(mt() % ((t % 2 == 0) ? 3 : 500));

        std::vector<int64_t> text(n);
        for (auto &c : text)
        {
            // Values in [-range, range], always including negatives.
            c = (int64_t)(mt() % (2 * (uint64_t)range + 1)) - range;
        }
        check_against_naive(text, true, "signed random");
    }
    std::cout << "[OK] signed random test passed (" << trials << " trials)" << std::endl;
}

void test_signed_extreme_values()
{
    std::cout << "[Test] signed version with INT64_MIN/INT64_MAX ..." << std::endl;
    std::vector<int64_t> text = {
        INT64_MAX, INT64_MIN, 0, -1, 1, INT64_MIN, INT64_MAX, -1};
    check_against_naive(text, true, "signed extreme values");
    std::cout << "[OK] signed extreme value test passed" << std::endl;
}

void test_signed_matches_unsigned_on_nonnegative(uint64_t trials, uint64_t max_len, uint64_t seed)
{
    std::cout << "[Test] signed and unsigned versions agree on non-negative input ..." << std::endl;
    std::mt19937_64 mt(seed);

    for (uint64_t t = 0; t < trials; ++t)
    {
        uint64_t n = mt() % (max_len + 1);
        std::vector<int64_t> text(n);
        for (auto &c : text)
        {
            c = (int64_t)(mt() % 256);
        }
        assert(stool::sais_suffix_array(text) == stool::sais_suffix_array_for_signed_characters(text));
    }
    std::cout << "[OK] signed/unsigned agreement test passed (" << trials << " trials)" << std::endl;
}

int main()
{
    std::cout << "\033[34mTest: SA-IS\033[0m" << std::endl;
    test_banana();
    test_edge_cases();
    test_unsigned_random(200, 300, 20260717);
    test_signed_random(200, 300, 424242);
    test_signed_extreme_values();
    test_signed_matches_unsigned_on_nonnegative(100, 300, 777);
    std::cout << "All SA-IS tests passed!" << std::endl;
    return 0;
}
