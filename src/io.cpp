#include "io.hpp"


namespace stool
{

std::vector<char> load_text(std::string filename)
{

	std::ifstream stream;
	stream.open(filename, std::ios::binary);

	std::vector<char> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << std::endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, std::ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, std::ios::beg);
	len = n / sizeof(char);

	vec.resize(len + 1, 0);
	stream.read((char *)&(vec)[0], len * sizeof(char));

	for (uint64_t i = 0; i < len; i++)
	{
		if (vec[i] == 0)
		{
			throw std::logic_error("The input text must not contain '0' character!");
		}
	}
	return vec;
}
std::vector<uint8_t> load_text_from_file(std::string filename, bool appendSpecialCharacter)
{

	std::ifstream stream;
	stream.open(filename, std::ios::binary);

	std::vector<uint8_t> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << std::endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, std::ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, std::ios::beg);
	len = n / sizeof(uint8_t);

	vec.resize(len + (appendSpecialCharacter ? 1 : 0 ), 0);
	stream.read((char *)&(vec)[0], len * sizeof(char));

	for (uint64_t i = 0; i < len; i++)
	{
		if (vec[i] == 0)
		{
			throw std::logic_error("The input text must not contain '0' character!");
		}
	}
	return vec;
}


std::vector<char> load_char_vec_from_file(std::string filename, bool appendSpecialCharacter){
	std::ifstream stream;
	stream.open(filename, std::ios::binary);

	std::vector<char> vec;

	if (!stream)
	{
		std::cerr << "error reading file " << std::endl;
		throw - 1;
	}
	uint64_t len;
	stream.seekg(0, std::ios::end);
	uint64_t n = (unsigned long)stream.tellg();
	stream.seekg(0, std::ios::beg);
	len = n / sizeof(uint8_t);

	vec.resize(len + (appendSpecialCharacter ? 1 : 0 ), 0);
	stream.read((char *)&(vec)[0], len * sizeof(char));

	for (uint64_t i = 0; i < len; i++)
	{
		if (vec[i] == 0)
		{
			throw std::logic_error("The input text must not contain '0' character!");
		}
	}
	return vec;
}

}