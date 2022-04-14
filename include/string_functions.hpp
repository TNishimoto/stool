#pragma once
#include <string>
#include <vector>

namespace stool
{

    class StringFunctions
    {
    public:
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
    };
} // namespace stool