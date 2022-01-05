#pragma once

#include <memory>
#include <queue>
#include <thread>
#include <vector>

#include "Task.h"
#include "Thread.h"

class ThreadPool
{
public:
	inline static unsigned short MaxConcurrency = std::thread::hardware_concurrency();

private:
	std::vector<Thread> _threads;
	std::queue<std::shared_ptr<Task>> _taskQueue;

	mutable CONDITION_VARIABLE _threadConditionLock;
	mutable CRITICAL_SECTION _criticalSection;

public:
	explicit ThreadPool(unsigned short concurrency);
	~ThreadPool();

	void Submit(const std::shared_ptr<Task> &task);

	unsigned short GetConcurrency() const;

private:
	void ThreadStart();
	static void StaticThreadStart(void *parameter);

	std::shared_ptr<Task> TryTakeTask();

	void LogCriticalSectionEntering(unsigned int line) const;
	void LogCriticalSectionLeaving(unsigned int line) const;
	static void LogThreadCreation(const Thread &thread);
};
