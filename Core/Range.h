#pragma once

#include <vector>

template <typename TType>
class Range
{
	typename std::vector<TType>::iterator _begin;
	typename std::vector<TType>::iterator _end;

public:
	Range(const typename std::vector<TType>::iterator &begin, const typename std::vector<TType>::iterator &end)
		: _begin(begin),
		  _end(end)
	{
	}

	const typename std::vector<TType>::iterator &GetBegin() const
	{
		return _begin;
	}

	const typename std::vector<TType>::iterator &GetEnd() const
	{
		return _end;
	}
};
