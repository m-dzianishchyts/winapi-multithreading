#pragma once

#include "Windows.h"

class Task
{
	void (*_function)(void *);
	void *_parameter;

	HANDLE _completionEvent;

public:
	explicit Task(void (*function)(void *), void *parameter = nullptr);
	~Task();

	void Perform() const;
	void WaitForCompletion() const;

	friend class ThreadPool;
};
