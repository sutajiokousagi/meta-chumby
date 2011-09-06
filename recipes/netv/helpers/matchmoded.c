/***
This code uses the BSD 2-clause license:

Copyright (c) 2011
  Andrew "bunnie" Huang / bunnie@bunniestudios.com
  Sean Cross / scross@chumby.com
All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

   Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***/
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

#define SINK_DISCONNECTED 0
#define SINK_CONNECTED 1
#define PLL_UNLOCKED   0
#define PLL_LOCKED     1

static int fd = 0;
static int   *mem_32 = 0;
static short *mem_16 = 0;
static char  *mem_8  = 0;
static int *prev_mem_range = 0;
static int read_kernel_memory(long offset, int virtualized, int size);

static int saw_trigger = 0;
static int sink_state = SINK_DISCONNECTED;
static int lockout_attach = 0;
static int attach_lockedout = 0;
static int rxpll_state = PLL_UNLOCKED;
static int event_counter = 0;
static int overlay_semaphore = 0;

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


static void
deepsleep( int seconds, int milliseconds ) {
  struct timespec waittime;
  struct timespec remtime;

  waittime.tv_sec = seconds;
  waittime.tv_nsec = milliseconds * 1000 * 1000;

  while(nanosleep(&waittime, &remtime) == -1) {
    // this makes us sleep even if we get a signal for the specified time
    waittime.tv_sec = remtime.tv_sec;
    waittime.tv_nsec = remtime.tv_nsec;
  }
}

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

// returns 1 if there is a TV present
// returns 0 if there isn't one
static int sink_status() {
  unsigned int gplr2;
  unsigned int hpd_state;
  // gpio_gplr2, bits 95:64
  gplr2 = read_kernel_memory(0xd4019008, 0, 4);
  hpd_state = gplr2 & 0x08000000 ? SINK_CONNECTED : SINK_DISCONNECTED;
  
  return hpd_state;
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

	// internal debate: do we use re-derived values here
	// or should we rely on the values derived by the handlers?
	// re-derived values are used for now but this may lead to inconsistency.
	// however, not re-deriving can lead to staleness and errors due to missed
	// interrupt events.
	t->status = STATUS_OK;
	if( sink_status() == SINK_DISCONNECTED ) {
	  fprintf( stderr, "  HPD says sink disconnected\n" );
	  t->status = STATUS_DISCONNECTED;
	} else {
	  fprintf( stderr, "  HPD says sink connected, and " );
	  // we're connected, but check if the PLL is unlocked; if it is, then there's no source
	  if ( !(buffer[32]&1) ) { // this is register 0x18 through some magic done earlier
	    fprintf( stderr, "Rx PLL is unlocked.\n" );
	    t->status = STATUS_NOSOURCE;
	  } else {
	    fprintf( stderr, "Rx PLL is locked.\n" );
	  }
	}
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


static char *
normalize_timing_info(struct timing_range **ranges, struct timing_info *ti)
{
	struct timing_range *tr;
	int current_range;
	int oldstatus;
	int old_vtotal;
	int old_htotal;
	
	int do_tweak = 0;

	struct timing_info check_timing;

	if (!ti || !ranges)
		return NULL;

	if (ti->status == STATUS_DISCONNECTED) {
		*ti = self_timed_720p;
		return NULL;
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

		do_tweak = 0;

#if USE_SYNC
		if (mismatches <= 7) {
#else
		// basically, pixclock is off plus one other thing should trigger a mismatch
		if (mismatches <= 5) {
#endif
		  oldstatus = ti->status;
		  old_vtotal = ti->vtotal_lines;
		  old_htotal = ti->htotal;

		  *ti = tr->actual; // "snap" to known CEA timing

		  ti->status = oldstatus;

		  if( ((old_vtotal < ti->vtotal_lines) && 
		       ((ti->vtotal_lines - old_vtotal) <= (ti->vtotal_lines / 100)))  ||
		      ((old_htotal > ti->htotal) && 
		       ((old_htotal - ti->htotal) <= (ti->htotal / 100)))
		      ) {
		    // pre-screen the case that CEA timings are off, and double-check it.
		    fprintf( stderr, "<warning> Non-CEA mode detected, checking if it's transient.\n" );
		    deepsleep(0, 40); // wait approx 1 frame to verify
		    read_timing_info(&check_timing);
		    if( (check_timing.vtotal_lines == old_vtotal) && (check_timing.htotal == old_htotal) ) {
		      do_tweak = 1;
		    } else {
		      do_tweak = 0;
		    }
		  }

		  if( do_tweak ) {
		    if( (old_vtotal < ti->vtotal_lines) && 
			((ti->vtotal_lines - old_vtotal) <= (ti->vtotal_lines / 100)) ) {
		      // allow for up to ~1% deviation in vertical size
		      fprintf( stderr, "<warning> Monitor not CEA compliant, tweaking vertical timing to accommodate.\n" );
		      if( ti->v_fp_lines > (ti->vtotal_lines - old_vtotal + 2) ) {
			ti->v_fp_lines -= (ti->vtotal_lines - old_vtotal);
			ti->vtotal_lines = old_vtotal;
		      } else if( ti->v_bp_lines > (ti->vtotal_lines - old_vtotal + 2) ) {
			ti->v_bp_lines -= (ti->vtotal_lines - old_vtotal);
			ti->vtotal_lines = old_vtotal;
		      } else {
			fprintf( stderr, "<warning> Insufficient vertical sync width to accommodate tweak, aborting.\n" );
		      }
		    }

		    if( (old_htotal > ti->htotal) && 
			((old_htotal - ti->htotal) <= (ti->htotal / 100)) ) {
		      // allow for up to ~1% deviation in horizontal size
		      fprintf( stderr, "<warning> Monitor not CEA compliant, tweaking horizontal timing to accommodate.\n" );
		      ti->h_bp += old_htotal - ti->htotal;
		      ti->htotal +=  old_htotal - ti->htotal;
		    }
		  }

		  return tr->name;
		}
	}

	if (ti->status == STATUS_OK)
		ti->status = STATUS_INVALID;

	return NULL;
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

static void switch_to_overlay()
{
	unsigned char buffer;
	
	fprintf( stderr, "<action> ***** switching to overlay mode.\n" );

	if( !lockout_attach ) {
	  read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	  buffer = (buffer & ~0x8) | 0x4;
	  write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	} else {
	  fprintf( stderr, "<action> switch to overlay mode ABORTED\n" );
	  attach_lockedout = 1;
	}
}

static void switch_to_720p()
{
	unsigned char buffer;
	struct timespec waittime;
	struct timespec remtime;

	read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	// trigger this only if we're coming from pass-through mode
	// i.e., don't double-switch to self-timed mode
	if( !(buffer & 0x8) ) {
	  attach_lockedout = 0;
	  lockout_attach = 1;
	  fprintf( stderr, "<action> ***** switching pixel pipe to 720pST.\n" );
	  buffer = (buffer & ~0x4) | 0x8;
	  write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));

#if 0  // let's see if we really need this...
	  /* Reset the PLL */
	  read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	  buffer = buffer | 0x10;
	  write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	  buffer = buffer & ~0x10;
	  write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
#endif
	  lockout_attach = 0;
	  
	  if( attach_lockedout ) {
	    attach_lockedout = 0;
	    fprintf( stderr, "<error> RACE CONDITION detected in 720pST switch. Things may get ugly.\n" );
	    switch_to_overlay();
	  }
	} else {
	  fprintf( stderr, "<action> Already in 720pST, ignoring call to set 720pST.\n" );
	}
}

static void trigger_hpd()
{
	unsigned char buffer;

	/* if semaphore is active, abort this, because someone already initiated the HPD pull */
	read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	if( buffer & 0x80 ) {
	  fprintf( stderr, "<action> HPD trigger requested, but blocked by semaphore. I give up...\n" );
	  return;
	}

	buffer |= 0x80;
	// set semaphore
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	fprintf( stderr, "<action> ***** Drastic: pulling HPD.\n" );
	event_counter++; // abort any thoughts about changing resolution if they were happening...
	
	/* read snoopctl */
	read_eeprom("/dev/i2c-0", DEVADDR>>1, 0, &buffer, sizeof(buffer));
	buffer = buffer | 0x8 | 0x4;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 0, &buffer, sizeof(buffer));

	deepsleep(0,150); // minimum required by spec is 100ms, 150ms should be plenty.
	event_counter++; // abort any thoughts about changing resolution if they were happening...

	buffer = (buffer & ~0x8) | 0x4;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 0, &buffer, sizeof(buffer));

	// clear semaphore
	read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	buffer &= 0x7F;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
}

static void handle_dummy(int num)
{
  return;
}

static void handle_rxlock(int num)
{
  unsigned char buffer;

  event_counter++;
  fprintf(stderr, "++++ <handler> Rx PLL event: ");

  read_eeprom("/dev/i2c-0", DEVADDR>>1, 0x18, &buffer, sizeof(buffer));

  if(buffer & 0x1) {
    rxpll_state = PLL_LOCKED;
    fprintf(stderr, " locked.\n" );
    switch_to_overlay();
  } else {
    saw_trigger = 0; // clear the trigger event on disconnect
    rxpll_state = PLL_UNLOCKED;
    fprintf(stderr, " unlocked.\n" );
  }
}

static void handle_km(int num)
{
  event_counter++;
  saw_trigger = 1;
  fprintf(stderr, "++++ <handler> Km trigger event\n" );
}

static void handle_hpd(int num)
{ 
  event_counter++;
  fprintf(stderr, "++++ <handler> HPD event: " );
  sink_state = sink_status();
  
  if( sink_state == SINK_CONNECTED ) { // clear bookkeeping on a new connection
    fprintf( stderr, " connected.\n" );
  } else {
    saw_trigger = 0; // clear the trigger event on disconnect
    fprintf( stderr, " disconnected.\n" );
  }
}

int
main(int argc, char **argv)
{
	int fb0, fpga;
	struct timing_info last_ti, new_ti, raw_ti;

	struct timing_range **ranges = timings;
	int current_range;
	unsigned short xsubi[3]; // we use this unitialized on purpose
	char code;
	char *modename = NULL;
	int invalid_count = 0;
	unsigned char buffer;

	// parse args
	if( argc == 1 ) {
	  printf( "Daemonizing...\n" );
	  if (daemon(0, 0)==-1)
	    perror("Unable to daemonize"); 
	} else {
	  code = *(argv[1]);
	  if( code == 'c' ) { // c for console mode
	    printf( "Not daemonizing, going console mode.\n" );
	    // don't do anything
	  } else if( code == 'v' ) {
	    printf( "matchmoded version 0.1\n" );
	    return 0;
	  } else {
	    printf( "Unrecognized argument, ignoring, daemonizing and moving on...\n" );
	  }
	}


	// install dummy handlers so we don't get killed while settling the system
	signal(SIGWINCH, handle_dummy);
	signal(SIGURG, handle_dummy);
	signal(SIGUSR2, handle_dummy);

	// start in overlay mode. always. helps to have a known entry state to the daemon.
	printf( "Resetting to a known mode, and waiting a few seconds for it to settle in...\n" );
	lockout_attach = 0;
	switch_to_overlay();
	lockout_attach = 1;
	deepsleep(4,0); // give the system 4 seconds to stabilize. This ia generous margin.
	lockout_attach = 0;

	// now that system is settled, install our handlers
	signal(SIGWINCH, handle_rxlock);
	signal(SIGURG, handle_km);
	signal(SIGUSR2, handle_hpd);



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


	invalid_count = 0; // count how long we've stayed in an invalid mode
	saw_trigger = 1; // safe assumption is we missed the trigger before starting up

	/* 
	 * This while(1) loop's purpose is to clean up after the mode switches
	 * done by the handlers and relay the stabilized state to the higher layers.
	 * 
	 * It also will intervene conservatively to force a self-timed mode in
	 * case no known mode is found.
	 */
	while (1) {

	  bzero(&new_ti, sizeof(new_ti));
	  bzero(&raw_ti, sizeof(raw_ti));

	  event_counter = 0; // detects events that happen while we're sleeping.

	  // pick a random interval between 1.0 and ~1.5 seconds
	  // why? because a perfect 1 second interval should actually
	  // "beat" against an integer sync rate slowly, which means
	  // you'll get series of timing samples aligned with blanking
	  // periods. Randomness breaks the repetition.
	  deepsleep(1, nrand48(xsubi) & 0x1FF);

	  /* We're now ready to determine if the mode has changed */
	  if (read_timing_info(&new_ti))
	    continue;

	  /* Timing info can be off slightly sometimes.  Pick a sane mode. */
	  raw_ti = new_ti;
	  modename = normalize_timing_info(ranges, &new_ti);


	  /* Do nothing if nothing has changed */
	  if( modename != NULL ) {
	    fprintf(stderr, "<*> Current mode: %s\n", modename );
	  } else {
	    fprintf(stderr, "<*> Current mode not supported or invalid.\n" );
	  }

	  /* Do nothing if nothing has changed */
	  if (!timingcmp(&last_ti, &new_ti) && (new_ti.status != STATUS_INVALID)) {
	    fprintf(stderr, "  No change (%dx%d - %d vs %dx%d - %d)\n",
		    last_ti.hactive, last_ti.vactive, last_ti.status,
		    new_ti.hactive, new_ti.vactive, new_ti.status);
	    continue;
	  }


	  /* By this point, we suspect the resolution has changed. */
	  /* but, before we take action, it doesn't hurt to double-check */
	  // wait a random interval to avoid sampling in a sync region
	  deepsleep(0, 8);

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
	  modename = normalize_timing_info(ranges, &new_ti);

	  if (!timingcmp(&last_ti, &new_ti) && (new_ti.status != STATUS_INVALID)) {
	    // (*) indicates that no change, but only on a double-check of no change
	    fprintf(stderr, "  No change (*) (%dx%d - %d vs %dx%d - %d)\n",
		    last_ti.hactive, last_ti.vactive, last_ti.status,
		    new_ti.hactive, new_ti.vactive, new_ti.status);
	    continue;
	  }
	  
	  if( event_counter != 0 ) {
	    invalid_count = 0; // reset my invalid count because we may have transitioned through valid while
	    // waiting for stuff to settle down
	    fprintf(stderr, "  Race condition detected, cowardly waiting another quanta.\n" );
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
	  fprintf(stderr, "pixclk_in_MHz       %4.2f  %4.2f  %4.2f\n", last_ti.pixclk_in_MHz, new_ti.pixclk_in_MHz, raw_ti.pixclk_in_MHz);
	  fprintf(stderr, "status              %4d  %4d  %4d\n", last_ti.status, new_ti.status, raw_ti.status);
	  
	  if(new_ti.status == STATUS_OK) {
	    invalid_count = 0;
	    set_timing(fb0, &new_ti);
	    send_message("connected", new_ti.hactive, new_ti.vactive, 16);
	    fprintf(stderr, "  Switched LCD to %dx%d\n", new_ti.hactive, new_ti.vactive);

#if 0 // this is too paranoid, causing spurious HPD triggers...
	    // the handlers already put us into overlay mode, so we don't need to do it now
	    // we only need to check that in fact, we saw a Km trigger
	    if( saw_trigger == 0 ) {
	      fprintf(stderr, "  !!!!!!! Triggering HPD to resync ciphers. Not ideal.\n" );
	      trigger_hpd();
	    }
#endif
	  } else if(new_ti.status == STATUS_INVALID || (new_ti.status == STATUS_DISCONNECTED || new_ti.status == STATUS_NOSOURCE) ) {
	    if( invalid_count > 4 ) {  // approx 4-6 seconds in an invalid state
	      read_eeprom("/dev/i2c-0", DEVADDR>>1, 3, &buffer, sizeof(buffer));
	      if( !(buffer & 0x8) ) { // switch if we're not already in the mode
		switch_to_720p();	   
		
		set_timing(fb0, &self_timed_720p);
		send_message("invalid", self_timed_720p.hactive, self_timed_720p.vactive, 16);
		fprintf(stderr, "  Switched LCD to 720p self-timed\n");

		deepsleep(4,0); // don't allow any new decisions on self-timed mode for a couple seconds
		// to avoid thrashing
	      }
	    } else {
	      invalid_count++;
	      continue;
	    }
	  } else {
	    fprintf(stderr, "<error> Reached an unknown status.\n");
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


///////////////// kernel memory read/write convenience functions

static int read_kernel_memory(long offset, int virtualized, int size) {
    int result;

    int *mem_range = (int *)(offset & ~0xFFFF);
    if( mem_range != prev_mem_range ) {
//        fprintf(stderr, "New range detected.  Reopening at memory range %p\n", mem_range);
        prev_mem_range = mem_range;

        if(mem_32)
            munmap(mem_32, 0xFFFF);
        if(fd)
            close(fd);

        if(virtualized) {
            fd = open("/dev/kmem", O_RDWR);
            if( fd < 0 ) {
                perror("Unable to open /dev/kmem");
                fd = 0;
                return -1;
            }
        }
        else {
            fd = open("/dev/mem", O_RDWR);
            if( fd < 0 ) {
                perror("Unable to open /dev/mem");
                fd = 0;
                return -1;
            }
        }

        mem_32 = mmap(0, 0xffff, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset&~0xFFFF);
        if( -1 == (int)mem_32 ) {
            perror("Unable to mmap file");

            if( -1 == close(fd) )
                perror("Also couldn't close file");

            fd=0;
            return -1;
        }
        mem_16 = (short *)mem_32;
        mem_8  = (char  *)mem_32;
    }

    int scaled_offset = (offset-(offset&~0xFFFF));
//    fprintf(stderr, "Returning offset 0x%08x\n", scaled_offset);
    if(size==1)
        result = mem_8[scaled_offset/sizeof(char)];
    else if(size==2)
        result = mem_16[scaled_offset/sizeof(short)];
    else
        result = mem_32[scaled_offset/sizeof(long)];

    return result;
}
