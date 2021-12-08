#include "UrLog.h"

#include <thread>

using namespace std::chrono;
using namespace urlog;
using namespace urlog::iouringbuffer;

UrLog::UrLog(std::string fileName)
{
	mIOUring.init(fileName);
}
	
UrLog::~UrLog() 
{
	if ( mLogBuffer.size() > 0 ) {
		submitRing(0);
	}
}
	
void UrLog::vlog(fmt::string_view format, fmt::format_args args)
{
	auto now = system_clock::now();
	auto now_epoch = now.time_since_epoch();
	auto currentMilliSecond = duration_cast<milliseconds>(now_epoch);

	mCurrentNanoSecond = duration_cast<nanoseconds>(now_epoch);

	if ( mCurrentMilliSecond != currentMilliSecond ) {
		mCurrentDateTime = fmt::format("[{:%Y-%m-%d %H:%M:}{:%S}]", fmt::localtime(system_clock::to_time_t(now)), now_epoch);
	}

	fmt::format_to(std::back_inserter(mLogBuffer), "{} ", mCurrentDateTime);
	fmt::vformat_to(std::back_inserter(mLogBuffer), format, args);

	if ( mCurrentMilliSecond != currentMilliSecond ) {
		submitRing(generateTransactionID(now_epoch));
		mCurrentMilliSecond = currentMilliSecond;
		mLogBuffer.clear();
	}
	mIOUring.complete();
}

uint64_t UrLog::generateTransactionID(system_clock::duration d)
{
	return duration_cast<nanoseconds>(d).count() + gettid();
}
	
void UrLog::submitRing(uint64_t transactionID)
{
	mIOUring.submit(transactionID, mLogBuffer.data(), mLogBuffer.size());
}
