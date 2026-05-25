#pragma once
#include <vector>
#include <iostream>
#include <list>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>

#ifdef __linux__
#include <malloc.h>
#elif defined(__APPLE__)
#include <sys/resource.h>
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
		/*
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
		*/
		static uint64_t getPeakRSS()
		{
			struct rusage r;
			getrusage(RUSAGE_SELF, &r);
			return r.ru_maxrss;
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
			const uint64_t bytes = Memory::getPeakRSS();
			std::cout << "Memory footprint: "
					  << (bytes / 1024) << " KB ("
					  << (bytes / (1024 * 1024)) << " MB)"
					  << std::endl;
#endif
		}

		template <class Key, class T>
		static std::size_t estimate_memory_usage(
			const std::unordered_map<Key, T> &mp)
		{
			using Map = std::unordered_map<Key, T>;
			using ValueType = typename Map::value_type;

			// unordered_map オブジェクト本体
			std::size_t bytes = sizeof(mp);

			// バケット配列
			// 多くの実装では各バケットはポインタ相当
			bytes += mp.bucket_count() * sizeof(void *);

			// 各ノードに格納される key-value pair
			bytes += mp.size() * sizeof(ValueType);

			// 各ノードの next pointer 等のオーバーヘッドの概算
			bytes += mp.size() * sizeof(void *);

			return bytes;
		}

		// std::unordered_set
		template <class Key, class Hash>
		static std::size_t estimate_memory_usage(
			const std::unordered_set<Key, Hash> &st)
		{
			using Set = std::unordered_set<Key, Hash>;
			using ValueType = typename Set::value_type;

			constexpr std::size_t estimated_node_overhead = 2 * sizeof(void *);

			return sizeof(st) + st.bucket_count() * sizeof(void *) + st.size() * (sizeof(ValueType) + estimated_node_overhead);
		}

		// std::map
		template <class Key, class T>
		static std::size_t estimate_memory_usage(
			const std::map<Key, T> &mp)
		{
			using Map = std::map<Key, T>;
			using ValueType = typename Map::value_type;

			// typical red-black tree node:
			// parent pointer, left pointer, right pointer, color/alignment, etc.
			constexpr std::size_t estimated_node_overhead = 4 * sizeof(void *);

			return sizeof(mp) + mp.size() * (sizeof(ValueType) + estimated_node_overhead);
		}

		// std::set
		template <class Key, class Compare, class Alloc>
		static std::size_t estimate_memory_usage(
			const std::set<Key, Compare, Alloc> &st)
		{
			using Set = std::set<Key, Compare, Alloc>;
			using ValueType = typename Set::value_type;

			// typical red-black tree node:
			// parent pointer, left pointer, right pointer, color/alignment, etc.
			constexpr std::size_t estimated_node_overhead = 4 * sizeof(void *);

			return sizeof(st) + st.size() * (sizeof(ValueType) + estimated_node_overhead);
		}

		template <class T, class Alloc>
		static std::size_t estimate_memory_usage(const std::vector<T, Alloc> &v)
		{
			using Vector = std::vector<T, Alloc>;
			using ValueType = typename Vector::value_type;

			return sizeof(v) + v.capacity() * sizeof(ValueType);
		}
	};

}