#include "UrLog.h"

#include <filesystem>

using namespace std::chrono;
using namespace urlog;
using namespace urlog::iouringbuffer;

namespace fs = std::filesystem;

UrLog::UrLog(const std::string path, const size_t maxSize, const size_t maxFile)
	:mSubmit(false), mPath(path), mMaxSize(maxSize), mMaxFile(maxFile)
{
	mIOUring.init(path);
	setRotatePath();
}
	
UrLog::~UrLog() 
{
	//printf("mLogBuffer.size: %lu\n", mLogBuffer.size());
	if ( mLogBuffer.size() > 0 ) {
		submitRing(0);
	}
}
	
void UrLog::setRotatePath()
{
	std::size_t found = mPath.find_last_of(".");
	std::string path = mPath.substr(0, found);
	std::string ext = mPath.substr(found + 1);

	mRotatePath.push_back(path + "." + ext);
	for ( std::size_t i = 1; i < mMaxFile; i++ ) {
		mRotatePath.push_back(fmt::format("{}.{}.{}", path, i, ext));
	}
}
	
void UrLog::prepareSubmit(const fmt::string_view& format, const fmt::format_args& args)
{
	auto now = system_clock::now();
	auto now_epoch = now.time_since_epoch();
	auto currentMilliSecond = duration_cast<milliseconds>(now_epoch);

	if ( mCurrentMilliSecond != currentMilliSecond ) {
		mCurrentDateTime = fmt::format("[{:%Y-%m-%d %H:%M:}{:%S}]", fmt::localtime(system_clock::to_time_t(now)), now_epoch);
		mCurrentMilliSecond = currentMilliSecond;
		mSubmit = true;
	}
	
	fmt::format_to(std::back_inserter(mLogBuffer), "{} ", mCurrentDateTime);
	fmt::vformat_to(std::back_inserter(mLogBuffer), format, args);
}
	
void UrLog::vlog(const fmt::string_view& format, const fmt::format_args& args)
{
	prepareSubmit(format, args);

	if ( mSubmit ) {
		submitRing(generateTransactionID(system_clock::now().time_since_epoch()));
		mLogBuffer.clear();
		mSubmit = false;
	}

	mIOUring.complete();
}

uint64_t UrLog::generateTransactionID(system_clock::duration d)
{
	return d.count();
}
	
void UrLog::setLoggingPath()
{
	size_t i = 0;
	for ( auto it = mRotatePath.begin(); it != mRotatePath.end(); it++, i++ ) {
		if ( *it == mPath ) {
			std::advance(it, 1);
			if ( it == mRotatePath.end() ) { // it's full
				mIOUring.reopen(mRotatePath[0]);
			}
			else {
				mIOUring.reopen(mRotatePath[i+1]);
			}
		}
	}
}
	
void UrLog::submitRing(uint64_t transactionID)
{
	if ( mMaxFile > 0 ) {
		if ( fs::file_size(mPath) >= mMaxSize ) {
			setLoggingPath();
		}
	}

	mIOUring.submit(transactionID, mLogBuffer.data(), mLogBuffer.size());
}
