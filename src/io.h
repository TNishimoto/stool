#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <exception>
#include <algorithm>
#include <fstream>
#include <chrono>

#include <time.h>
//using namespace std;
namespace stool
{
	class IO
	{
	public:
		template <typename T>
		static void load(std::ifstream &stream, std::vector<T> &vec, bool allReading)
		{
			if (!stream)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}
			uint64_t len;
			if (allReading)
			{
				stream.seekg(0, std::ios::end);
				uint64_t n = (unsigned long)stream.tellg();
				stream.seekg(0, std::ios::beg);
				len = n / sizeof(T);
			}
			else
			{
				stream.read((char *)(&len), sizeof(uint64_t));
			}
			vec.resize(len);
			stream.read((char *)&(vec)[0], len * sizeof(T));
		}
		static uint64_t getSize(std::ifstream &stream)
		{
			stream.seekg(0, std::ios::end);
			uint64_t n = (unsigned long)stream.tellg();
			stream.seekg(0, std::ios::beg);
			return n;
		}

		template <typename T>
		static void load(std::ifstream &stream, std::vector<T> &vec)
		{
			load(stream, vec, true);
		}
		template <typename T>
		static void load(std::string &filename, std::vector<T> &vec)
		{
			std::cout << "Loading: " << filename << std::endl;

			std::ifstream inputStream1;
			inputStream1.open(filename, std::ios::binary);
			load(inputStream1, vec);

			inputStream1.close();
			inputStream1.clear();
		}
		template <typename T>
		static void load(std::string &filename, std::vector<T> &vec, uint64_t hash)
		{
			std::cout << "Loading: " << filename << std::endl;

			std::ifstream inputStream;
			inputStream.open(filename, std::ios::binary);
			uint64_t n = getSize(inputStream);
			uint64_t hashSize = sizeof(uint64_t);
			uint64_t dataSize = (n - hashSize) / sizeof(T);
			uint64_t loadhash;
			inputStream.read((char *)(&loadhash), sizeof(uint64_t));
			if (loadhash != hash)
			{
				std::cout << "\033[31m";
				std::cout << "Error: " << filename << " has an invalid hash.";
				std::cout << "\033[39m" << std::endl;
				std::exit(EXIT_FAILURE);
			}

			vec.resize(dataSize);
			inputStream.read((char *)&(vec)[0], dataSize * sizeof(T));

			inputStream.close();
			inputStream.clear();
		}

		template <typename T>
		static bool write(std::ofstream &out, std::vector<T> &text, bool allWriting)
		{
			if (!out)
				return 1;

			if (allWriting)
			{
				out.write(reinterpret_cast<const char *>(&text[0]), text.size() * sizeof(T));
			}
			else
			{
				uint64_t len = text.size();
				out.write(reinterpret_cast<const char *>(&len), sizeof(uint64_t));
				out.write(reinterpret_cast<const char *>(&text[0]), text.size() * sizeof(T));
			}
			return true;
		}

		template <typename CONTAINER>
		static bool write_bits(std::ofstream &out, CONTAINER &text)
		{
			if (!out)
				return 1;

			uint64_t xsize = text.size();
			out.write((char *)(&xsize), sizeof(uint64_t));
			uint64_t x = 0;
			std::vector<uint8_t> buffer;
			while (x * 8 < text.size())
			{
				uint64_t SIZE = text.size() - (x * 8);
				uint64_t w = std::min(SIZE, (uint64_t)8UL);
				uint8_t byte = 0;
				for (size_t j = 0; j < w; ++j)
					byte = (byte << 1) | text[(x * 8) + j];
				buffer.push_back(byte);
				if (buffer.size() > 8192)
				{
					out.write((char *)(&buffer[0]), sizeof(uint8_t) * buffer.size());
					buffer.clear();
				}
				x++;
			}
			if (buffer.size() > 0)
			{
				out.write((char *)(&buffer[0]), sizeof(uint8_t) * buffer.size());
				buffer.clear();
			}

			return true;
		}
		/*
		template <typename T>
		static bool write_deque(std::ofstream &out, std::deque<T> &text)
		{
			if (!out)
				return 1;

			uint64_t xsize = text.size();
			out.write((char *)(&xsize), sizeof(uint64_t));
			std::vector<T> buffer;
			for(uint64_t i=0;i<text.size();i++){
				buffer.push_back(text[i]);
				if (buffer.size() > 8192)
				{
					out.write((char *)(&buffer[0]), sizeof(T) * buffer.size());
					buffer.clear();
				}
			}
			if (buffer.size() > 0)
			{
				out.write((char *)(&buffer[0]), sizeof(T) * buffer.size());
				buffer.clear();
			}

			return true;
		}
		template <typename T>
		static bool load_deque(std::ifstream &file, std::deque<T> &output)
		{
			uint64_t xsize = 0;
			file.read((char *)&xsize, sizeof(uint64_t));
			
			std::vector<T> buffer;
			buffer.resize(xsize);
			file.read((char *)&buffer[0], sizeof(T) * xsize);
			output.resize(xsize);
			for(uint64_t i=0;i<buffer.size();i++){
				output[i] = buffer[i];
			}
			return true;
		}
		*/

		template <typename CONTAINER>
		static bool load_bits(std::ifstream &file, CONTAINER &output)
		{
			uint64_t xsize = 0;
			file.read((char *)&xsize, sizeof(uint64_t));
			
			std::vector<uint8_t> buffer;
			uint64_t size = (xsize / 8) + ((xsize % 8) == 0 ? 0 : 1);
			buffer.resize(size, 0);
			file.read((char *)&buffer[0], sizeof(uint8_t) * buffer.size());

			output.resize(xsize);
			uint64_t y = 0;

			for(uint64_t i=0;i<buffer.size();i++){
				uint8_t byte = buffer[i];
				uint64_t w = std::min(xsize - (i * 8), (uint64_t)8UL);

				for (size_t j = 0; j < w; ++j){
					output[y+ w - 1 - j] = byte & 1;
					byte = byte >> 1;
				}
				y += w;
			}

			return true;
		}

		template <typename T>
		static bool write(std::ofstream &out, std::vector<T> &text)
		{
			write(out, text, true);
			return true;
		}
		template <typename T>
		static bool write(std::ofstream &out, std::vector<T> &text, uint64_t hash)
		{
			if (!out)
				return 1;

			out.write(reinterpret_cast<const char *>(&hash), sizeof(uint64_t));
			out.write(reinterpret_cast<const char *>(&text[0]), text.size() * sizeof(T));

			return true;
		}

		template <typename T>
		static bool write(std::string &filename, std::vector<T> &text)
		{
			std::cout << "Writing: " << filename << std::endl;
			std::ofstream out(filename, std::ios::out | std::ios::binary);
			write<T>(out, text);
			out.close();
			return true;
		}
		template <typename T>
		static bool write(std::string &filename, std::vector<T> &text, uint64_t hash)
		{
			std::cout << "Writing: " << filename << std::endl;
			std::ofstream out(filename, std::ios::out | std::ios::binary);
			write<T>(out, text, hash);
			out.close();
			return true;
		}

		static bool load(std::ifstream &file, std::string &output)
		{

			if (!file)
			{
				std::cerr << "error reading file " << std::endl;
				return false;
			}
			file.seekg(0, std::ios::end);
			uint64_t n = (unsigned long)file.tellg();
			file.seekg(0, std::ios::beg);

			output.resize(n / sizeof(char));

			file.read((char *)&(output)[0], n);
			file.close();
			file.clear();

			return true;
		}
		static void load(std::string filename, std::string &output)
		{
			std::cout << "Loading: " << filename << std::endl;
			std::ifstream inputStream;
			inputStream.open(filename, std::ios::binary);
			load(inputStream, output);
		}

		//static bool write(string &filename, vector<int32_t> &text);
		static bool write(std::ofstream &os, std::string &text)
		{
			os.write((const char *)(&text[0]), sizeof(char) * text.size());
			return true;
		}
		static bool write(std::string &filename, std::string &text)
		{
			std::ofstream out(filename, std::ios::out | std::ios::binary);
			if (!out)
				return 1;
			return write(out, text);
		}
	};
	class FileReader
	{
	public:
		static bool read(std::ifstream &file, std::vector<char> &output, uint64_t bufferSize, uint64_t textSize)
		{
			if (file.eof())
			{
				throw -1;
			}
			uint64_t i = file.tellg();
			if (i == textSize)
				return false;

			uint64_t tmpBufferSize = std::min(textSize - i, bufferSize);
			output.resize(tmpBufferSize);
			file.read((char *)&(output)[0], tmpBufferSize * sizeof(char));

			return true;
		}
		static uint64_t getTextSize(std::ifstream &file)
		{
			file.seekg(0, std::ios::end);
			uint64_t textSize = (uint64_t)file.tellg() / sizeof(char);
			file.seekg(0, std::ios::beg);
			return textSize;
		}
		static uint64_t getTextSize(std::string filename)
		{

			std::ifstream inputStream;
			inputStream.open(filename, std::ios::binary);
			uint64_t p = getTextSize(inputStream);
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

		static std::pair<bool, uint64_t> equalCheck(std::string filename, std::string filename2, uint64_t bufferSize)
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

			std::vector<char> tmp1, tmp2;
			uint64_t sum = 0;

			while (true)
			{
				bool b1 = FileReader::read(stream, tmp1, bufferSize, textSize);
				//bool b2 = FileReader::read(stream2, tmp2, bufferSize, textSize2);

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