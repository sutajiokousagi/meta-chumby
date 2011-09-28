#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define PROCESS "/usr/bin/NeTVBrowser", "-qws"

static int child_pid;
static int should_quit;

void kill_child(int ignored) {
	should_quit = 1;
	kill(child_pid, SIGTERM);
}

int main(int argc, char **argv) {
	int status;

	signal(SIGTERM, kill_child);
	signal(SIGINT,  kill_child);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGSEGV, SIG_IGN);
	signal(SIGFPE,  SIG_IGN);

	while(!should_quit) {
		pid_t pid;

		/* If there's no child running (or if it died), restart it */
		fprintf(stderr, "Child PID: %d\n", child_pid);
		if (child_pid <= 0) {
			if (!(child_pid = fork())) {
				execlp(PROCESS, NULL);
				exit(-1);
			}
		}
				
		pid = waitpid(child_pid, &status, 0);
		if (pid == -1)
			perror("Couldn't wait for pid");
		
		if (WIFEXITED(status))
			should_quit = 1;
		else {
			kill(child_pid, SIGTERM);
			child_pid = 0;
		}
	}

	return 0;
}
