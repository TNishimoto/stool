#include "sa_bwt_lcp.hpp"
#include <vector>
#include <iostream>



namespace stool
{

template <typename CHAR,typename INDEX>
std::vector<INDEX> constructSA(std::vector<CHAR> &text)
{
    std::vector<INDEX> sa;
    
    uint64_t n = text.size();
    sa.resize(n);

    divsufsort64((const unsigned char *)&text[0], (int64_t *)&sa[0], n);
    return sa;
    
}
template std::vector<uint64_t> stool::constructSA<uint8_t,uint64_t>(std::vector<uint8_t>&);

template <typename CHAR,typename INDEX>
std::vector<INDEX> constructISA(std::vector<CHAR> &text, std::vector<INDEX> &sa)
{
    std::vector<INDEX> isa;
    uint64_t n = text.size();
    isa.resize(n);

    for (uint64_t i = 0; i < text.size(); ++i)
    {
        isa[sa[i]] = i;
    }
    return isa;
}
template std::vector<uint64_t> stool::constructISA<uint8_t,uint64_t>(std::vector<uint8_t>&, std::vector<uint64_t>&);

/*
void constructSA(string &text, vector<uint64_t> &sa, vector<uint64_t> &isa)
{

    std::cout << "constructing Suffix Array...";
    uint64_t n = text.size();
    isa.resize(n);
    sa.resize(n);

    int64_t *SA = (int64_t *)malloc(n * sizeof(int64_t));
    divsufsort64((const unsigned char *)&text[0], SA, n);

    for (uint64_t i = 0; i < text.size(); ++i)
    {
        sa[i] = SA[i];
        isa[SA[i]] = i;
    }
    free(SA);
    std::cout << "[END]" << std::endl;
}
*/
template <typename CHAR,typename INDEX>
std::vector<INDEX> constructLCP(std::vector<CHAR> &text, std::vector<INDEX> &sa, std::vector<INDEX> &isa)
{
    std::vector<INDEX> lcp;
    lcp.resize(text.size(), 0);
    INDEX n = text.size();
    INDEX k = 0;
    for (INDEX i = 0; i < n; i++)
    {

        if (i % 10000000 == 0)
            std::cout << "\r"
                      << "constructing LCP Array : [" << i << "/" << n << "]" << std::flush;

        INDEX x = isa[i];
        if (x == 0)
        {
            lcp[x] = 0;
        }
        else
        {
            while (text[sa[x] + k] == text[sa[x - 1] + k])
            {
                k++;
            }
        }
        lcp[x] = k;
        if (k > 0)
            k--;
    }
    std::cout << "[END]"<< std::endl;
    return lcp;
}
template std::vector<uint64_t> constructLCP(std::vector<uint8_t> &, std::vector<uint64_t> &, std::vector<uint64_t> &);

template <typename CHAR,typename INDEX>
std::vector<INDEX> constructLCP(std::vector<CHAR> &text, std::vector<INDEX> &sa)
{
    std::vector<INDEX> isa = stool::constructISA<CHAR,INDEX>(text,sa);
    return constructLCP<CHAR,INDEX>(text,sa,isa);
    //lcp.resize(text.size(), 0);

}
template std::vector<uint64_t> constructLCP(std::vector<uint8_t> &, std::vector<uint64_t> &);

template <typename CHAR,typename INDEX>
std::vector<CHAR> constructBWT(std::vector<CHAR> &text, std::vector<INDEX> &sa){    
    std::vector<CHAR> bwt;
    bwt.resize(text.size());
    INDEX n = text.size();
    for(INDEX i=0;i<text.size();i++){
        if(sa[i] == 0){
            bwt[i] = text[n-1];
        }else{
            bwt[i] = text[sa[i]-1];
        }
    }
    return bwt;
}
template std::vector<uint8_t> constructBWT(std::vector<uint8_t> &, std::vector<uint64_t> &); 
template std::vector<char> constructBWT(std::vector<char> &, std::vector<uint64_t> &); 
} // namespace stool