#include "Logger.h"

Logger::Logger::Logger() : _criticalSection()
{
	InitializeCriticalSection(&_criticalSection);
}

Logger::Logger::~Logger()
{
	DeleteCriticalSection(&_criticalSection);
}
