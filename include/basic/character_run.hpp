#pragma once
#include <vector>
#include <deque>
#
namespace stool
{
    /**
     * @brief Represents a run of consecutive identical characters [Unchecked AI's Comment]
     * 
     * This struct holds information about a run-length encoded character sequence,
     * storing the character value and the length of the run.
     * 
     * @tparam CHAR The character type (default: uint8_t)
     * @tparam INDEX The index type for length (default: uint64_t)
     */
    template <typename CHAR = uint8_t, typename INDEX = uint64_t>
    struct CharacterRun{
        public:
        CHAR character;  ///< The character value in this run
        INDEX length;    ///< The length of this run

        /**
         * @brief Constructs a CharacterRun with specified character and length
         * 
         * @param _char The character value
         * @param _length The length of the run
         */
        CharacterRun(CHAR _char, INDEX _length): character(_char), length(_length) {

        }
        
        /**
         * @brief Default constructor
         */
        CharacterRun() {

        }

    };
}