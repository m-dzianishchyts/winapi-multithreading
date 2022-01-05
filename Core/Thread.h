#pragma once

#include "Windows.h"

class Thread
{
	HANDLE _threadHandle;
	DWORD _threadId;

public:
	Thread(void (*startRoutine)(void *), void *parameter);

	void Start() const;
	void Join() const;

	DWORD GetThreadId() const;
};
