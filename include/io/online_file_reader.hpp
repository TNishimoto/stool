#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{
	/**
	 * @brief A class for reading files in an online (streaming) manner
	 * 
	 * This class provides functionality to read files in chunks without loading
	 * the entire file into memory at once. It supports both binary and text files
	 * and provides iterator-based access to file contents.
	 */
	class OnlineFileReader
	{
		std::string filename;
		std::ifstream stream;
			std::vector<uint8_t> buffer;
		uint64_t text_length;
		bool is_used;


	public:
		/**
		 * @brief Constructs an OnlineFileReader with the specified filename
		 * @param _filename The path to the file to be read
		 */
		OnlineFileReader(std::string _filename)
		{
			this->filename = _filename;
			this->text_length = stool::OnlineFileReader::get_text_size(this->filename);
			this->is_used = false;
		}
		
		/**
		 * @brief Returns the size of the file in bytes
		 * @return The total number of bytes in the file
		 */
		uint64_t size() const {
			return this->text_length;
		}
		
		/**
		 * @brief Opens the file for reading
		 */
		void open(){
			this->stream.open(filename, std::ios::binary);
		}
		
		/**
		 * @brief Closes the file
		 */
		void close(){
			this->stream.close();
		}

		/**
		 * @brief Reads a chunk of data from a file stream into a char vector
		 * @param file The input file stream
		 * @param output The vector to store the read data
		 * @param bufferSize The maximum number of bytes to read
		 * @param textSize The total size of the file
		 * @return true if data was successfully read, false if end of file reached
		 * @throws -1 if file is at end of file
		 */
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
			if (output.size() != tmpBufferSize)
			{
				output.resize(tmpBufferSize);
			}
			file.read((char *)&(output)[0], tmpBufferSize * sizeof(char));

			return true;
		}

		/**
		 * @brief Reads a chunk of data from a file stream into a uint8_t vector
		 * @param file The input file stream
		 * @param output The vector to store the read data
		 * @param bufferSize The maximum number of bytes to read
		 * @param textSize The total size of the file
		 * @return true if data was successfully read, false if end of file reached
		 * @throws -1 if file is at end of file
		 */
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
		
		/**
		 * @brief Gets the size of a file in bytes from a file stream
		 * @param file The input file stream
		 * @return The total number of bytes in the file
		 */
		static uint64_t get_text_size(std::ifstream &file)
		{
			file.seekg(0, std::ios::end);
			uint64_t textSize = (uint64_t)file.tellg() / sizeof(char);
			file.seekg(0, std::ios::beg);
			return textSize;
		}
		
		/**
		 * @brief Gets the size of a file in bytes from a filename
		 * @param filename The path to the file
		 * @return The total number of bytes in the file
		 */
		static uint64_t get_text_size(std::string filename)
		{

			std::ifstream inputStream;
			inputStream.open(filename, std::ios::binary);
			uint64_t p = get_text_size(inputStream);
			inputStream.close();
			return p;
		}
		
		/**
		 * @brief Reads the entire file into a char vector
		 * @param file The input file stream
		 * @param output The vector to store the entire file content
		 * @return true if data was successfully read, false if file is empty
		 */
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
		
		/**
		 * @brief Extracts the alphabet (unique characters) from a file
		 * @param filename The path to the file
		 * @param buffer_size The size of the buffer for reading chunks (default: 16000)
		 * @return A vector containing all unique characters found in the file
		 */
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

		/**
		 * @brief Compares two files for equality
		 * @param filename The path to the first file
		 * @param filename2 The path to the second file
		 * @param bufferSize The size of the buffer for reading chunks
		 * @return A pair containing (true, file_size) if files are equal, (false, position) if different
		 * @throws -1 if the first file cannot be opened
		 */
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
		
		/**
		 * @brief Static buffer size constant for the iterator
		 */
		static inline constexpr int STATIC_BUFFER_SIZE = 8192;

		/**
		 * @brief Iterator class for OnlineFileReader
		 * 
		 * This iterator provides forward iteration over the bytes in a file,
		 * reading the file in chunks to avoid loading the entire file into memory.
		 */
		class OnlineFileReaderIterator
		{
			using iterator_category = std::forward_iterator_tag;
			using value_type = uint8_t;
			using difference_type = std::ptrdiff_t;

		public:
			std::ifstream *stream;
			std::vector<uint8_t> *buffer;
			uint64_t text_size;
			uint64_t current_position;
			uint16_t current_position_in_buffer;

			/**
			 * @brief Constructs an OnlineFileReaderIterator
			 * @param _stream Pointer to the file stream
			 * @param _buffer Pointer to the buffer for storing chunks
			 * @param _text_size The total size of the file
			 * @param is_end Whether this is an end iterator
			 */
			OnlineFileReaderIterator(std::ifstream *_stream, std::vector<uint8_t> *_buffer, uint64_t _text_size, bool is_end)
			{
				if (is_end)
				{
					this->stream = nullptr;
					this->buffer = nullptr;
					this->text_size = _text_size;
					this->current_position = UINT64_MAX;
					this->current_position_in_buffer = UINT16_MAX;
				}
				else
				{
					this->stream = _stream;
					this->buffer = _buffer;
					this->text_size = _text_size;
					bool b = OnlineFileReader::read(*this->stream, *this->buffer, STATIC_BUFFER_SIZE, this->text_size);
					if (b)
					{
						this->current_position_in_buffer = 0;
						this->current_position++;
					}
					else
					{
						this->current_position_in_buffer = UINT16_MAX;
						this->current_position = UINT64_MAX;
					}
				}
			}

			/**
			 * @brief Dereference operator to get the current byte
			 * @return The byte at the current position
			 */
			uint8_t operator*() const
			{
				return (*buffer)[current_position_in_buffer];
			}
			
			/**
			 * @brief Pre-increment operator to move to the next byte
			 * @return Reference to this iterator
			 */
			OnlineFileReaderIterator &operator++()
			{
				if ((uint64_t)(this->current_position_in_buffer + 1) < (uint64_t)this->buffer->size())
				{
					this->current_position_in_buffer++;
					this->current_position++;
				}
				else
				{
					bool b = OnlineFileReader::read(*this->stream, *this->buffer, STATIC_BUFFER_SIZE, this->text_size);
					if (b)
					{
						this->current_position_in_buffer = 0;
						this->current_position++;
					}
					else
					{
						this->current_position_in_buffer = UINT16_MAX;
						this->current_position = UINT64_MAX;
					}
				}
				return *this;
			}

			/**
			 * @brief Post-increment operator to move to the next byte
			 * @return A copy of this iterator before incrementing
			 */
			OnlineFileReaderIterator operator++(int)
			{
				OnlineFileReaderIterator temp = *this;
				++(*this);
				return temp;
			}
			
			/**
			 * @brief Equality comparison operator
			 * @param other The iterator to compare with
			 * @return true if both iterators point to the same position
			 */
			bool operator==(const OnlineFileReaderIterator &other) const { return current_position == other.current_position; }
			
			/**
			 * @brief Inequality comparison operator
			 * @param other The iterator to compare with
			 * @return true if iterators point to different positions
			 */
			bool operator!=(const OnlineFileReaderIterator &other) const { return current_position != other.current_position; }
		};

		/**
		 * @brief Returns an iterator pointing to the beginning of the file
		 * @return An iterator pointing to the first byte of the file
		 * @throws std::runtime_error if the iterator has already been used
		 */
		OnlineFileReaderIterator begin()
		{
			if(this->is_used){
				throw std::runtime_error("Error: OnlineFileReaderIterator");
			}else{
				this->is_used = true;
				return OnlineFileReaderIterator(&this->stream, &this->buffer, this->text_length, false);

			}
		}
		
		/**
		 * @brief Returns an iterator pointing to the end of the file
		 * @return An end iterator
		 */
		OnlineFileReaderIterator end() 
		{
			return OnlineFileReaderIterator(&this->stream, &this->buffer, this->text_length, true);

		}

	};
} // namespace stool