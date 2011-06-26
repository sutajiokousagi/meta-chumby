/***
    writecached_Km.c
    bunnie@bunniestudois.com  BSD licensed

    utility to write a cached Km value, if it exists, into the FPGA.
 ***/

#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>

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

#define DEVADDR 0x3C
//#define DEVADDR 0x94

int write_byte(unsigned char adr, unsigned char dat) {
  if(write_eeprom("/dev/i2c-0", DEVADDR>>1, adr, &dat, sizeof(dat))) {
    fprintf(stderr, "Couldn't write\n");
    return 1;
  }
}

int main() {
  FILE *km_file;
  unsigned long long Km;
  int i;

  km_file = fopen("/psp/km_cache", "rb" );
  if( km_file == NULL ) {
    printf( "Can't open Km cache for reading. Aborting without affecting any state.\n" );
  } else {
    fread( &Km, sizeof(unsigned long long), 1, km_file );

    printf( "Cached Km of %014llx found.\n", Km );
    for( i = 0; i < 7; i++ ) {
      write_byte( i + 0x19, Km & 0xFF );
      Km >>= 8;
    }
    fclose(km_file);
  }
  
  return 0;

}
