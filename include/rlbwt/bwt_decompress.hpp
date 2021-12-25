#pragma once

#include <sdsl/bit_vectors.hpp>
#include <iostream>
#include <sdsl/wt_gmr.hpp>
#include <sdsl/wt_algorithm.hpp>
#include <sdsl/wavelet_trees.hpp>

using namespace std;
using namespace sdsl;

namespace stool
{
    namespace bwt
    {
        /*
        void load(string filename, sdsl::int_vector<> &output)
        {
            std::ifstream inp;
            std::vector<char> buffer;
            uint64_t bufferSize = 8192;
            buffer.resize(8192);

            inp.open(filename, std::ios::binary);
            bool inputFileExist = inp.is_open();
            if (!inputFileExist)
            {
                std::cout << filename << " cannot open." << std::endl;

                throw std::runtime_error("error");
            }
            uint64_t textSize = stool::FileReader::getTextSize(inp);
            uint8_t prevChar = 255;
            uint64_t x = 0;
            uint64_t count_run = 0;
            output.resize(textSize);
            uint64_t pos = 0;
            while (true)
            {
                bool b = stool::FileReader::read(inp, buffer, bufferSize, textSize);
                if (!b)
                {
                    break;
                }

                for (uint64_t i = 0; i < buffer.size(); i++)
                {
                    output[pos++] = (uint8_t)buffer[i];
                }
            }
            inp.close();
        }
        */

        bool load(string &filename, std::vector<uint8_t> &output)
        {
            std::ifstream file;
            file.open(filename, std::ios::binary);

            if (!file)
            {
                std::cerr << "error reading file " << endl;
                return false;
            }
            file.seekg(0, ios::end);
            auto n = (unsigned long)file.tellg();
            file.seekg(0, ios::beg);

            output.resize(n / sizeof(char));

            file.read((char *)&(output)[0], n);
            file.close();
            file.clear();

            return true;
        }
        /*
        bool load_bwt(string &filename, int_vector<> &bwt)
        {
            std::ifstream file;
            file.open(filename, std::ios::binary);

            if (!file)
            {
                std::cerr << "error reading file " << endl;
                return false;
            }
            file.seekg(0, ios::end);
            auto n = (unsigned long)file.tellg();
            file.seekg(0, ios::beg);

            output.resize(n / sizeof(char));

            file.read((char *)&(output)[0], n);
            file.close();
            file.clear();

            return true;
        }
        */
        static uint64_t LF(uint64_t i, int_vector<> &bwt, std::vector<uint64_t> &C, wt_gmr<> &wt)
        {
            uint8_t c = bwt[i];
            uint64_t cNum = wt.rank(i, c);
            return C[c] + cNum;
        }

        std::vector<char> decompress_bwt(string filename)
        {

            std::vector<uint8_t> bwt;
            load(filename, bwt);
            std::cout << "file: " << filename << std::endl;

            std::vector<uint64_t> FreqArr, C;
            FreqArr.resize(256, 0);
            C.resize(256, 0);

            int_vector<> iv;
            iv.resize(bwt.size());

            uint8_t minChar = 255;
            //uint64_t minCharMinOcc = UINT64_MAX;
            std::vector<uint64_t> minCharOccs;

            for (uint64_t i = 0; i < bwt.size(); i++)
            {
                iv[i] = bwt[i];
                if (bwt[i] < minChar)
                {
                    minChar = bwt[i];
                }
                FreqArr[bwt[i]]++;
            }
            for (uint64_t i = 0; i < bwt.size(); i++)
            {
                if (bwt[i] == minChar)
                {
                    minCharOccs.push_back(i);
                }
            }

            for (uint64_t i = 1; i < FreqArr.size(); i++)
            {
                C[i] = C[i - 1] + FreqArr[i - 1];
            }
            wt_gmr<> wt;
            construct_im(wt, iv);

            std::vector<char> outputText;
            int64_t x = iv.size() - 1;
            std::vector<bool> checker;
            checker.resize(iv.size(), false);
            uint64_t m = 0;
            outputText.resize(iv.size(), 0);
            uint64_t p = minCharOccs[m++];

            while (x >= 0)
            {
                outputText[x] = iv[p];
                if (checker[p])
                {
                    std::cout << "Error!" << std::endl;
                    throw -1;
                }
                else
                {
                    checker[p] = true;
                }
                x--;
                p = LF(p, iv, C, wt);
                if (x >= 0 && iv[p] == minChar)
                {
                    p = minCharOccs[m++];
                }
            }
            return outputText;

            /*
    string outputFile = file + ".iv";
    store_to_file(iv, outputFile);
    */
        }
    } // namespace bwt
} // namespace stool