#pragma once
#include "../online_file_reader.hpp"
#include "../message.hpp"
#include "./bwt_analysis_result.hpp"

namespace stool
{
    class RLEIO
    {
    public:
        static void build_RLBWT_from_BWT(const std::vector<uint8_t> &bwt, std::vector<uint8_t> &output_chars, std::vector<uint64_t> &output_runs, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            if (message_paragraph >= 0 && bwt.size() > 0)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing RLBWT from BWT..." << std::flush;
            }
            std::chrono::system_clock::time_point st1, st2;
            st1 = std::chrono::system_clock::now();

            rlbwt2::BWTAnalysisResult ar;
            ar.analyze(bwt);

            output_chars.resize(ar.run_count, UINT8_MAX);
            output_runs.resize(ar.run_count, UINT64_MAX);

            stool::ForwardRLE frle(bwt.begin(), bwt.end(), bwt.size());

            uint64_t i = 0;
            for (CharacterRun<uint8_t, uint64_t> v : frle)
            {
                output_chars[i] = v.character;
                output_runs[i] = v.length;
                i++;
            }

            st2 = std::chrono::system_clock::now();
            if (message_paragraph >= 0 && bwt.size() > 0)
            {
                uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
                uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
                uint64_t per_time = ((double)ms_time / (double)bwt.size()) * 1000000;

                std::cout << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
            }
        }

        static void build_RLBWT_from_BWT_file(std::string file_path, std::vector<uint8_t> &output_chars, std::vector<uint64_t> &output_runs, int message_paragraph = stool::Message::SHOW_MESSAGE)
        {
            if (message_paragraph >= 0)
            {
                std::cout << stool::Message::get_paragraph_string(message_paragraph) << "Constructing RLBWT from BWT file..." << std::flush;
            }
            std::chrono::system_clock::time_point st1, st2;
            st1 = std::chrono::system_clock::now();

            rlbwt2::BWTAnalysisResult ar;
            ar.analyze(file_path);
            uint64_t text_size = ar.str_size;

            output_chars.resize(ar.run_count, UINT8_MAX);
            output_runs.resize(ar.run_count, UINT64_MAX);

            stool::OnlineFileReader ofr(file_path);
            ofr.open();
            stool::ForwardRLE frle(ofr.begin(), ofr.end(), ofr.size());
            uint64_t i = 0;
            for (CharacterRun<uint8_t, uint64_t> v : frle)
            {
                output_chars[i] = v.character;
                output_runs[i] = v.length;
                i++;
            }
            ofr.close();


            st2 = std::chrono::system_clock::now();
            if (message_paragraph >= 0 && text_size > 0)
            {
                uint64_t sec_time = std::chrono::duration_cast<std::chrono::seconds>(st2 - st1).count();
                uint64_t ms_time = std::chrono::duration_cast<std::chrono::milliseconds>(st2 - st1).count();
                uint64_t per_time = ((double)ms_time / (double)text_size) * 1000000;

                std::cout << "[END] Elapsed Time: " << sec_time << " sec (" << per_time << " ms/MB)" << std::endl;
            }

        }
    };

} // namespace stool