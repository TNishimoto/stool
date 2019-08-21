#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

namespace stool
{

template <typename T>
std::vector<T> load(std::string filename)
{

	std::ifstream stream;
	stream.open(filename, std::ios::binary);

	std::vector<T> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << std::endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, std::ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, std::ios::beg);
	len = n / sizeof(T);

	vec.resize(len);
	stream.read((char *)&(vec)[0], len * sizeof(T));
	return vec;
}

std::vector<char> load_text(std::string filename);
std::vector<uint8_t> load_text_from_file(std::string filename, bool appendSpecialCharacter = true);

template <typename T>
bool write_vector(std::string &filename, std::vector<T> &text)
{
	std::cout << "Writing: " << filename << std::endl;
	std::ofstream out(filename, std::ios::out | std::ios::binary);
	//write<T>(out, text);

	if (!out)
		return 1;
	uint64_t size = text.size();
	out.write(reinterpret_cast<const char *>(&size), sizeof(uint64_t));
	out.write(reinterpret_cast<const char *>(&text[0]), text.size() * sizeof(T));

	out.close();
	return true;
}
template <typename T>
bool load_vector(std::string &filename, std::vector<T> &text, bool has_size_info)
{

	std::cout << "Loading: " << filename << std::endl;
	std::ifstream file;
	file.open(filename, std::ios::binary);

	if (!file)
	{
		std::cerr << "error reading file " << std::endl;
		return false;
	}

	if (has_size_info)
	{
		uint64_t size = UINT64_MAX;
		file.read((char *)&(size), sizeof(uint64_t));
		text.resize(size);
		file.read((char *)&(text)[0], size * sizeof(T));
		file.close();
		file.clear();
	}
	else
	{
		uint64_t len;
		file.seekg(0, std::ios::end);
		uint64_t n = (unsigned long)file.tellg();
		file.seekg(0, std::ios::beg);
		len = n / sizeof(T);

		text.resize(len);
		file.read((char *)&(text)[0], len * sizeof(T));
	}
	return true;
}

/*
template <typename T>
void load_vector(std::string filename, std::string &output)
{
	std::cout << "Loading: " << filename << std::endl;
	std::ifstream inputStream;
	inputStream.open(filename, std::ios::binary);

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

}
*/

/*
std::vector<int64_t> construct_sa(std::vector<char> &text)
{
	std::vector<int64_t> SA;
	int64_t n = text.size();
	SA.resize(n);

	divsufsort64((const unsigned char *)&text[0], (int64_t *)&SA[0], n);

	return SA;
}
*/

} // namespace stool
