#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>
//#include "other_functions.hpp"
//#include "rle_farray.hpp"
//#include "rlbwt_functions.hpp"

namespace stool
{
  namespace bwt
  {

    /*
    This class is the generator for BackwardISAIterator. 
  */
    template <typename LF_DATA_STRUCTURE>
    class BackwardISA
    {
      /*
    This iterator enumerates ranks of suffixes in the input text in the back-to-front order of the text, 
    i.e., the i-th value is ISA[|T|-i], where T is the input text and ISA is the inverse suffix array of T.
  */
    public:
      using INDEX = typename LF_DATA_STRUCTURE::INDEX;

      class iterator
      {
      private:
        INDEX _pos;
        INDEX _end_pos;
        const LF_DATA_STRUCTURE &_lfds;

      public:
        iterator() = default;

        iterator(INDEX __pos, INDEX __end_pos, const LF_DATA_STRUCTURE &__lfds)
            : _pos(__pos), _end_pos(__end_pos), _lfds(__lfds)
        {
        }

        iterator &operator++()
        {
          //std::cout << this->_pos << ": ";
          this->_pos = _lfds.lf(this->_pos);
          //std::cout << this->_pos << std::endl;

          if (this->_pos == this->_end_pos)
          {
            this->_pos = std::numeric_limits<INDEX>::max();
            this->_end_pos = std::numeric_limits<INDEX>::max();
          }
          return *this;
        }
        INDEX operator*() const
        {
          return this->_pos;
        }
        bool operator!=(const iterator &rhs) const
        {
          return this->_pos != rhs._pos;
        }
        bool is_end() const
        {
          return _pos == std::numeric_limits<INDEX>::max() && _end_pos == std::numeric_limits<INDEX>::max();
        }
      };

    private:
      const LF_DATA_STRUCTURE *_lfds = nullptr;
      INDEX _end_position;
      INDEX _str_size;

    public:
      using const_iterator = iterator;
      BackwardISA()
      {
      }

      void set(const LF_DATA_STRUCTURE *__lfds, INDEX __end_position, INDEX __str_size)
      {
        this->_lfds = __lfds;
        this->_end_position = __end_position;
        this->_str_size = __str_size;
      }

      iterator begin() const
      {
        auto p = iterator(this->_end_position, this->_end_position, *this->_lfds);
        return p;
      }
      iterator end() const
      {
        return iterator(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), *this->_lfds);
      }
      std::vector<INDEX> to_isa() const
      {
        std::vector<INDEX> r;
        INDEX size = this->str_size;
        r.resize(size);
        INDEX p = size;
        for (INDEX c : *this)
        {
          r[--p] = c;
        }
        assert(p == 0);
        return r;
      }
      std::vector<INDEX> to_sa() const
      {
        std::vector<INDEX> r;
        INDEX size = this->str_size;
        r.resize(size);
        INDEX p = size;
        for (INDEX c : *this)
        {
          r[c] = --p;
        }
        assert(p == 0);
        return r;
      }
    };

  } // namespace rlbwt
} // namespace stool