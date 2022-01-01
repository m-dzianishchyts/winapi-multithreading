#include "FileUtils.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "Range.h"

std::vector<std::string> ReadAllLines(const std::string &filePath);
std::vector<long long> DistributeLoadBetweenThreads(long long totalLoad, unsigned int concurrency);
std::vector<Range<std::string>> PrepareSortRanges(std::vector<std::string> &lines,
                                                  const std::vector<long long> &threadLoads);
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
	std::cout << "Lines in file \"" << inputPath << "\": " << lines.size() << std::endl;

	if (threadPool)
	{
		std::vector<Task> sortTasks;
		unsigned short concurrency = threadPool->GetConcurrency();
		std::vector<long long> threadLoads = DistributeLoadBetweenThreads(static_cast<long long>(lines.size()),
		                                                                  concurrency);
		std::vector<Range<std::string>> sortRanges = PrepareSortRanges(lines, threadLoads);
		for (const Range<std::string> &sortRange : sortRanges)
		{
			std::function sortFunction = [sortRange]() mutable
			{
				std::sort(sortRange.GetBegin(), sortRange.GetEnd());
			};
			Task sortTask(sortFunction);
			sortTasks.push_back(sortTask);
		}

		for (const Task &sortTask : sortTasks)
		{
			threadPool->Submit(sortTask);
		}

		for (const Task &sortTask : sortTasks)
		{
			sortTask.WaitForCompletion();
		}

		for (const Range<std::string> &sortRange : sortRanges)
		{
			std::inplace_merge(lines.begin(), sortRange.GetBegin(), sortRange.GetEnd());
		}
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

void WriteAllLines(const std::string &filePath, const std::vector<std::string> &lines)
{
	std::ofstream outputStream(filePath);
	for (const std::string &line : lines)
	{
		outputStream << line << std::endl;
	}
}
