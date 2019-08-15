#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <exception>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <time.h>
//#include <stdio.h>
//#include <windows.h>
//#include <Psapi.h>      // Psapi.Lib
//using namespace std;

namespace stool
{

class Counter{
	char c = '.';
	uint64_t baseline = 2;
	uint64_t i = 2;

	public:
	Counter(){

	}
	Counter(uint64_t _baseline) : baseline(_baseline), i(_baseline){
		
	}
	Counter(char _c, uint64_t _baseline) : c(_c), baseline(_baseline), i(_baseline){
		
	}

	void increment(){
		if(this->i==0){
			this->baseline *= 2;
			this->i = this->baseline;
			std::cout << this->c << std::flush;
		}else{
			--this->i;
		}

	}

};

class Printer
{
  public:
	template <class X>
	static std::string toIntegerString(const std::vector<X> &items)
	{
		std::string s = "";
		s += "[";
		//int k = items.size();
		for (int i = 0; i < (int)items.size(); i++)
		{
			if(items[i] == UINT64_MAX){
			s += "-";

			}else{
			s += std::to_string(items[i]);
			}
			if (i != (int)items.size() - 1)
				s += ", ";
		}
		s += "]";
		return s;
	}
	template <class X>
	static void toIntegerString(const std::vector<X> &items, std::string &result)
	{
		result += "[";
		//int k = items.size();
		for (unsigned int i = 0; i < (int)items.size(); i++)
		{
			result += std::to_string(items[i]);
			if (i != items.size() - 1)
				result += ", ";
		}
		result += "]";
	}
	template <class X>
	static void toIntegerString(const std::vector<X> &items, std::string &result, uint64_t padding)
	{
		result += "[";
		//int k = items.size();
		for (uint64_t i = 0; i < items.size(); i++)
		{
			std::string s = "";

			if(items[i] == UINT64_MAX){
			s = "-";
			}else if(items[i] == UINT64_MAX-1){
			s = "*";
			}else{
			s = std::to_string(items[i]);
			}
			while(s.size() < padding){
				s += " ";
			}

			result += s;
			if (i != items.size() - 1)
				result += ",";
		}
		result += "]";
	}
	static void toIntegerString(const std::string &items, std::string &result, uint64_t padding)
	{
		result += "[";
		//int k = items.size();
		for (uint64_t i = 0; i < items.size(); i++)
		{
			std::string s = "";

			if((uint64_t)items[i] == UINT64_MAX){
			s = "-";
			}
			else if((uint64_t)items[i] == UINT64_MAX-1){
			s = "*";
			}
			else{
			s = items[i];
			}
			while(s.size() < padding){
				s += " ";
			}

			result += s;
			if (i != items.size() - 1)
				result += ",";
		}
		result += "]";
	}
	template <class X>
	static std::string toString(std::vector<X> &items, uint64_t padding)
	{
		std::string s = "";
		toIntegerString<X>(items, s, padding);
		return s;
	}
	static std::string toString(std::string &str, uint64_t padding)
	{
		std::string s = "";
		toIntegerString(str, s, padding);
		return s;
	}
	


	template <class X>
	static void print(const std::vector<X> &items)
	{
		std::cout << toIntegerString<X>(items) << std::endl;
	}
	template <class X>
	static void print(const std::string name, const std::vector<X> &items)
	{
		std::cout << name << ": " << toIntegerString<X>(items) << std::endl;
	}


	
	/*
	template <class X>
	static void write(std::string filename, std::vector<X> &items)
	{
		std::ofstream out(filename, std::ios::out | std::ios::binary);
		if (!out)
		{
			throw - 1;
		}
		std::string s = toIntegerString<X>(items);
		out.write((const char *)&s[0], sizeof(char) * s.size());
	}
	static bool write(std::ofstream &os, std::string &text)
	{
		os.write((const char *)(&text[0]), sizeof(char) * text.size());
		return true;
	}
	static bool write(std::string filename, std::string &text)
	{
		std::ofstream out(filename, std::ios::out | std::ios::binary);
		if (!out)
			return 1;
		return write(out, text);
	}
	static bool debugWrite(std::string &text)
	{
		std::ofstream out("debug.txt", std::ios::out | std::ios::binary | std::ios::app);
		if (!out)
			return 1;
		return write(out, text);
	}
	template <class X>
	static void debugWrite(std::vector<X> &items)
	{
		std::ofstream out("debug.txt", std::ios::out | std::ios::binary | std::ios::app);
		if (!out)
		{
			throw - 1;
		}
		std::string s = toIntegerString<X>(items);
		out.write((const char *)&s[0], sizeof(char) * s.size());
	}
	*/

};
} // namespace my