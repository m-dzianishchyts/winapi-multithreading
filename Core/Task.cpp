#include "Task.h"

#include <iostream>

Task::Task(std::function<void()> function)
	: _function(std::move(function)),
	  _completionEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr))
{
}

void Task::Perform() const
{
	_function();
	// std::cout << "Thread #" << GetCurrentThreadId() << " has completed task." << std::endl;
	SetEvent(_completionEvent);
}

void Task::WaitForCompletion() const
{
	WaitForSingleObject(_completionEvent, INFINITE);
}