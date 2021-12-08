cmake_minimum_required (VERSION 3.14)

project (urlog CXX)

set (CMAKE_VERBOSE_MAKEFILE off)
set (FETCHCONTENT_QUIET on)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_BUILD_TYPE Debug)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pg")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)	

include(FetchContent)

FetchContent_Declare(
	fmt
	GIT_REPOSITORY https://github.com/fmtlib/fmt.git
)
FetchContent_Declare(
	liburing
	GIT_REPOSITORY https://github.com/axboe/liburing.git
)

# liburing_SOURCE_DIR 변수 자동 생성 및 target_link_libraries 시 include path, library path 등을 자동으로 잡아줌
FetchContent_MakeAvailable(fmt liburing)
set (FETCHCONTENT_UPDATES_DISCONNECTED on)

if (NOT EXISTS build)
file (MAKE_DIRECTORY build)
endif()

include_directories (${CMAKE_SOURCE_DIR}/include ${liburing_SOURCE_DIR}/src/include)
link_directories (${liburing_BINARY_DIR}/lib)
link_libraries (uring)

file (GLOB SRCS src/*.cpp)

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
add_library (urlog STATIC ${SRCS})
#add_library (urlog SHARED ${SRCS})
target_link_libraries (urlog fmt liburing.a)

# liburing 경우는 cmake 기반이 아니라서, custom 명령으로 처리하였음.
# TODO: liburing 을 c++ wrapper 라이브러리로 cmake 기반으로 재작성..?
add_custom_target(
	liburing
	cd ${liburing_SOURCE_DIR} && ./configure --prefix=${liburing_BINARY_DIR} && make -j && make install
)
add_dependencies(urlog liburing)

add_subdirectory (example)
add_subdirectory (bench)
add_subdirectory (tail_monitor)
