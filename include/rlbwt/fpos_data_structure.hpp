#pragma once
#include "./rank_support_vector_wrapper.hpp"

namespace stool
{
    namespace rlbwt2
    {

        class FPosDataStructure
        {
        public: // std::vector<uint64_t> fposArray;
            /*
            Let R be the output array of length r.
            R[i] stores the index of the L-run corresponding to i-th F-run.
            */
            static std::vector<uint64_t> construct_rle_fl_mapper(const sdsl::int_vector<8> &bwt)
            {
                std::vector<uint64_t> indexes;
                uint64_t size = bwt.size();
                indexes.resize(size);
                for (uint64_t i = 0; i < size; i++)
                {
                    indexes[i] = i;
                }
                sort(indexes.begin(), indexes.end(),
                     [&](const uint64_t &x, const uint64_t &y)
                     {
                         assert(x < bwt.size() && y < bwt.size());
                         if (bwt[x] == bwt[y])
                         {
                             return x < y;
                         }
                         else
                         {
                             return (uint64_t)bwt[x] < (uint64_t)bwt[y];
                         }
                     });
                return indexes;
            }
            /*
            Let R be the output array of length r.
            R[i] stores the starting position of the F-run corresponding to i-th L-run.
            */
            template <typename LPOSDS>
            static std::vector<uint64_t> construct_fpos_array(const sdsl::int_vector<8> &bwt, const LPOSDS &lpos_vec)
            {
                std::vector<uint64_t> fvec = construct_rle_fl_mapper(bwt);
                std::vector<uint64_t> output;
                output.resize(fvec.size(), 0);
                uint64_t fsum = 0;
                for (uint64_t i = 0; i < fvec.size(); i++)
                {
                    uint64_t x = fvec[i];
                    assert(x < output.size());
                    output[x] = fsum;
                    uint64_t run = lpos_vec[x + 1] - lpos_vec[x];
                    /*
                    if(bwt[x] == 97){
                    std::cout << "[" << fsum << ", " << (fsum + run -1) << "], " << std::flush;

                    }
                    */
                    fsum += run;
                }
                return output;
            }
            template <typename LPOSDS>
            static void check(const sdsl::int_vector<8> &bwt, const LPOSDS &lpos_vec, const std::vector<uint64_t> &r)
            {
                std::vector<bool> checker;

                uint64_t size = lpos_vec[lpos_vec.size() - 1];
                checker.resize(size, false);
                for (uint64_t i = 0; i < 256; i++)
                {
                    for (uint64_t x = 0; x < bwt.size(); x++)
                    {

                        if (bwt[x] == i)
                        {
                            uint64_t run = lpos_vec[x + 1] - lpos_vec[x];
                            for (uint64_t p = 0; p < run; p++)
                            {
                                uint64_t y = r[x] + p;
                                assert(y < checker.size());

                                if (y > 0 && !checker[y - 1])
                                {
                                    // std::cout << i << "/" << x << std::endl;
                                    assert(false);
                                }
                                checker[y] = true;
                            }
                        }
                    }
                }

                std::cout << "OK!" << std::endl;
            }
            template <typename LPOSDS>
            static std::vector<uint64_t> construct(const sdsl::int_vector<8> &bwt, const LPOSDS &lpos_vec)
            {
                std::cout << "Building Fpos_vec(std::vector<uint64_t>) ..." << std::flush;
                std::vector<uint64_t> v1 = construct_fpos_array(bwt, lpos_vec);
                std::cout << "[Finished]" << std::endl;

                // check(bwt, lpos_vec, v1);
                return v1;
            }
            /*
            uint64_t &operator[](uint64_t i)
            {
                return fposArray[i];
            }
            */
        };
    } // namespace rlbwt2
} // namespace stool