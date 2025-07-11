#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include "../basic/byte.hpp"
#include "../debug/print.hpp"
#include "../basic/character_run.hpp"

namespace stool
{

    /**
     * @brief Forward iterator for Run-Length Encoding (RLE) of text
     * 
     * This class provides a forward iterator that traverses text and groups
     * consecutive identical characters into runs. It implements run-length
     * encoding on-the-fly without storing the entire encoded sequence in memory.
     * 
     * @tparam TEXT_ITERATOR_BEGIN Iterator type for the beginning of text
     * @tparam TEXT_ITERATOR_END Iterator type for the end of text
     * @tparam CHAR The character type (default: uint8_t)
     */
    template <typename TEXT_ITERATOR_BEGIN, typename TEXT_ITERATOR_END, typename CHAR = uint8_t>
    class ForwardRLE
    {
        TEXT_ITERATOR_BEGIN t_beg;  ///< Iterator to the beginning of text
        TEXT_ITERATOR_END t_end;    ///< Iterator to the end of text
        uint64_t text_length;       ///< Total length of the text

    public:
        using Run = CharacterRun<CHAR, uint64_t>;  ///< Type alias for character run
        
        /**
         * @brief Iterator class for traversing RLE runs
         * 
         * This iterator provides access to consecutive runs of identical characters
         * in the text. It implements the standard forward iterator interface.
         */
        class ForwardRLEIterator
        {

        public:
            TEXT_ITERATOR_BEGIN t_it;  ///< Current position in the text
            uint64_t current_pos;      ///< Current position index
            uint64_t text_length;      ///< Total length of the text
            uint64_t current_run_length;  ///< Length of the current run
            CHAR current_character;    ///< Character of the current run

            using iterator_category = std::forward_iterator_tag;  ///< Iterator category
            using value_type = CHAR;                              ///< Value type
            using difference_type = std::ptrdiff_t;               ///< Difference type
            
            /**
             * @brief Constructs a ForwardRLEIterator
             * 
             * @param _t_it Iterator to the text position
             * @param _text_length Total length of the text
             * @param is_end Whether this is an end iterator
             */
            ForwardRLEIterator(TEXT_ITERATOR_BEGIN _t_it, uint64_t _text_length, bool is_end): t_it(_t_it) {
                this->text_length = _text_length;
                if(is_end){
                    this->current_pos = UINT64_MAX;
                }else{
                    this->current_pos = 0;
                    ++(*this);
                }
            }

            /**
             * @brief Dereference operator
             * 
             * @return CharacterRun representing the current run
             */
            CharacterRun<CHAR, uint64_t> operator*() const
            {
                return CharacterRun<CHAR, uint64_t>(current_character, current_run_length);
            }

            /**
             * @brief Pre-increment operator
             * 
             * Advances the iterator to the next run of characters.
             * 
             * @return Reference to this iterator
             */
            ForwardRLEIterator &operator++()
            {
                if(current_pos < text_length){
                    this->current_character = *t_it;
                    this->current_run_length = 1;                    
                    ++t_it;
                    this->current_pos++;

                    while(this->current_pos < text_length){                        
                        if(this->current_character == *t_it){
                            this->current_run_length++;
                            ++t_it;
                            this->current_pos++;

                        }else{
                            break;
                        }
                    }
                }else{
                    this->current_pos = UINT64_MAX;
                }
                    return *this;

            }

            /**
             * @brief Post-increment operator
             * 
             * @return Copy of this iterator before incrementing
             */
            ForwardRLEIterator operator++(int)
            {
                ForwardRLEIterator temp = *this;
                ++(*this);
                return temp;
            }

            /**
             * @brief Equality comparison operator
             * 
             * @param other Iterator to compare with
             * @return true if iterators are at the same position
             */
            bool operator==(const ForwardRLEIterator &other) const { return current_pos == other.current_pos; }
            
            /**
             * @brief Inequality comparison operator
             * 
             * @param other Iterator to compare with
             * @return true if iterators are at different positions
             */
            bool operator!=(const ForwardRLEIterator &other) const { return current_pos != other.current_pos; }
            
            /**
             * @brief Less than comparison operator
             * 
             * @param other Iterator to compare with
             * @return true if this iterator is before the other
             */
            bool operator<(const ForwardRLEIterator &other) const { return current_pos < other.current_pos; }
            
            /**
             * @brief Greater than comparison operator
             * 
             * @param other Iterator to compare with
             * @return true if this iterator is after the other
             */
            bool operator>(const ForwardRLEIterator &other) const { return current_pos > other.current_pos; }
            
            /**
             * @brief Less than or equal comparison operator
             * 
             * @param other Iterator to compare with
             * @return true if this iterator is before or at the same position as the other
             */
            bool operator<=(const ForwardRLEIterator &other) const { return current_pos <= other.current_pos; }
            
            /**
             * @brief Greater than or equal comparison operator
             * 
             * @param other Iterator to compare with
             * @return true if this iterator is after or at the same position as the other
             */
            bool operator>=(const ForwardRLEIterator &other) const { return current_pos >= other.current_pos; }
        };
        
        /**
         * @brief Constructs a ForwardRLE object
         * 
         * @param _t_beg Iterator to the beginning of text
         * @param _t_end Iterator to the end of text
         * @param _text_length Total length of the text
         */
        ForwardRLE(TEXT_ITERATOR_BEGIN _t_beg, TEXT_ITERATOR_END _t_end, uint64_t _text_length) : t_beg(_t_beg), t_end(_t_end), text_length(_text_length)
        {

        }

        /**
         * @brief Returns an iterator to the beginning of the RLE sequence
         * 
         * @return ForwardRLEIterator pointing to the first run
         */
        ForwardRLEIterator begin() const
        {
            return ForwardRLEIterator(this->t_beg, this->text_length, false);
        }
        
        /**
         * @brief Returns an iterator to the end of the RLE sequence
         * 
         * @return ForwardRLEIterator pointing past the last run
         */
        ForwardRLEIterator end() const
        {
            return ForwardRLEIterator(this->t_end, this->text_length, true);
        }
        
        /**
         * @brief Returns the total length of the original text
         * 
         * @return Total number of characters in the text
         */
        uint64_t size() const {
            return this->text_length;
        }
        
        /**
         * @brief Converts the RLE sequence back to the original text
         * 
         * This function reconstructs the original text by expanding all runs
         * into their constituent characters.
         * 
         * @return Vector containing the original text
         */
        std::vector<CHAR> to_text_vector() const {
            std::vector<CHAR> r;
            r.resize(this->text_length);
            uint64_t x = 0;

            for(Run v : *this){
                for(uint64_t i = 1; i <= v.length; i++){
                    r[x++] = v.character;
                }
            }
            return r;
        }
    };
    /*
    template <typename T>
    using IntegerDeque8 = IntegerDeque<uint8_t>;
    */


}
