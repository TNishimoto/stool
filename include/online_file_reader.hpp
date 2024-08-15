#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{
	class OnlineFileReader
	{
	public:
		static bool read(std::ifstream &file, std::vector<uint8_t> &output, uint64_t bufferSize, uint64_t textSize)
		{
			if (file.eof())
			{
				throw -1;
			}
			uint64_t i = file.tellg();
			if (i == textSize)
				return false;

			uint64_t tmpBufferSize = std::min(textSize - i, bufferSize);
			if (output.size() != tmpBufferSize)
			{
				output.resize(tmpBufferSize);
			}
			file.read((char *)&(output)[0], tmpBufferSize * sizeof(char));

			return true;
		}
		static uint64_t get_text_size(std::ifstream &file)
		{
			file.seekg(0, std::ios::end);
			uint64_t textSize = (uint64_t)file.tellg() / sizeof(char);
			file.seekg(0, std::ios::beg);
			return textSize;
		}
		static uint64_t get_text_size(std::string filename)
		{

			std::ifstream inputStream;
			inputStream.open(filename, std::ios::binary);
			uint64_t p = get_text_size(inputStream);
			inputStream.close();
			return p;
		}
		static bool read(std::ifstream &file, std::vector<char> &output)
		{
			file.seekg(0, std::ios::end);
			uint64_t textSize = (uint64_t)file.tellg() / sizeof(char);
			file.seekg(0, std::ios::beg);

			uint64_t i = file.tellg();
			if (i == textSize)
				return false;

			output.resize(textSize);
			file.read((char *)&(output)[0], textSize * sizeof(char));

			return true;
		}
		static std::vector<uint8_t> get_alphabet(std::string filename, uint64_t buffer_size = 16000)
		{
			uint64_t text_size = get_text_size(filename);
			std::vector<bool> checker;
			checker.resize(256, false);

			std::ifstream inputStream;
			inputStream.open(filename, std::ios::binary);
			std::vector<uint8_t> buffer;
			while (read(inputStream, buffer, buffer_size, text_size))
			{
				for (uint8_t c : buffer)
				{
					checker[c] = true;
				}
			}
			inputStream.close();

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

		static std::pair<bool, uint64_t> equal_check(std::string filename, std::string filename2, uint64_t bufferSize)
		{
			std::ifstream stream, stream2;
			stream.open(filename, std::ios::binary);
			stream2.open(filename2, std::ios::binary);

			if (!stream)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}

			stream.seekg(0, std::ios::end);
			uint64_t textSize = (uint64_t)stream.tellg() / sizeof(char);
			stream.seekg(0, std::ios::beg);

			stream2.seekg(0, std::ios::end);
			uint64_t textSize2 = (uint64_t)stream2.tellg() / sizeof(char);
			stream2.seekg(0, std::ios::beg);

			if (textSize != textSize2)
			{
				stream.close();
				stream2.close();

				return std::pair<bool, uint64_t>(false, UINT64_MAX);
			}

			std::vector<uint8_t> tmp1, tmp2;
			uint64_t sum = 0;

			while (true)
			{
				bool b1 = OnlineFileReader::read(stream, tmp1, bufferSize, textSize);

				for (uint64_t i = 0; i < tmp1.size(); i++)
				{
					if (tmp1[i] != tmp2[i])
					{
						stream.close();
						stream2.close();

						return std::pair<bool, uint64_t>(false, sum);
					}
					else
					{
						sum++;
					}
				}
				if (!b1)
					break;
			}
			stream.close();
			stream2.close();
			return std::pair<bool, uint64_t>(true, textSize);
		}
	};
} // namespace stool