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
	/**
	 * @brief Memory utility class for debugging and monitoring memory usage
	 * 
	 * This class provides static methods to monitor and display memory usage
	 * information for the current process. It supports cross-platform memory
	 * monitoring with specific implementations for Linux and macOS systems.
	 * 
	 * The class uses platform-specific memory allocation APIs to provide
	 * accurate memory usage statistics. It automatically detects the operating
	 * system and uses the appropriate API for that platform.
	 * 
	 * @note This class is designed for debugging purposes and should not be
	 *       used in production code for critical memory management decisions.
	 * @note Platform support is limited to Linux and macOS systems.
	 * 
	 * @see print_memory_usage() for memory usage display functionality
	 */
	class Memory
	{
		public:

		/**
		 * @brief Prints the current memory usage to standard output
		 *
		 * This function displays the total allocated memory space for the current process.
		 * The implementation varies depending on the operating system:
		 * - On Linux: Uses mallinfo2() to get memory statistics and displays in bytes
		 * - On macOS: Uses malloc_zone_statistics() to get memory statistics and displays in KB
		 *
		 * The function automatically detects the platform and uses the appropriate
		 * memory allocation API for that system.
		 *
		 * @note This function is platform-specific and only works on Linux and macOS
		 * @note The output format differs between platforms (bytes vs KB)
		 *
		 * @see mallinfo2() for Linux implementation
		 * @see malloc_zone_statistics() for macOS implementation
		 */
		static void print_memory_usage()
		{
#ifdef __linux__
			struct mallinfo2 mi = mallinfo2();
			std::cout << "Total allocated space: " << mi.uordblks << " bytes" << std::endl;
#elif defined(__APPLE__)
			malloc_zone_t *zone = malloc_default_zone();
			malloc_statistics_t stats;
			malloc_zone_statistics(zone, &stats);
			std::cout << "Total allocated space: " << (stats.size_in_use / 1000) << "KB" << std::endl;
#endif
		}
	};

}