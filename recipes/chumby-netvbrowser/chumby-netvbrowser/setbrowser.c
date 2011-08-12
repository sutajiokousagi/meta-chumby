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

#include <sys/socket.h>
#include <sys/un.h>

#include <sys/dir.h>
#include <dirent.h>

#include <arpa/inet.h>

static int find_socket(char *name, char *out, int outsize)
{
	struct dirent *de;
	DIR *dir;
	int found = 0;
	char name_tmp[strlen(name)+3];

	dir = opendir("/tmp/");
	if (!dir)
		return 1;

	name_tmp[0] = '-';
	memcpy(name_tmp+1, name, sizeof(name_tmp)-3);
	name_tmp[sizeof(name_tmp)-2] = '-';
	name_tmp[sizeof(name_tmp)-1] = '\0';

	while (!found && (de = readdir(dir))) {
		if (strstr(de->d_name, name_tmp) && !strstr(de->d_name, "-lockfile")) {
			snprintf(out, outsize, "%s/%s", "/tmp/", de->d_name);
			found = 1;
		}
	}

	closedir(dir);
	return !found;
}

static int read_var(char *path, struct fb_var_screeninfo *var)
{
	int fd;
	fd = open(path, O_RDWR);
	if (fd == -1) {
		perror("Unable to open framebuffer");
		return 1;
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, var)) {
		perror("FBIOGET_VSCREENINFO failed");
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}


static int send_message(char *sig, char *message, int length)
{
	int sock;
	int len;
	struct sockaddr_un remote;

	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == sock) {
		perror("Unable to create socket");
		return 1;
	}


	remote.sun_family = AF_UNIX;
	if (find_socket(sig, remote.sun_path, sizeof(remote.sun_path))) {
		fprintf(stderr, "Unable to locate socket: File not found\n");
		close(sock);
		return 1;
	}
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);


	if (connect(sock, (struct sockaddr *)&remote, len) == -1) {
		perror("Unable to conncet to socket");
		close(sock);
		return 1;
	}

	send(sock, message, length, 0);

	close(sock);
	return 0;
}


int main(int argc, char **argv)
{
	struct fb_var_screeninfo var;
	char res[4096];

	if (read_var("/dev/fb0", &var))
		return 1;

	bzero(res, sizeof(res));
	snprintf(res+4, sizeof(res)-1,
		"NeTVBrowser|~|SetResolution|~|%d|~|%d|~|%d",
		var.xres, var.yres, var.bits_per_pixel);
	((long *)res)[0] = htonl(strlen(res+4)+1);
	send_message("NeTVBr", res, strlen(res+4)+5);

	return 0;
}
