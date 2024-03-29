#include "IOUring.h"
#include "SystemException.h"

using namespace urlog::iouring;
using namespace urlog::exception;

constexpr unsigned MAX_IO_URING_QUEUE_SIZE = 8192;

IOUring::IOUring()
	: mFileFD(-1), mRemainingCompletions(0)
{
}

IOUring::~IOUring()
{
	// test
	//printf("mIOUringBuffers: %lu\n", mIOUringBuffers.size());
	std::uintmax_t sum = 0;
	for ( auto it = begin(mIOUringBuffers); it != end(mIOUringBuffers); it++ ) {
		sum += it->get()->getBufferSize();
	}
	//printf("total buffer: %luk\n", sum / 1024);

	while ( mRemainingCompletions > 0 ) {
		complete();
	}
	//io_uring_unregister_files(&mRing);
	if ( mFileFD >= 0 ) close(mFileFD);
	if ( mCqes ) free(mCqes);
	io_uring_queue_exit(&mRing);
}

void IOUring::init(const std::string path)
{
	int rv = io_uring_queue_init(MAX_IO_URING_QUEUE_SIZE, &mRing, 0); // IORING_SETUP_ATTACH_WQ
	if ( rv < 0 ) {
		throw SystemException(-rv, "io_uring_queue_init() is failed");
	}

	mFileFD = open(path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
	if ( mFileFD < 0 ) {
		throw SystemException("open() is failed: (path: {})", path.c_str());
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
		
	for ( int i = 0; i < completions; i++ ) {
		if ( mCqes[i]->res < 0 ) {
			throw SystemException(-mCqes[i]->res, "complete() async task is failed");
			continue;
		}
	
		for ( auto& it : mIOUringBuffers ) {
			if ( it.get()->getTransactionID() == mCqes[i]->user_data ) {
				it.get()->setLockStatus(false);
			}
		}
	}
}

IOUringBuffer* IOUring::getIOUringBuffer(uint64_t transactionID, const void* data, const size_t size)
{
	for ( auto& it : mIOUringBuffers ) {
		if ( it.get()->getLockStatus() == false ) {
			it.get()->make(transactionID, data, size);
			return it.get();
		}
	}

	// empty or all lock
	std::unique_ptr<IOUringBuffer> buffer = std::make_unique<IOUringBuffer>();

	buffer->make(transactionID, data, size);

	mIOUringBuffers.push_back(std::move(buffer));

	return mIOUringBuffers.back().get();
}
	
void IOUring::reopen(const std::string path)
{
	/*while ( mRemainingCompletions > 0 ) {
		complete();
	}*/

	if ( mFileFD >= 0 ) close(mFileFD);

	mFileFD = open(path.c_str(), O_CREAT | O_WRONLY | O_APPEND | O_TRUNC, 0644);
	if ( mFileFD < 0 ) {
		throw SystemException("reopen() is failed: (path: {})", path.c_str());
	}
}
