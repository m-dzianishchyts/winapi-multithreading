#include "Task.h"

#include <iostream>

Task::Task(void (*function)(void *), void *parameter)
	: _function(function),
	  _parameter(parameter),
	  _completionEvent(CreateEventW(nullptr, TRUE, FALSE, nullptr))
{
}

void Task::Perform() const
{
	_function(_parameter);
	SetEvent(_completionEvent);
}

void Task::WaitForCompletion() const
{
	WaitForSingleObject(_completionEvent, INFINITE);
}
