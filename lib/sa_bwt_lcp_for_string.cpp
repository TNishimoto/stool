#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include "../include/sa_bwt_lcp_for_string.hpp"

namespace stool
{

    std::vector<uint64_t> constructISA(const std::string &text, const std::vector<uint64_t> &sa)
    {
        std::vector<uint64_t> isa;
        uint64_t n = text.size();
        isa.resize(n);

        for (uint64_t i = 0; i < text.size(); ++i)
        {
            isa[sa[i]] = i;
        }
        return isa;
    }

    std::vector<uint64_t> constructLCP(const std::string &text, const std::vector<uint64_t> &sa, const std::vector<uint64_t> &isa)
    {
        std::vector<uint64_t> lcp;
        lcp.resize(text.size(), 0);
        uint64_t n = text.size();
        uint64_t k = 0;
        if (text.size() > 1000000)
            std::cout << "Constructing LCP Array" << std::flush;

        for (uint64_t i = 0; i < n; i++)
        {

            assert(i < n);
            uint64_t x = isa[i];
            assert(x < n);

            if (x == 0)
            {
                lcp[x] = 0;
            }
            else
            {
                while (sa[x] + k < n && sa[x - 1] + k < n && text[sa[x] + k] == text[sa[x - 1] + k])
                {
                    assert(sa[x] + k < n);
                    assert(sa[x - 1] + k < n);

                    k++;
                }
            }
            lcp[x] = k;

            assert(x == 0 || (x > 0 && ((n - sa[x - 1]) >= k)));

            if (k > 0)
                k--;
        }

        if (n > 1000000)
            std::cout << "[END]" << std::endl;
        return lcp;
    }

} // namespace stool