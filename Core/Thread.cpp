#include "Thread.h"

#include "processthreadsapi.h"

Thread::Thread(void (*startRoutine)(void*), void* parameter) : _threadId(-1)
{
	_threadHandle = CreateThread(nullptr, 0, reinterpret_cast<DWORD (*)(void *)>(startRoutine), parameter,
	                             CREATE_SUSPENDED, &_threadId);
}

Thread::~Thread()
{
	CloseHandle(_threadHandle);
}

void Thread::Start() const
{
	ResumeThread(_threadHandle);
}

DWORD Thread::GetThreadId() const
{
	return _threadId;
}
