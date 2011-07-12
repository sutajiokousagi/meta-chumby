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

#include <linux/i2c.h>
#include <linux/i2c-dev.h>


/* this is the I2C device address (host-side) of the FPGA */
#define DEVADDR 0x3C

/* It's a 26 MHz part */
#define REFCLK_FREQ_HZ  26000000



struct timing_info {
	unsigned int hactive;
	unsigned int vactive;
	int htotal;
	unsigned int vtotal_pix;
	unsigned int vtotal_lines;
	unsigned int h_fp;
	unsigned int h_bp;
	unsigned int v_fp_pix;
	unsigned int v_fp_lines;
	unsigned int v_bp_pix;
	unsigned int v_bp_lines;
	unsigned int hsync_width;
	unsigned int vsync_width_pix;
	unsigned int vsync_width_lines;
	unsigned int refclock_count;
	unsigned int lcd_de_latency_lines;
	unsigned int lcd_vsync_latency_lines;
	double pixclk_in_MHz;
	double ns_per_pix;
};


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
match_mode(int fd, struct fb_var_screeninfo *var)
{
	uint8_t 			buffer[32];
	struct timing_info		timing_info;

	if(read_eeprom("/dev/i2c-0", DEVADDR>>1, 32, buffer, sizeof(buffer)))
		return 1;

	parse_timing_info(buffer, &timing_info);

        var->xres = timing_info.hactive;
        var->yres = timing_info.vactive;
        var->xres_virtual = timing_info.hactive;
        var->yres_virtual = timing_info.vactive;
        var->xoffset = 0;
        var->yoffset = 0;
        var->right_margin = timing_info.h_fp;
        var->left_margin = timing_info.h_bp;
        var->lower_margin = timing_info.v_fp_lines;
        var->upper_margin = timing_info.v_bp_lines;
        var->hsync_len = timing_info.hsync_width;
        var->vsync_len = timing_info.vsync_width_lines;
        var->pixclock = timing_info.pixclk_in_MHz * 1000;

	var->activate |= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
}

int
main(int argc, char **argv)
{
	int fd, err;
	struct fb_var_screeninfo var;

	fd = open("/dev/fb0", O_RDWR);
        if (-1 == fd) {
                perror("Unable to open framebuffer");
                return 1;
        }


	err = ioctl(fd, FBIOGET_VSCREENINFO, &var);
	if (-1 == err) {
		perror("Unable to get variable screen info");
		return 1;
	}


	match_mode(fd, &var);

	fprintf(stderr, "Setting new mode:\n");
	print_vinfo(&var);

	err = ioctl(fd, FBIOPUT_VSCREENINFO, &var);
	if (-1 == err) {
		perror("Unable to set variable screen info");
		return 1;
	}


	close(fd);
	return 0;
}
