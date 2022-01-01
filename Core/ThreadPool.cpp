#include "ThreadPool.h"

#include <cassert>
#include <iostream>

#include "synchapi.h"

static CRITICAL_SECTION logCriticalSection;

ThreadPool::ThreadPool(unsigned short concurrency) : _threadConditionLock(), _criticalSection()
{
	assert(concurrency > 0);
	InitializeConditionVariable(&_threadConditionLock);
	InitializeCriticalSection(&_criticalSection);
	InitializeCriticalSection(&logCriticalSection);

	_threads.reserve(concurrency);
	for (unsigned short threadIndex = 0; threadIndex < concurrency; threadIndex++)
	{
		Thread thread(StaticThreadStart, this);
		_threads.emplace_back(thread);
		thread.Start();
		// LogThreadCreation(thread);
	}
}

void ThreadPool::Submit(const Task &task)
{
	EnterCriticalSection(&_criticalSection);
	// LogCriticalSectionEntering(__LINE__);

	_taskQueue.push(task);
	WakeConditionVariable(&_threadConditionLock);

	LeaveCriticalSection(&_criticalSection);
	// LogCriticalSectionLeaving(__LINE__);
}

unsigned short ThreadPool::GetConcurrency() const
{
	return static_cast<unsigned short>(_threads.size());
}

void ThreadPool::StaticThreadStart(void *parameter)
{
	auto threadPool = static_cast<ThreadPool*>(parameter);
	threadPool->ThreadStart();
}

void ThreadPool::ThreadStart()
{
	while (true)
	{
		EnterCriticalSection(&_criticalSection);
		// LogCriticalSectionEntering(__LINE__);

		while (_taskQueue.empty())
		{
			// LogCriticalSectionLeaving(__LINE__);
			SleepConditionVariableCS(&_threadConditionLock, &_criticalSection, INFINITE);
			// LogCriticalSectionEntering(__LINE__);
		}

		Task taskToPerform = _taskQueue.front();
		_taskQueue.pop();

		taskToPerform.Perform();

		// LogCriticalSectionLeaving(__LINE__);
		LeaveCriticalSection(&_criticalSection);
	}
}

void ThreadPool::LogCriticalSectionEntering(unsigned int line) const
{
	EnterCriticalSection(&logCriticalSection);
	// stringStream << "Thread #" << GetCurrentThreadId() << " entered critical section "
	// 	<< _criticalSection.DebugInfo << " (lock count: " << _criticalSection.LockCount << ")" << std::endl;
	std::cout << "Thread #" << GetCurrentThreadId() << " entered critical section "
		<< &_criticalSection << " at line " << line
		<< " (lock count: " << ((-1 - _criticalSection.LockCount) >> 2) << ")" << std::endl;
	LeaveCriticalSection(&logCriticalSection);
}

void ThreadPool::LogCriticalSectionLeaving(unsigned int line) const
{
	EnterCriticalSection(&logCriticalSection);
	// stringStream << "Thread #" << GetCurrentThreadId() << " left critical section "
	// 	<< _criticalSection.DebugInfo << " (lock count: " << _criticalSection.LockCount << ")" << std::endl;
	std::cout << "Thread #" << GetCurrentThreadId() << " left critical section "
		<< &_criticalSection << " at line " << line
		<< " (lock count: " << ((-1 - _criticalSection.LockCount) >> 2) << ")" << std::endl;
	LeaveCriticalSection(&logCriticalSection);
}

void ThreadPool::LogThreadCreation(const Thread &thread)
{
	EnterCriticalSection(&logCriticalSection);
	// stringStream << "Thread #" << thread.GetThreadId() << " created." << std::endl;
	std::cout << "Thread #" << thread.GetThreadId() << " created." << std::endl;
	LeaveCriticalSection(&logCriticalSection);
}
