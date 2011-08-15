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

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "matchmoded_timings.h"

/* this is the I2C device address (host-side) of the FPGA */
#define DEVADDR 0x3C

/* It's a 26 MHz part */
#define REFCLK_FREQ_HZ  26000000

#define FPGA_IOC_MAGIC  'c'

#define FPGA_IOCWTEST    _IOW(FPGA_IOC_MAGIC,  1, int)
#define FPGA_IOCRTEST    _IOR(FPGA_IOC_MAGIC,  2, int)
#define FPGA_IOCRESET    _IOW(FPGA_IOC_MAGIC,  3, int)
#define FPGA_IOCLED0     _IOW(FPGA_IOC_MAGIC,  4, int)
#define FPGA_IOCLED1     _IOW(FPGA_IOC_MAGIC,  5, int)
#define FPGA_IOCDONE     _IOR(FPGA_IOC_MAGIC,  6, int)
#define FPGA_IOCINIT     _IOR(FPGA_IOC_MAGIC,  7, int)

// whether or not we should use sync info to determine modes
#define USE_SYNC 0 

static struct timing_info self_timed_720p = {
        .hactive 		= 1280,
        .vactive 		= 720,
        .h_fp 			= 220,
	.h_bp 			= 110,
	.v_fp_lines 		= 20,
	.v_bp_lines 		= 5,
	.hsync_width 		= 40,
	.vsync_width_lines 	= 5,
	.pixclk_in_MHz 		= 74,
	.status			= STATUS_DISCONNECTED,
};


static int
timingcmp(struct timing_info *ti1, struct timing_info *ti2)
{
	if ( (ti1->hactive           == ti2->hactive)
	  && (ti1->vactive           == ti2->vactive)
	  && (ti1->htotal            == ti2->htotal)
	  && (ti1->vtotal_lines      == ti2->vtotal_lines)
	  && (ti1->h_fp              == ti2->h_fp)
	  && (ti1->h_bp              == ti2->h_bp)
	  && (ti1->hsync_width       == ti2->hsync_width)
	  && (ti1->v_fp_lines        == ti2->v_fp_lines)
	  && (ti1->v_bp_lines        == ti2->v_bp_lines)
	  && (ti1->vsync_width_lines == ti2->vsync_width_lines)
	  && (ti1->pixclk_in_MHz     == ti2->pixclk_in_MHz)
	  && (ti1->status            == ti2->status) )
		return 0;
	return 1;
}

static void
normalize_timing_info(struct timing_range **ranges, struct timing_info *ti)
{
	struct timing_range *tr;
	int current_range;
	if (!ti || !ranges)
		return;

	if (ti->status == STATUS_DISCONNECTED) {
		*ti = self_timed_720p;
		return;
	}

	for (current_range=0; ranges[current_range]; current_range++) {
		int mismatches = 0;
		tr = ranges[current_range];

		// less reliable measures, such as active lines and pixels, are weighted lighter
		if (!(ti->hactive >= tr->lower.hactive && ti->hactive <= tr->upper.hactive))
			mismatches+=1;
		if (!(ti->vactive >= tr->lower.vactive && ti->vactive <= tr->upper.vactive))
			mismatches+=1;

		// total pixel counts are more reliable, as they only rely upon the gross detection
		// of syncs and are less influenced by "bouncing" on sync edges
		if (!(ti->htotal >= tr->lower.htotal && ti->htotal <= tr->upper.htotal))
			mismatches+=2;
		if (!(ti->vtotal_lines >= tr->lower.vtotal_lines && ti->vtotal_lines <= tr->upper.vtotal_lines))
			mismatches+=2;

#if USE_SYNC
		if (!(ti->h_fp >= tr->lower.h_fp && ti->h_fp <= tr->upper.h_fp))
			mismatches++;
		if (!(ti->h_bp >= tr->lower.h_bp && ti->h_bp <= tr->upper.h_bp))
			mismatches++;
		if (!(ti->hsync_width >= tr->lower.hsync_width && ti->hsync_width <= tr->upper.hsync_width))
			mismatches++;
		if (!(ti->v_fp_lines >= tr->lower.v_fp_lines && ti->v_fp_lines <= tr->upper.v_fp_lines))
			mismatches++;
		if (!(ti->v_bp_lines >= tr->lower.v_bp_lines && ti->v_bp_lines <= tr->upper.v_bp_lines))
			mismatches++;
		if (!(ti->vsync_width_lines >= tr->lower.vsync_width_lines && ti->vsync_width_lines <= tr->upper.vsync_width_lines))
			mismatches++;
#endif
		
		// pixclock is a very robust measure, if this is off, then weight it heavily:
		// it's more than a correct htotal and vtotal combined
		if (!(ti->pixclk_in_MHz >= tr->lower.pixclk_in_MHz && ti->pixclk_in_MHz <= tr->upper.pixclk_in_MHz))
			mismatches+=5;

#if USE_SYNC
		if (mismatches <= 7) {
			*ti = tr->actual;
			return;
		}
#else
		// basically, pixclock is off plus one other thing should trigger a mismatch
		if (mismatches <= 5) {
			*ti = tr->actual;
			return;
		}
#endif
	}

	if (ti->status == STATUS_OK)
		ti->status = STATUS_INVALID;
}


static void
calculate_timing_margin(struct timing_range *tr, int margin) {
	margin += 100;
	tr->lower.hactive = tr->actual.hactive * 100 / margin;
	tr->upper.hactive = tr->actual.hactive * margin / 100;

	tr->lower.vactive = tr->actual.vactive * 100 / margin;
	tr->upper.vactive = tr->actual.vactive * margin / 100;

	tr->lower.htotal = tr->actual.htotal * 100 / margin;
	tr->upper.htotal = tr->actual.htotal * margin / 100;

	tr->lower.vtotal_lines = tr->actual.vtotal_lines * 100 / margin;
	tr->upper.vtotal_lines = tr->actual.vtotal_lines * margin / 100;

	tr->lower.h_fp = tr->actual.h_fp * 100 / margin;
	tr->upper.h_fp = tr->actual.h_fp * margin / 100;

	tr->lower.h_bp = tr->actual.h_bp * 100 / margin;
	tr->upper.h_bp = tr->actual.h_bp * margin / 100;

	tr->lower.hsync_width = tr->actual.hsync_width * 100/margin;
	tr->upper.hsync_width = tr->actual.hsync_width * margin/100;

	tr->lower.v_fp_lines = tr->actual.v_fp_lines * 100/margin;
	tr->upper.v_fp_lines = tr->actual.v_fp_lines * margin/100;

	tr->lower.v_bp_lines = tr->actual.v_bp_lines * 100/margin;
	tr->upper.v_bp_lines = tr->actual.v_bp_lines * margin/100;

	tr->lower.vsync_width_lines = tr->actual.vsync_width_lines * 100/margin;
	tr->upper.vsync_width_lines = tr->actual.vsync_width_lines * margin/100;

	// pixclocks should be very tightly constrained as this is a reliable measurement
	tr->lower.pixclk_in_MHz = tr->actual.pixclk_in_MHz * 100.0/2.0;
	tr->upper.pixclk_in_MHz = tr->actual.pixclk_in_MHz * 2.0/100;
}

/*
static void
print_vinfo(struct fb_var_screeninfo *vinfo)
{
	fprintf(stderr, "\txres: %d\n", vinfo->xres);
	fprintf(stderr, "\tyres: %d\n", vinfo->yres);
	fprintf(stderr, "\txres_virtual: %d\n", vinfo->xres_virtual);
	fprintf(stderr, "\tyres_virtual: %d\n", vinfo->yres_virtual);
	fprintf(stderr, "\txoffset: %d\n", vinfo->xoffset);
	fprintf(stderr, "\tyoffset: %d\n", vinfo->yoffset);
	fprintf(stderr, "\tbits_per_pixel: %d\n", vinfo->bits_per_pixel);
	fprintf(stderr, "\tgrayscale: %d\n", vinfo->grayscale);
	fprintf(stderr, "\tnonstd: %d\n", vinfo->nonstd);
	fprintf(stderr, "\tactivate: %d\n", vinfo->activate);
	fprintf(stderr, "\theight: %d\n", vinfo->height);
	fprintf(stderr, "\twidth: %d\n", vinfo->width);
	fprintf(stderr, "\taccel_flags: %d\n", vinfo->accel_flags);
	fprintf(stderr, "\tpixclock: %d\n", vinfo->pixclock);
	fprintf(stderr, "\tleft_margin: %d\n", vinfo->left_margin);
	fprintf(stderr, "\tright_margin: %d\n", vinfo->right_margin);
	fprintf(stderr, "\tupper_margin: %d\n", vinfo->upper_margin);
	fprintf(stderr, "\tlower_margin: %d\n", vinfo->lower_margin);
	fprintf(stderr, "\thsync_len: %d\n", vinfo->hsync_len);
	fprintf(stderr, "\tvsync_len: %d\n", vinfo->vsync_len);
	fprintf(stderr, "\tsync: %d\n", vinfo->sync);
	fprintf(stderr, "\tvmode: %d\n", vinfo->vmode);
	fprintf(stderr, "\tred: %d/%d\n", vinfo->red.length, vinfo->red.offset);
	fprintf(stderr, "\tgreen: %d/%d\n", vinfo->green.length, vinfo->green.offset);
	fprintf(stderr, "\tblue: %d/%d\n", vinfo->blue.length, vinfo->blue.offset);
	fprintf(stderr, "\talpha: %d/%d\n", vinfo->transp.length, vinfo->transp.offset);
}
*/


static int
parse_timing_info(uint8_t *buffer, struct timing_info *t)
{
	t->hactive = buffer[0] | ((buffer[1] & 0xf) << 8);
	t->vactive = buffer[2] | ((buffer[3] & 0xf) << 8);
	t->htotal  = buffer[4] | ((buffer[5] & 0xf) << 8);
	if (!t->htotal)
	      t->htotal = -1;
	t->vtotal_pix = buffer[6] | (buffer[7] << 8) | (buffer[8] << 16);
	t->vtotal_lines = t->vtotal_pix / t->htotal;
	t->h_fp = buffer[9];
	t->h_bp = buffer[0xa];
	t->v_fp_pix = buffer[0xb] | (buffer[0xc] << 8) | (buffer[0xd] << 16);
	t->v_fp_lines = t->v_fp_pix / t->htotal;
	t->v_bp_pix = buffer[0xe] | (buffer[0xf] << 8) | (buffer[0x10] << 16);
	t->v_bp_lines = t->v_bp_pix / t->htotal;
	t->hsync_width = buffer[0x11];
	t->vsync_width_pix = buffer[0x12] | (buffer[0x13] << 8) | (buffer[0x14] << 16);
	t->vsync_width_lines = t->vsync_width_pix / t->htotal;
	t->refclock_count = buffer[0x15] | (buffer[0x16] << 8) | (buffer[0x17] << 16);

	/* Since registers saturate, sometimes horizontal bp ends up as 255.
	 * Figure out the real horizontal bp by subtracting everything that
	 * isn't the horizontal bp from the horizontal total value.
	 */
	t->h_bp = t->htotal - t->hactive - t->h_fp - t->hsync_width;

	if (t->htotal * t->vtotal_lines) {
		t->ns_per_pix = (t->refclock_count * (1.0 / REFCLK_FREQ_HZ)) / (t->vtotal_pix);
		t->pixclk_in_MHz = (1.0 / t->ns_per_pix) / 1000000.0;
	}
	else {
		t->ns_per_pix = -1.0;
		t->pixclk_in_MHz = -1.0;
	}

	t->status = STATUS_OK;
	if (!(buffer[32]&1))
		t->status = STATUS_DISCONNECTED;
	fprintf(stderr, "Status: %d (0x%02x)\n", t->status, buffer[32]);

	return 0;
}



static int
write_eeprom(char *i2c_device, int addr, int start_reg,
	     unsigned char *buffer, int bytes)
{
	unsigned char               data[bytes+1];
	int                         device_file;
	struct i2c_rdwr_ioctl_data  packets;
	struct i2c_msg              messages[1];

	if((device_file = open(i2c_device, O_RDWR))==-1) {
		perror("Unable to open i2c device");
		return 1;
	}

	// Set the address we'll read to the start address.
	data[0] = start_reg;
	memcpy(data+1, buffer, bytes);

	messages[0].addr    = addr;
	messages[0].flags   = 0;
	messages[0].len     = bytes+1;
	messages[0].buf     = data;

	packets.msgs        = messages;
	packets.nmsgs       = 1;

	if(ioctl(device_file, I2C_RDWR, &packets) < 0) {
		perror("Unable to communicate with i2c device");
		close(device_file);
		return 1;
	}

	close(device_file);
	return 0;
}


static int
read_eeprom(char *i2c_device, int addr, int start_reg,
	    unsigned char *buffer, int bytes)
{
	int				byte;
	int				device_file;
	struct i2c_rdwr_ioctl_data	packets;
	struct i2c_msg			messages[2];
	unsigned char			output;

	byte = (start_reg   ) & 0xff;

	if((device_file = open(i2c_device, O_RDWR))==-1) {
		perror("Unable to open i2c device");
		return 1;
	}

	/* Send the address we'll read to the start address. */
	output = byte;

	messages[0].addr	= addr;
	messages[0].flags	= 0;
	messages[0].len		= sizeof(output);
	messages[0].buf		= &output;

	messages[1].addr	= addr;
	messages[1].flags	= I2C_M_RD;
	messages[1].len		= bytes;
	messages[1].buf		= (unsigned char *)buffer;

	packets.msgs		= messages;
	packets.nmsgs		= 2;

	if(ioctl(device_file, I2C_RDWR, &packets) < 0) {
		perror("Unable to communicate with i2c device");
		close(device_file);
		return 1;
	}

	close(device_file);
	return 0;
}


static int
read_timing_info(struct timing_info *ti)
{
	uint8_t 			buffer[33];

	if(read_eeprom("/dev/i2c-0", DEVADDR>>1, 32, buffer, sizeof(buffer)-1))
		return 1;

	if(read_eeprom("/dev/i2c-0", DEVADDR>>1, 0x18, buffer+32, 1))
		return 1;

	parse_timing_info(buffer, ti);
	return 0;
}


static int
set_timing(int fb0, struct timing_info *timing_info)
{
	struct fb_var_screeninfo var;
	int err;

	err = ioctl(fb0, FBIOGET_VSCREENINFO, &var);
	if (-1 == err) {
		perror("Unable to get variable screen info");
		return 1;
	}

        var.xres = timing_info->hactive;
        var.yres = timing_info->vactive;
        var.xres_virtual = timing_info->hactive;
        var.yres_virtual = timing_info->vactive;
        var.xoffset = 0;
        var.yoffset = 0;
        var.right_margin = timing_info->h_fp;
        var.left_margin = timing_info->h_bp;
        var.lower_margin = timing_info->v_fp_lines;
        var.upper_margin = timing_info->v_bp_lines;
        var.hsync_len = timing_info->hsync_width;
        var.vsync_len = timing_info->vsync_width_lines;
        var.pixclock = timing_info->pixclk_in_MHz * 1000;

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


static void
send_message(char *txt, int width, int height, int bpp)
{
}


static int
load_fpga_firmware(int fpga, char *filename)
{
	char firmware_path[1024];
	unsigned char reg_buf[32];
	unsigned int bytes_left, offset;
	struct stat buf;
	int fd, err;


	fprintf(stderr, "Resetting the FPGA to load firmware %s\n", filename);

	snprintf(firmware_path, sizeof(firmware_path)-1, "/lib/firmware/%s", filename);

	if (-1 == stat(firmware_path, &buf)) {
		perror("Couldn't load firware file");
		return 1;
	}

	char firmware[buf.st_size];

	fd = open(firmware_path, O_RDONLY);
	if (-1 == fd) {
		perror("Couldn't open firmware file");
		return 1;
	}

	bytes_left = sizeof(firmware);
	offset = 0;
	while (bytes_left > 0) {
		err = read(fd, firmware+offset, bytes_left);
		if (-1 == err) {
			perror("Couldn't read firmware file");
			close(fd);
			return 2;
		}
		if (0 == err) {
			fprintf(stderr, "Firmware file was smaller than it should be\n");
			close(fd);
			return 3;
		}

		offset += err;
		bytes_left -= err;
	}
	close(fd);


	/* Read the old register values */
	read_eeprom("/dev/i2c-0", DEVADDR>>1, 0, reg_buf, sizeof(reg_buf));


	/* Reset the FPGA */
	ioctl(fpga, FPGA_IOCRESET, NULL);
	usleep(40000); /* Takes up to 40 ms to respond */

	bytes_left = sizeof(firmware);
	offset = 0;
	while (bytes_left > 0) {
		err = write(fpga, firmware+offset, bytes_left>4096?4096:bytes_left);
		if (-1 == err) {
			perror("Couldn't write firmware file");
			fprintf(stderr, "Error at offset %d\n", offset);
			return 4;
		}
		if (0 == err) {
			fprintf(stderr, "FPGA closed firmware descriptor\n");
			return 5;
		}
		offset += err;
		bytes_left -= err;
	}
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 0, reg_buf, sizeof(reg_buf));

	return 0;
}

static void switch_to_720p()
{
	unsigned char buffer;

	/* Reset the PLL */
	read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	buffer = buffer | 0x10;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	buffer = buffer & ~0x10;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));

	read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	buffer = (buffer & ~0x4) | 0x8;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
}

static void switch_to_overlay()
{
	unsigned char buffer;

	/* Reset the PLL */
	read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	buffer = buffer | 0x10;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	buffer = buffer & ~0x10;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));

	read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	buffer = (buffer & ~0x8) | 0x4;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
}

static void trigger_hpd()
{
	unsigned char buffer;

	/* read comptl, 
	read_eeprom("/dev/i2c-0", DEVADDR>>1, 0, &buffer, sizeof(buffer));
	buffer = buffer | 0x8;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 0, &buffer, sizeof(buffer));

	sleep(1); /* pull it for 1s */

	buffer = buffer & ~0x8;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 0, &buffer, sizeof(buffer));
}

static void handle_winch(int num)
{

	fprintf(stderr, "Window size changed\n");
}

int
main(int argc, char **argv)
{
	int fb0, fpga;
	struct timing_info last_ti, new_ti, raw_ti;
	struct timespec waittime;

	struct timing_range **ranges = timings;
	int current_range;
	unsigned short xsubi[3]; // we use this unitialized on purpose

	signal(SIGWINCH, handle_winch);

	/* Calculate a 10% timing margin */
	for (current_range = 0; ranges[current_range]; current_range++)
		calculate_timing_margin(ranges[current_range], 10);

	fb0 = open("/dev/fb0", O_RDWR);
        if (-1 == fb0) {
                perror("Unable to open framebuffer /dev/fb0");
                return 1;
        }

	fpga = open("/dev/fpga", O_RDWR);
	if (-1 == fpga) {
		perror("Unable to open fpga interface /dev/fpga");
		return 2;
	}

	/*
	 * Attempt to read current timing information.  An error here
	 * indicates i2c is broken.
	 */
	if (read_timing_info(&last_ti))
		return 3;
	last_ti.status = STATUS_UNKNOWN;


	if (daemon(0, 0)==-1)
		perror("Unable to daemonize");


	/* There are two goals of this while() loop.
	 * 	First, we want to detect when the source has been unplugged.
	 * Since there's no signal for that, we have to poll, currently once
	 * per second, to see if the resolution is valid.  If it isn't, we'll
	 * have to switch to self-timed mode.
	 * 	Second, se want to detect when the resolution has changed.
	 * If the new resolution is valid, switch the display to the new
	 * resolution.  Otherwise, switch to self-timed mode and tell the UI
	 * to display an error.
	 */
	while (1) {

		waittime.tv_sec = 1;
		// pick a random interval between 1.0 and ~1.5 seconds
		// why? because a perfect 1 second interval should actually
		// "beat" against an integer sync rate slowly, which means
		// you'll get series of timing samples aligned with blanking
		// periods. Randomness breaks the repetition.
		waittime.tv_nsec = (nrand48(xsubi) & 0x1FFFFFFF);
		if (nanosleep(&waittime, NULL) == -1) {
			/*
			 * Nanosleep returned non-zero.  See if we got hit
			 * by a signal, which indicates some sort of HPD.
			 */
			if (errno != EINTR) {
				perror("Unable to sleep");
				return 1;
			}

			/* We got a state change.  Wait for things to settle. */
			waittime.tv_sec = 0;
			waittime.tv_nsec = 100000000; /* 100 msec */
			while (nanosleep(&waittime, NULL) == -1 && errno == EINTR);
		}


		/* We're now ready to determine if the mode has changed */
		if (read_timing_info(&new_ti))
			continue;

		/* Timing info can be off slightly sometimes.  Pick a sane mode. */
		raw_ti = new_ti;
		normalize_timing_info(ranges, &new_ti);


		/* Do nothing if nothing has changed */
		if (!timingcmp(&last_ti, &new_ti)) {
			fprintf(stderr, "Timing mode didn't change (%dx%d - %d vs %dx%d - %d)\n",
				last_ti.hactive, last_ti.vactive, last_ti.status,
				new_ti.hactive, new_ti.vactive, new_ti.status);
			continue;
		}


		/* By this point, we suspect the resolution has changed. */
		/* but, before we take action, it doesn't hurt to double-check */
		// wait a random interval to avoid sampling in a sync region
		waittime.tv_nsec = (nrand48(xsubi) & 0x7ffffff); // ~ 8 ms
		while (nanosleep(&waittime, NULL) == -1 && errno == EINTR);

		////////////
		// repeat the above timing info check code, as if it were ground hog day
		// there is some debate if this is the right thing to do: maybe it's better
		// to check if the re-sampled timing mode matches the new timing mode precisely,
		// instead of checking a simple delta against the original timing mode
		
		/* We're now ready to determine if the mode has changed */
		if (read_timing_info(&new_ti))
			continue;

		/* Timing info can be off slightly sometimes.  Pick a sane mode. */
		raw_ti = new_ti;
		normalize_timing_info(ranges, &new_ti);


		/* Do nothing if nothing has changed */
		if (!timingcmp(&last_ti, &new_ti)) {
			fprintf(stderr, "Timing mode didn't change (%dx%d - %d vs %dx%d - %d)\n",
				last_ti.hactive, last_ti.vactive, last_ti.status,
				new_ti.hactive, new_ti.vactive, new_ti.status);
			continue;
		}

		/////////////
		/* By this point, we're pretty sure the resolution has changed */

		/*
		 * If we're changing from one invalid mode to another,
		 * don't take any action at all.
		 */
		if ((last_ti.status == STATUS_INVALID && last_ti.status == new_ti.status)
		 || (last_ti.status == STATUS_DISCONNECTED && last_ti.status == new_ti.status)) {
			last_ti = new_ti;
			fprintf(stderr, "Still an invalid mode\n");
			continue;
		}

		fprintf(stderr, "                     Old    New    Raw\n");
		fprintf(stderr, "----------------------------------------\n");
		fprintf(stderr, "hactive             %4d  %4d  %4d\n", last_ti.hactive, new_ti.hactive, raw_ti.hactive);
		fprintf(stderr, "vactive             %4d  %4d  %4d\n", last_ti.vactive, new_ti.vactive, raw_ti.vactive);
		fprintf(stderr, "htotal              %4d  %4d  %4d\n", last_ti.htotal, new_ti.htotal, raw_ti.htotal);
		fprintf(stderr, "vtotal_lines        %4d  %4d  %4d\n", last_ti.vtotal_lines, new_ti.vtotal_lines, raw_ti.vtotal_lines);
		fprintf(stderr, "h_fp                %4d  %4d  %4d\n", last_ti.h_fp, new_ti.h_fp, raw_ti.h_fp);
		fprintf(stderr, "h_fp                %4d  %4d  %4d\n", last_ti.h_bp, new_ti.h_bp, raw_ti.h_bp);
		fprintf(stderr, "hsync_width         %4d  %4d  %4d\n", last_ti.hsync_width, new_ti.hsync_width, raw_ti.hsync_width);
		fprintf(stderr, "v_fp_lines          %4d  %4d  %4d\n", last_ti.v_fp_lines, new_ti.v_fp_lines, raw_ti.v_fp_lines);
		fprintf(stderr, "v_bp_lines          %4d  %4d  %4d\n", last_ti.v_bp_lines, new_ti.v_bp_lines, raw_ti.v_bp_lines);
		fprintf(stderr, "vsync_width_lines   %4d  %4d  %4d\n", last_ti.vsync_width_lines, new_ti.vsync_width_lines, raw_ti.vsync_width_lines);
		fprintf(stderr, "pixclk_in_MHz       %4f  %4f  %4f\n", last_ti.pixclk_in_MHz, new_ti.pixclk_in_MHz, raw_ti.pixclk_in_MHz);
		fprintf(stderr, "status              %4d  %4d  %4d\n", last_ti.status, new_ti.status, raw_ti.status);


		/* If the mode is INVALID, switch to self-timed mode */
		if (new_ti.status == STATUS_INVALID) {
			if (last_ti.status == STATUS_OK || last_ti.status == STATUS_UNKNOWN)
				switch_to_720p();
			set_timing(fb0, &self_timed_720p);
			send_message("invalid", self_timed_720p.hactive, self_timed_720p.vactive, 16);
			fprintf(stderr, "Switched to invalid mode\n");
		}
		else if(new_ti.status == STATUS_DISCONNECTED) {
			if (last_ti.status == STATUS_OK || last_ti.status == STATUS_UNKNOWN)
				switch_to_720p();
			set_timing(fb0, &self_timed_720p);
			send_message("disconnected", self_timed_720p.hactive, self_timed_720p.vactive, 16);
			fprintf(stderr, "Switched to disconnected mode\n");
		}
		else if(new_ti.status == STATUS_OK) {
			if (last_ti.status != STATUS_OK)
				switch_to_overlay();
			set_timing(fb0, &new_ti);
			send_message("connected", new_ti.hactive, new_ti.vactive, 16);
			fprintf(stderr, "Switched to connected, %dx%d mode\n", new_ti.hactive, new_ti.vactive);
			// trigger an HPD if the last mode was invalid, because it means
			// we'll have lost sync on the cipher
			if ( (last_ti.status == STATUS_INVALID) ) {
			  sleep(2); // let res settle before we pull this
			  fprintf(stderr, "triggering HPD to resync ciphers\n" );
			  trigger_hpd();
			}
		}

		/* Tell both NeTVBrowser and the flash player that the resolution changed */
		system("setbrowser r");
		system("setplayer r");

		last_ti = new_ti;
	}

	close(fb0);
	close(fpga);

	return 0;
}
