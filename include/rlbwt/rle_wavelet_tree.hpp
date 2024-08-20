#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <vector>
#include <type_traits>
//#include "./rinterval_storage.hpp"
#include "./fpos_data_structure.hpp"
#include "./rle.hpp"

namespace stool
{
    namespace rlbwt2
    {

        //template <typename INDEX_SIZE>
        class RLEWaveletTree
        {
        public:
            using INDEX = uint64_t;
            //using RINTERVAL = RInterval<INDEX_SIZE>;
            using CHAR = uint8_t;
            using UCHAR = typename std::make_unsigned<CHAR>::type;
            using CHAR_VEC = sdsl::int_vector<>;
            using FPOSDS = stool::rlbwt2::LightFPosDataStructure;

        private:
            stool::rlbwt2::RLE<CHAR> *rlbwt;
            stool::WT wt;

        public:

            stool::rlbwt2::LightFPosDataStructure _fposDS;

            //RLEWaveletTree(stool::rlbwt2::RLE<CHAR> *_rlbwt, string inputFile) : rlbwt(_rlbwt)

            RLEWaveletTree(stool::rlbwt2::RLE<CHAR> *_rlbwt) : rlbwt(_rlbwt)
            {
                uint64_t data_structure_bytes = 0;
                const sdsl::int_vector<>* head_char_vec_pointer = rlbwt->get_head_char_vec();
                auto lpos_vec_pointer = rlbwt->get_lpos_vec();

                //sdsl::store_to_file(*head_char_vec_pointer, inputFile + ".tmp");

                stool::WT _wt;
                //construct(_wt, inputFile + ".tmp");
                construct_im(_wt, *head_char_vec_pointer);

                this->wt.swap(_wt);

                data_structure_bytes += sdsl::size_in_bytes(wt);
                std::cout << "WT using memory = " << sdsl::size_in_bytes(wt) / 1000 << "[KB]" << std::endl;
                this->_fposDS.build(head_char_vec_pointer, *lpos_vec_pointer, &wt);
                std::cout << "FPOS Vec using memory = " << _fposDS.get_using_memory() / 1000 << "[KB]" << std::endl;
                data_structure_bytes += _fposDS.get_using_memory();

            }

            const stool::EliasFanoVector *get_lpos_vec_pointer() const
            {
                return this->rlbwt->get_lpos_vec();
            }
            stool::rlbwt2::RLE<CHAR>* get_rlbwt() const {
                return this->rlbwt;
            } 
            const sdsl::int_vector<> *get_head_chars_pointer() const
            {
                return this->rlbwt->get_head_char_vec();
            }
            const stool::WT *get_wavelet_tree_pointer() const
            {
                return &this->wt;
            }

            bool checkMaximalRepeat(uint64_t left, uint64_t right)
            {
                uint64_t x = this->rlbwt->get_lindex_containing_the_position(left);
                uint64_t d = this->rlbwt->get_run(x);
                bool isMaximalRepeat = (this->rlbwt->get_lpos(x) + d - 1) < right;
                return isMaximalRepeat;
            }

            INDEX get_fpos(INDEX index, INDEX diff) const
            {
                return _fposDS[index] + diff;
            }
            /*
            void to_rinterval(uint64_t left, uint64_t right, RINTERVAL &output) const
            {
                assert(left <= right);
                output.beginIndex = this->rlbwt->get_lindex_containing_the_position(left);
                output.beginDiff = left - this->rlbwt->get_lpos(output.beginIndex);
                output.endIndex = this->rlbwt->get_lindex_containing_the_position(right);
                output.endDiff = right - this->rlbwt->get_lpos(output.endIndex);
            }
            */
            
            uint64_t get_using_memory() const {
                return sdsl::size_in_bytes(this->wt) + this->_fposDS.get_using_memory();
            }

            uint64_t lf(INDEX i) const {
                uint64_t x = this->rlbwt->get_lindex_containing_the_position(i);
                uint64_t pos = this->rlbwt->get_lpos(x);
                uint64_t lf = this->get_fpos(x, i - pos);
                //std::cout << "[" << i << ", " << x << ", " << pos << ", " << lf << "]" << std::endl;
                return lf;

            }
        };
    } // namespace rlbwt2
} // namespace stool