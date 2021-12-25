#pragma once

#include <string>
#include <thread>
#include <vector>

namespace Util
{
	class FileUtils final
	{
	public:
		inline static unsigned int MaxConcurrency = std::thread::hardware_concurrency();
		inline static unsigned int NoConcurrency = 1;

		FileUtils() = delete;

		static void Sort(const std::string &inputPath, const std::string &outputPath, unsigned int concurrency);
		static void Sort(const std::string &inputPath, unsigned int concurrency,
		                 std::vector<std::string> &outputSortedLines);
		static void Sort(const std::string &inputPath, const std::string &outputPath, unsigned int concurrency,
		                 std::vector<std::string> &outputSortedLines);

	private:
		static void Sort(const std::string &inputPath, const std::string *outputPath, unsigned int concurrency,
		                 std::vector<std::string> *outputSortedLines);
	};
}
