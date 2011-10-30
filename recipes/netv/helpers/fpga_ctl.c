/***
This code uses the BSD 2-clause license:

Copyright (c) 2011, Andrew "bunnie" Huang / bunnie@bunniestudios.com
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
/***
    fpga_ctl.c
    command line utilities to manipulate FPGA state
 ***/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

/*
 * Ioctl definitions -- should sync to kernel source instead of copying here
 */

#define FPGA_IOC_MAGIC  'c'

#define FPGA_IOCWTEST    _IOW(FPGA_IOC_MAGIC,  1, int)
#define FPGA_IOCRTEST    _IOR(FPGA_IOC_MAGIC,  2, int)
#define FPGA_IOCRESET    _IOW(FPGA_IOC_MAGIC,  3, int)
#define FPGA_IOCLED0     _IOW(FPGA_IOC_MAGIC,  4, int)
#define FPGA_IOCLED1     _IOW(FPGA_IOC_MAGIC,  5, int)
#define FPGA_IOCDONE     _IOR(FPGA_IOC_MAGIC,  6, int)
#define FPGA_IOCINIT     _IOR(FPGA_IOC_MAGIC,  7, int)

/* 
 * Functions for the ioctl calls 
 */

ioctl_reset(int file_desc)
{
  int ret_val;

  ret_val = ioctl(file_desc, FPGA_IOCRESET, NULL);

  if (ret_val < 0) {
    printf("ioctl_reset failed:%d\n", ret_val);
    exit(-1);
  }
}

ioctl_led0(int file_desc, int state) {
  int ret_val;

  ret_val = ioctl(file_desc, FPGA_IOCLED0, state);

  if (ret_val < 0) {
    printf("ioctl_led0 failed:%d\n", ret_val);
    exit(-1);
  }
}

ioctl_led1(int file_desc, int state) {
  int ret_val;

  ret_val = ioctl(file_desc, FPGA_IOCLED1, state);

  if (ret_val < 0) {
    printf("ioctl_led1 failed:%d\n", ret_val);
    exit(-1);
  }
}

ioctl_done(int file_desc, int *state) {
  int ret_val;
  
  ret_val = ioctl(file_desc, FPGA_IOCDONE, state);
  
  if (ret_val < 0) {
    printf("ioctl_done failed:%d\n", ret_val);
    exit(-1);
  }
}

ioctl_init(int file_desc, int *state) {
  int ret_val;
  
  ret_val = ioctl(file_desc, FPGA_IOCINIT, state);
  
  if (ret_val < 0) {
    printf("ioctl_init failed:%d\n", ret_val);
    exit(-1);
  }
}

int write_eeprom(char *i2c_device, int addr, int start_reg,
                 unsigned char *buffer, int bytes) {
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

/////////////////////// this stuff should go in a header file eventually
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

// this is the I2C device address (host-side) of the FPGA
#define DEVADDR 0x3C

// this list of I2C register addresses should be filled out when I have more time...
#define FPGA_SNOOP_CTL_ADR 0x00
#define FPGA_COMP_CTL_ADR  0x03
#define FPGA_EXT1_CTL_ADR  0x0c
#define FPGA_CHROMA_R_ADR  0x0d
#define FPGA_CHROMA_G_ADR  0x0e
#define FPGA_CHROMA_B_ADR  0x0f
#define FPGA_GENSTAT_ADR   0x10
#define FPGA_LOCKTOL_0     0x11
#define FPGA_LOCKTOL_1     0x12
#define FPGA_LOCKTGT_0     0x15
#define FPGA_LOCKTGT_1     0x16
#define FPGA_LOCKTGT_2     0x17
#define FPGA_PLLSTAT_ADR   0x18
#define FPGA_KM_0          0x19
#define FPGA_KM_1          0x1A
#define FPGA_KM_2          0x1B
#define FPGA_KM_3          0x1C
#define FPGA_KM_4          0x1D
#define FPGA_KM_5          0x1E	/*  */
#define FPGA_KM_6          0x1F
#define FPGA_DNA_ADR       0x38 
//#define FPGA_MINOR_ADR     0x3e // defunct
#define FPGA_MAJOR_ADR     0x3f // only one byte now for version numbering

/////////////////////// probably this stuff might want to go in a helper .c file
void print_fpga_version() {
  unsigned char major = 0xff;

  //  if(read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_MINOR_ADR, &minor, 1)) {
  //    printf( "can't access FPGA.\n" );
  //    return;
  //  }
  if(read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_MAJOR_ADR, &major, 1)) {
    printf( "can't access FPGA.\n" );
    return;
  }
  
  printf( "FPGA reports a version code of: %d\n", major );
}

int dump_hdmi_timings() {
    unsigned char buffer[32];
    int i;
    struct timinginfo t;
    double ns_per_pix;
    unsigned char sync;

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
      ns_per_pix = (((double) t.refclock_count) * (1.0 / (double) REFCLK_FREQ_HZ)) / ((double)t.vtotal_pix);
      t.pixclk_in_MHz = (1.0 / ns_per_pix) / 1000000.0;
    } else {
      ns_per_pix = -1.0;
      t.pixclk_in_MHz = -1.0;
    }

    //    t.lcd_de_latency_lines = buffer[0x18] | ((buffer[0x19] & 0xf) << 8);

    //    t.lcd_vsync_latency_lines = buffer[0x1a] | ((buffer[0x1b] & 0xf) << 8);

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

    //    printf( "\n" );
    //    printf( "LCD_Vsync latency in lines: %d\n", t.lcd_vsync_latency_lines );
    //    printf( "LCD_de latency in lines: %d\n", t.lcd_de_latency_lines );

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

    printf( "\nSupplemental debug info: \n" );
    printf( "Field time in pixels: %d\n", t.vtotal_pix );
    printf( "Field time in 26 MHz refclks: %d\n", t.refclock_count );

    return 0;
}

//#define STATS_CORRECT_MINOR 2
#define STATS_CORRECT_MAJOR 0xd
int dump_registers(int stats) {
    unsigned char buffer[32];
    int i;
    int zero;
    int bytes_per_line = 4;

    unsigned char minor = 0xff;
    unsigned char major = 0xff;
    unsigned int locktol, locktgt;

    unsigned long long device_id = 0LL;
    unsigned char c;
    
    //    if(read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_MINOR_ADR, &minor, 1)) {
    //      printf( "can't access FPGA.\n" );
    //      return;
    //    }
    if(read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_MAJOR_ADR, &major, 1)) {
      printf( "can't access FPGA.\n" );
      return;
    }

    if(read_eeprom("/dev/i2c-0", DEVADDR>>1, 0, buffer, sizeof(buffer))) {
        return 1;
    }

    if( stats ) {
      bytes_per_line = 16;
      printf("Raw dump of register bank:" );
    }

    for(i=0; i<sizeof(buffer)/sizeof(*buffer); i++) {
      if( (i % bytes_per_line) == 0 ) {
	printf( "\n0x%02x: ", i );
      }
      printf("%02x ", buffer[i]);
    }
    printf("\n\n");

    if( stats ) {
      if( (STATS_CORRECT_MAJOR != major) ) {
	printf( "Stats interpretations do not match FPGA version (expected: %d, got: %d)\n",
		STATS_CORRECT_MAJOR,
		major);
	printf( "Going on anyways, but caveat hacker.\n" );
      } 
      printf( "Select stats (as of version %d): \n", STATS_CORRECT_MAJOR );
      if( buffer[FPGA_SNOOP_CTL_ADR] & 0x4 ) {
	printf( "EDID squashing is on, run 'snoop 0' to check result.\n" );
      }
      if( buffer[FPGA_SNOOP_CTL_ADR] & 0x8 ) {
	printf( "Hot plug detect is forced into an unplugged state.\n" );
      }

      if( buffer[FPGA_COMP_CTL_ADR] & 0x1 ) {
	printf( "HDCP encryption will be applied when cipher is initialized & requested.\n" );
	zero = 1;
	for( i = 0; i < 7; i++ ) {
	  if( buffer[FPGA_KM_0 + i] != 0 )
	    zero = 0;
	}
	if( zero ) {
	  printf("...but Km is set to zero, which means you probably need to run derive_km.\n" );
	}
      }

      if( buffer[FPGA_COMP_CTL_ADR] & 0x4 ) {
	printf( "Compositing of LCD over HDMI stream is enabled.\n" );
      }

      if( buffer[FPGA_COMP_CTL_ADR] & 0x10 ) {
	printf( "Transmitter commanded to reset during self-mode.\n" );
      }
      
      if( buffer[FPGA_COMP_CTL_ADR] & 0x20 ) {
	printf( "Smart genlock is selected.\n" );
      }

      if( buffer[FPGA_COMP_CTL_ADR] & 0x40 ) {
	printf( "Genlock machine is commanded into reset state.\n" );
      }
      
      if( buffer[FPGA_GENSTAT_ADR] & 0x2 ) {
	printf( "Genlock is locked.\n" );
      } else {
	printf( "Genlock is not locked.\n" );
      }

      locktol = 0;
      locktgt = 0;
      locktol = buffer[FPGA_LOCKTOL_0] | buffer[FPGA_LOCKTOL_1] << 8;
      locktgt = buffer[FPGA_LOCKTGT_0] | buffer[FPGA_LOCKTGT_1] << 8 |
	buffer[FPGA_LOCKTGT_2] << 16;
      if( locktol == 0 ) {
	printf( "Lock tolerance is 0; you probably didn't mean for that.\n" );
      }
      if( locktgt == 0 ) {
	printf( "Lock target is 0; you probably didn't mean for that.\n" );
      }

      if( buffer[FPGA_GENSTAT_ADR] & 0x80 ) {
	printf( "This stream is probably HDCP encrypted.\n" );
      } else {
	printf( "This stream is probably not HDCP encrypted.\n" );
      }

      for( i = 0; i < 7; i++ ) {
	read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_DNA_ADR + i, &c, 1);
	device_id <<= 8;
	device_id |= (c & 0xFF);
      }
      printf( "Device ID: %014llx\n", device_id );
    }
    
    return 0;
}

int dump_register(unsigned char address) {
  unsigned char buffer;

  read_eeprom("/dev/i2c-0", DEVADDR>>1, (unsigned long) address, &buffer, 1);
  printf("0x%02x: %02hx\n", (unsigned long) address, buffer);
  
  return 0;
}

void print_help(char code) {
    printf( "FPGA control routine.\n" );
    printf( "Command character %c not recognized.\n", code );
    printf( "r       hardware reset of the FPGA to clean state for reconfiguration\n");
    printf( "o       return the state of the FPGA programming done pin\n" );
    printf( "V       print the version number of the FPGA\n" );
    printf( "t       dump the timing state of the HDMI stream\n" );
    printf( "d       dump the control set registers (raw values)\n" );
    printf( "d [adr] dump the the raw value at [adr]\n" );
    printf( "s       print basic stats on FPGA\n" );
    printf( "H       force hot plug detect to disconnect sink\n" );
    printf( "h       return hot plug detect to normal operation\n" );
    printf( "E       turn on EDID squashing (be sure to program modeline first\n" );
    printf( "e       turn off EDID squashing\n" );
    printf( "c       turn off compositing\n" );
    printf( "C       turn on compositing\n" );
    printf( "l [val] set lock target\n" );
    printf( "L [val] set lock tolerance\n" );
    printf( "w [adr] [dat] write data [dat] to address [adr]\n" );
    //    printf( "x [r,g,b] [value] set chroma key of r/g/b to [value]\n" );
    printf( "x o     turn off chroma key\n" );
    printf( "x i     turn on chroma key (fixed to 240,0,240 rgb)\n" );
    printf( "7       set self-timed mode to 720p (warning: resets many settings)\n" );
    printf( "Y       set to overlay mode with typical defaults\n" );
    printf( "p       print PLL status\n" );
    printf( "n       return device serial number\n" );
}

int main(int argc, char **argv)
{
  int file_desc, retval;
  char code;
  unsigned char buffer, adr, data;
  char supplement;
  unsigned int temp;
  unsigned long long device_id = 0LL;
  int i;

  if(argc < 2) {
    fprintf(stderr, "Usage: %s <op>\n", argv[0]);
    print_help('_');
    return 1;
  }
  code = *(argv[1]);
  
  file_desc = open("/dev/fpga", 0);
  if (file_desc < 0) {
    printf("Can't open device file: %s\n", "/dev/fpga");
    exit(-1);
  }

  read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_MAJOR_ADR, &buffer, 1);
  if( buffer >= 192 ) {
    printf( "FPGA is configured to drive a motor board on the HDMI input, you can't use this utility for the current configuration.\n" );
    printf( "If you have no idea what this message is about, disconnect the device from the HDMI input NOW or else you may permanantly damage it, and reboot the board to get back to a typical consumer-use application configuration.\n" );
    exit(0);
  }

  switch(code) {
  case 'r':
    ioctl_reset(file_desc);
    break;
  case '0':
      ioctl_led0(file_desc, 0);
      break;
  case '1':
    ioctl_led1(file_desc, 0);
    break;
  case '2':
    ioctl_led0(file_desc, 1);
    break;
  case '3':
    ioctl_led1(file_desc, 1 );
    break;
  case 'o':
    ioctl_done(file_desc, &retval);
    printf( "Done pin state: %d\n", retval );;
    break;
  case 'i':
    ioctl_init(file_desc, &retval);
    printf( "Init pin state: %d\n", retval );;
    break;
  case 'V':
    print_fpga_version();
    break;
  case 't':
    dump_hdmi_timings();
    break;
  case 'd':
    if( argc == 2 ) {
      dump_registers(0);
    }
    if( argc == 3 ) {
      dump_register((unsigned char) strtol(argv[2],NULL,0));
    }
    break;
  case 's':
    dump_registers(1);
    break;
  case 'H':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_SNOOP_CTL_ADR, &buffer, 1);
    buffer |= 0x8;
    write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_SNOOP_CTL_ADR, &buffer, sizeof(buffer));
    break;
  case 'h':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_SNOOP_CTL_ADR, &buffer, 1);
    buffer &= 0xF7;
    write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_SNOOP_CTL_ADR, &buffer, sizeof(buffer));
    break;

  case 'E':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_SNOOP_CTL_ADR, &buffer, 1);
    buffer |= 0x4;
    write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_SNOOP_CTL_ADR, &buffer, sizeof(buffer));
    break;
  case 'e':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_SNOOP_CTL_ADR, &buffer, 1);
    buffer &= 0xFB;
    write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_SNOOP_CTL_ADR, &buffer, sizeof(buffer));
    break;

  case 'w':
    if( argc != 4 ) {
      printf( "Write requires [adr] and [dat] arguments.\n" );
    } else {
      adr = (unsigned char) strtol(argv[2],NULL,0);
      read_eeprom("/dev/i2c-0", DEVADDR>>1, adr, &buffer, 1);
      printf( "0x%02x: 0x%02x -> ", adr, buffer );
      buffer = (unsigned char) strtol(argv[3],NULL,0);
      write_eeprom("/dev/i2c-0", DEVADDR>>1, adr, &buffer, sizeof(buffer));
      read_eeprom("/dev/i2c-0", DEVADDR>>1, adr, &buffer, 1);
      printf( "0x%02x\n", buffer );
    }
    break;

  case 'l':
    if( argc != 3 ) {
      printf( "Missing mandatory target argument\n" );
    } else {
      temp = strtol(argv[2],NULL,0);
      buffer = temp & 0xFF;
      write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_LOCKTGT_0, &buffer, sizeof(buffer));
      buffer = (temp >> 8) & 0xFF;
      write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_LOCKTGT_1, &buffer, sizeof(buffer));
      buffer = (temp >> 16) & 0xFF;
      write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_LOCKTGT_2, &buffer, sizeof(buffer));
    }
    break;

  case 'L':
    if( argc != 3 ) {
      printf( "Missing mandatory tolerance argument\n" );
    } else {
      temp = strtol(argv[2],NULL,0);
      buffer = temp & 0xFF;
      write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_LOCKTOL_0, &buffer, sizeof(buffer));
      buffer = (temp >> 8) & 0xFF;
      write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_LOCKTOL_1, &buffer, sizeof(buffer));
    }
    break;

  case 'C':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_COMP_CTL_ADR, &buffer, 1);
    buffer |= 0x4;
    write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_COMP_CTL_ADR, &buffer, sizeof(buffer));
    break;
  case 'c':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_COMP_CTL_ADR, &buffer, 1);
    buffer &= ~0x4;
    write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_COMP_CTL_ADR, &buffer, sizeof(buffer));
    break;

  case 'x':
    if( argc == 3 ) {
      supplement = *(argv[2]);
      if( supplement == 'o' ) {
	read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, 1);
	buffer &= ~0x2;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, sizeof(buffer));
      } else if( supplement == 'i' ) {
	read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, 1);
	buffer |= 0x2;
	write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, sizeof(buffer));
      } else {
	printf( "unrecognized chroma extension code\n" );
      }
#if 0
    } else if( argc == 4 ) {
      supplement = *(argv[2]);
      data = (unsigned char) strtol(argv[3],NULL,0);
      if( (data & 0x3) != 0 ) {
	printf( "Note: only bits 7-2 are used in chroma key computation.\n" );
	printf( "Also, LSB is extended, so to chroma 0x4, you must specify 0x7.\n" );
      }
      switch( supplement ) {
      case 'r':
	adr = FPGA_CHROMA_R_ADR;
	break;
      case 'g':
	adr = FPGA_CHROMA_G_ADR;
	break;
      case 'b':
	adr = FPGA_CHROMA_B_ADR;
	break;
      default:
	printf( "unrecognized chroma channel (r,g,b are only valid options)\n" );
      }
      write_eeprom("/dev/i2c-0", DEVADDR>>1, (int) adr, &data, 1);
#endif
    } else {
      printf( "Not enough or invalid arguments to chroma command.\n" );
    }
    break;
    
  case '7':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_COMP_CTL_ADR, &buffer, 1);
    buffer &= 0xF0;
    buffer |= 0x08;
    write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_COMP_CTL_ADR, &buffer, sizeof(buffer));
    break;
    
  case 'Y':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_COMP_CTL_ADR, &buffer, 1);
    buffer &= 0xF0;
    buffer |= 0x05;
    write_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_COMP_CTL_ADR, &buffer, sizeof(buffer));
    break;
    
  case 'p':
    read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_PLLSTAT_ADR, &buffer, 1);
    if( buffer & 1 )
      printf( "Rx PLL is locked\n" );
    else
      printf( "Rx PLL is not locked\n" );

    if( buffer & 2 ) 
      printf( "Tx PLL is locked\n" );
    else
      printf( "Tx PLL is not locked\n" );

    if( buffer & 4 )
      printf( "720p clock synthesizer is locked\n" );
    else
      printf( "720p clock synthesizer is not locked\n" );

    if( buffer & 8 )
      printf( "Inter-channel skew alignment error on Rx.\n" );
    else
      printf( "Seems like the Rx channel symbols are properly aligned.\n" );

    if( buffer & 0x10 )
      printf( "Red channel has received a valid pixel since last channel reset\n" );
    else
      printf( "Red channel has yet to receive a valid pixel since last channel reset\n" );
    
    if( buffer & 0x20 )
      printf( "Green channel has received a valid pixel since last channel reset\n" );
    else
      printf( "Green channel has yet to receive a valid pixel since last channel reset\n" );
    
    if( buffer & 0x40 )
      printf( "Blue channel has received a valid pixel since last channel reset\n" );
    else
      printf( "Blue channel has yet to receive a valid pixel since last channel reset\n" );
    
    if( buffer & 0x80 )
      printf( "Rx is aligned and should be producing valid data\n" );
    else
      printf( "Rx channel is still initializing\n" );

    break;

  case 'n':
    for( i = 0; i < 7; i++ ) {
      read_eeprom("/dev/i2c-0", DEVADDR>>1, FPGA_DNA_ADR + i, &buffer, 1);
      device_id <<= 8;
      device_id |= (buffer & 0xFF);
    }
    printf( "Device ID: %014llx\n", device_id );
    break;

  default:
    print_help(code);
  }

  close(file_desc);
  
  return 0;
}
