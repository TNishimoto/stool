#pragma once
#include <vector>

namespace stool
{
    /**
     * @brief A naive implementation of a dynamic string using std::vector<uint8_t>
     * 
     * This class provides basic string manipulation operations including insertion,
     * deletion, and modification of characters and substrings. It uses a vector
     * of uint8_t to store the string data internally.
     */
    class NaiveDynamicString
    {
    public:
        /** @brief Internal storage for the string data */
        std::vector<uint8_t> text;
        
        /**
         * @brief Initialize the string with a sentinel character
         * 
         * Adds a '$' character at the beginning of the string as a sentinel.
         * This is typically used in string algorithms that require a sentinel.
         */
        void initialzie()
        {
            this->text.push_back('$');
        }
        void clear(){
            this->text.clear();
        }
        
        /**
         * @brief Append a character to the end of the string
         * @param c The character to append (uint8_t)
         */
        void push_back(uint8_t c){
            this->text.push_back(c);
        }
        
        /**
         * @brief Remove the last character from the string
         * 
         * Removes the character at the end of the string. If the string is empty,
         * the behavior is undefined.
         */
        void pop_back(){
            this->text.pop_back();
        }

        void remove(uint64_t pos){
            this->text.erase(this->text.begin() + pos);
        }

        /**
         * @brief Insert a character at a specific position
         * @param pos The position where to insert the character (0-based index)
         * @param character The character to insert
         * 
         * Inserts the specified character at the given position, shifting all
         * subsequent characters to the right.
         */
        void insert_char(uint64_t pos, int64_t character)
        {
            this->text.insert(this->text.begin() + pos, character);
        }
        void insert(uint64_t pos, uint8_t c){
            this->text.insert(this->text.begin() + pos, c);
        }
        std::string to_string() const{
            std::string s;
            for (auto it : this->text)
            {
                s.push_back(it);
            }
            return s;
        }
        
        /**
         * @brief Insert a string at a specific position
         * @param pos The position where to insert the string (0-based index)
         * @param pattern The string to insert
         * 
         * Inserts the specified string at the given position, shifting all
         * subsequent characters to the right. This operation creates a new
         * vector internally and swaps it with the current one.
         */
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
        
        /**
         * @brief Delete a character at a specific position
         * @param pos The position of the character to delete (0-based index)
         * 
         * Removes the character at the specified position, shifting all
         * subsequent characters to the left.
         */
        void delete_char(uint64_t pos)
        {
            this->text.erase(this->text.begin() + pos);
        }
        
        /**
         * @brief Delete a substring from the string
         * @param pos The starting position of the substring to delete (0-based index)
         * @param len The length of the substring to delete
         * 
         * Removes len characters starting from the specified position.
         * This operation is performed by repeatedly calling delete_char.
         */
        void delete_string(uint64_t pos, uint64_t len)
        {
            for (uint64_t i = 0; i < len; i++)
            {
                this->text.erase(this->text.begin() + pos);
            }
        }

        /**
         * @brief Convert the internal representation to a standard string
         * @return A std::string representation of the current string
         * 
         * Creates a new std::string by copying all characters from the
         * internal vector.
         */
        std::string to_str() const
        {
            std::string s;
            for (auto it : this->text)
            {
                s.push_back(it);
            }
            return s;
        }

        /**
         * @brief Get the current size of the string
         * @return The number of characters in the string
         */
        int64_t size() const 
        {
            return this->text.size();
        }
    };

}
