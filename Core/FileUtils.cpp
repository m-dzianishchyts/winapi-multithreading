#include "FileUtils.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>

#include "Logger.h"
#include "Range.h"

std::vector<std::string> ReadAllLines(const std::string &filePath);
std::vector<long long> DistributeLoadBetweenThreads(long long totalLoad, unsigned int concurrency);
std::vector<Range<std::string>> PrepareSortRanges(std::vector<std::string> &lines,
                                                  const std::vector<long long> &threadLoads);
void SortRange(const Range<std::string> *sortRange);
void WriteAllLines(const std::string &filePath, const std::vector<std::string> &lines);

void Util::FileUtils::Sort(const std::string &inputPath, const std::string &outputPath)
{
	Sort(inputPath, &outputPath, nullptr, nullptr);
}

void Util::FileUtils::Sort(const std::string &inputPath, std::vector<std::string> &outputSortedLines)
{
	Sort(inputPath, nullptr, nullptr, &outputSortedLines);
}

void Util::FileUtils::Sort(const std::string &inputPath, const std::string &outputPath,
                           std::vector<std::string> &outputSortedLines)
{
	Sort(inputPath, &outputPath, nullptr, &outputSortedLines);
}

void Util::FileUtils::Sort(const std::string &inputPath, const std::string &outputPath, ThreadPool &threadPool)
{
	Sort(inputPath, &outputPath, &threadPool, nullptr);
}

void Util::FileUtils::Sort(const std::string &inputPath, ThreadPool &threadPool,
                           std::vector<std::string> &outputSortedLines)
{
	Sort(inputPath, nullptr, &threadPool, &outputSortedLines);
}

void Util::FileUtils::Sort(const std::string &inputPath, const std::string &outputPath, ThreadPool &threadPool,
                           std::vector<std::string> &outputSortedLines)
{
	Sort(inputPath, &outputPath, &threadPool, &outputSortedLines);
}

void Util::FileUtils::Sort(const std::string &inputPath, const std::string *outputPath, ThreadPool *threadPool,
                           std::vector<std::string> *outputSortedLines)
{
	std::vector<std::string> lines = ReadAllLines(inputPath);
#ifdef _DEBUG
	Logger::Global << "Lines in file \"" << inputPath << "\": " << lines.size();
#endif

	if (threadPool)
	{
		std::vector<std::shared_ptr<Task>> sortTasks;
		unsigned short concurrency = threadPool->GetConcurrency();
		std::vector<long long> threadLoads = DistributeLoadBetweenThreads(static_cast<long long>(lines.size()),
		                                                                  concurrency);
		std::vector<Range<std::string>> sortRanges = PrepareSortRanges(lines, threadLoads);
		for (Range<std::string> &sortRange : sortRanges)
		{
			auto taskPtr = std::make_shared<Task>(reinterpret_cast<void (*)(void *)>(SortRange), &sortRange);
			sortTasks.push_back(taskPtr);
		}

		auto beforeSubmission = std::chrono::steady_clock::now();
		for (const std::shared_ptr<Task> &sortTaskPtr : sortTasks)
		{
			threadPool->Submit(sortTaskPtr);
		}

		for (const std::shared_ptr<Task> &sortTaskPtr : sortTasks)
		{
			sortTaskPtr->WaitForCompletion();
		}
		auto afterCompletion = std::chrono::steady_clock::now();

		for (const Range<std::string> &sortRange : sortRanges)
		{
			std::merge(lines.begin(), sortRange.GetBegin(), sortRange.GetBegin(), sortRange.GetEnd(), lines.begin());
		}
		auto afterMerge = std::chrono::steady_clock::now();

#ifdef _DEBUG
		Logger::Global << "Time for parallel sorting: " << (afterCompletion - beforeSubmission).count();
		Logger::Global << "Time for merging: " << (afterMerge - afterCompletion).count();
#endif
	} else
	{
		std::sort(lines.begin(), lines.end());
	}

	if (outputPath)
	{
		WriteAllLines(*outputPath, lines);
	}

	if (outputSortedLines)
	{
		outputSortedLines->insert(outputSortedLines->begin(), lines.begin(), lines.end());
	}
}

std::vector<std::string> ReadAllLines(const std::string &filePath)
{
	std::filesystem::path path = std::filesystem::current_path();
	std::vector<std::string> lines;
	std::ifstream inputStream(filePath);
	std::string line;
	while (std::getline(inputStream, line))
	{
		lines.push_back(line);
	}

	return lines;
}

std::vector<long long> DistributeLoadBetweenThreads(long long totalLoad, unsigned int concurrency)
{
	assert(totalLoad >= 0);
	long long basicThreadLoad = totalLoad / concurrency;
	std::vector threadLoads(concurrency, basicThreadLoad);
	long long extraLoad = totalLoad % concurrency;

	size_t threadIndex = 0;
	while (extraLoad > 0)
	{
		threadLoads[threadIndex]++;
		extraLoad--;
		threadIndex++;
	}

	return threadLoads;
}

std::vector<Range<std::string>> PrepareSortRanges(std::vector<std::string> &lines,
                                                  const std::vector<long long> &threadLoads)
{
	std::vector<Range<std::string>> sortRanges;
	auto rangeBegin = lines.begin();
	auto rangeEnd = lines.begin();
	for (long long threadLoad : threadLoads)
	{
		rangeBegin = rangeEnd;
		rangeEnd = rangeBegin + threadLoad;
		Range<std::string> sortRange(rangeBegin, rangeEnd);
		sortRanges.push_back(sortRange);
	}
	return sortRanges;
}

void SortRange(const Range<std::string> *sortRange)
{
	std::sort(sortRange->GetBegin(), sortRange->GetEnd());
}

void WriteAllLines(const std::string &filePath, const std::vector<std::string> &lines)
{
	std::ofstream outputStream(filePath);
	for (const std::string &line : lines)
	{
		outputStream << line << std::endl;
	}
}
