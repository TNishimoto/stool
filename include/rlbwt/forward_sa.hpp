#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
//#include "rlbwt.hpp"
#include "../bwt/backward_isa.hpp"
#include "../debug/print.hpp"
#include "../specialized_collection/elias_fano_vector.hpp"


//#include "rlbwt_functions.hpp"

namespace stool
{
  namespace rlbwt2
  {

    class ForwardSA
    {
    public:
      /*
    This iterator enumerates the suffix array of the original input text, 
    i.e., the i-th value is SA[i], where SA is the suffix array of the original input text.
  */
      using INDEX = uint32_t;

    private:
      //RLBWT<CHAR> &_rlbwt;
      stool::EliasFanoVector sorted_end_ssa;
      sdsl::int_vector<> next_sa_value_vec;

      INDEX _first_sa_value;
      INDEX _str_size;

      class iterator
      {
      private:
        INDEX _sa_value;
        //INDEX _first_sa_value;
        const ForwardSA &_sa;

      public:
        //iterator() = default;

        iterator(const INDEX __sa_value, const ForwardSA &__sa) : _sa_value(__sa_value), _sa(__sa)
        {
        }

      public:
        iterator &operator++()
        {
          uint64_t rank = _sa.sorted_end_ssa.rank(this->_sa_value + 1) - 1;
          uint64_t diff = this->_sa_value - _sa.sorted_end_ssa[rank];

          this->_sa_value = diff + _sa.next_sa_value_vec[rank];
          if (this->_sa_value == _sa._first_sa_value)
          {
            this->_sa_value = std::numeric_limits<INDEX>::max();
          }
          return *this;
        }
        INDEX operator*()
        {
          return this->_sa_value;
        }
        bool operator!=(const iterator &rhs)
        {
          return (_sa_value != rhs._sa_value);
        }
      };

    public:
      //using const_iterator = iterator;
      ForwardSA()
      {
      }
      INDEX first_sa_value()
      {
        return _first_sa_value;
      }

      iterator begin() const
      {
        auto it = iterator(_first_sa_value, *this);
        return it;
      }
      iterator end() const
      {
        auto it = iterator(std::numeric_limits<INDEX>::max(), *this);
        return it;
      }
      std::vector<INDEX> to_sa() const
      {
        std::vector<INDEX> r;
        r.resize(_str_size);
        INDEX p = 0;
        for (INDEX c : *this)
        {
          r[p++] = c;
        }
        return r;
      }
      INDEX str_size() const
      {
        return this->_str_size;
      }
      INDEX size() const
      {
        return this->_str_size;
      }

    public:
      stool::EliasFanoVector *get_sorted_end_ssa()
      {
        return &this->sorted_end_ssa;
      }
      sdsl::int_vector<> *get_next_sa_value_vec()
      {
        return &this->next_sa_value_vec;
      }

      template <typename LF_DATA_STRUCTURE>
      void build(LF_DATA_STRUCTURE &lfds)
      {
        std::vector<std::pair<INDEX, INDEX>> pmarr;

        auto rlbwt = lfds.get_rlbwt();

        uint64_t str_size = rlbwt->str_size();
        uint64_t rle_size = rlbwt->rle_size();

        this->_str_size = str_size;

        pmarr.resize(rle_size, std::pair<INDEX, INDEX>(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max()));

        uint64_t dollerLpos = rlbwt->get_end_rle_lposition();
        //std::cout << dollerLpos << std::endl;

        uint64_t x = rlbwt->get_lpos(dollerLpos);
        x = lfds.lf(x);
        uint64_t sa_value = str_size - 1;

        std::cout << "ISA SCAN" << std::endl;
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        for (uint64_t i = 0; i < str_size; i++)
        {
          uint64_t lindex = rlbwt->get_lindex_containing_the_position(x);
          uint64_t run = rlbwt->get_run(lindex);
          uint64_t diff = x - rlbwt->get_lpos(lindex);

          if (diff == 0)
          {
            uint64_t xindex = lindex > 0 ? lindex - 1 : rle_size - 1;
            pmarr[xindex] = std::pair<INDEX, INDEX>(pmarr[xindex].first, sa_value);
          }

          if (run == diff + 1)
          {
            pmarr[lindex] = std::pair<INDEX, INDEX>(sa_value, pmarr[lindex].second);
          }
          x = lfds.lf(x);
          sa_value--;
        }
        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        std::cout << "FINISHED ISA SCAN" << std::endl;

        double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        double pertime = elapsed / (double)this->_str_size;
        std::cout << (elapsed / 1000) << "[ms]" << std::endl;
        std::cout << pertime << "[micro/per]" << std::endl;

        this->_first_sa_value = pmarr[rle_size - 1].second;

        std::sort(pmarr.begin(), pmarr.end(), [](const std::pair<INDEX, INDEX> &lhs, const std::pair<INDEX, INDEX> &rhs)
                  { return lhs.first < rhs.first; });

        stool::EliasFanoVectorBuilder builder;
        builder.initialize(str_size + 1, rle_size + 1);
        for (auto &it : pmarr)
        {
          builder.push(it.first);
        }
        builder.push(str_size);
        builder.finish();
        this->sorted_end_ssa.build_from_builder(builder);
        std::cout << "finished" << std::endl;
        this->next_sa_value_vec.resize(rle_size);
        for (uint64_t i = 0; i < rle_size; i++)
        {
          this->next_sa_value_vec[i] = pmarr[i].second;
        }
      }
    };

  } // namespace rlbwt
} // namespace stool