#pragma once
#include "./fpos_data_structure.hpp"

namespace stool
{
    namespace rlbwt2
    {

        class LightFPosDataStructure
        {
        public:
            const sdsl::int_vector<8> *bwt;
            stool::WT *wt;

            std::vector<stool::EliasFanoVector> efv_vec;
            std::vector<uint64_t> C2;

            LightFPosDataStructure()
            {
            }
            template <typename LPOSVEC>
            void build(const sdsl::int_vector<8> *_bwt, const LPOSVEC &_lposvec, stool::WT *_wt)
            {
                this->bwt = _bwt;
                this->wt = _wt;
                std::cout << "Building Fpos_vec(EliasFanoVector) ..." << std::flush;
                /*
                LightFPosDataStructure::construct_C(bwt, this->C);
                LightFPosDataStructure::construct_sorted_fpos_array(_bwt, _lposvec, this->fposSortedArray);
                */
#if DEBUG
                this->rank_test();
#endif
                this->build(_lposvec);
                std::cout << "[Finished]" << std::endl;

#if DEBUG
                //this->check(_lposvec);
#endif
            }
            /*
            template <typename LPOSVEC>
            void check(const LPOSVEC &_lposvec)
            {

                std::vector<uint64_t> collectVec = FPosDataStructure::construct(*this->bwt, _lposvec);

                for (uint64_t i = 0; i < this->bwt->size(); i++)
                {
                    uint8_t c = (*this->bwt)[i];
                    uint64_t rank1 = wt->rank(i + 1, c);
                    assert(rank1 < this->efv_vec[c].size());
                    uint64_t q = C2[c] + this->efv_vec[c][rank1];

                    uint64_t p = collectVec[i];

                    if (p != q)
                    {
                        std::cout << "LightFPosDataStructure Error, i = " << i << ", Collect = " << p << ", Test = " << q << std::endl;
                        throw -1;
                    }
                }
            }
            */


            static void construct_C(const sdsl::int_vector<8> &bwt_head_chars, std::vector<uint64_t> &C)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                std::vector<uint64_t> CK;
                CK.resize(CHARMAX, 0);
                C.resize(CHARMAX, 0);
                uint64_t rle = bwt_head_chars.size();
                for (uint64_t i = 0; i < rle; i++)
                {
                    uint8_t c = bwt_head_chars[i];
                    // uint64_t l = rlbwt.get_run(i);
                    CK[c] += 1;
                }
                for (uint64_t i = 1; i < CHARMAX; i++)
                {
                    C[i] = C[i - 1] + CK[i - 1];
                }
            }
#if DEBUG
            void rank_test()
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                std::vector<uint64_t> C_run_sum;
                C_run_sum.resize(CHARMAX, 1);
                uint64_t rle = this->bwt->size();
                for (uint64_t i = 0; i < rle; i++)
                {
                    uint8_t c = (*this->bwt)[i];
                    uint64_t rank1 = wt->rank(i + 1, c);
                    assert(C_run_sum[c] == rank1);
                    C_run_sum[c]++;
                }
            }
#endif
            template <typename LPOSVEC>
            void build(const LPOSVEC &lposvec)
            {
                uint64_t CHARMAX = UINT8_MAX + 1;
                std::vector<uint64_t> C_run_sum;
                std::vector<uint64_t> numVec;
                std::vector<uint64_t> numC;

                std::vector<stool::EliasFanoVectorBuilder> builders;

                C_run_sum.resize(CHARMAX, 0);
                numVec.resize(CHARMAX, 0);
                this->C2.resize(CHARMAX, 0);
                this->efv_vec.resize(CHARMAX);
                builders.resize(CHARMAX);
                uint64_t rle = this->bwt->size();

                for (uint64_t i = 0; i < rle; i++)
                {

                    uint8_t c = (*this->bwt)[i];
                    uint64_t l = lposvec[i + 1] - lposvec[i];
                    C_run_sum[c] += l;
                    numVec[c]++;
                }

                for (uint64_t i = 1; i < CHARMAX; i++)
                {
                    this->C2[i] = this->C2[i - 1] + C_run_sum[i - 1];
                }

                for (uint64_t i = 0; i < CHARMAX; i++)
                {
                    builders[i].initialize(C_run_sum[i] + 1, numVec[i]);
                }

                std::vector<uint64_t> tmp_sum;
                tmp_sum.resize(CHARMAX, 0);

                for (uint64_t i = 0; i < rle; i++)
                {
                    uint8_t c = (*this->bwt)[i];
                    // std::cout << (uint64_t)c << "/" << numVec[c]<< std::endl;
                    builders[c].push(tmp_sum[c]);
                    // std::cout << "a" << std::endl;
                    uint64_t l = lposvec[i + 1] - lposvec[i];
                    tmp_sum[c] += l;
                }

                for (uint64_t i = 0; i < CHARMAX; i++)
                {
                    builders[i].finish();
                    this->efv_vec[i].build_from_builder(builders[i]);
                }
            }

            uint64_t operator[](uint64_t i) const
            {

                uint8_t c = (*this->bwt)[i];
                uint64_t rank1 = ((*wt).rank(i + 1, c) - 1);

                assert(rank1 < this->efv_vec[c].size());
                uint64_t q = C2[c] + this->efv_vec[c][rank1];

                return q;
            }
            uint64_t size() const
            {
                return this->bwt->size();
            }
            uint64_t get_using_memory() const
            {
                uint64_t x = 0;

                for (auto &it : this->efv_vec)
                {
                    x += it.get_using_memory();
                }
                return x;
            }
        };
    } // namespace rlbwt2
} // namespace stool