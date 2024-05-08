#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <stdio.h>


namespace stool
{

    std::vector<uint64_t> constructISA(const std::string &text, const std::vector<uint64_t> &sa);

    std::vector<uint64_t> constructLCP(const std::string &text, const std::vector<uint64_t> &sa, const std::vector<uint64_t> &isa);

} // namespace stool