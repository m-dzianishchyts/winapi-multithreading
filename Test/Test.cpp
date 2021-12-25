#include "pch.h"

#include "CppUnitTest.h"
#include "DurationFormat.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Util;

namespace Test
{
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

	TEST_CLASS(FileUtilTests)
	{
		inline static const std::string EMPTY_FILE = "../../test.txt";

	public:
		TEST_METHOD(Sort_EmptyFile)
		{
			std::vector<std::string> sortedLines;

			auto simpleExecutionTime = std::chrono::nanoseconds(0);
			auto parallelExecutionTime = std::chrono::nanoseconds(0);
			for (unsigned int i = 0; i <= 1000; ++i)
			{
				sortedLines.clear();
				auto timeBeforeInvocation = std::chrono::steady_clock::now();
				FileUtils::Sort(EMPTY_FILE, FileUtils::MaxConcurrency, sortedLines);
				auto timeAfterInvocation = std::chrono::steady_clock::now();
				simpleExecutionTime += timeAfterInvocation - timeBeforeInvocation;
				Assert::IsTrue(std::is_sorted(sortedLines.begin(), sortedLines.end()));

				sortedLines.clear();
				timeBeforeInvocation = std::chrono::steady_clock::now();
				FileUtils::Sort(EMPTY_FILE, FileUtils::NoConcurrency, sortedLines);
				timeAfterInvocation = std::chrono::steady_clock::now();
				parallelExecutionTime += timeAfterInvocation - timeBeforeInvocation;
				Assert::IsTrue(std::is_sorted(sortedLines.begin(), sortedLines.end()));
			}

			std::string resultMessage = PreparePerformanceTestResultMessage("Simple sorting", simpleExecutionTime,
			                                                                "Parallel sorting", parallelExecutionTime);
			Logger::WriteMessage(resultMessage.c_str());
			Assert::IsTrue(parallelExecutionTime < simpleExecutionTime, L"Parallel execution is slower.");
		}
	};
}
