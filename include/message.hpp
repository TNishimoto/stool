#include <string>

namespace stool
{
    class Message
    {
    public:
        static inline constexpr int dont_show_message = -1;

        static int add_message_paragraph(int message_paragraph)
        {
            if (message_paragraph >= 0)
            {
                return message_paragraph + 1;
            }
            else
            {
                return Message::dont_show_message;
            }
        }

        static std::string get_paragraph_string(int message_paragraph){
            if(message_paragraph >= 0){
                return std::string(' ', message_paragraph * 2);
            }else{
                return std::string(' ', 0);
            }
        }

    };

}