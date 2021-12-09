#include <string.h>

#include "IOUringBuffer.h"
#include "SystemException.h"

using namespace urlog::exception;
using namespace urlog::iouringbuffer;

IOUringBuffer::IOUringBuffer()
	:mTransactionID(0), mBuffer(nullptr), mBufferSize(0), mBufferCapacity(0), mLockStatus(false)
{
}

IOUringBuffer::~IOUringBuffer()
{
	freeBuffer();
}
			
uint64_t IOUringBuffer::getTransactionID() const
{
	return mTransactionID;
}

void IOUringBuffer::setLockStatus(bool lock)
{
	mLockStatus = lock;
}

bool IOUringBuffer::getLockStatus() const
{
	return mLockStatus;
}

void* IOUringBuffer::getBuffer() const
{
	return mBuffer;
}

size_t IOUringBuffer::getBufferSize() const
{
	return mBufferSize;
}

void IOUringBuffer::freeBuffer()
{
	if ( mBuffer ) {
		free(mBuffer);
		mBuffer = nullptr;
	}
}

void IOUringBuffer::make(uint64_t transactionID, const void* data, const size_t size)
{
	if ( mBuffer == nullptr ) {
		mBuffer = malloc(size);
		if ( mBuffer == nullptr ) {
			throw SystemException("malloc() is failed: (size: {})", size);
		}
		mBufferCapacity = size;
	}
	else {
		if ( mBufferCapacity < size ) {
			mBuffer = realloc(mBuffer, size);
			if ( mBuffer == nullptr ) {
				throw SystemException("realloc() is failed: (size: {})", size);
			}
			mBufferCapacity = size;
		}
	}

	mBufferSize = size;
	std::memcpy(mBuffer, data, size);

	mTransactionID = transactionID;
	mLockStatus = true;
}
