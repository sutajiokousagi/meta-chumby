#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <strings.h>
#include <string.h>

static void print_syntax(char *name) {
	printf("syntax: %s [-s -f -h -n -t]\n"
		"\t-s\t- display software version\n"
		"\t-f\t- display firmware build number\n"
		"\t-h\t- display hardware version number\n"
		"\t-n\t- display hardware serial number\n"
		"\t-t\t- display hardware time since last crypto processor power cycle\n"
	, name);
}

static void print_file(char *filename) {
	int fd;
	char bfr[256];
	int bytes_read;
	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		printf("unknown");
		return;
	}

	while( (bytes_read = read(fd, bfr, sizeof(bfr))) > 0)
		write(STDOUT_FILENO, bfr, bytes_read);
	close(fd);
}

static void print_cpi_response(char *cpi_query) {
	FILE *p;
	char cmd[512];
	char bfr[256];
	int bytes_read;

	bzero(cmd, sizeof(cmd));
	snprintf(cmd, sizeof(cmd)-1, "echo '%s' | cpi -i -o", cpi_query);

	p = popen(cmd, "r");
	if (!p) {
		perror("Unable to open cpi command");
		printf("unknown");
		return;
	}

	while( (bytes_read = read(fileno(p), bfr, sizeof(bfr))) > 0)
		write(STDOUT_FILENO, bfr, bytes_read);

	pclose(p);
}
	
static void print_sw_version() {
	print_file("/etc/software_version");
}

static void print_fw_version() {
	print_file("/etc/firmware_build");
}

static void print_hw_version() {
	print_cpi_response("<?xml version=\"1.0\"?><cpi version=\"1.0\"><query_list><query type=\"hwvr\"></query></query_list></cpi>");
}

static void print_hw_serial() {
	print_cpi_response("<?xml version=\"1.0\"?><cpi version=\"1.0\"><query_list><query type=\"snum\"></query></query_list></cpi>");
}

static void print_hw_time() {
	print_cpi_response("<?xml version=\"1.0\"?><cpi version=\"1.0\"><query_list><query type=\"time\"></query></query_list></cpi>");
}



int main(int argc, char **argv) {
	if (argc < 2) {
		print_syntax(argv[0]);
		return 1;
	}
	else if(!strcmp(argv[1], "-s"))
		print_sw_version();
	else if(!strcmp(argv[1], "-f"))
		print_fw_version();
	else if(!strcmp(argv[1], "-h"))
		print_hw_version();
	else if(!strcmp(argv[1], "-n"))
		print_hw_serial();
	else if(!strcmp(argv[1], "-t"))
		print_hw_time();
	else {
		print_syntax(argv[0]);
		return 1;
	}
	
	return 0;
}
