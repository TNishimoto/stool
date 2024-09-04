#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>

#ifdef __linux__
#include <malloc.h>
#elif defined(__APPLE__)
#include <stdlib.h>
#include <malloc/malloc.h>
#endif

namespace stool
{
	void print_memory_usage()
	{
#ifdef __linux__
		struct mallinfo mi = mallinfo();
		std::cout << "Total allocated space: " << mi.uordblks << " bytes" << std::endl;
#elif defined(__APPLE__)
		malloc_zone_t *zone = malloc_default_zone();
		malloc_statistics_t stats;
		malloc_zone_statistics(zone, &stats);
		std::cout << "Total allocated space: " << (stats.size_in_use / 1000) << "KB" << std::endl;
#endif
	}
}