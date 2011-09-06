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
/*
 * dumptiming.c
 *
 * bunnie@chumby.com
 * I2C code by Aaron "Caustik" Robinson
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

int main(int argc, char **argv) {
    unsigned char buffer[32];
    int i;

    printf( "--DEPRACATED-- please use fpga_ctl d\n" );
    if(argc == 2) {
      read_eeprom("/dev/i2c-0", DEVADDR>>1, strtol(argv[1], NULL, 0), buffer, 1);
      printf("0x%02x: %02hx\n", strtol(argv[1], NULL, 0), buffer[0]);
      return 1;
    }

    if(read_eeprom("/dev/i2c-0", DEVADDR>>1, 0, buffer, sizeof(buffer))) {
        return 1;
    }

    for(i=0; i<sizeof(buffer)/sizeof(*buffer); i++) {
      if( (i % 4) == 0 ) {
	printf( "\n0x%02x: ", i );
      }
      printf("%02x ", buffer[i]);
    }
    printf("\n");

    return 0;
}
