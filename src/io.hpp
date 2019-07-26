#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "divsufsort.h"
#include "divsufsort64.h"

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


