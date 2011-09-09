#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>


static int
set_timing(int fb0)
{
	struct fb_var_screeninfo var;
	int err;

	err = ioctl(fb0, FBIOGET_VSCREENINFO, &var);
	if (-1 == err) {
		perror("Unable to get variable screen info");
		return 1;
	}

	var.xres = 1024;
	var.yres = 600;
	var.xres_virtual = 1024;
	var.yres_virtual = 600;
	var.xoffset = 0;
	var.yoffset = 0;
	var.right_margin = 60;
	var.left_margin = 60;
	var.lower_margin = 7;
	var.upper_margin = 7;
	var.hsync_len = 40;
	var.vsync_len = 5;
	var.pixclock = 44.2 * 1000;

	var.activate |= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;

	err = ioctl(fb0, FBIOPUT_VSCREENINFO, &var);
	if (-1 == err) {
		perror("Unable to set variable screen info");
		return 1;
	}


	/* Hack to force the display to take the values */
	close(open("/dev/fb0", O_RDWR));

	return 0;
}


int main() {
	int fb0;

	fb0 = open("/dev/fb0", O_RDWR);
	if (-1 == fb0) {
		perror("Unable to open framebuffer /dev/fb0");
		return 1;
	}

	set_timing(fb0);

	return 0;
}
