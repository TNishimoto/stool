#pragma once
#include "../online_file_reader.hpp"

namespace stool
{
    class RLEIO
    {
    public:
        static void build_RLBWT_from_BWT(std::vector<uint8_t> &bwt, std::vector<uint8_t> &output_chars, std::vector<uint64_t> &output_runs)
        {

                int64_t tmp_l = 1;
                for (int64_t i = 1; i < (int64_t)bwt.size(); i++)
                {
                    if (bwt[i] != bwt[i - 1])
                    {
                        output_runs.push_back(tmp_l);
                        output_chars.push_back(bwt[i - 1]);
                        tmp_l = 1;
                    }
                    else
                    {
                        tmp_l++;
                    }
                }
                if (tmp_l > 0)
                {
                    output_runs.push_back(tmp_l);
                    output_chars.push_back(bwt[bwt.size() - 1]);
                    tmp_l = 0;
                }
        }
        

        static void build_RLBWT_from_BWT_file(std::string file_path, std::vector<uint8_t> &output_chars, std::vector<uint64_t> &output_runs, uint64_t buffer_size = 16000)
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