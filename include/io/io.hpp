#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{

	/**
	 * @brief A utility class for file I/O operations [Unchecked AI's Comment] 
	 * 
	 * The IO class provides static methods for reading and writing various data types
	 * to and from files. It supports binary and text file operations, including
	 * vectors, strings, and bit-level operations.
	 */
	class IO
	{
	public:
		/**
		 * @brief Loads data from a binary stream into a vector
		 * 
		 * @tparam T The data type to load
		 * @param stream The input file stream
		 * @param vec The vector to store the loaded data
		 * @param allReading If true, reads the entire file; if false, reads length first
		 * @throws int Throws -1 if stream is invalid
		 */
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

		/**
		 * @brief Loads data from a binary stream into a string
		 * 
		 * @param stream The input file stream
		 * @param vec The string to store the loaded data
		 * @param allReading If true, reads the entire file; if false, reads length first
		 * @throws int Throws -1 if stream is invalid
		 */
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

		/**
		 * @brief Loads data from a binary stream into a vector (reads entire file)
		 * 
		 * @tparam T The data type to load
		 * @param stream The input file stream
		 * @param vec The vector to store the loaded data
		 */
		template <typename T>
		static void load(std::ifstream &stream, std::vector<T> &vec)
		{
			load(stream, vec, true);
		}

		/**
		 * @brief Loads data from a file into a vector
		 * 
		 * @tparam T The data type to load
		 * @param filename The name of the file to read from
		 * @param vec The vector to store the loaded data
		 */
		template <typename T>
		static void load(std::string &filename, std::vector<T> &vec)
		{
			std::ifstream inputStream1;
			inputStream1.open(filename, std::ios::binary);
			load(inputStream1, vec);
		}

		/**
		 * @brief Loads data from a file into a string
		 * 
		 * @param filename The name of the file to read from
		 * @param vec The string to store the loaded data
		 */
		static void load(std::string &filename, std::string &vec)
		{
			std::ifstream inputStream1;
			inputStream1.open(filename, std::ios::binary);
			load(inputStream1, vec, true);
		}

		/**
		 * @brief Loads text data from a file into a vector
		 * 
		 * @tparam T The data type to load
		 * @param filename The name of the file to read from
		 * @param vec The vector to store the loaded data
		 */
		template <typename T>
		static void load_text(std::string &filename, std::vector<T> &vec)
		{
			load(filename, vec);
		}

		/**
		 * @brief Loads text data from a file into a string
		 * 
		 * @param filename The name of the file to read from
		 * @param vec The string to store the loaded data
		 */
		static void load_text(std::string &filename, std::string &vec)
		{
			load(filename, vec);
		}

		/**
		 * @brief Loads the first 64 bits from a file
		 * 
		 * @param filename The name of the file to read from
		 * @return uint64_t The 64-bit value read from the file
		 * @throws int Throws -1 if file cannot be opened
		 */
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

		/**
		 * @brief Loads text data from a file with optional end marker handling
		 * 
		 * @tparam T The data type to load
		 * @param filename The name of the file to read from
		 * @param output_vec The vector to store the loaded data
		 * @param appendEndMarker If true, appends an end marker to the data
		 * @param end_marker The end marker character (default: 0)
		 * @throws std::runtime_error If the file contains the end marker when appendEndMarker is true
		 */
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

		/**
		 * @brief Loads bit data from a file into a container
		 * 
		 * @tparam CONTAINER The container type to store the bit data
		 * @param file The input file stream
		 * @param output The container to store the loaded bit data
		 * @return bool True if successful, false otherwise
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

		/**
		 * @brief Gets the size of a file stream in bytes
		 * 
		 * @param stream The file stream to measure
		 * @return uint64_t The size of the file in bytes
		 */
		static uint64_t getSize(std::ifstream &stream)
		{
			stream.seekg(0, std::ios::end);
			uint64_t n = (unsigned long)stream.tellg();
			stream.seekg(0, std::ios::beg);
			return n;
		}

		/**
		 * @brief Writes vector data to an output stream
		 * 
		 * @tparam T The data type to write
		 * @param out The output file stream
		 * @param text The vector data to write
		 * @param allWriting If true, writes only data; if false, writes length first
		 * @return bool True if successful, false otherwise
		 */
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

		/**
		 * @brief Writes vector data to an output stream (writes entire data)
		 * 
		 * @tparam T The data type to write
		 * @param out The output file stream
		 * @param text The vector data to write
		 * @return bool True if successful, false otherwise
		 */
		template <typename T>
		static bool write(std::ofstream &out, std::vector<T> &text)
		{
			write(out, text, true);
			return true;
		}

		/**
		 * @brief Writes vector data to a file
		 * 
		 * @tparam T The data type to write
		 * @param filename The name of the file to write to
		 * @param text The vector data to write
		 * @return bool True if successful, false otherwise
		 */
		template <typename T>
		static bool write(std::string &filename, std::vector<T> &text)
		{
			std::cout << "Writing: " << filename << std::endl;
			std::ofstream out(filename, std::ios::out | std::ios::binary);
			write<T>(out, text);
			out.close();
			return true;
		}

		/**
		 * @brief Writes string data to an output stream
		 * 
		 * @param os The output file stream
		 * @param text The string data to write
		 * @return bool True if successful, false otherwise
		 */
		static bool write(std::ofstream &os, std::string &text)
		{
			os.write((const char *)(&text[0]), sizeof(char) * text.size());
			return true;
		}

		/**
		 * @brief Writes string data to a file
		 * 
		 * @param filename The name of the file to write to
		 * @param text The string data to write
		 * @return bool True if successful, false otherwise
		 */
		static bool write(std::string &filename, std::string &text)
		{
			std::ofstream out(filename, std::ios::out | std::ios::binary);
			if (!out)
				return 1;
			return write(out, text);
		}

		/**
		 * @brief Writes bit data from a container to an output stream
		 * 
		 * @tparam CONTAINER The container type containing the bit data
		 * @param out The output file stream
		 * @param text The container with bit data to write
		 * @return bool True if successful, false otherwise
		 */
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