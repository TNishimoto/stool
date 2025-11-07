#pragma once
#include <vector>
#include <deque>
#
namespace stool
{
    /**
     * @brief The structure representing a run of characters
     * \ingroup BasicClasses
     */
    template <typename CHAR = uint8_t, typename INDEX = uint64_t>
    struct CharacterRun{
        public:
        CHAR character;  ///< The character value in this run
        INDEX length;    ///< The length of this run

        CharacterRun(CHAR _char, INDEX _length): character(_char), length(_length) {

        }
        
        CharacterRun() {

        }

    };
}