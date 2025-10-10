#pragma once
#include <cassert>
#include <chrono>
#include <random>
#include <algorithm>
#include <unordered_set>

namespace stool
{
  namespace bwt
  {

    /**
     * @brief Generator class for BackwardISAIterator [Unchecked AI's Comment] 
     * 
     * This class provides functionality to iterate through ranks of suffixes in the input text
     * in back-to-front order. The i-th value corresponds to ISA[|T|-i], where T is the input text
     * and ISA is the inverse suffix array of T.
     * 
     * @tparam LF_DATA_STRUCTURE The type of the LF (Last-to-First) data structure used for BWT operations
     */
    template <typename LF_DATA_STRUCTURE>
    class BackwardISA
    {
      /**
       * @brief Iterator class for enumerating suffix ranks in back-to-front order
       * 
       * This iterator traverses the ranks of suffixes in the input text from the end to the beginning,
       * providing access to the inverse suffix array values in reverse order.
       */
    public:
      using INDEX = typename LF_DATA_STRUCTURE::INDEX;

      class iterator
      {
      private:
        INDEX _pos;        ///< Current position in the iteration
        INDEX _end_pos;    ///< End position marker
        const LF_DATA_STRUCTURE &_lfds;  ///< Reference to the LF data structure

      public:
        /**
         * @brief Default constructor
         */
        iterator() = default;

        /**
         * @brief Constructor with position and LF data structure
         * 
         * @param __pos Starting position for the iteration
         * @param __end_pos End position marker
         * @param __lfds Reference to the LF data structure
         */
        iterator(INDEX __pos, INDEX __end_pos, const LF_DATA_STRUCTURE &__lfds)
            : _pos(__pos), _end_pos(__end_pos), _lfds(__lfds)
        {
        }

        /**
         * @brief Pre-increment operator
         * 
         * Advances the iterator to the next position using the LF mapping.
         * When reaching the end position, sets both _pos and _end_pos to maximum value.
         * 
         * @return Reference to this iterator
         */
        iterator &operator++()
        {
          //std::cout << this->_pos << ": ";
          //std::cout << this->_pos << std::endl;

          if (this->_pos == this->_end_pos)
          {
            this->_pos = std::numeric_limits<INDEX>::max();
            this->_end_pos = std::numeric_limits<INDEX>::max();
          }else{
            this->_pos = _lfds.lf(this->_pos);
          }
          return *this;
        }

        /**
         * @brief Dereference operator
         * 
         * @return Current position value
         */
        INDEX operator*() const
        {
          return this->_pos;
        }

        /**
         * @brief Inequality comparison operator
         * 
         * @param rhs Right-hand side iterator to compare with
         * @return true if iterators are not equal, false otherwise
         */
        bool operator!=(const iterator &rhs) const
        {
          return this->_pos != rhs._pos;
        }

        /**
         * @brief Check if iterator has reached the end
         * 
         * @return true if iterator is at the end, false otherwise
         */
        bool is_end() const
        {
          return _pos == std::numeric_limits<INDEX>::max() && _end_pos == std::numeric_limits<INDEX>::max();
        }
      };

    private:
      const LF_DATA_STRUCTURE *_lfds = nullptr;  ///< Pointer to the LF data structure
      INDEX _end_marker_position_in_BWT;          ///< Position of the end marker in BWT
      INDEX _str_size;                            ///< Size of the input string

    public:
      using const_iterator = iterator;

      /**
       * @brief Default constructor
       */
      BackwardISA()
      {
      }

      /**
       * @brief Set the parameters for the BackwardISA object
       * 
       * @param __lfds Pointer to the LF data structure
       * @param __end_marker_position_in_BWT Position of the end marker in the BWT
       * @param __str_size Size of the input string
       */
      void set(const LF_DATA_STRUCTURE *__lfds, INDEX __end_marker_position_in_BWT, INDEX __str_size)
      {
        this->_lfds = __lfds;
        this->_end_marker_position_in_BWT = __end_marker_position_in_BWT;
        this->_str_size = __str_size;
      }

      /**
       * @brief Get iterator pointing to the beginning
       * 
       * Creates an iterator starting from the position obtained by applying LF mapping
       * to the end marker position in BWT.
       * 
       * @return Iterator pointing to the first element
       */
      iterator begin() const
      {

        uint64_t x = this->_lfds->lf(this->_end_marker_position_in_BWT);
        auto p = iterator(x, this->_end_marker_position_in_BWT, *this->_lfds);
        return p;
      }

      /**
       * @brief Get iterator pointing to the end
       * 
       * @return Iterator pointing to the end (past the last element)
       */
      iterator end() const
      {
        return iterator(std::numeric_limits<INDEX>::max(), std::numeric_limits<INDEX>::max(), *this->_lfds);
      }

      /**
       * @brief Convert the backward ISA iteration to a complete ISA array
       * 
       * Iterates through all positions and constructs the complete inverse suffix array
       * by filling the result vector in reverse order.
       * 
       * @return Vector containing the complete inverse suffix array
       */
      std::vector<INDEX> to_isa() const
      {
        std::vector<INDEX> r;
        INDEX size = this->_str_size;
        r.resize(size);
        INDEX p = size;
        for (INDEX c : *this)
        {
          r[--p] = c;
        }
        assert(p == 0);
        return r;
      }
    };

  } // namespace rlbwt
} // namespace stool