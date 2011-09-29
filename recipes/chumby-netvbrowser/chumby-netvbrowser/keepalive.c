#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>

#define PROCESS "/usr/bin/NeTVBrowser.real"
//#define PROCESS "./target"

static int child_pid;
static int should_quit;

void kill_child(int ignored) {
	should_quit = 1;
	kill(child_pid, SIGTERM);
}

void child_died(int ignored) {
	int status;
	pid_t pid;

	pid = waitpid(child_pid, &status, 0);
	if (pid == -1) {
		perror("Couldn't wait for pid");
		should_quit = 1;
	}
	
	if (WIFEXITED(status))
		should_quit = 1;
	else {
		syslog(LOG_WARNING,
		     "Child exited %d, signaled %d, returned status: 0x%08x\n",
		    WEXITSTATUS(status), WTERMSIG(status), status);
		kill(child_pid, SIGTERM);
		child_pid = 0;
	}
}

int main(int argc, char **argv) {
	int i;
	char *my_args[argc+1];

	/* Copy our arguments to the process arguments list */
	my_args[0] = PROCESS;
	for(i=1; i<argc; i++)
		my_args[i] = argv[i];
	my_args[i] = NULL;

	signal(SIGTERM, kill_child);
	signal(SIGINT,  kill_child);
	signal(SIGCHLD, child_died);
	signal(SIGSEGV, SIG_IGN);
	signal(SIGFPE,  SIG_IGN);

	openlog("NeTVBrowser", 0, LOG_LOCAL0);

	while(!should_quit) {

		/* If there's no child running (or if it died), restart it */
		if (child_pid <= 0) {
			if (!(child_pid = fork())) {
				execvp(PROCESS, my_args);
				perror("Couldn't start up " PROCESS);
				exit(0);
			}
			syslog(LOG_INFO, "Started child. PID: %d\n", child_pid);
		}

		/*
		 * Do nothing, wait for SIGCHLD to hit, which will
		 * cause select() to return.
		 */
		select(0, NULL, NULL, NULL, NULL);
	}

	return 0;
}
