#pragma once

template <class DurationIn, class FirstDuration, class...RestDurations>
std::string FormatDuration(DurationIn duration)
{
	auto firstDurationPart = std::chrono::duration_cast<FirstDuration>(duration);

	std::string formatResult = std::to_string(firstDurationPart.count());

	if constexpr (sizeof...(RestDurations) > 0)
	{
		formatResult += "'" + FormatDuration<DurationIn, RestDurations...>(duration - firstDurationPart);
	}

	return formatResult;
}

template <class DurationIn>
std::string FormatDuration(DurationIn)
{
	return {};
}
