#include "Thread.h"

#include <process.h>

#include "processthreadsapi.h"

Thread::Thread(void (*startRoutine)(void *), void *parameter) : _threadId(-1)
{
	_threadHandle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(startRoutine), parameter,
	                             CREATE_SUSPENDED, &_threadId);
}

void Thread::Start() const
{
	ResumeThread(_threadHandle);
}

void Thread::Join() const
{
	WaitForSingleObject(_threadHandle, INFINITE);
}

DWORD Thread::GetThreadId() const
{
	return _threadId;
}
