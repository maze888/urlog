#pragma once

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/time.h>

#include <string>
#include <memory>
#include <vector>
#include <system_error>

#include "fmt/chrono.h"

#include "IOUring.h"

constexpr unsigned MAX_IO_URING_QUEUE_SIZE = 1024;

namespace urlog {

class UrLog {
public:
	UrLog(std::string fileName);
	~UrLog();

	template <typename S, typename... Args>
	void log(const S& format, Args&&... args) 
	{
		vlog(format, fmt::make_args_checked<Args...>(format, args...));
	}

private:
	void vlog(fmt::string_view format, fmt::format_args args);
	uint64_t generateTransactionID(std::chrono::system_clock::duration d);
	void submitRing(uint64_t transactionID);

	std::string mLogBuffer;
	urlog::iouring::IOUring mIOUring;

	std::chrono::milliseconds mCurrentMilliSecond;
	std::chrono::nanoseconds mCurrentNanoSecond;
	std::string mCurrentDateTime;
};

};
