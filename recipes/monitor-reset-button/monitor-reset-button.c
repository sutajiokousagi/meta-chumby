#include <errno.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/reboot.h>
#include <syslog.h>
#include <signal.h>

#define RESET_SECONDS 5

static int nuke_storage() {
	int fd;
	char zeroes[16];

	fd = open("/dev/mmcblk0", O_RDWR);
	if (-1 == fd) {
		perror("Unable to open /dev/mmcblk0");
		return -1;
	}

	if (-1 == lseek(fd, 480, SEEK_SET)) {
		perror("Unable to seek to partition 3");
		return -1;
	}

	bzero(zeroes, sizeof(zeroes));
	if (write(fd, zeroes, sizeof(zeroes)) != sizeof(zeroes)) {
		perror("Unable to write zeroes");
		return -1;
	}

	close(fd);
	return 0;
}

static int kill_watchdog() {
	int fd;
	int pid;
	char bfr[128];

	fd = open("/var/run/watchdog.pid", O_RDONLY);
	if (-1 == fd) {
		perror("Unable to find watchdog pid -- is it running");
		return -1;
	}

	bzero(bfr, sizeof(bfr));
	read(fd, bfr, sizeof(bfr)-1);
	close(fd);

	sscanf(bfr, "%d", &pid);
	if (pid)
		return kill(pid, SIGTERM);
	return -1;
}
	

static int do_reset_system(void) {
	syslog(LOG_INFO, "Button hit.  Nuking storage partition and rebooting.");
	kill_watchdog();
	nuke_storage();
	sync();
	reboot(RB_AUTOBOOT);
	
	return 0;
}

int main(int argc, char **argv) {
	int fd;
	struct input_event ev;
	int button_down = 0;


	fd = open("/dev/input/event0", O_RDONLY);

	if (fd == -1) {
		perror("Unable to open reset button");
		return 1;
	}

	daemon(0, 0);
	openlog("reset-button", 0, LOG_USER);

	while (1) {
		int bytes;
		fd_set fds;
		struct timeval delay;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		delay.tv_sec = RESET_SECONDS;
		delay.tv_usec = 0;

		/* See if there's any data available */
		bytes = select(fd+1, &fds, NULL, NULL, &delay);
		if (bytes == -1 && EINTR)
			continue;
		if (bytes == -1) {
			syslog(LOG_ERR, "Unable to run select: %m");
			close(fd);
			return 1;
		}

		/*
		 * This will trigger if the button is down, and if
		 * zero bytes were read (likely due to ETIMEDOUT).
		 */
		if (!bytes) {
			if (button_down)
				return do_reset_system();
			continue;
		}

		bytes = read(fd, &ev, sizeof(ev));

		if (bytes == -1 && errno == EINVAL)
			continue;

		if (bytes == -1) {
			syslog(LOG_ERR, "Unable to read from button: %m");
			close(fd);
			return 1;
		}

		if (!bytes) {
			syslog(LOG_ERR, "Reached end-of-file on input button: Is it a regular file?");
			close(fd);
			return 1;
		}

		if (ev.type != EV_KEY)
			continue;

		/* Keydown */
		button_down = ev.value;
	}
}
