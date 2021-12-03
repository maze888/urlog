#include "IOUring.h"
#include "SystemException.h"

#include <fcntl.h>

using namespace urlog::iouring;
using namespace urlog::exception;

constexpr unsigned MAX_IO_URING_QUEUE_SIZE = 256;

IOUring::IOUring()
	: mFileFD(-1), mRemainingCompletions(0)
{
}

IOUring::~IOUring()
{
	//printf("size: %ld\n", mIOUringBuffers.size());
	while ( mRemainingCompletions > 0 ) {
		complete();
	}
	//io_uring_unregister_files(&mRing);
	if ( mFileFD >= 0 ) close(mFileFD);
	if ( mCqes ) free(mCqes);
	io_uring_queue_exit(&mRing);
}

void IOUring::init(const std::string fileName)
{
	int rv = io_uring_queue_init(MAX_IO_URING_QUEUE_SIZE, &mRing, 0);
	//int rv = io_uring_queue_init(MAX_IO_URING_QUEUE_SIZE, &mRing, IORING_SETUP_SQPOLL | IORING_SETUP_SQ_AFF);
	//int rv = io_uring_queue_init(MAX_IO_URING_QUEUE_SIZE, &mRing, IORING_SETUP_SQPOLL);
	if ( rv < 0 ) {
		throw SystemException(-rv, "io_uring_queue_init(64) is failed");
	}

	mFileFD = open(fileName.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
	if ( mFileFD < 0 ) {
		throw SystemException("open() is failed: (path: {})", fileName.c_str());
	}

	if ( io_uring_register_files(&mRing, &mFileFD, 1) ) {
		throw SystemException("io_uring_register_files() is failed");
	}

	mCqes = (struct io_uring_cqe **)malloc(sizeof(struct io_uring_cqe *) * MAX_IO_URING_QUEUE_SIZE);
	if ( !mCqes ) {
		throw SystemException("malloc() is failed");
	}
}
	
void IOUring::submit(uint64_t transactionID, const void* data, size_t size)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(&mRing);
	if ( !sqe ) {
		throw SystemException("io_uring_get_sqe() is failed");
	}

	IOUringBuffer *buffer = getIOUringBuffer(transactionID, data, size);

	//sqe->flags |= IOSQE_IO_LINK;
	//io_uring_sqe_set_data(sqe, data);
	// EBADF

	struct iovec iov;
	iov.iov_base = buffer->getBuffer();
	iov.iov_len = buffer->getBufferSize();

	io_uring_prep_writev(sqe, mFileFD, &iov, 1, 0);
	io_uring_sqe_set_data(sqe, (void *)transactionID);

	/*if ( io_uring_register_buffers(&mRing, &iov, 1) ) {
		SystemException("io_uring_register_buffers() is failed");
	}*/
	
	mRemainingCompletions += io_uring_submit(&mRing);
}
	
void IOUring::complete()
{
	int completions = io_uring_peek_batch_cqe(&mRing, mCqes, MAX_IO_URING_QUEUE_SIZE);

	if ( completions == 0 ) return;

	io_uring_cq_advance(&mRing, completions);
	mRemainingCompletions -= completions;
		
	//if ( completions > 1 ) printf("completions: %d\n", completions);

	for ( int i = 0; i < completions; i++ ) {
		if ( mCqes[i]->res < 0 ) {
			throw SystemException(-mCqes[i]->res, "complete() async task is failed");
		}
	
		//printf("res: %d\n", mCqes[i]->res);
		for ( auto it = begin(mIOUringBuffers); it != end(mIOUringBuffers); it++ ) {
			if ( it->get()->getTransactionID() == mCqes[i]->user_data ) {
				it->get()->setLockStatus(false);
			}
		}
	}
}

IOUringBuffer* IOUring::getIOUringBuffer(uint64_t transactionID, const void* data, const size_t size)
{
	for ( auto it = begin(mIOUringBuffers); it != end(mIOUringBuffers); it++ ) {
		if ( it->get()->getLockStatus() == false ) {
			it->get()->make(transactionID, data, size);
			//printf("here return.\n");
			return it->get();
		}
	}

	// empty or all lock
	std::unique_ptr<IOUringBuffer> buffer = std::make_unique<IOUringBuffer>();

	buffer->make(transactionID, data, size);

	mIOUringBuffers.push_back(std::move(buffer));

	return mIOUringBuffers.back().get();
}
