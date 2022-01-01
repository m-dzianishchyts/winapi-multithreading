#pragma once
#include <functional>

#include "Windows.h"

class Task
{
	std::function<void()> _function;
	HANDLE _completionEvent;

public:
	explicit Task(std::function<void()> function);

	void Perform() const;
	void WaitForCompletion() const;
};
