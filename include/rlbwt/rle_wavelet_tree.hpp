#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
// #include "./rinterval_storage.hpp"
#include "./light_fpos_data_structure.hpp"
#include "./rle.hpp"

namespace stool
{
    namespace rlbwt2
    {

        // template <typename INDEX_SIZE>
        template <typename RLBWT, typename FPOS_TYPE = stool::rlbwt2::LightFPosDataStructure>
        class LFDataStructure
        {
        public:
            using INDEX = uint64_t;
            using UCHAR = typename std::make_unsigned<typename RLBWT::char_type>::type;
            using CHAR_VEC = sdsl::int_vector<>;

        private:
            RLBWT *rlbwt;
            FPOS_TYPE *fpos_array;

            // stool::WT wt;

        public:
            // RLEWaveletTree(stool::rlbwt2::RLE<CHAR> *_rlbwt, string inputFile) : rlbwt(_rlbwt)

            LFDataStructure(RLBWT *_rlbwt, FPOS_TYPE *_fpos_array) : rlbwt(_rlbwt), fpos_array(_fpos_array)
            {
            }

            INDEX get_fpos(INDEX index, INDEX diff) const
            {
                return (*fpos_array)[index] + diff;
            }

            uint64_t lf(INDEX i) const
            {
                uint64_t x = this->rlbwt->get_lindex_containing_the_position(i);
                uint64_t pos = this->rlbwt->get_lpos(x);
                uint64_t lf = this->get_fpos(x, i - pos);
                // std::cout << "[" << i << ", " << x << ", " << pos << ", " << lf << "]" << std::endl;
                return lf;
            }

            bool verifyBWT() const
            {
                std::cout << "verify BWT" << std::flush;
                uint64_t len = this->rlbwt->str_size();
                std::vector<bool> checker;
                checker.resize(len, false);
                uint64_t pos = this->rlbwt->get_end_marker_position();

                for (int64_t i = 0; i < len; i++)
                {
                    if (checker[pos])
                    {
                        throw std::logic_error("Error: DynamicRLBWT::verify(), LF Error");
                    }
                    checker[pos] = true;

                    pos = this->lf(pos);
                }
                std::cout << "[END]" << std::endl;

            }
        };
        class WaveletTreeOnHeadChars
        {
        public:
            template <typename CHAR = uint8_t>
            static stool::WT build(stool::rlbwt2::RLE<CHAR> *_rlbwt)
            {
                const sdsl::int_vector<8> *head_char_vec_pointer = _rlbwt->get_head_char_vec();
                stool::WT _wt;
                // construct(_wt, inputFile + ".tmp");
                construct_im(_wt, *head_char_vec_pointer);
                return _wt;
            }
        };
        /*
        class RLEWaveletTreeBuilder
        {

            template <typename CHAR = uint8_t>
            static RLEWaveletTree<CHAR, stool::EliasFanoVector> build(stool::rlbwt2::RLE<CHAR, stool::EliasFanoVector> *_rlbwt)
            {
                                // uint64_t data_structure_bytes = 0;
                const sdsl::int_vector<> *head_char_vec_pointer = rlbwt->get_head_char_vec();
                auto lpos_vec_pointer = rlbwt->get_lpos_vec();

                // sdsl::store_to_file(*head_char_vec_pointer, inputFile + ".tmp");

                stool::WT _wt;
                // construct(_wt, inputFile + ".tmp");
                construct_im(_wt, *head_char_vec_pointer);

                this->wt.swap(_wt);

                // data_structure_bytes += sdsl::size_in_bytes(wt);
                std::cout << "WT using memory = " << sdsl::size_in_bytes(wt) / 1000 << "[KB]" << std::endl;
                this->_fposDS.build(head_char_vec_pointer, *lpos_vec_pointer, &wt);
                std::cout << "FPOS Vec using memory = " << _fposDS.get_using_memory() / 1000 << "[KB]" << std::endl;
                // data_structure_bytes += _fposDS.get_using_memory();

            }
        };
        */

    } // namespace rlbwt2
} // namespace stool