#pragma once

#include <string>
#include <memory>

namespace urlog::iouringbuffer {

	class IOUringBuffer {
		public:
			IOUringBuffer();
			~IOUringBuffer();

			uint64_t getTransactionID() const;
			void setLockStatus(bool lock);
			bool getLockStatus() const;
			void* getBuffer() const;
			size_t getBufferSize() const;

			void make(uint64_t transactionID, const void* data, const size_t size);
		private:
			uint64_t mTransactionID;
			void* mBuffer;
			size_t mBufferSize;
			size_t mBufferCapacity;
			bool mLockStatus;
	};

};
