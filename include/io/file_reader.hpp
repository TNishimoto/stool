#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{

    /**
     * @brief A utility class for file reading operations
     */
	class FileReader
	{
	public:
		/**
		 * @brief Loads a vector from a binary stream
		 *
		 * @tparam T The data type to load
		 * @param stream The input file stream. This file must represent a \a vector<T>
		 * @param output_vec The vector loaded from the stream.
		 * @throws int Throws -1 if stream is invalid
		 * @note The \a output_vec must be empty.
		 */
		template <typename T>
		static void load_vector(std::ifstream &stream, std::vector<T> &output_vec)
		{
			if (!stream)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}
			uint64_t len;
			stream.seekg(0, std::ios::end);
			uint64_t n = (unsigned long)stream.tellg();
			stream.seekg(0, std::ios::beg);
			len = n / sizeof(T);

			output_vec.resize(len);
			stream.read((char *)&(output_vec)[0], len * sizeof(T));
		}

		/**
		 * @brief Loads a vector from a binary stream
		 *
		 * @tparam T The data type to load
		 * @param stream The input file stream. This file must represent the size of a \a vector<T> and the vector itself.
		 * @param output_vec The vector loaded from the stream.
		 * @throws int Throws -1 if stream is invalid
		 * @note The \a output_vec must be empty.
		 */
		template <typename T>
		static void load_size_and_vector(std::ifstream &stream, std::vector<T> &output_vec)
		{
			if (!stream)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}
			uint64_t len;
			stream.read((char *)(&len), sizeof(uint64_t));
			output_vec.resize(len);
			stream.read((char *)&(output_vec)[0], len * sizeof(T));
		}

		/**
		 * @brief Loads a vector from a binary stream
		 *
		 * @param stream The input file stream. This file must represent a \a string
		 * @param output_str The string loaded from the stream.
		 * @throws int Throws -1 if stream is invalid
		 * @note The \a output_str must be empty.
		 */
		static void load_string(std::ifstream &stream, std::string &output_str)
		{
			if (!stream)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}
			uint64_t len;
			stream.seekg(0, std::ios::end);
			uint64_t n = (unsigned long)stream.tellg();
			stream.seekg(0, std::ios::beg);
			len = n / sizeof(char);
			output_str.resize(len);
			stream.read((char *)&(output_str)[0], len * sizeof(char));
		}

		/**
		 * @brief Loads a vector from a binary stream
		 *
		 * @tparam T The data type to load
		 * @param stream The input file stream. This file must represent the size of a \a string and the string itself.
		 * @param output_str The string loaded from the stream.
		 * @throws int Throws -1 if stream is invalid
		 * @note The \a output_str must be empty.
		 */
		static void load_size_and_string(std::ifstream &stream, std::string &output_str)
		{
			if (!stream)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}
			uint64_t len;
			stream.read((char *)(&len), sizeof(uint64_t));
			output_str.resize(len);
			stream.read((char *)&(output_str)[0], len * sizeof(char));
		}


		/**
		 * @brief Loads a vector from a file
		 *
		 * @tparam T The data type to load
		 * @param filename The name of the file to read from. This file must represent a \a vector<T>.
		 * @param output_vec The vector loaded from the file.
		 * @throws int Throws -1 if stream is invalid
		 * @note The \a output_vec must be empty.
		 */
		template <typename T>
		static void load_vector(std::string &filename, std::vector<T> &output_vec)
		{
			std::ifstream inputStream1;
			inputStream1.open(filename, std::ios::binary);
			load_vector(inputStream1, output_vec);
		}


		/**
		 * @brief Loads a string from a file
		 *
		 * @tparam T The data type to load
		 * @param filename The name of the file to read from. This file must represent a \a string.
		 * @param output_str The string loaded from the file.
		 * @throws int Throws -1 if stream is invalid
		 * @note The \a output_str must be empty.
		 */
		static void load_string(std::string &filename, std::string &output_str)
		{
			std::ifstream inputStream1;
			inputStream1.open(filename, std::ios::binary);
			load_string(inputStream1, output_str);
		}


		/**
		 * @brief Loads the first 64 bits from a file
		 *
		 * @param filename The name of the file to read from
		 * @return The first 64-bit value read from the file
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
		 * @brief Loads vector data from a given file, and the end marker is appended to the loaded data.
		 *
		 * @tparam T The data type to load
		 * @param filename The name of the file to read from
		 * @param output_vec The vector to store the loaded data
		 * @param end_marker The end marker character (default: 0). The end marker occurs in the loaded data as the last character even if the end marker is not the last character of the file.
		 * @throws std::runtime_error If the file contains the end marker.
		 */
		template <typename T>
		static void load_vector_with_end_marker_if_no_end_marker(std::string &filename, std::vector<T> &output_vec, uint8_t end_marker = 0)
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

			output_vec.resize(len + 1, end_marker);
			stream.read((char *)&(output_vec)[0], len * sizeof(T));

			uint64_t counter = 0;
			for (uint8_t c : output_vec)
			{
				if (c == end_marker)
				{
					counter++;
				}
			}
			if (counter == 1)
			{
				std::cout << filename << " does not contain the end marker. So the end marker is appended to this text." << std::endl;
			}
			else
			{
				std::cout << "This program assumes that " << filename << " does not contain the end marker, but this text contains the end marker." << std::endl;
				throw std::runtime_error("An error occurs in load_text function.");
			}
	}

		/**
		 * @brief Loads bits from a given file
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
		 * @brief Gets the number of bytes in a file stream
		 */
		static uint64_t get_byte_count(std::ifstream &stream)
		{
			stream.seekg(0, std::ios::end);
			uint64_t n = (unsigned long)stream.tellg();
			stream.seekg(0, std::ios::beg);
			return n;
		}

		/**
		 * @brief Writes vector data to an output stream [Unchecked AI's Comment]
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
		 * @brief Writes vector data to an output stream (writes entire data) [Unchecked AI's Comment]
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
		 * @brief Writes vector data to a file [Unchecked AI's Comment]
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
		 * @brief Writes string data to an output stream [Unchecked AI's Comment]
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
		 * @brief Writes string data to a file [Unchecked AI's Comment]
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
		 * @brief Writes bit data from a container to an output stream [Unchecked AI's Comment]
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