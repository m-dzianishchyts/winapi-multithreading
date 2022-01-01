#include "ThreadPool.h"

#include <cassert>
#include <iostream>

#include "synchapi.h"

#ifdef _DEBUG
#define DebugThreadCreation(x) LogThreadCreation(x)
#else
#define DebugThreadCreation(x)
#endif

#if defined(_DEBUG) && defined(DEBUG_SYNCHRONIZATION)
#define DebugCriticalSectionEntering(x) LogCriticalSectionEntering(x)
#define DebugCriticalSectionLeaving(x) LogCriticalSectionLeaving(x)
#else
#define DebugCriticalSectionEntering(x)
#define DebugCriticalSectionLeaving(x)
#endif

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
		DebugThreadCreation(thread);
	}
}

void ThreadPool::Submit(const Task &task)
{
	EnterCriticalSection(&_criticalSection);
	DebugCriticalSectionEntering(__LINE__);

	_taskQueue.push(task);
	WakeConditionVariable(&_threadConditionLock);

	LeaveCriticalSection(&_criticalSection);
	DebugCriticalSectionLeaving(__LINE__);
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
		DebugCriticalSectionEntering(__LINE__);

		while (_taskQueue.empty())
		{
			DebugCriticalSectionLeaving(__LINE__);
			SleepConditionVariableCS(&_threadConditionLock, &_criticalSection, INFINITE);
			DebugCriticalSectionEntering(__LINE__);
		}

		Task taskToPerform = _taskQueue.front();
		_taskQueue.pop();

		DebugCriticalSectionLeaving(__LINE__);
		LeaveCriticalSection(&_criticalSection);

		taskToPerform.Perform();
	}
}

void ThreadPool::LogCriticalSectionEntering(unsigned int line) const
{
	std::cout << "Thread #" << GetCurrentThreadId() << " entered critical section "
		<< &_criticalSection << " at line " << line
		<< " (lock count: " << ((-1 - _criticalSection.LockCount) >> 2) << ")" << std::endl;
}

void ThreadPool::LogCriticalSectionLeaving(unsigned int line) const
{
	std::cout << "Thread #" << GetCurrentThreadId() << " left critical section "
		<< &_criticalSection << " at line " << line
		<< " (lock count: " << ((-1 - _criticalSection.LockCount) >> 2) << ")" << std::endl;
}

void ThreadPool::LogThreadCreation(const Thread &thread)
{
	std::cout << "Thread #" << thread.GetThreadId() << " created." << std::endl;
}
