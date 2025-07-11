#pragma once
#include <string>

namespace stool
{
    /**
     * @brief Message utility class for handling debug message formatting
     * 
     * This class provides static methods for managing message paragraphs and
     * generating indentation strings for debug output formatting.
     */
    class Message
    {
    public:
        /** @brief Constant indicating no message should be displayed */
        static inline constexpr int NO_MESSAGE = -1;
        
        /** @brief Constant indicating a message should be shown */
        static inline constexpr int SHOW_MESSAGE = 0;

        /**
         * @brief Increments the message paragraph level
         * 
         * @param message_paragraph Current paragraph level (0 or positive integer)
         * @return int New paragraph level, or NO_MESSAGE if input is negative
         * 
         * This function increments the paragraph level by 1 if the input is
         * non-negative. If the input is negative, it returns NO_MESSAGE.
         */
        static int add_message_paragraph(int message_paragraph)
        {
            if (message_paragraph >= 0)
            {
                return message_paragraph + 1;
            }
            else
            {
                return Message::NO_MESSAGE;
            }
        }

        /**
         * @brief Generates indentation string based on paragraph level
         * 
         * @param message_paragraph Paragraph level (0 or positive integer)
         * @return std::string String containing spaces for indentation
         * 
         * This function creates an indentation string with spaces. The number
         * of spaces is calculated as message_paragraph * 2. If the input is
         * negative, an empty string is returned.
         */
        static std::string get_paragraph_string(int message_paragraph){
            if(message_paragraph >= 0){
                return std::string(message_paragraph * 2, ' ');
            }else{
                return std::string(0, ' ');
            }
        }

    };

}