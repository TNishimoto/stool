#pragma once
#include <vector>
#include <deque>
#include <bitset>
#include "../byte.hpp"
#include "../print.hpp"

namespace stool
{
    template <typename CHAR = uint8_t, typename INDEX = uint64_t>
    struct CharacterRun{
        public:
        CHAR character;
        INDEX length;

        CharacterRun(CHAR _char, INDEX _length): character(_char), length(_length) {

        }
        CharacterRun() {

        }

    };

    template <typename TEXT_ITERATOR_BEGIN, typename TEXT_ITERATOR_END, typename CHAR = uint8_t>
    class ForwardRLE
    {
        TEXT_ITERATOR_BEGIN t_beg;
        TEXT_ITERATOR_END t_end;
        uint64_t text_length;



    public:
        using Run = CharacterRun<CHAR, uint64_t>;
        
        class ForwardRLEIterator
        {

        public:
            TEXT_ITERATOR_BEGIN t_it;
            uint64_t current_pos;
            uint64_t text_length;
            uint64_t current_run_length;
            CHAR current_character;

            using iterator_category = std::forward_iterator_tag;
            using value_type = CHAR;
            using difference_type = std::ptrdiff_t;
            ForwardRLEIterator(TEXT_ITERATOR_BEGIN _t_it, uint64_t _text_length, bool is_end): t_it(_t_it) {
                this->text_length = _text_length;
                if(is_end){
                    this->current_pos = UINT64_MAX;
                }else{
                    this->current_pos = 0;
                    ++(*this);
                }
            }

            CharacterRun<CHAR, uint64_t> operator*() const
            {
                return CharacterRun<CHAR, uint64_t>(current_character, current_run_length);
            }

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

            ForwardRLEIterator operator++(int)
            {
                ForwardRLEIterator temp = *this;
                ++(*this);
                return temp;
            }


            bool operator==(const ForwardRLEIterator &other) const { return current_pos == other.current_pos; }
            bool operator!=(const ForwardRLEIterator &other) const { return current_pos != other.current_pos; }
            bool operator<(const ForwardRLEIterator &other) const { return current_pos < other.current_pos; }
            bool operator>(const ForwardRLEIterator &other) const { return current_pos > other.current_pos; }
            bool operator<=(const ForwardRLEIterator &other) const { return current_pos <= other.current_pos; }
            bool operator>=(const ForwardRLEIterator &other) const { return current_pos >= other.current_pos; }
        };
        ForwardRLE(TEXT_ITERATOR_BEGIN _t_beg, TEXT_ITERATOR_END _t_end, uint64_t _text_length) : t_beg(_t_beg), t_end(_t_end), text_length(_text_length)
        {

        }


        ForwardRLEIterator begin() const
        {
            return ForwardRLEIterator(this->t_beg, this->text_length, false);
        }
        ForwardRLEIterator end() const
        {
            return ForwardRLEIterator(this->t_end, this->text_length, true);
        }
        uint64_t size() const {
            return this->text_length;
        }
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
