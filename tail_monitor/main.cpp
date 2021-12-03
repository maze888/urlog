#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>

void print_tail(const char *path, std::uintmax_t old_size, std::uintmax_t new_size)
{
	std::uintmax_t read_size = new_size - old_size;
	
	FILE *fp = fopen(path, "r");
	if ( !fp ) {
		fprintf(stderr, "fopen() is failed: (errno: %d, errmsg: %s, path: %s)\n", errno, strerror(errno), path);
		exit(1);
	}
	fseek(fp, old_size, SEEK_SET);

	char *buf = (char *)malloc(read_size);
	if ( !buf ) {
		fprintf(stderr, "malloc() is failed: (errno: %d, errmsg: %s, size: %lu", errno, strerror(errno), read_size);
		exit(1);
	}

	size_t nread = fread(buf, 1, read_size, fp);
	fprintf(stdout, "%s", buf);
	(void)nread;

	free(buf);
	fclose(fp);
}

int main(int argc, char **argv)
{
	if ( argc != 2 ) {
		fprintf(stderr, "Usage: %s (path) [-s (millisecond interval)]\n", argv[0]);
		return 1;
	}

	int opt, millisecond = 0;
	while ( (opt = getopt(argc, argv, "s:")) != -1 ) {
		switch(opt) {
			case 's':
				millisecond = atoi(optarg);
				break;
		}
	}

	if ( millisecond == 0 ) {
		millisecond = 1;
	}

	try {
		std::uintmax_t old_size, new_size;

		old_size = std::filesystem::file_size(argv[1]);

		while (1) {
			new_size = std::filesystem::file_size(argv[1]);

			if ( old_size < new_size ) {
				print_tail(argv[1], old_size, new_size);
				old_size = new_size;
			}
			else if ( old_size >= new_size ) { // this case is new file
				old_size = std::filesystem::file_size(argv[1]);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
		}
	} 
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
