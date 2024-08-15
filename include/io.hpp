#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

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

			for (uint64_t i = 0; i < buffer.size(); i++)
			{
				uint8_t byte = buffer[i];
				uint64_t w = std::min(xsize - (i * 8), (uint64_t)8UL);

				for (size_t j = 0; j < w; ++j)
				{
					output[y + w - 1 - j] = byte & 1;
					byte = byte >> 1;
				}
				y += w;
			}

			return true;
		}

		static uint64_t getSize(std::ifstream &stream)
		{
			stream.seekg(0, std::ios::end);
			uint64_t n = (unsigned long)stream.tellg();
			stream.seekg(0, std::ios::beg);
			return n;
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

		template <typename T>
		static bool write(std::ofstream &out, std::vector<T> &text)
		{
			write(out, text, true);
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
	};
} // namespace stool