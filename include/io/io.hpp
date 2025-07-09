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

		static void load(std::ifstream &stream, std::string &vec, bool allReading)
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
				len = n / sizeof(char);
			}
			else
			{
				stream.read((char *)(&len), sizeof(uint64_t));
			}
			vec.resize(len);
			stream.read((char *)&(vec)[0], len * sizeof(char));
		}

		template <typename T>
		static void load(std::ifstream &stream, std::vector<T> &vec)
		{
			load(stream, vec, true);
		}
		template <typename T>
		static void load(std::string &filename, std::vector<T> &vec)
		{
			std::ifstream inputStream1;
			inputStream1.open(filename, std::ios::binary);
			load(inputStream1, vec);
		}
		static void load(std::string &filename, std::string &vec)
		{
			std::ifstream inputStream1;
			inputStream1.open(filename, std::ios::binary);
			load(inputStream1, vec, true);
		}


		template <typename T>
		static void load_text(std::string &filename, std::vector<T> &vec)
		{
			load(filename, vec);
		}
		static void load_text(std::string &filename, std::string &vec)
		{
			load(filename, vec);
		}

		static uint64_t load_first_64bits(std::string filename)
		{
			std::ifstream stream;
			stream.open(filename, std::ios::binary);

			if (!stream)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}
			uint64_t value;
			stream.read((char *)&value, sizeof(uint64_t));
			return value;
		}


		template <typename T>
		static void load_text(std::string &filename, std::vector<T> &output_vec, bool appendEndMarker, uint8_t end_marker = 0)
		{
			std::cout << "Loading file: " << filename << std::endl;
			std::ifstream stream;
			stream.open(filename, std::ios::binary);

			if (!stream)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}
			uint64_t len;
			stream.seekg(0, std::ios::end);
			uint64_t n = (unsigned long)stream.tellg();
			stream.seekg(0, std::ios::beg);
			len = (n / sizeof(T));

			output_vec.resize(len+1, end_marker);
			stream.read((char *)&(output_vec)[0], len * sizeof(T));

			if(appendEndMarker){
				uint64_t counter = 0;
				for(uint8_t c : output_vec){
					if(c == end_marker){
						counter++;
					}
				}
				if(counter == 1){
					std::cout << filename << " does not contain the end marker. So the end marker is appended to this text." << std::endl;
				}else{
					std::cout << "This program assumes that " << filename << " does not contain the end marker, but this text contains the end marker." << std::endl;
					throw std::runtime_error("An error occurs in load_text function.");
				}
			}else{
				output_vec.pop_back();
			}
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
				uint8_t _byte = buffer[i];
				uint64_t w = std::min(xsize - (i * 8), (uint64_t)8UL);

				for (size_t j = 0; j < w; ++j)
				{
					output[y + w - 1 - j] = _byte & 1;
					_byte = _byte >> 1;
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
				uint8_t _byte = 0;
				for (size_t j = 0; j < w; ++j)
					_byte = (_byte << 1) | text[(x * 8) + j];
				buffer.push_back(_byte);
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