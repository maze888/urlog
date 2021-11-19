#include <string.h>

#include "IOUringBuffer.h"
#include "SystemException.h"

using namespace urlog::exception;
using namespace urlog::iouringbuffer;

IOUringBuffer::IOUringBuffer()
	:mTransactionID(0), mBuffer(NULL), mBufferSize(0), mBufferCapacity(0), mLockStatus(false)
{
}

IOUringBuffer::~IOUringBuffer()
{
	if ( mBuffer ) {
		free(mBuffer);
		mBuffer = NULL;
	}
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

void IOUringBuffer::make(uint64_t transactionID, const void* data, const size_t size)
{
	if ( mBuffer == NULL ) {
		mBuffer = malloc(size);
		if ( mBuffer == NULL ) {
			throw SystemException("malloc() is failed: (size: {})", size);
		}
		mBufferCapacity = size;
	}
	else {
		while ( mBufferCapacity < size ) {
			mBufferCapacity <<= 1;
			mBuffer = realloc(mBuffer, mBufferCapacity);
			if ( mBuffer == NULL ) {
				throw SystemException("realloc() is failed: (size: {})", mBufferCapacity);
			}
		}
	}

	mBufferSize = size;
	memcpy(mBuffer, data, size);

	mTransactionID = transactionID;
	mLockStatus = true;
}
