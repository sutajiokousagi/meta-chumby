#include <errno.h>
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

static int do_reset_system(void) {
	syslog(LOG_INFO, "Button hit.  Rebooting.");
	unlink("/psp/network_config");
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
		delay.tv_sec = 2;
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
		button_down = !ev.value;
	}
}
