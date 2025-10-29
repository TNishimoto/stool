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
	 */
	class OnlineFileReader
	{
		std::string filepath;
		std::ifstream stream;
		std::vector<uint8_t> buffer;
		uint64_t text_length;
		bool is_used;

	public:
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
			 * @brief Constructs an OnlineFileReaderIterator for a given file \p F representing a text \p T[0..n-1]
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
			 * @brief Returns \p T[i] for the current position \p i
			 */
			uint8_t operator*() const
			{
				return (*buffer)[current_position_in_buffer];
			}

			/**
			 * @brief Increments the current position \p i by 1
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
			 * @brief Increments the current position \p i by 1
			 */
			OnlineFileReaderIterator operator++(int)
			{
				OnlineFileReaderIterator temp = *this;
				++(*this);
				return temp;
			}

			/**
			 * @brief ==
			 */
			bool operator==(const OnlineFileReaderIterator &other) const { return current_position == other.current_position; }

			/**
			 * @brief !=
			 */
			bool operator!=(const OnlineFileReaderIterator &other) const { return current_position != other.current_position; }
		};

		/**
		 * @brief Constructs an OnlineFileReader with the specified filepath \p filepath
		 */
		OnlineFileReader(std::string _filepath)
		{
			this->filepath = _filepath;
			this->text_length = stool::OnlineFileReader::get_text_size(this->filepath);
			this->is_used = false;
		}

		/**
		 * @brief Returns the length of the text stored in the file \p filepath
		 */
		uint64_t size() const
		{
			return this->text_length;
		}

		/**
		 * @brief Opens the file for reading
		 */
		void open()
		{
			this->stream.open(filepath, std::ios::binary);
		}

		/**
		 * @brief Closes the file
		 */
		void close()
		{
			this->stream.close();
		}

		/**
		 * @brief Reads \p T[i..i+m-1] from a given file \p F and writes the read data into a vector \p output
		 * @param file_F The input file stream. T[0..i-1] has been read.
		 * @return true if data was successfully read, false if end of file reached
		 * @throws -1 if file is at end of file
		 */
		static bool read(std::ifstream &file_F, std::vector<char> &output, uint64_t buffer_size_m, uint64_t text_size_n)
		{
			if (file_F.eof())
			{
				throw -1;
			}
			uint64_t i = file_F.tellg();
			if (i == text_size_n)
				return false;

			uint64_t tmp_buffer_size = std::min(text_size_n - i, buffer_size_m);
			if (output.size() != tmp_buffer_size)
			{
				output.resize(tmp_buffer_size);
			}
			file_F.read((char *)&(output)[0], tmp_buffer_size * sizeof(char));

			return true;
		}

		/**
		 * @brief Reads \p T[i..i+m-1] from a given file \p F and writes the read data into a vector \p output
		 * @param file_F The input file stream. T[0..i-1] has been read.
		 * @return true if data was successfully read, false if end of file reached
		 * @throws -1 if file is at end of file
		 */
		static bool read(std::ifstream &file_F, std::vector<uint8_t> &output, uint64_t buffer_size_m, uint64_t text_size_n)
		{
			if (file_F.eof())
			{
				throw -1;
			}
			uint64_t i = file_F.tellg();
			if (i == text_size_n)
				return false;

			uint64_t tmp_buffer_size = std::min(text_size_n - i, buffer_size_m);
			if (output.size() != tmp_buffer_size)
			{
				output.resize(tmp_buffer_size);
			}
			file_F.read((char *)&(output)[0], tmp_buffer_size * sizeof(char));

			return true;
		}

		/**
		 * @brief Returns the length \p n of the text \p T[0..n-1] stored in the file \p F
		 */
		static uint64_t get_text_size(std::ifstream &file_F)
		{
			file_F.seekg(0, std::ios::end);
			uint64_t textSize = (uint64_t)file_F.tellg() / sizeof(char);
			file_F.seekg(0, std::ios::beg);
			return textSize;
		}

		/**
		 * @brief Returns the length \p n of the text \p T[0..n-1] stored in the file \p filepath
		 */
		static uint64_t get_text_size(std::string filepath)
		{

			std::ifstream inputStream;
			inputStream.open(filepath, std::ios::binary);
			uint64_t p = get_text_size(inputStream);
			inputStream.close();
			return p;
		}

		/**
		 * @brief Reads \p T[0..n-1] from a given file \p F and writes the read data into a vector \p output
		 * @return true if data was successfully read, false if file is empty
		 */
		static bool read(std::ifstream &file_F, std::vector<char> &output)
		{
			file_F.seekg(0, std::ios::end);
			uint64_t textSize = (uint64_t)file_F.tellg() / sizeof(char);
			file_F.seekg(0, std::ios::beg);

			uint64_t i = file_F.tellg();
			if (i == textSize)
				return false;

			output.resize(textSize);
			file_F.read((char *)&(output)[0], textSize * sizeof(char));

			return true;
		}

		/**
		 * @brief Returns the alphabet \p U of the text \p T stored in the file \p filepath
		 * @param buffer_size_m The size of the buffer for reading chunks (default: 16000)
		 * @return A vector containing all unique characters in \p U
		 */
		static std::vector<uint8_t> get_alphabet(std::string filepath, uint64_t buffer_size_m = 16000)
		{
			uint64_t text_size = get_text_size(filepath);
			std::vector<bool> checker;
			checker.resize(256, false);

			std::ifstream inputStream;
			inputStream.open(filepath, std::ios::binary);
			std::vector<uint8_t> buffer;
			while (read(inputStream, buffer, buffer_size_m, text_size))
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
		 * @brief Compares two files \p F and \p Q for equality
		 * @param buffer_size_m The size of the buffer for reading chunks
		 * @return A pair (\p b, \p lcp). Here, \p b is true if the files are equal, false otherwise. \p lcp is the length of the longest common prefix between the two files if the text lengths are the same, UINT64_MAX otherwise.
		 * @throws -1 if the first file cannot be opened
		 */
		static std::pair<bool, uint64_t> equal_check(std::string filepath_F, std::string filepath_Q, uint64_t buffer_size_m = 16000)
		{
			std::ifstream stream_F, stream_Q;
			stream_F.open(filepath_F, std::ios::binary);
			stream_Q.open(filepath_Q, std::ios::binary);

			if (!stream_F)
			{
				std::cerr << "error reading file " << std::endl;
				throw -1;
			}

			stream_F.seekg(0, std::ios::end);
			uint64_t textSize_F = (uint64_t)stream_F.tellg() / sizeof(char);
			stream_F.seekg(0, std::ios::beg);

			stream_Q.seekg(0, std::ios::end);
			uint64_t textSize_Q = (uint64_t)stream_Q.tellg() / sizeof(char);
			stream_Q.seekg(0, std::ios::beg);

			if (textSize_F != textSize_Q)
			{
				stream_F.close();
				stream_Q.close();

				return std::pair<bool, uint64_t>(false, UINT64_MAX);
			}

			std::vector<uint8_t> tmp_F, tmp_Q;
			uint64_t sum = 0;

			while (true)
			{
				bool b1 = OnlineFileReader::read(stream_F, tmp_F, buffer_size_m, textSize_F);

				for (uint64_t i = 0; i < tmp_F.size(); i++)
				{
					if (tmp_F[i] != tmp_Q[i])
					{
						stream_F.close();
						stream_Q.close();

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
			stream_F.close();
			stream_Q.close();
			return std::pair<bool, uint64_t>(true, textSize_F);
		}

		

		/**
		 * @brief Returns an iterator pointing to the beginning of the file
		 * @return An iterator pointing to the first byte of the file
		 * @throws std::runtime_error if the iterator has already been used
		 */
		OnlineFileReaderIterator begin()
		{
			if (this->is_used)
			{
				throw std::runtime_error("Error: OnlineFileReaderIterator");
			}
			else
			{
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