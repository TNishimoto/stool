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
	 * @brief A utility class for monitoring memory usage
     * \ingroup DebugClasses
	 */
	class Memory
	{
	public:
#if defined(__APPLE__)
		static uint64_t current_memory_footprint_bytes()
		{
			task_vm_info_data_t info;
			mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
			kern_return_t kr = task_info(mach_task_self(),
										 TASK_VM_INFO,
										 reinterpret_cast<task_info_t>(&info),
										 &count);
			if (kr != KERN_SUCCESS)
				return 0;
			return static_cast<uint64_t>(info.phys_footprint);
		}
#endif

		/**
		 * @brief Prints the current memory usage of this program
		 *
		 * @note This function is only available on Linux and macOS systems.
		 * @note This function uses mallinfo2() for Linux and task_info() for macOS to get the memory usage.
		 * @note The output format differs between platforms (bytes vs KB)
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