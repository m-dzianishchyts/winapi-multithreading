#include "ThreadPool.h"

#include <cassert>

#include "Logger.h"
#include "synchapi.h"

ThreadPool::ThreadPool(unsigned short concurrency) :
	_threadConditionLock(),
	_criticalSection()
{
	assert(concurrency > 0);
	InitializeConditionVariable(&_threadConditionLock);
	InitializeCriticalSection(&_criticalSection);

	_threads.reserve(concurrency);
	for (unsigned short threadIndex = 0; threadIndex < concurrency; threadIndex++)
	{
		Thread thread(StaticThreadStart, this);
		_threads.emplace_back(thread);
		thread.Start();
#ifdef _DEBUG
		LogThreadCreation(thread);
#endif
	}
}

ThreadPool::~ThreadPool()
{
	DeleteCriticalSection(&_criticalSection);
}

void ThreadPool::Submit(const std::shared_ptr<Task> &task)
{
	EnterCriticalSection(&_criticalSection);
#if defined(_DEBUG) && defined(DEBUG_SYNCHRONIZATION)
	LogCriticalSectionEntering(__LINE__);
#endif

	_taskQueue.push(task);
	WakeConditionVariable(&_threadConditionLock);

#if defined(_DEBUG) && defined(DEBUG_SYNCHRONIZATION)
	LogCriticalSectionLeaving(__LINE__);
#endif
	LeaveCriticalSection(&_criticalSection);
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

std::shared_ptr<Task> ThreadPool::TryTakeTask()
{
	EnterCriticalSection(&_criticalSection);
#if defined(_DEBUG) && defined(DEBUG_SYNCHRONIZATION)
		LogCriticalSectionEntering(__LINE__);
#endif

	while (_taskQueue.empty())
	{
#if defined(_DEBUG) && defined(DEBUG_SYNCHRONIZATION)
			LogCriticalSectionLeaving(__LINE__);
#endif
		SleepConditionVariableCS(&_threadConditionLock, &_criticalSection, INFINITE);
#if defined(_DEBUG) && defined(DEBUG_SYNCHRONIZATION)
			LogCriticalSectionEntering(__LINE__);
#endif
	}

	std::shared_ptr<Task> task = _taskQueue.front();
	_taskQueue.pop();

#if defined(_DEBUG) && defined(DEBUG_SYNCHRONIZATION)
		LogCriticalSectionLeaving(__LINE__);
#endif
	LeaveCriticalSection(&_criticalSection);

	return task;
}

void ThreadPool::ThreadStart()
{
	while (true)
	{
		std::shared_ptr<Task> taskPtr = TryTakeTask();
		taskPtr->Perform();
	}
}

void ThreadPool::LogCriticalSectionEntering(unsigned int line) const
{
	Logger::Global << "Thread #" << GetCurrentThreadId() << " entered critical section "
		<< &_criticalSection << " at line " << line
		<< " (lock count: " << ((-1 - _criticalSection.LockCount) >> 2) << ")";
}

void ThreadPool::LogCriticalSectionLeaving(unsigned int line) const
{
	Logger::Global << "Thread #" << GetCurrentThreadId() << " left critical section "
		<< &_criticalSection << " at line " << line
		<< " (lock count: " << ((-1 - _criticalSection.LockCount) >> 2) << ")";
}

void inline ThreadPool::LogThreadCreation(const Thread &thread)
{
	Logger::Global << "Thread #" << thread.GetThreadId() << " created.";
}
