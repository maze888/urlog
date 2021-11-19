#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "liburing.h"
#include "IOUringBuffer.h"

using namespace urlog::iouringbuffer;

namespace urlog::iouring {

class IOUring {
public:
	IOUring();
	~IOUring();

	void init(const std::string fileName);
	void submit(uint64_t transactionID, const void* data, const size_t size);
	void complete();

private:
	IOUringBuffer* getIOUringBuffer(uint64_t transactionID, const void* data, const size_t size);
	int mFileFD;
	struct io_uring mRing;
	struct io_uring_cqe **mCqes;
	
	int mRemainingCompletions;

	std::vector<std::unique_ptr<IOUringBuffer>> mIOUringBuffers;
};

};
