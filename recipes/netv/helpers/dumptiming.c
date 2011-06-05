/*
 * dumptiming.c
 *
 * bunnie@chumby.com
 * based on code by Aaron "Caustik" Robinson
 * (c) Copyright Chumby Industries, 2007, 2011
 * Distributed under BSD license
 */

#include <sys/ioctl.h>
#include <string.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>


static inline unsigned short swab_16(unsigned short val) {
	int t = val;
	t<<=8;
	t|=(t>>16);
	return (unsigned short)t;
}


int read_eeprom(char *i2c_device, int addr, int start_reg,
                unsigned char *buffer, int bytes) {
    int                         byte;
    int                         device_file;
    //int page = 0;
    struct i2c_rdwr_ioctl_data  packets;
    struct i2c_msg              messages[2];
    unsigned char               output;

    // On this chip, the upper two bits of the memory address are
    // represented in the i2c address, and the lower eight are clocked in
    // as the memory address.  This gives a crude mechanism for 4 pages of
    // 256 bytes each.
    byte = (start_reg   ) & 0xff;
    //page = (start_reg>>8) & 0x03;

    if((device_file = open(i2c_device, O_RDWR))==-1) {
        perror("Unable to open i2c device");
        return 1;
    }

    // Set the address we'll read to the start address.
    output = byte;

    messages[0].addr    = addr;
    messages[0].flags   = 0;
    messages[0].len     = sizeof(output);
    messages[0].buf     = &output;

    messages[1].addr    = addr;
    messages[1].flags   = I2C_M_RD;
    messages[1].len     = bytes;
    messages[1].buf     = (unsigned char *)buffer;

    packets.msgs        = messages;
    packets.nmsgs       = 2;

    if(ioctl(device_file, I2C_RDWR, &packets) < 0) {
        perror("Unable to communicate with i2c device");
        close(device_file);
        return 1;
    }
    
    close(device_file);
    return 0;
}

#define DEVADDR 0x3C
//#define DEVADDR 0x94

#define REFCLK_FREQ_HZ  26000000

struct timinginfo {
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
};

int main(int argc, char **argv) {
    unsigned char buffer[32];
    int i;
    struct timinginfo t;
    double ns_per_pix;
    unsigned char sync;

    if(argc == 2) {
      read_eeprom("/dev/i2c-0", DEVADDR>>1, strtol(argv[1], NULL, 0), buffer, 1);
      printf("0x%02x: %02hx\n", strtol(argv[1], NULL, 0), buffer[0]);
      return 1;
    }

    // grab the buffer in
    if(read_eeprom("/dev/i2c-0", DEVADDR>>1, 32, buffer, sizeof(buffer))) {
        return 1;
    }
    
    t.hactive = buffer[0] | ((buffer[1] & 0xf) << 8);
    t.vactive = buffer[2] | ((buffer[3] & 0xf) << 8);
    t.htotal  = buffer[4] | ((buffer[5] & 0xf) << 8);
    if( t.htotal == 0 ) {
      t.htotal = -1;
    }
    t.vtotal_pix = buffer[6] | (buffer[7] << 8) | (buffer[8] << 16);
    t.vtotal_lines = t.vtotal_pix / t.htotal;
    t.h_fp = buffer[9];
    t.h_bp = buffer[0xa];
    t.v_fp_pix = buffer[0xb] | (buffer[0xc] << 8) | (buffer[0xd] << 16);
    t.v_fp_lines = t.v_fp_pix / t.htotal;
    t.v_bp_pix = buffer[0xe] | (buffer[0xf] << 8) | (buffer[0x10] << 16);
    t.v_bp_lines = t.v_bp_pix / t.htotal;
    t.hsync_width = buffer[0x11];
    t.vsync_width_pix = buffer[0x12] | (buffer[0x13] << 8) | (buffer[0x14] << 16);
    t.vsync_width_lines = t.vsync_width_pix / t.htotal;
    t.refclock_count = buffer[0x15] | (buffer[0x16] << 8) | (buffer[0x17] << 16);
    //    printf( "Refclock raw value: 0x%08x\n", t.refclock_count );
    if( (t.htotal * t.vtotal_lines) != 0 ) {
      ns_per_pix = (((double) t.refclock_count) * (1.0 / (double) REFCLK_FREQ_HZ)) / ((double)(t.htotal * t.vtotal_lines));
      t.pixclk_in_MHz = (1.0 / ns_per_pix) / 1000000.0;
    } else {
      ns_per_pix = -1.0;
      t.pixclk_in_MHz = -1.0;
    }

    t.lcd_de_latency_lines = buffer[0x18] | ((buffer[0x19] & 0xf) << 8);

    t.lcd_vsync_latency_lines = buffer[0x1a] | ((buffer[0x1b] & 0xf) << 8);

    printf( "--DEPRACATED-- please use fpga_ctl t\n" );
    printf( "Horizontal active pixels: %d\n", t.hactive );
    printf( "Vertical active lines: %d\n", t.vactive );
    printf( "Horizontal total pixels: %d\n", t.htotal );
    printf( "Vertical total lines: %d\n", t.vtotal_lines );
    printf( "Horizontal fp, bp: %d, %d\n", t.h_fp, t.h_bp );
    printf( "Vertical fp, bp in lines: %d, %d\n", t.v_fp_lines, t.v_bp_lines );
    printf( "Hsync width: %d\n", t.hsync_width );
    printf( "Vsync width in lines: %d\n", t.vsync_width_lines );
    printf( "Pixclock in MHz: %g\n", t.pixclk_in_MHz );

    read_eeprom("/dev/i2c-0", DEVADDR>>1, 0x10, &sync, 1);
    printf( "VSYNC polarity is " );
    if( sync & 0x40 ) {
      printf( "positive\n" );
    } else {
      printf( "negative\n" );
    }

    printf( "HSYNC polarity is " );
    if( sync & 0x20 ) {
      printf( "positive\n" );
    } else {
      printf( "negative\n" );
    }

    printf( "\n" );
    printf( "LCD_Vsync latency in lines: %d\n", t.lcd_vsync_latency_lines );
    printf( "LCD_de latency in lines: %d\n", t.lcd_de_latency_lines );

    printf( "\nRecommended LCD timings:\n" );
    printf( "regutil -w LCD_SPUT_V_H_TOTAL=0x%08x\n", 
	    (t.vtotal_lines << 16) | (t.htotal & 0xffff) );
    printf( "regutil -w LCD_SPU_V_H_ACTIVE=0x%08x\n", 
	    (t.vactive << 16) | (t.hactive & 0xffff) );
    printf( "regutil -w LCD_SPU_H_PORCH=0x%08x\n", 
	    (t.h_bp << 16) | (t.h_fp & 0xffff) );
    printf( "regutil -w LCD_SPU_V_PORCH=0x%08x\n",
	    (t.v_bp_lines << 16) | (t.v_fp_lines & 0xffff) );
    printf( "regutil -w LCD_CFG_SCLK_DIV=0x%08x\n", 0x90000001 ); // get clock from FPGA
    printf( "set lower nibble of LCD_SPU_DUMB_CTRL to 0x3 (don't ever invert sync)\n" );

    return 0;
}
