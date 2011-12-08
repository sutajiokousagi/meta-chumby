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

#include <time.h>
#include <sys/time.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define DEV_I2C_0  		"/dev/i2c-0"

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

void ioctl_reset(int file_desc)
{
  int ret_val;

  ret_val = ioctl(file_desc, FPGA_IOCRESET, NULL);

  if (ret_val < 0) {
    printf("ioctl_reset failed:%d\n", ret_val);
    exit(-1);
  }
}

void ioctl_led0(int file_desc, int state) {
  int ret_val;

  ret_val = ioctl(file_desc, FPGA_IOCLED0, state);

  if (ret_val < 0) {
    printf("ioctl_led0 failed:%d\n", ret_val);
    exit(-1);
  }
}

void ioctl_led1(int file_desc, int state) {
  int ret_val;

  ret_val = ioctl(file_desc, FPGA_IOCLED1, state);

  if (ret_val < 0) {
    printf("ioctl_led1 failed:%d\n", ret_val);
    exit(-1);
  }
}

void ioctl_done(int file_desc, int *state) {
  int ret_val;
  
  ret_val = ioctl(file_desc, FPGA_IOCDONE, state);
  
  if (ret_val < 0) {
    printf("ioctl_done failed:%d\n", ret_val);
    exit(-1);
  }
}

void ioctl_init(int file_desc, int *state) {
  int ret_val;
  
  ret_val = ioctl(file_desc, FPGA_IOCINIT, state);
  
  if (ret_val < 0) {
    printf("ioctl_init failed:%d\n", ret_val);
    exit(-1);
  }
}

int write_eeprom(const char *i2c_device, int addr, int start_reg,
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

int read_eeprom(const char *i2c_device, int addr, int start_reg,
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
#define FPGA_PWM_L_DIV_ADR 0x0d
#define FPGA_PWM_M_DIV_ADR 0x0e
#define FPGA_GENSTAT_ADR   0x10
#define FPGA_SERV_PER1_ADR 0x12 // LSB
#define FPGA_SERV_PER2_ADR 0x13
#define FPGA_SERV_PER3_ADR 0x14 // MSB
#define FPGA_SERV1_W1_ADR  0x15 // LSB
#define FPGA_SERV1_W2_ADR  0x16 
#define FPGA_SERV1_W3_ADR  0x17 // MSB
#define FPGA_SERV2_W1_ADR  0x09 // LSB
#define FPGA_SERV2_W2_ADR  0x0a 
#define FPGA_SERV2_W3_ADR  0x0b // MSB
#define FPGA_PLLSTAT_ADR   0x18
#define FPGA_BRD_CTL_ADR   0x19
#define FPGA_DIG_OUT_ADR   0x1a
#define FPGA_MOTEN_ADR     0x1b
#define FPGA_PWM1_ADR      0x1c
#define FPGA_PWM2_ADR      0x1d
#define FPGA_PWM3_ADR      0x1e
#define FPGA_PWM4_ADR      0x1f
#define FPGA_DIG_IN_ADR    0x20
#define FPGA_ADC_LSB_ADR   0x21
#define FPGA_ADC_MSB_ADR   0x22
#define FPGA_MOT_STAT_ADR  0x23
#define FPGA_SCAN_TST0_ADR 0x24
#define FPGA_SCAN_TST1_ADR 0x25
#define FPGA_SCAN_TST2_ADR 0x26
#define FPGA_SCAN_TST3_ADR 0x27

#define FPGA_DNA_ADR       0x38 
//#define FPGA_MINOR_ADR     0x3e // defunct
#define FPGA_MAJOR_ADR     0x3f // only one byte now for version numbering

/////////////////////// probably this stuff might want to go in a helper .c file
void print_fpga_version() {
  unsigned char major = 0xff;

  //  if(read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MINOR_ADR, &minor, 1)) {
  //    printf( "can't access FPGA.\n" );
  //    return;
  //  }
  if(read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MAJOR_ADR, &major, 1)) {
    printf( "-1\n" );
    return;
  }
  
  printf( "%d\n", major );
}

//#define STATS_CORRECT_MINOR 2
#define STATS_CORRECT_MAJOR 0xd
int dump_registers(int stats) {
    unsigned char buffer[32];
    unsigned int i;
    int zero;
    int bytes_per_line = 4;

    unsigned char minor = 0xff;
    unsigned char major = 0xff;
    unsigned int locktol, locktgt;

    unsigned long long device_id = 0LL;
    unsigned char c;
    
    //    if(read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MINOR_ADR, &minor, 1)) {
    //      printf( "can't access FPGA.\n" );
    //      return 1;
    //    }
    if(read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MAJOR_ADR, &major, 1)) {
      printf( "can't access FPGA.\n" );
      return 1;
    }

    if(read_eeprom(DEV_I2C_0, DEVADDR>>1, 0, buffer, sizeof(buffer))) {
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

      for( i = 0; i < 7; i++ ) {
	read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DNA_ADR + i, &c, 1);
	device_id <<= 8;
	device_id |= (c & 0xFF);
      }
      printf( "Device ID: %014llx\n", device_id );
    }
    
    return 0;
}

void microsleep(unsigned int us) {
  struct timespec waittime;
  struct timespec remtime;

  waittime.tv_sec = us / 1000000;
  waittime.tv_nsec = (us % 1000000) * 1000;

  while(nanosleep(&waittime, &remtime) == -1) {
    // this makes us sleep even if we get a signal for the specified time
    waittime.tv_sec = remtime.tv_sec;
    waittime.tv_nsec = remtime.tv_nsec;
  }
}

int dump_register(unsigned char address) {
  unsigned char buffer;

  read_eeprom(DEV_I2C_0, DEVADDR>>1, (unsigned long) address, &buffer, 1);
  printf("0x%02lx: %02hx\n", (unsigned long) address, buffer);
  
  return 0;
}

void print_help(char code) {
    printf( "FPGA control routine.\n" );
    printf( "Command character %c not recognized.\n", code );
    printf( "r       hardware reset of the FPGA to clean state for reconfiguration\n");
    printf( "o       return the state of the FPGA programming done pin\n" );
    printf( "V       print the version number of the FPGA\n" );

    printf( "a [ch]  read ADC value from channel ch0-7\n" );
    printf( "a       dump all ADC  channel values\n" );
    printf( "S [ch] [s,m] set channel ch[1,2] to mode [s,m] (servo or motor mode)\n" );
    printf( "S [ch] returns the servo mode status of channel [ch]\n" );
    printf( "s [ch] [val] set servo channel ch [1,2] to angle val [0.0-180.0, floating point]\n" );
    printf( "e [val] set servo pulse period to [val] microseconds (integer only)\n" );
    printf( "e      get servo pulse period\n" );
    printf( "m [ch] [cmd] motor command for channel ch1-4. cmd = [f,r,s] -> forward, reverse, stop\n" );
    printf( "M [xxxx] set motor channel [1234] to respective states [f,r,s,x] (x means don't change)\n" );
    printf( "p [ch] [dc] PWM duty cycle for channel ch1-4 and duty cycle dc0-255\n" );
    printf( "P [div] set PWM divider (div = 0-65535, rate = 101.5kHz / (div + 2))\n" );
    printf( "f [freq] set motor PWM to frequency [freq] Hz\n" );
    printf( "u [val] set digital output to value val\n" );
    printf( "u [ch] [val] set digital output bit [ch] to value [val]\n" );
    printf( "i       print the value of the digital inputs\n" );
    printf( "i [ch]  print the value of channel [ch]\n" );

    printf( "d       dump the control set registers (raw values)\n" );
    printf( "d [adr] dump the the raw value at [adr]\n" );
    printf( "w [adr] [dat] write data [dat] to address [adr]\n" );
    printf( "D [tst] detect if the motor controller board is attached using test value [tst].\n" );
    printf( "A       auto-detect if motor controller board is attached.\n" );
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
  unsigned long a1, a2;
  double f1, f2;
  char *mcode;

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

  read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MAJOR_ADR, &buffer, 1);
  if( (buffer < 192) && (code != 'r') ) {
    printf( "FPGA is not configured to drive a motor board, please follow this procedure:\n" );
    printf( "1. Make sure the HDMI input is connected to the motor board. Do not connect an HDMI device to the port! We are not liable for damages if you wire this up wrong.\n" );
    printf( "2. Issue the command 'netv_service motor'; this will switch the NeTV into motor driver mode.\n" );
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
    //  case 'i':
    //    ioctl_init(file_desc, &retval);
    //    printf( "Init pin state: %d\n", retval );;
    //    break;
  case 'V':
    print_fpga_version();
    break;
  case 'd':
    if( argc == 2 ) {
      dump_registers(0);
    }
    if( argc == 3 ) {
      dump_register((unsigned char) strtol(argv[2],NULL,0));
    }
    break;

  case 'p':
    if( argc != 4 ) {
      printf( "Insufficient arguments\n" );
      print_help('p');
      break;
    } else {
      a1 = strtol(argv[2],NULL,0);
      a2 = strtol(argv[3],NULL,0);
      if( (a1 > 4) || (a1 < 1) ) {
	printf( "Channel %ld is out of range (1-4)\n", a1 );
	break;
      }
      if( (a2 > 255) || (a2 < 0) ) {
	printf( "Duty cycle on percentage is out of range (0-255)\n", a2 );
	break;
      }
      buffer = a2;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_PWM1_ADR + a1 - 1, &buffer, sizeof(buffer));
    }
    break;

  case 'P':
    if( argc != 3 ) {
      printf( "Insufficient arguments\n" );
      print_help('p');
      break;
    } else {
      a1 = strtol(argv[2],NULL,0);
      if( (a1 > 65535) || (a1 < 0) ) {
	printf( "Divider is out of range (0-65535)\n" );
	break;
      }
      buffer = a1 & 0xFF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_PWM_L_DIV_ADR, &buffer, sizeof(buffer));
      buffer = (a1 & 0xFF00) >> 8;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_PWM_M_DIV_ADR, &buffer, sizeof(buffer));
    }
    break;

  case 'f':
    if( argc != 3 ) {
      printf( "Insufficient arguments\n" );
      print_help('f');
      break;
    } else {
      a2 = strtol(argv[2], NULL, 0);
      if( (a2 > 50000) || (a2 < 1) ) {
	printf( "Frequency %d is out of range (1 - 50000)\n", a2 );
      }
      a1 = (101500 - (a2 * 2)) / a2;
      printf( "Note: after rounding, frequency is %d Hz\n", (101500 / (a1 + 2)));
      
      buffer = a1 & 0xFF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_PWM_L_DIV_ADR, &buffer, sizeof(buffer));
      buffer = (a1 & 0xFF00) >> 8;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_PWM_M_DIV_ADR, &buffer, sizeof(buffer));
    }
    break;

  case 'u':
    if( argc < 3 ) {
      printf( "Insufficient arguments\n" );
      print_help('u');
      break;
    } else {
      if( argc == 3 ) {
	a1 = strtol(argv[2],NULL,0);
	if( (a1 > 255) || (a1 < 0) ) {
	  printf( "Digital output is out of range (0-255)\n" );
	  break;
	}
	buffer = a1;
	write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &buffer, sizeof(buffer));
      } else {  // argc is 4 or greater, but ignore extra arguments
	a1 = strtol(argv[3],NULL,0);
	a1 = a1 ? 1 : 0;
	
	a2 = strtol(argv[2],NULL,0);
	if( (a1 > 7) || (a1 < 0) ) {
	  printf( "Channel specifier %d out of range (0-7)\n", a1);
	  break;
	}

	// read back the current output register setting
	read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &buffer, 1);
	buffer &= ~(1 << a2);
	buffer |= a1 << a2;
	write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &buffer, sizeof(buffer));
      }

      
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
      buffer &= 0xC7;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer |= 0x20; // trigger sample
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer &= 0xDF; // kill sample
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer |= 0x10; // initiate transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer &= 0xEF; // clear transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    }
    break;

  case 'a':
    if( argc == 2 ) {
      for( a1 = 0; a1 < 8; a1++ ) {
	// first conversion sets address
	read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
	buffer &= 0xE0;
	buffer |= (a1 & 0x7);
	buffer |= 0x08;
	// setup addres
	write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
	buffer |= 0x10;
	// initiate conversion
	write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
	buffer &= 0xEF; // clear transfer
	write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));

	i = 0;
	do {
	  read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MOT_STAT_ADR, &buffer, 1);
	  i++;
	} while ( ((buffer & 0x08) == 0) && (i < 1000) );
	if( i >= 1000 ) {
	  printf( "Note: ADC timed out during readback\n" );
	}

	// second conversion reads out the data
	read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
	buffer &= 0xE0;
	buffer |= (a1 & 0x7);
	buffer |= 0x08;
	// setup addres
	write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
	buffer |= 0x10;
	// initiate conversion
	write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
	buffer &= 0xEF; // clear transfer
	write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));

	i = 0;
	do {
	  read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MOT_STAT_ADR, &buffer, 1);
	  i++;
	} while ( ((buffer & 0x08) == 0) && (i < 1000) );
	if( i >= 1000 ) {
	  printf( "Note: ADC timed out during readback\n" );
	}

	temp = 0;
	read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_ADC_LSB_ADR, &buffer, 1);
	temp = (buffer >> 4) & 0xF;
	read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_ADC_MSB_ADR, &buffer, 1);
	temp |= ((buffer << 4) & 0xF0);
	printf( "0x%02x ", temp & 0xFF );
      }
      printf( "\n" );

    } else {
      if( argc != 3 ) {
	printf( "Insufficient arguments\n" );
	print_help('a');
	break;
      }
      a1 = strtol(argv[2],NULL,0);
      if( (a1 > 7) || (a1 < 0) ) {
	printf( "ADC channel is out of range (0-7)\n" );
	break;
      }
      // first conversion sets address
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
      buffer &= 0xE0;
      buffer |= (a1 & 0x7);
      buffer |= 0x08;
      // setup addres
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer |= 0x10;
      // initiate conversion
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer &= 0xEF; // clear transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));

      i = 0;
      do {
	read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MOT_STAT_ADR, &buffer, 1);
	i++;
      } while ( ((buffer & 0x08) == 0) && (i < 1000) );
      if( i >= 1000 ) {
	printf( "Note: ADC timed out during readback\n" );
      }

      // second conversion reads out the data
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
      buffer &= 0xE0;
      buffer |= (a1 & 0x7);
      buffer |= 0x08;
      // setup addres
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer |= 0x10;
      // initiate conversion
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer &= 0xEF; // clear transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));

      i = 0;
      do {
	read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MOT_STAT_ADR, &buffer, 1);
	i++;
      } while ( ((buffer & 0x08) == 0) && (i < 1000) );
      if( i >= 1000 ) {
	printf( "Note: ADC timed out during readback\n" );
      }

      temp = 0;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_ADC_LSB_ADR, &buffer, 1);
      temp = (buffer >> 4) & 0xF;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_ADC_MSB_ADR, &buffer, 1);
      temp |= ((buffer << 4) & 0xF0);
      printf( "%ld: 0x%02x\n", a1, temp & 0xFF );
    }
    break;

  case 'm':
    if( argc != 4) {
      printf( "Insufficient arguments\n" );
      print_help('m');
      break;
    } else {
      a1 = strtol(argv[2],NULL,0);
      if( (a1 > 4) || (a1 < 1) ) {
	printf( "Motor channel is out of range (1-4)\n" );
	break;
      }
      supplement = *(argv[3]);
      if( !(supplement == 'f' || supplement == 'r' || supplement == 's') ) {
	printf( "Motor command %c not recognized.\n", supplement );
	break;
      }

      // buffer now gets the motor enable value
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MOTEN_ADR, &buffer, 1);

      if( supplement == 'f' ) {
	buffer &= ~(0x3 << ((a1 - 1) * 2));
	buffer |= 0x2 << ((a1 - 1) * 2);
      } else if( supplement == 'r' ) {
	buffer &= ~(0x3 << ((a1 - 1) * 2));
	buffer |= 0x1 << ((a1 - 1) * 2);
      } else {
	buffer &= ~(0x3 << ((a1 - 1) * 2));
      }
      //      printf( "dir %c, buffer %02x\n", supplement, buffer );
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MOTEN_ADR, &buffer, sizeof(buffer));

      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
      buffer &= 0xC7;
      buffer |= 0x10; // initiate transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer &= 0xEF; // clear transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    }
    break;

  case 'S':
    if( argc == 3 ) {
      a1 = strtol(argv[2],NULL,0);
      if( !((a1 == 1) || (a1 ==2)) ) {
	printf( "Only channels 1 and 2 can be toggled to servo or motor control\n" );
      }
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
      
      if( a1 == 1 ) {
	if( buffer & 0x40 ) {
	  printf( "Channel 1 is in servo mode.\n" );
	} else { 
	  printf( "Channel 1 is in motor mode.\n" );
	}
      } else { 
	if( buffer & 0x80 ) {
	  printf( "Channel 2 is in servo mode.\n" );
	} else { 
	  printf( "Channel 2 is in motor mode.\n" );
	}
      }
    } else {
      if( argc != 4 ) {
	printf( "Servo channel set requires [ch] and [mode] arguments.\n" );
	break;
      }
      a1 = strtol(argv[2],NULL,0);
      if( !((a1 == 1) || (a1 ==2)) ) {
	printf( "Only channels 1 and 2 can be toggled to servo or motor control\n" );
      }
      supplement = *(argv[3]);
      if( !(supplement == 's' || supplement == 'm') ) {
	printf( "Only valid modes are s (servo mode) and m (motor mode)\n" );
      }

      // now set a default pulse length equal to 20 ms
      // assuming most people are lazy and don't do this, and 20ms is pretty standard (and safe)
      //0x7ef40 = 520000 = 20 ms @ 26 MHz clock periods
      buffer = 0x40;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER1_ADR, &buffer, sizeof(buffer));
      buffer = 0xEF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER2_ADR, &buffer, sizeof(buffer));
      buffer = 0x07;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER3_ADR, &buffer, sizeof(buffer));

      // read back board control register value so as not to bash anything
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
      if( supplement == 's' ) {
	buffer |= ((a1 == 1) ? 0x40 : 0x80);
      } else {
	buffer &= ((a1 == 1) ? 0xBF : 0x7F);
      }
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    }
    break;

  case 'e':
    if( argc == 2 ) {
      buffer = 0;
      a2 = 0;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER1_ADR, &buffer, sizeof(buffer));
      a2 = buffer & 0xFF;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER2_ADR, &buffer, sizeof(buffer));
      a2 |= (buffer << 8) & 0xFF00;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER3_ADR, &buffer, sizeof(buffer));
      a2 |= (buffer << 16) & 0xFF0000;
      
      printf( "Servo period is approximately %ld microseconds\n", (a2 * 385) / 10000 );
    } else {
      if( argc != 3 ) {
	printf( "Servo period set requires an argument in microseconds (math is accurate to <0.5%%)\n" );
	printf( "Accuracy is limited by cheesball integer rounding in driver, not hardware limits\n" );
	break;
      }
      a1 = strtol(argv[2],NULL,0);
      a2 = (a1 * 10000) / 385; // gets you to about 0.5% accuracy
      // if you want more accuracy, rewrite the routine to use floating point or long longs

      buffer = a2 & 0xFF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER1_ADR, &buffer, sizeof(buffer));
      buffer = (a2 >> 8) & 0xFF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER2_ADR, &buffer, sizeof(buffer));
      buffer = (a2 >> 16) & 0xFF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SERV_PER3_ADR, &buffer, sizeof(buffer));
    }
    break;

  case 's':
    if( argc != 4 ) {
      printf( "Servo angle set requires channel [1,2] argument and angle [0-180, fp], argument\n" );
      printf( "Note: this function assumes a 'typical' analog servo with 1ms-2ms pulse width, and a total range of 180 degrees.\n" );
      break;
    } else {
      a1 = strtol(argv[2],NULL,0);
      if( !(a1 == 1 || a1 == 2) ) {
	printf( "Only channels 1 and 2 are supported for servo mode\n" );
      }
      f1 = strtod(argv[3],NULL);
      if( (f1 <= 0.0) || (f1 >= 180.0) ) {
	printf( "Only angles from 0 to 180 degrees is allowed, got %.2lf.\n", f1 );
      }
      printf( "Setting servo %ld angle to %0.2lf\n", a1, f1);
      // 1ms min, 1.5ms mid, 2ms max
      f2 = (1000.0 * 1000.0) * (f1 / 180.0);  // convert to nanoseconds pulse length
      f2 += 1000000.0; // add 1 ms for the minimum time
      f2 = f2 / 38.46153846; // divide to get # of clock cycles to wait
      a2 = (unsigned long) f2;
      // now bounds check a2 to prevent servo blow-outs
      if( a2 < 26000 ) {
	a2 = 26000;
      }
      if( a2 > 52000 ) {
	a2 = 52000;
      }
      
      // now commit to the right chanel
      if( a1 == 1 ) {
	adr = FPGA_SERV1_W1_ADR;
      } else {
	adr = FPGA_SERV2_W1_ADR;
      }

      buffer = a2 & 0xFF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, adr, &buffer, sizeof(buffer));
      buffer = (a2 >> 8) & 0xFF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, adr + 1, &buffer, sizeof(buffer));
      buffer = (a2 >> 16) & 0xFF;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, adr + 2, &buffer, sizeof(buffer));
    }
    break;
      
  case 'M':
    if( argc != 3 ) {
      printf( "Insufficient arguments\n" );
      print_help('M');
      break;
    } else {
      mcode = argv[2];
      if( strlen(mcode) != 4 ) {
	printf( "Motor argument must have format 'xxxx', where x is one of f,r,s,x\n" );
	break;
      }
      
      for( i = 0; i < 3; i++ ) {
	if( !(mcode[i] == 'f' || mcode[i] == 'r' || mcode[i] == 's' || mcode[i] == 'x') ) {
	  printf( "Motor argument must have format 'xxxx', where x is one of f,r,s,x\n" );
	  printf( "Position %d with value %c is not valid.\n", i, mcode[i] );
	}
      }
      
      // buffer now gets the motor enable value
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MOTEN_ADR, &buffer, 1);

      for( i = 0; i < 4; i++ ) {
	supplement = mcode[i];
	a1 = i + 1;
	if( supplement == 'f' ) {
	  buffer &= ~(0x3 << ((a1 - 1) * 2));
	  buffer |= 0x2 << ((a1 - 1) * 2);
	} else if( supplement == 'r' ) {
	  buffer &= ~(0x3 << ((a1 - 1) * 2));
	  buffer |= 0x1 << ((a1 - 1) * 2);
	} else if( supplement == 's' ) {
	  buffer &= ~(0x3 << ((a1 - 1) * 2));
	} else {
	  // 'x' case is "leave as is"
	}
      }
      //      printf( "dir %c, buffer %02x\n", supplement, buffer );
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MOTEN_ADR, &buffer, sizeof(buffer));

      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
      buffer &= 0xC7;
      buffer |= 0x10; // initiate transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer &= 0xEF; // clear transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    }
    break;

  case 'i':
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
    buffer &= 0xC7;
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    buffer |= 0x20; // trigger sample
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    buffer &= 0xDF; // kill sample
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    buffer |= 0x10; // initiate transfer
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    buffer &= 0xEF; // clear transfer
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));

    if( argc == 2 ) {
      dump_register((unsigned char) FPGA_DIG_IN_ADR);
    } else {
      if( argc != 3 ) {
	printf( "Insufficient arguments\n" );
	print_help('i');
	break;
      } 
      a1 = strtol(argv[2],NULL,0);
      if( (a1 > 7) || (a1 < 0) ) {
	printf( "Channel value %d out of range (0-7)\n", a1 );
	break;
      }
      read_eeprom(DEV_I2C_0, DEVADDR>>1, (unsigned long) FPGA_DIG_IN_ADR, &buffer, 1);
      printf( "%d\n", ((buffer >> a1) & 0x1) ? 1 : 0 );
    }
    break;

  case 'w':
    if( argc != 4 ) {
      printf( "Write requires [adr] and [dat] arguments.\n" );
    } else {
      adr = (unsigned char) strtol(argv[2],NULL,0);
      read_eeprom(DEV_I2C_0, DEVADDR>>1, adr, &buffer, 1);
      printf( "0x%02x: 0x%02x -> ", adr, buffer );
      buffer = (unsigned char) strtol(argv[3],NULL,0);
      write_eeprom(DEV_I2C_0, DEVADDR>>1, adr, &buffer, sizeof(buffer));
      read_eeprom(DEV_I2C_0, DEVADDR>>1, adr, &buffer, 1);
      printf( "0x%02x\n", buffer );
    }
    break;

  case 'n':
    for( i = 0; i < 7; i++ ) {
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DNA_ADR + i, &buffer, 1);
      device_id <<= 8;
      device_id |= (buffer & 0xFF);
    }
    printf( "Device ID: %014llx\n", device_id );
    break;

  case 'D':
    if( argc != 3 ) {
      printf( "Write requires [tst] argument.\n" );
    } else {
      a1 = (unsigned char) strtol(argv[2],NULL,0);
      
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &buffer, 1);
      data = buffer; // stash the dig_out value to restore later

      // set test values for the shift chain
      buffer = a1 & 0xff;
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &buffer, sizeof(buffer));

      // set the test mode bit
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, 1);
      buffer |= 0x80; 
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, sizeof(buffer));

      // commit shift values
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
      buffer &= 0xC7;
      buffer |= 0x10; // initiate transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
      buffer &= 0xEF; // clear transfer
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));

      microsleep(10000); // wait a millisecond

      // clear the test mode bit
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, 1);
      buffer &= 0x7F; 
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, sizeof(buffer));

      // restore dig_out value
      write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &data, sizeof(buffer));

      // reveal the shift chain value
      a2 = 0;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SCAN_TST0_ADR, &buffer, 1);
      a2 |= buffer & 0xFF;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SCAN_TST1_ADR, &buffer, 1);
      a2 |= (buffer & 0xFF) << 8;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SCAN_TST2_ADR, &buffer, 1);
      a2 |= (buffer & 0xFF) << 16;
      read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SCAN_TST3_ADR, &buffer, 1);
      a2 |= (buffer & 0xFF) << 24;
      printf( "shift chain result: 0x%08x\n", a2 );
    }
    break;

  case 'A':
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_MAJOR_ADR, &buffer, 1);
    if( buffer < 193 ) {
      fprintf( stderr, "FPGA version %d is incompatible with motor card auto-detection.\n", buffer );
      printf( "0\n" );
      return -1;
    }

    a1 = 0x6A;
      
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &buffer, 1);
    data = buffer; // stash the dig_out value to restore later

    // set test values for the shift chain
    buffer = a1 & 0xff;
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &buffer, sizeof(buffer));

    // set the test mode bit
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, 1);
    buffer |= 0x80; 
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, sizeof(buffer));

    // commit shift values
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
    buffer &= 0xC7;
    buffer |= 0x10; // initiate transfer
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    buffer &= 0xEF; // clear transfer
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));

    microsleep(10000); // wait a millisecond

    // reveal the shift chain value
    a2 = 0;
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SCAN_TST0_ADR, &buffer, 1);
    a2 |= buffer & 0xFF;
      
    if( a2 != 0x6A ) {
      // not attached
      printf( "0\n" );
      return -1;
    }

    a1 = 0x72;
    // set test values for the shift chain
    buffer = a1 & 0xff;
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &buffer, sizeof(buffer));
    
    // set the test mode bit
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, 1);
    buffer |= 0x80; 
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, sizeof(buffer));

    // commit shift values
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, 1);
    buffer &= 0xC7;
    buffer |= 0x10; // initiate transfer
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    buffer &= 0xEF; // clear transfer
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_BRD_CTL_ADR, &buffer, sizeof(buffer));
    
    microsleep(10000); // wait a millisecond

    // reveal the shift chain value
    a2 = 0;
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_SCAN_TST0_ADR, &buffer, 1);
    a2 |= buffer & 0xFF;
    
    if( a2 != 0x72 ) {
      // not attached
      printf( "0\n" );
      return -1;
    }

    // clear the test mode bit
    read_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, 1);
    buffer &= 0x7F; 
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_EXT1_CTL_ADR, &buffer, sizeof(buffer));

    // restore dig_out value
    write_eeprom(DEV_I2C_0, DEVADDR>>1, FPGA_DIG_OUT_ADR, &data, sizeof(buffer));
    
    printf( "1\n" );
    return 0;
    break;
    

  default:
    print_help(code);
  }

  close(file_desc);
  
  return 0;
}
