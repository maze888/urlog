#include "SystemException.h"

using namespace urlog::exception;

const char *
SystemException::what() const noexcept
{
	return mErrorString.c_str();
}

void
SystemException::formatString(fmt::string_view format, fmt::format_args args)
{
	std::error_condition econd = std::generic_category().default_error_condition(mErrno);

	fmt::format_to(std::back_inserter(mErrorString), "(errno: {}, errmsg: {}, ", mErrno, econd.message());
	fmt::vformat_to(std::back_inserter(mErrorString), format, args);
        
	mErrorString.append(")");
}
