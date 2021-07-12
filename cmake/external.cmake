include (ExternalProject)
ExternalProject_Add (fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
	CMAKE_ARGS -DFMT_TEST=off -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/fmt
)

set (LIBURING_DIR ${CMAKE_SOURCE_DIR}/liburing-prefix/src/liburing)
ExternalProject_Add (liburing
	GIT_REPOSITORY https://github.com/axboe/liburing.git
	CONFIGURE_COMMAND ${LIBURING_DIR}/configure --prefix=${CMAKE_SOURCE_DIR}/liburing
	BUILD_IN_SOURCE 1
)
