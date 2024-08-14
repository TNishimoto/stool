#pragma once
#include <string>
#include <vector>

namespace stool
{

    class StringFunctions
    {
    public:
        static std::vector<uint8_t> get_alphabet(std::vector<uint8_t> &text)
        {
            std::vector<bool> checker;
            checker.resize(256, false);
            for (auto c : text)
            {
                checker[c] = true;
            }

            std::vector<uint8_t> r;
            for (size_t i = 0; i < checker.size(); i++)
            {
                if (checker[i])
                {
                    r.push_back(i);
                }
            }
            return r;
        }

        // Return the longest common extension of text[i..] and text[j..].
        static uint64_t LCE(const std::string &text, uint64_t i, uint64_t j)
        {
            if (i > j)
                return LCE(text, j, i);
            uint64_t max = text.size() - j;
            uint64_t x = 0;
            for (x = 0; x < max; x++)
            {
                if (text[i + x] != text[j + x])
                {
                    break;
                }
            }
            return x;
        }
        // Reverse the input text.
        static void reverse(std::string &text)
        {
            std::string tmp = text;
            for (uint64_t i = 0; i < text.size(); i++)
            {
                text[i] = tmp[text.size() - 1 - i];
            }
        }
        static std::vector<uint8_t> to_reversed_string(std::vector<uint8_t> &text)
        {
            std::vector<uint8_t> r;
            r.resize(text.size(), 0);

            size_t size = text.size();
            for (size_t i = 0; i < size; i++)
            {
                r[i] = text[size - i - 1];
            }
            return r;
        }

        static std::vector<std::string> get_all_strings(uint64_t len, std::vector<uint8_t> &alphabets)
        {
            std::vector<std::string> r;

            if (len == 0)
            {
                return r;
            }
            else if (len == 1)
            {
                for (uint8_t c : alphabets)
                {
                    std::string s;
                    s.push_back(c);
                    r.push_back(s);
                }
                return r;
            }
            else
            {
                std::vector<std::string> pref_vec = get_all_strings(len - 1, alphabets);
                for (std::string &s : pref_vec)
                {
                    for (uint8_t c : alphabets)
                    {
                        std::string new_str = s + (char)c;
                        r.push_back(new_str);
                    }
                }
                return r;
            }
        }
        static std::vector<std::string> get_all_strings(uint64_t len, uint8_t alphabet_size)
        {
            std::vector<uint8_t> alphabets;
            alphabets.push_back('a');
            alphabets.push_back('b');
            alphabets.push_back('c');
            alphabets.push_back('d');
            alphabets.push_back('e');
            alphabets.push_back('f');
            alphabets.push_back('g');
            alphabets.push_back('h');
            alphabets.push_back('i');
            alphabets.push_back('j');
            alphabets.push_back('k');
            alphabets.push_back('l');
            alphabets.push_back('m');
            alphabets.push_back('n');
            alphabets.push_back('o');
            alphabets.push_back('p');
            alphabets.push_back('q');
            alphabets.push_back('r');
            alphabets.push_back('s');
            alphabets.push_back('t');
            alphabets.push_back('u');
            while (alphabets.size() > alphabet_size)
            {
                alphabets.pop_back();
            }
            return get_all_strings(len, alphabets);
        }
    };
} // namespace stool