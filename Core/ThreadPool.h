#pragma once

#include <queue>
#include <vector>

#include "Task.h"
#include "Thread.h"

class ThreadPool
{
	std::vector<Thread> _threads;
	std::queue<Task> _taskQueue;

	mutable CONDITION_VARIABLE _threadConditionLock;
	mutable CRITICAL_SECTION _criticalSection;

public:
	explicit ThreadPool(unsigned short concurrency);
	~ThreadPool();

	void Submit(const Task &task);

	unsigned short GetConcurrency() const;

private:
	void ThreadStart();
	static void StaticThreadStart(void *parameter);

	void LogCriticalSectionEntering(unsigned int line) const;
	void LogCriticalSectionLeaving(unsigned int line) const;
	static void LogThreadCreation(const Thread &thread);
};
