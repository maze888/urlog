#pragma once

#include "fmt/chrono.h"

#include "IOUring.h"

constexpr unsigned MAX_IO_URING_QUEUE_SIZE = 1024;

namespace urlog {

class UrLog {
public:
	UrLog(const std::string path, const size_t maxSize = 0, const size_t maxFile = 0);
	~UrLog();

	template <typename S, typename... Args>
	void log(const S& format, Args&&... args) 
	{
		vlog(format, fmt::make_args_checked<Args...>(format, args...));
	}
	
private:
	void setRotatePath();
	void prepareSubmit(const fmt::string_view& format, const fmt::format_args& args);
	void vlog(const fmt::string_view& format, const fmt::format_args& args);
	uint64_t generateTransactionID(std::chrono::system_clock::duration d);
	void setLoggingPath();
	virtual void submitRing(uint64_t transactionID);

	std::string mLogBuffer;
	urlog::iouring::IOUring mIOUring;
	bool mSubmit;

	std::chrono::milliseconds mCurrentMilliSecond;
	std::string mCurrentDateTime;
	
	std::string mPath;
	std::size_t mMaxSize;
	std::size_t mMaxFile;
	std::vector<std::string> mRotatePath;
};

};
