#include "FileUtils.h"

int main()
{
	const std::string inputPath = "../test.txt";
	const std::string outputPath = "../test_output.txt";

	Util::FileUtils::Sort(inputPath, outputPath, Util::FileUtils::MaxConcurrency);
	return 0;
}
