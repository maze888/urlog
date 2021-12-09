#include "UrLog.h"

/*void test(urlog::UrLog& urlog, int howmany)
{
	for ( int i = 0; i < howmany + 10; i++ ) {
		if ( i > howmany ) sleep(1);
		urlog.log("[basic_st] [info] Hello logger: msg number {}\n", i);
	}
}*/

void test(int howmany)
{
	urlog::UrLog urlog("./test.log");

	for ( int i = 0; i < howmany; i++ ) {
		urlog.log("[basic_st] [info] Hello logger: msg number {}\n", i);
	}
}

int main(int argc, char **argv)
{
	if ( argc != 2 ) {
		fprintf(stderr, "Usage: %s (loop count)\n", argv[0]);
		return 1;
	}
	
	unlink("./test.log");
	
	//urlog::UrLog urlog("./test.log");
	//for ( int i = 0; i < atoi(argv[1]); i++ ) {
		//test(urlog, atoi(argv[1]));
	//}
	
	test(atoi(argv[1]));

	/*std::string mPath = "test.log";
	std::size_t mMaxFile = 10;

	std::size_t found = mPath.find_last_of(".");
	std::string path = mPath.substr(0, found);
	std::string ext = mPath.substr(found + 1);

	//mRotatePath.push_back(path + ext);
	for ( std::size_t i = 1; i < mMaxFile; i++ ) {
		//s = fmt::format("{}.{}.{}", path, i, ext);
		//mRotatePath.push_back(fmt::format("{}.{}.{}", path, i, ext));
		std::cout << fmt::format("{}.{}.{}", path, i, ext) << std::endl;
	}*/

	return 0;
}
