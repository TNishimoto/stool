#pragma once
#include <string>

namespace stool
{
    /**
     * @brief Message utility class for computing the paragraph level of log messages 
     */
    class Message
    {
    public:
        /** @brief Constant indicating no message should be displayed */
        static inline constexpr int NO_MESSAGE = -1;
        
        /** @brief Constant indicating a message should be shown */
        static inline constexpr int SHOW_MESSAGE = 0;

        /**
         * @brief Increments the message paragraph level if the given paragraph level is non-negative
         */
        static int increment_paragraph_level(int paragraph_level)
        {
            if (paragraph_level >= 0)
            {
                return paragraph_level + 1;
            }
            else
            {
                return Message::NO_MESSAGE;
            }
        }

        /**
         * @brief Generates indentation string based on paragraph level (e.g., get_paragraph_string(1) returns "  ")
         */
        static std::string get_paragraph_string(int paragraph_level){
            if(paragraph_level >= 0){
                return std::string(paragraph_level * 2, ' ');
            }else{
                return std::string(0, ' ');
            }
        }

    };

}