#pragma once
#include "../online_file_reader.hpp"

namespace stool
{
    class RLEIO
    {
    public:
        static void build_RLBWT_from_BWT(std::string file_path, std::vector<uint8_t> &output_chars, std::vector<uint64_t> &output_runs, uint64_t buffer_size = 16000)
        {
            // this->clear();

            std::vector<uint8_t> chars = stool::OnlineFileReader::get_alphabet(file_path, buffer_size);
            uint64_t text_size = stool::OnlineFileReader::get_text_size(file_path);

            if (buffer_size < 1000)
            {
                buffer_size = 1000;
            }

            std::vector<uint8_t> buffer;

            std::ifstream inputStream;
            inputStream.open(file_path, std::ios::binary);
            {
                std::vector<uint8_t> buffer;

                uint8_t c;
                uint64_t rl = 0;

                while (stool::OnlineFileReader::read(inputStream, buffer, buffer_size, text_size))
                {
                    for (uint8_t c2 : buffer)
                    {
                        if (c == c2)
                        {
                            rl++;
                        }
                        else
                        {
                            if (rl != 0)
                            {
                                output_chars.push_back(c);
                                output_runs.push_back(rl);
                            }
                            c = c2;
                            rl = 1;
                        }
                    }
                }
                if (rl != 0)
                {
                    output_chars.push_back(c);
                    output_runs.push_back(rl);
                }
            }
            inputStream.close();
        }
    };

} // namespace stool