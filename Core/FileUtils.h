#pragma once

#include <string>
#include <thread>
#include <vector>

#include "ThreadPool.h"

namespace Util
{
	class FileUtils final
	{
	public:
		inline static unsigned short MaxConcurrency = std::thread::hardware_concurrency();
		inline static unsigned short NoConcurrency = 1;

		FileUtils() = delete;

		static void Sort(const std::string &inputPath, const std::string &outputPath);
		static void Sort(const std::string &inputPath, std::vector<std::string> &outputSortedLines);
		static void Sort(const std::string &inputPath, const std::string &outputPath,
		                 std::vector<std::string> &outputSortedLines);

		static void Sort(const std::string &inputPath, const std::string &outputPath, ThreadPool &threadPool);
		static void Sort(const std::string &inputPath, ThreadPool &threadPool,
		                 std::vector<std::string> &outputSortedLines);
		static void Sort(const std::string &inputPath, const std::string &outputPath, ThreadPool &threadPool,
		                 std::vector<std::string> &outputSortedLines);
	private:
		static void Sort(const std::string &inputPath, const std::string *outputPath, ThreadPool *threadPool,
		                 std::vector<std::string> *outputSortedLines);
	};
}
