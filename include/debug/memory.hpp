#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>

#ifdef __linux__
#include <malloc.h>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <iostream>
#include <cstdint>
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

		#if defined(__APPLE__)
		static uint64_t current_memory_footprint_bytes() {
			task_vm_info_data_t info;
			mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
			kern_return_t kr = task_info(mach_task_self(),
										 TASK_VM_INFO,
										 reinterpret_cast<task_info_t>(&info),
										 &count);
			if (kr != KERN_SUCCESS) return 0;
			// phys_footprint はプロセスの実使用メモリの良い近似（バイト単位）
			return static_cast<uint64_t>(info.phys_footprint);
		}
#endif

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
const uint64_t bytes = current_memory_footprint_bytes();
      std::cout << "Memory footprint: "
                << (bytes / 1024) << " KB ("
                << (bytes / (1024 * 1024)) << " MB)"
                << std::endl;
#endif
		}
	};

}