#pragma once

#include <functional>
#include <future>
#include <queue>
#include <sstream>
#include <vector>

#include "Task.h"
#include "Thread.h"

static std::stringstream stringStream;

class ThreadPool
{
	std::vector<Thread> _threads;
	std::queue<Task> _taskQueue;

	mutable CONDITION_VARIABLE _threadConditionLock;
	mutable CRITICAL_SECTION _criticalSection;

public:
	explicit ThreadPool(unsigned short concurrency);

	void Submit(const Task &task);

	unsigned short GetConcurrency() const;

private:
	static void StaticThreadStart(void *parameter);
	static void LogThreadCreation(const Thread &thread);

	void LogCriticalSectionEntering(unsigned int line) const;
	void LogCriticalSectionLeaving(unsigned int line) const;
	void ThreadStart();
};
