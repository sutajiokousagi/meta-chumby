#include <stdio.h>
#include <stdlib.h>

#include "init-example.h"

int main(int argc, char **argv) {
	if (daemon(0, 0)) {
		perror("Unable to daemonize");
		return 1;
	}

	run_daemon();
	return 0;
}
