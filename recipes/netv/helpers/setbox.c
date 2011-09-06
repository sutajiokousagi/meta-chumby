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

int puti2c(unsigned char reg, unsigned char val) {
    unsigned char old_val, new_val;
    //    read_eeprom("/dev/i2c-0", DEVADDR>>1, reg, &old_val, sizeof(old_val));
    if(write_eeprom("/dev/i2c-0", DEVADDR>>1, reg, &val, sizeof(val))) {
        fprintf(stderr, "Couldn't write\n");
        return 1;
    }
    //    read_eeprom("/dev/i2c-0", DEVADDR>>1, reg, &new_val, sizeof(new_val));
    //    fprintf(stderr, "%d: %02hx -> %02hx\n", reg, old_val, new_val);
}

int geti2c(unsigned char reg) {
  unsigned char val;

  read_eeprom("/dev/i2c-0", DEVADDR>>1, reg, &val, sizeof(val));
  
  return val;
}

#define REG_W_L 4
#define REG_W_H 5
#define REG_H_L 6
#define REG_H_H 7
#define REG_X_L 8
#define REG_X_H 9
#define REG_Y_L 10
#define REG_Y_H 11

int main(int argc, char **argv) {
    unsigned char reg;
    unsigned char val;
    unsigned int x, y, w, h;

    fprintf( stderr, "WARNING: Setbox is deprecated as of FPGA version 9 and later. Running this command has no effect on FPGA version 9.\n" );
    fprintf( stderr, "You should consider changing your script so that it does not use this program, as we may reclaim and re-use the registers in the future.\n" );

    if(argc < 5) {
        fprintf(stderr, "Usage: %s <x> <y> <width> <height>\n", argv[0]);
        return 1;
    }

    x  = strtol(argv[1], NULL, 0);
    if( x > 0 )
      x = x - 1;
    y  = strtol(argv[2], NULL, 0);
    if( y > 0 )
      y = y - 1;
    w  = strtol(argv[3], NULL, 0) - 1;
    if( w > 0 )
      w = w - 1; // correct for "start from zero" 
    h  = strtol(argv[4], NULL, 0) - 1;
    if( h > 0 )
      h = h - 1;
    
    puti2c(REG_H_H, 0) ; // disable updating

    //    val=swab_16(val);
    puti2c(REG_X_H, (x >> 8) & 0xff);
    puti2c(REG_X_L, x & 0xFF);

    puti2c(REG_Y_H, (y >> 8) & 0xff );
    puti2c(REG_Y_L, y & 0xFF );

    puti2c(REG_W_H, (w >> 8) & 0xff );
    puti2c(REG_W_L, w & 0xff );

    puti2c(REG_H_L, h & 0xff );

    puti2c(REG_H_H, ((h >> 8) & 0xff) | 0x80 ); // enable updating

    return 0;
}
