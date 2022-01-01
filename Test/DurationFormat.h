#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

template <class DurationIn, class FirstDuration, class...RestDurations>
std::string FormatDuration(DurationIn duration)
{
	auto firstDurationPart = std::chrono::duration_cast<FirstDuration>(duration);

	std::ostringstream durationStream;
	durationStream << std::setw(3) << std::setfill('0') << firstDurationPart.count();

	if constexpr (sizeof...(RestDurations) > 0)
	{
		durationStream << "'" << FormatDuration<DurationIn, RestDurations...>(duration - firstDurationPart);
	}

	return durationStream.str();
}

template <class DurationIn>
std::string FormatDuration(DurationIn)
{
	return {};
}
