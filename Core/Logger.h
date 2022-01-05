#pragma once

#include <iostream>
#include <sstream>

#include "Windows.h"
#include "synchapi.h"

namespace Logger
{
	class Logger
	{
		CRITICAL_SECTION _criticalSection;
	public:
		Logger();
		~Logger();

		template <typename T>
		void Log(T &message)
		{
			EnterCriticalSection(&_criticalSection);
			std::clog << message.str() << std::endl;
			message.flush();
			LeaveCriticalSection(&_criticalSection);
		}
	};

	inline Logger Global;

	struct LoggerBuffer
	{
		std::stringstream StringStream;

		LoggerBuffer() = default;
		LoggerBuffer(const LoggerBuffer &) = delete;

		LoggerBuffer &operator=(const LoggerBuffer &) = delete;
		LoggerBuffer &operator=(LoggerBuffer &&) = delete;

		LoggerBuffer(LoggerBuffer &&loggerBuffer) noexcept : StringStream(move(loggerBuffer.StringStream))
		{
		}

		template <typename T>
		LoggerBuffer &operator<<(T &&message)
		{
			StringStream << std::forward<T>(message);
			return *this;
		}

		~LoggerBuffer()
		{
			Global.Log(StringStream);
		}
	};

	template <typename T>
	LoggerBuffer operator<<(Logger &logger, T &&message)
	{
		LoggerBuffer buf;
		buf.StringStream << std::forward<T>(message);
		return buf;
	}
}
