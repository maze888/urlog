#pragma once

#include <fmt/format.h>

namespace urlog::exception {

class SystemException : public std::system_error {
public:
    template <typename S, typename... Args>
    SystemException(const S& format, Args&&... args)
    {
		mErrno = errno;
		formatString(format, fmt::make_args_checked<Args...>(format, args...));
    }
    
	template <typename S, typename... Args>
    SystemException(int error, const S& format, Args&&... args)
    {
		mErrno = error;
        formatString(format, fmt::make_args_checked<Args...>(format, args...));
    }

    ~SystemException() {};

    const char * what() const noexcept;

private:
    void formatString(fmt::string_view format, fmt::format_args args);

    std::string mErrorString;
	int mErrno;
};

};
