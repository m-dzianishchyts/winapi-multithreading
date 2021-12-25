#include "FileUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

std::vector<std::string> ReadAllLines(const std::string &filePath);
std::vector<size_t> DistributeLoadBetweenThreads(size_t totalLoad, unsigned int concurrency);
void WriteAllLines(const std::string &filePath, const std::vector<std::string> &lines);

void Util::FileUtils::Sort(const std::string &inputPath, const std::string &outputPath, unsigned int concurrency)
{
	Sort(inputPath, &outputPath, concurrency, nullptr);
}

void Util::FileUtils::Sort(const std::string &inputPath, unsigned int concurrency,
                           std::vector<std::string> &outputSortedLines)
{
	Sort(inputPath, nullptr, concurrency, &outputSortedLines);
}

void Util::FileUtils::Sort(const std::string &inputPath, const std::string &outputPath, unsigned int concurrency,
                     std::vector<std::string> &outputSortedLines)
{
	Sort(inputPath, &outputPath, concurrency, &outputSortedLines);
}

void Util::FileUtils::Sort(const std::string &inputPath, const std::string *outputPath, unsigned int concurrency,
                     std::vector<std::string> *outputSortedLines)
{
	std::vector<std::string> lines = ReadAllLines(inputPath);

	std::vector<size_t> threadLoads = DistributeLoadBetweenThreads(lines.size(), concurrency);

	// TODO: Implement multi-thread file sorting here:

	std::sort(lines.begin(), lines.end());

	// End

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

std::vector<size_t> DistributeLoadBetweenThreads(size_t totalLoad, unsigned int concurrency)
{
	size_t basicThreadLoad = totalLoad / concurrency;
	std::vector threadLoads(concurrency, basicThreadLoad);
	size_t extraLoad = totalLoad % concurrency;

	size_t threadIndex = 0;
	while (extraLoad > 0)
	{
		threadLoads[threadIndex]++;
		extraLoad--;
		threadIndex++;
	}

	return threadLoads;
}

void WriteAllLines(const std::string &filePath, const std::vector<std::string> &lines)
{
	std::ofstream outputStream(filePath);
	for (const std::string &line : lines)
	{
		outputStream << line << std::endl;
	}
}
