/*
 * dcid_utility.c
 *
 * Aaron "Caustik" Robinson
 * (c) Copyright Chumby Industries, 2007
 * All rights reserved
 */

#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>

#define SOURCE 1
#define SINK 0

void compute_keys( unsigned long long Ksv, unsigned int source, unsigned long long *key );

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

unsigned char snoop_hdcp_byte(unsigned char adr) {
  char c;

  write_byte( 0x1, (unsigned char) adr & 0xFF );
  write_byte( 0x0, 0x1 | 0x2 );
  write_byte( 0x0, 0x1 );
  
  c = read_byte(0x2);
}

/* 
example hdcp record
    lsb        msb <-sink   ri
00: 09 74 30 d7 fa 00 00 00 e3 2b 00 00 00 00 00 00 
10: 68 f1 e6 12 76 00 00 00 9c 4d 8a 4b 46 32 82 31 
    lsb        msb <-source lsb      An         msb

example edid record
00: 00 ff ff ff ff ff ff 00 4c 2d f6 03 36 32 44 54 
10: 07 13 01 03 80 10 09 78 2a ee 91 a3 54 4c 99 26 
20: 0f 50 54 bf ef 80 a9 40 81 80 81 40 71 4f 01 01 
30: 01 01 01 01 01 01 28 3c 80 a0 70 b0 23 40 30 20 
40: 36 00 a0 5a 00 00 00 1a 01 1d 00 bc 52 d0 1e 20 
50: b8 28 55 40 a0 5a 00 00 00 1e 00 00 00 fd 00 32 
60: 4b 1b 51 11 00 0a 20 20 20 20 20 20 00 00 00 fc 
70: 00 53 79 6e 63 4d 61 73 74 65 72 0a 20 20 01 ed 
80: 02 03 1f f2 4b 93 04 12 83 14 05 20 21 22 1f 10 
90: 23 09 07 07 83 01 00 00 66 03 0c 00 10 00 80 8c 
a0: 0a d0 8a 20 e0 2d 10 10 3e 96 00 a0 5a 00 00 00 
b0: 18 01 1d 00 72 51 d0 1e 20 6e 28 55 00 a0 5a 00 
c0: 00 00 1e 01 1d 80 d0 72 1c 16 20 10 2c 25 80 a0 
d0: 5a 00 00 00 9e 01 1d 80 18 71 1c 16 20 58 2c 25 
e0: 00 a0 5a 00 00 00 9e 8c 0a d0 90 20 40 31 20 0c 
f0: 40 55 00 a0 5a 00 00 00 18 00 00 00 00 00 00 31 
*/

int main(int argc, char **argv) {
    unsigned char reg;

    unsigned int i, num;
    unsigned int mode;

    unsigned long long source_ksv = 0LL;
    unsigned long long sink_ksv = 0LL;
    unsigned long long ksv_temp = 0LL;
    unsigned long long Km = 0LL;
    unsigned long long Kmp = 0LL;
    
    unsigned long long source_pkey[40];
    unsigned long long sink_pkey[40];

    if(argc != 1) {
        fprintf(stderr, "Usage: %s\n", argv[0]);
        return 1;
    }

    for( i = 0; i < 5; i++ ) {
      sink_ksv <<= 8;
      sink_ksv |= (snoop_hdcp_byte(4 - i) & 0xff);
    }

    for( i = 0; i < 5; i++ ) {
      source_ksv <<= 8;
      source_ksv |= (snoop_hdcp_byte(4 - i + 0x10) & 0xff);
    }

    printf( "source ksv: %010llx\n", source_ksv );
    printf( "sink ksv: %010llx\n", sink_ksv );
    compute_keys( source_ksv, SOURCE, source_pkey );
    compute_keys( sink_ksv, SINK, sink_pkey );


    ksv_temp = source_ksv; // source Ksv
    num = 0;
    for( i = 0; i < 40; i++ ) {
      if( ksv_temp & 1LL ) {
	num++;
	Km += sink_pkey[i]; // used to select sink's keys
	Km %=  72057594037927936LL;
	//	printf( "Km %014llx\n", Km );
      }
      ksv_temp >>= 1LL;
    }
    //    printf( "num 1's: %d\n", num );
    // km is the sink km

    ksv_temp = sink_ksv; // sink Ksv
    num = 0;
    for( i = 0; i < 40; i++ ) {
      if( ksv_temp & 1LL ) {
	num++;
	Kmp += source_pkey[i]; // used to select source's keys
	Kmp %=  72057594037927936LL;
	//	printf( "Kmp %014llx\n", Kmp );
      }
      ksv_temp >>= 1LL;
    }
    //    printf( "num 1's: %d\n", num );
    // Kmp is the source Km
  
    Km &= 0xFFFFFFFFFFFFFFLL;
    Kmp &= 0xFFFFFFFFFFFFFFLL;
  
    printf( "\n" );
    printf( "Km : %014llx\n", Km );
    printf( "Km': %014llx\n", Kmp );

    if( Km != Kmp ) {
      printf( "Km is not equal to Km', can't encrypt this stream.\n" );
      exit(0);
    }
    
    for( i = 0; i < 7; i++ ) {
      write_byte( i + 0x19, Km & 0xFF );
      Km >>= 8;
    }
    
    // set the exclusion on the edid sniffer, just in case it's not already set.
    write_byte( 0x18, 0x74 ); 

    return 0;
}