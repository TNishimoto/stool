#pragma once
#include <vector>

namespace stool
{
    class NaiveDynamicString
    {
    public:
        std::vector<uint8_t> text;
        void initialzie()
        {
            this->text.push_back('$');
        }
        void push_back(uint8_t c){
            this->text.push_back(c);
        }
        void pop_back(){
            this->text.pop_back();
        }


        void insert_char(uint64_t pos, int64_t character)
        {
            this->text.insert(this->text.begin() + pos, character);
        }
        void insert_string(uint64_t pos, std::vector<uint8_t> &pattern)
        {
            std::vector<uint8_t> _text;
            for (uint64_t i = 0; i < pos; i++)
            {
                _text.push_back(text[i]);
            }
            for (uint8_t c : pattern)
            {
                _text.push_back(c);
            }
            for (uint64_t i = pos; i < text.size(); i++)
            {
                _text.push_back(text[i]);
            }
            this->text.swap(_text);
        }
        void delete_char(uint64_t pos)
        {
            this->text.erase(this->text.begin() + pos);
        }
        void delete_string(uint64_t pos, uint64_t len)
        {
            for (uint64_t i = 0; i < len; i++)
            {
                this->text.erase(this->text.begin() + pos);
            }
        }

        std::string to_str() const
        {
            std::string s;
            for (auto it : this->text)
            {
                s.push_back(it);
            }
            return s;
        }

        int64_t size() const 
        {
            return this->text.size();
        }
    };

}
