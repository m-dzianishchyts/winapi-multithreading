#include "pch.h"

namespace Test
{
	std::vector<std::tuple<std::string, unsigned short>> PrepareDataSource();

	std::string PreparePerformanceTestResultMessage(const std::string &defaultMethodName,
	                                                const std::chrono::nanoseconds &defaultMethodDuration,
	                                                const std::string &advancedMethodName,
	                                                const std::chrono::nanoseconds &advancedMethodDuration);

	class FileUtilsParameterizedTests : public testing::TestWithParam<std::tuple<std::string, unsigned short>>
	{
	public:
		inline static const std::string FILE_1_KB = "../test_1kb.txt";
		inline static const std::string FILE_10_KB = "../test_10kb.txt";
		inline static const std::string FILE_1_MB = "../test_1mb.txt";
		inline static const std::string FILE_10_MB = "../test_10mb.txt";

	protected:
		inline static std::map<std::string, unsigned short> _fileCountMap = {
			{FILE_1_KB, 50},
			{FILE_10_KB, 25},
			{FILE_1_MB, 10},
			{FILE_10_MB, 5},
		};

		ThreadPool *_testThreadPool {};

		void SetUp() override
		{
			unsigned short concurrency = std::get<1>(GetParam());
			_testThreadPool = new ThreadPool(concurrency);
		}

		void TearDown() override
		{
			delete _testThreadPool;
		}
	};

	INSTANTIATE_TEST_CASE_P(FileUtilsTests,
	                        FileUtilsParameterizedTests,
	                        testing::ValuesIn(PrepareDataSource()));

	TEST_P(FileUtilsParameterizedTests, Performance_SimpleVsParallel)
	{
		std::vector<std::string> sortedLines;
		std::string filePath = std::get<0>(GetParam());
		unsigned short iterationCount = _fileCountMap[filePath];

		auto simpleExecutionTime = std::chrono::nanoseconds(0);
		auto parallelExecutionTime = std::chrono::nanoseconds(0);
		for (unsigned int i = 0; i < iterationCount; i++)
		{
			sortedLines.clear();
			auto timeBeforeInvocation = std::chrono::steady_clock::now();
			Util::FileUtils::Sort(filePath, sortedLines);
			auto timeAfterInvocation = std::chrono::steady_clock::now();
			simpleExecutionTime += timeAfterInvocation - timeBeforeInvocation;
			ASSERT_TRUE(std::is_sorted(sortedLines.begin(), sortedLines.end()));

			sortedLines.clear();
			timeBeforeInvocation = std::chrono::steady_clock::now();
			Util::FileUtils::Sort(filePath, *_testThreadPool, sortedLines);
			timeAfterInvocation = std::chrono::steady_clock::now();
			parallelExecutionTime += timeAfterInvocation - timeBeforeInvocation;
			ASSERT_TRUE(std::is_sorted(sortedLines.begin(), sortedLines.end()));
		}

		std::string resultMessage = PreparePerformanceTestResultMessage("Simple sorting", simpleExecutionTime,
		                                                                "Parallel sorting", parallelExecutionTime);
		std::cout << resultMessage.c_str();
		ASSERT_TRUE(parallelExecutionTime < simpleExecutionTime) << "Parallel sorting is slower." << std::endl;
	}

	std::vector<std::tuple<std::string, unsigned short>> PrepareDataSource()
	{
		std::vector<std::tuple<std::string, unsigned short>> dataSource;
		std::vector filePaths {
			FileUtilsParameterizedTests::FILE_1_KB,
			FileUtilsParameterizedTests::FILE_10_KB,
			FileUtilsParameterizedTests::FILE_1_MB,
			FileUtilsParameterizedTests::FILE_10_MB
		};
		for (const std::string &filePath : filePaths)
		{
			for (unsigned short concurrency = 1; concurrency <= Util::FileUtils::MaxConcurrency; concurrency++)
			{
				dataSource.emplace_back(filePath, concurrency);
			}
		}
		return dataSource;
	}

	std::string PreparePerformanceTestResultMessage(const std::string &defaultMethodName,
	                                                const std::chrono::nanoseconds &defaultMethodDuration,
	                                                const std::string &advancedMethodName,
	                                                const std::chrono::nanoseconds &advancedMethodDuration)
	{
		std::stringstream resultMessage;
		std::string defaultExecutionTimeFormatted = FormatDuration<std::chrono::nanoseconds,
		                                                           std::chrono::seconds,
		                                                           std::chrono::milliseconds,
		                                                           std::chrono::microseconds>(defaultMethodDuration);
		std::string advancedExecutionTimeFormatted = FormatDuration<std::chrono::nanoseconds,
		                                                            std::chrono::seconds,
		                                                            std::chrono::milliseconds,
		                                                            std::chrono::microseconds>(advancedMethodDuration);

		resultMessage << defaultMethodName << " time: " << defaultExecutionTimeFormatted << "us" << std::endl;
		resultMessage << advancedMethodName << " time: " << advancedExecutionTimeFormatted << "us";

		auto numericSimpleExecutionTime = static_cast<double>(defaultMethodDuration.count());
		auto numericParallelExecutionTime = static_cast<double>(advancedMethodDuration.count());
		double advancedMethodPerformanceAdvantage = 100 * (numericSimpleExecutionTime / numericParallelExecutionTime -
			1);
		resultMessage << " (" << advancedMethodPerformanceAdvantage << "% faster)" << std::endl;

		return resultMessage.str();
	}
}
