/***
This code uses the BSD 2-clause license:

Copyright (c) 2011
  Andrew "bunnie" Huang / bunnie@bunniestudios.com
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
/*
 * I2C code by Aaron "caustik" Robinson
 */

#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

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
    unsigned char               byte;
    int                         device_file;
    struct i2c_rdwr_ioctl_data  packets;
    struct i2c_msg              messages[2];

    // On this chip, the upper two bits of the memory address are
    // represented in the i2c address, and the lower eight are clocked in
    // as the memory address.  This gives a crude mechanism for 4 pages of
    // 256 bytes each.
    byte = start_reg;

    if((device_file = open(i2c_device, O_RDWR))==-1) {
        perror("Unable to open i2c device");
        return 1;
    }

    // Set the address we'll read to the start address.
    messages[0].addr    = addr;
    messages[0].flags   = 0;
    messages[0].len     = sizeof(byte);
    messages[0].buf     = &byte;

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

int write_byte(unsigned char adr, unsigned char dat) {
  if(write_eeprom("/dev/i2c-0", DEVADDR>>1, adr, &dat, sizeof(dat))) {
    fprintf(stderr, "Couldn't write\n");
    return 1;
  }
}

unsigned char read_byte(unsigned char adr) {
  char c;

  read_eeprom("/dev/i2c-0", DEVADDR>>1, adr, &c, sizeof(c));

  return c;
}

int main(int argc, char **argv) {
    unsigned char reg;

    unsigned int i;
    unsigned int mode;

    unsigned char modeline[256];
    FILE *infile;
    char *filename;

    char c;
    int scan;

    unsigned char snoop_ctl = 0;

    unsigned char checksum;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <hexfile>\n", argv[0]);
        return 1;
    }
    filename = argv[1];

    infile = fopen(filename, "r" );
    if( infile == NULL ) {
      printf( "Can't open %s for reading, quitting.\n" );
      exit(0);
    }

    for( i = 0; i< 256; i++ ) {
      modeline[i] = 0xff;
    }

    i  = 0;
    while( !feof(infile) ) {
      fscanf(infile, "%x ", &scan );
      c = (unsigned char) (scan & 0xff);
      modeline[i++] = c;
    }

    snoop_ctl = read_byte(0);
    snoop_ctl &= ~0x40;
    write_byte( 0, snoop_ctl );

    checksum = 0;
    printf( "writing to classic modeline rom:\n" );
    for( i = 0; i < 127; i++ ) {
      write_byte( 0x14, modeline[i] );
      write_byte( 0x13, i & 0x7F );
      write_byte( 0x13, i | 0x80 );
      write_byte( 0x13, i & 0x7F );
      checksum += modeline[i];

      if( (i % 16) == 0 ) {
	printf( "\n%02x: ", i );
      }
      printf( "%02x ", modeline[i]);
    }
    checksum = 0 - checksum;
    write_byte( 0x14, checksum );
    write_byte( 0x13, 127 & 0x7F );
    write_byte( 0x13, 127 | 0x80 );
    write_byte( 0x13, 127 & 0x7F );
    printf( "%02x ", checksum);
    
    printf( "\n" );

    snoop_ctl |= 0x40;
    write_byte( 0, snoop_ctl );

    checksum = 0;
    printf( "writing to HDMI modeline rom:\n" );
    for( i = 128; i < 255; i++ ) {
      write_byte( 0x14, modeline[i] );
      write_byte( 0x13, i & 0x7F );
      write_byte( 0x13, i | 0x80 );
      write_byte( 0x13, i & 0x7F );
      checksum += modeline[i];

      if( (i % 16) == 0 ) {
	printf( "\n%02x: ", i );
      }
      printf( "%02x ", modeline[i]);
    }
    checksum = 0 - checksum;
    write_byte( 0x14, checksum );
    write_byte( 0x13, 255 & 0x7F );
    write_byte( 0x13, 255 | 0x80 );
    write_byte( 0x13, 255 & 0x7F );
    printf( "%02x ", checksum);
    
    printf( "\n" );
    
    return 0;
}
