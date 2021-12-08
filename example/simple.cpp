#include "UrLog.h"

using namespace std::chrono;

int main(int argc, char **argv)
{
	urlog::UrLog urlog("./test.log");

	if ( argc != 2 ) {
		fprintf(stderr, "Usage: %s (loop count)\n", argv[0]);
		return 1;
	}

	//FILE *fp = fopen("./test.log", "w");
	for ( int i = 0; i < atoi(argv[1]); i++ ) {
		urlog.log("[basic_st] [info] Hello logger: msg number {}\n", i);
		//printf("[basic_st] [info] Hello logger: msg number %d\n", i);
		//fprintf(fp, "[basic_st] [info] Hello logger: msg number %d\n", i);
		//sleep(3);
	}
	//fclose(fp);

	//printf("%ld %ld\n", time(NULL), system_clock::to_time_t(system_clock::now()));
	//printf("%ld %ld\n", time(NULL), std::time(nullptr));

	/*using namespace std::chrono;

	auto now = system_clock::now();

	//auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
	auto ms = duration_cast<milliseconds>(now.time_since_epoch());
	fmt::print("{} {}", now, ms);*/

	return 0;
}
