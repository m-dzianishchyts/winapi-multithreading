#pragma once

#include "Windows.h"

class Thread
{
	HANDLE _threadHandle;
	DWORD _threadId;

public:
	Thread(void (*startRoutine)(void *), void *parameter);
	~Thread();

	void Start() const;

	DWORD GetThreadId() const;
};
