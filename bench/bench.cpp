#include "UrLog.h"

#include <thread>

constexpr int TEST_THREAD_COUNT = 4;

using namespace std::chrono;

void bench_st(int howmany)
{
	urlog::UrLog urlog("./logs/basic_st.log");

	for ( int i = 0; i < howmany; i++ ) {
		urlog.log("[basic_st] [info] Hello logger: msg number {}\n", i);
	}
}

void bench_mt(int howmany, int threadCount)
{
	std::vector<std::thread> threads;

	threads.reserve(threadCount);
	for ( int i = 0; i < threadCount; i++ ) {
		threads.emplace_back([&]() {
			urlog::UrLog urlog("./logs/basic_mt.log");

			for ( int j = 0; j < howmany / threadCount; j++ ) {
				urlog.log("[basic_mt] [info] Hello logger: msg number {}\n", j);
			}
		});
	}

	for ( auto &t : threads) {
		t.join();
	}
}

void bench_st_test(int howmany)
{
	auto start = high_resolution_clock::now();
	bench_st(howmany);
	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();
	fmt::print("{:<30} ELapsed: {:0.2f} secs {:>16L}/sec\n", "basic_st", delta_d, (int)(howmany / delta_d));
}

void bench_mt_test(int howmany, int threadCount)
{
	auto start = high_resolution_clock::now();
	bench_mt(howmany, threadCount);
	auto delta = high_resolution_clock::now() - start;
	auto delta_d = duration_cast<duration<double>>(delta).count();
	fmt::print("{:<30} ELapsed: {:0.2f} secs {:>16L}/sec\n", "basic_mt", delta_d, (int)(howmany / delta_d));
}

int main(int argc, char **argv)
{
	if ( argc != 2 ) {
		fprintf(stderr, "Usage: %s (howmany count)\n", argv[0]);
		return EXIT_FAILURE;
	}

	try {
		int howmany = std::stoi(argv[1]);

		bench_st_test(howmany);
		bench_mt_test(howmany, TEST_THREAD_COUNT);
	}
	catch (std::exception &e) {
		fmt::print("{}\n", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
