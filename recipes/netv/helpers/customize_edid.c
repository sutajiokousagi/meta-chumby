/***
This code uses the BSD 2-clause license:

Copyright (c) 2011, Andrew "bunnie" Huang
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
 * snoop.c
 *
 * Utility to report snooped EDID and HDCP values
 *
 * I2C routines by Aaron "Caustik" Robinson
 */

#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/mman.h>

// this list should be merged with the one in fpga_ctl.c
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

extern unsigned char* make_hdmi(char *modelist);
extern unsigned char *make_edid(char *modelist);

static int fd = 0;
static int   *mem_32 = 0;
static short *mem_16 = 0;
static char  *mem_8  = 0;
static int *prev_mem_range = 0;
static int read_kernel_memory(long offset, int virtualized, int size);

#define SINK_DISCONNECTED 0
#define SINK_CONNECTED 1

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

void write_modeline(char *modeline) {
    unsigned int i;

    unsigned char snoop_ctl = 0;
    unsigned char checksum;

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
}

char default_edidname[] = "/psp/cached.edid";
int main(int argc, char **argv) {
    unsigned char reg;

    unsigned int i;
    unsigned int mode;
    unsigned char snoopctl, snoopctl_orig;

    unsigned char edid[256];
    unsigned char cached_edid[256];
    unsigned char netv_edid[256];
    unsigned char *hdmi;
    unsigned char *new_edid;
    FILE *edid_file = NULL;
    FILE *test_file = NULL;
    char *edid_fname = NULL;
    char *test_fname = NULL;
    unsigned int cache_valid = 0;
    unsigned char buffer;
    unsigned char compctl;
    unsigned char cea_list[128];
    char c;
    int cx;
    int temp;
    int num_modes = 0;

    cx = getopt(argc, argv, "hf:T:");
    while(cx != -1 ) {
      printf( "%02x", cx );
      c = (char) cx;
      switch(c) {
      case 'f':
	edid_fname = malloc(strlen(optarg) * sizeof(char));
	if( edid_fname != NULL ) {
	  // safe to use strcpy because buffer is allocated dynamically
	  // and matched to length of optarg
	  strcpy(edid_fname, optarg);
	}
	break;
      case 'T':
	test_fname = malloc(strlen(optarg) * sizeof(char));
	if( test_fname != NULL ) {
	  strcpy(test_fname, optarg);
	}
	break;
      case 'h':
      default:
	fprintf(stderr, "Usage: %s [-h] [-f path-to-edid-cache] [-T file-for-test]\n", argv[0]);
	exit(0);
	break;
      } // switch
      cx = getopt(argc, argv, "hf:T:");
    } // while

    bzero(cached_edid, sizeof(cached_edid) );

    if( edid_fname == NULL )
      edid_fname = default_edidname;

    edid_file = fopen(edid_fname, "rw+");

    if( edid_file != NULL ) {
      for( i = 0; !feof(edid_file) && (i < 256); i ++ ) {
	fscanf(edid_file, "%x%*[ \n\t]", &temp );
	cached_edid[i] = temp & 0xFF;
      }
      if( parse_edid( cached_edid ) ) {
	cache_valid = 0;
      } else {
	cache_valid = 1;
      }
      rewind( edid_file );
    } else {
      cache_valid = 0;
      edid_file = fopen(edid_fname, "w+");
      if( edid_file == NULL ) {
	fprintf(stderr, "Can't open/create %s for writing.\n", edid_fname );
	exit(1);
      }
    }
    if( cache_valid ) 
      fprintf(stderr,"Cached EDID has a valid structure.\n" );
    else
      fprintf(stderr,"Cached EDID has an invalid structure.\n" );

    for( i = 0; i< 256; i++ ) {
      edid[i] = 0xff;
      netv_edid[i] = 0xff;
    }
    
    if( test_fname == NULL ) {
      // first check and see if a TV is present
      if( sink_status() == SINK_DISCONNECTED ) {
	if( cache_valid ) {
	  fprintf(stderr, "No sink detected, but cache is valid. Cowardly quitting and doing nothing. User should be instructed to plug in the TV before boot.\n" );
	  // need to commit the cached modeline though, that's a requirement of this program...
	  // all non-test exit paths should leave with at least a valid modeline written in
	  write_modeline(cached_edid);

	  exit(1);
	} else { 
	  // put in a bogus 720p-only record
	  for( i = 0; i < 128; i++ ) {
	    cea_list[i] = 0;
	  }
	  cea_list[4] = 1; // set 720p support

	  // now build a new EDID based upon the supported modes from the monitor
	  new_edid = make_edid(cea_list);
	  memcpy( netv_edid, new_edid, sizeof(unsigned char) * 128);
	  free(new_edid);

	  hdmi = make_hdmi(cea_list);
	  memcpy( &(netv_edid[128]), hdmi, sizeof(unsigned char) * 128);
	  free(hdmi); // HDMI is calloc'd inside make_hdmi

	  fprintf( stderr, "No sink, but EDID cache is invalid. Committing a 'safe' 720p-only EDID to disk.\n" );
	  // if cache is invalid...
	  // write the EDID to a file
	  for( i = 0; i < 256; i++ ) {
	    if( (i % 16 == 0) && (i != 0) ) {
	      fprintf(edid_file, "\n" );
	    }
	    fprintf(edid_file, "%02x ", netv_edid[i] );
	  }
	  fprintf(edid_file, "\n" );
	  fclose( edid_file );
	  system("sync"); // force it to commit to flash

	  write_modeline(netv_edid);

	  exit(1);
	}
      }
      
      snoopctl = read_byte(0x0);
      snoopctl_orig = snoopctl;

      compctl = read_byte(0x3);
      fprintf(stderr, "Checking semaphore...\n" );
      if( (compctl & 0x80) ) {
	// semaphore is set
	fprintf(stderr, "Semaphore set, someone else is running: exiting.\n" );
	return 1;
      }
      compctl |= 0x80; // set the semaphore
      write_byte( 0x3, compctl );
      fprintf(stderr, "Grabbed semaphore!\n" );


      // turn off EDID squashing
      buffer = read_byte( FPGA_SNOOP_CTL_ADR ) ;
      buffer &= 0xFB;
      write_byte( FPGA_SNOOP_CTL_ADR, buffer );
      
      
      snoopctl = read_byte(0x0);
      snoopctl &= 0xFE;
      snoopctl |= 0x08;  // force HPD, leave all other bits intact
      write_byte( 0x0, snoopctl ); // hpd is now forcing
      
      deepsleep(0, 150); // 150 ms sleep
      
      snoopctl &= 0xF7;
      write_byte( 0x0, snoopctl ); // hpd is now releasing
      
      deepsleep(1, 0); // 1 s sleep to get the EDIDs transferred


      for( i = 0; i < 0x100; i++ ) {
	write_byte( 0x1, (unsigned char) i & 0xFF );
	write_byte( 0x0, snoopctl | 0x2 );
	write_byte( 0x0, snoopctl );
	if( (i % 16) == 0 ) {
	  fprintf(stderr, "\n%02x: ", i );
	}
	edid[i] = read_byte(0x2);
	fprintf(stderr, "%02x ", edid[i]);
      }
      fprintf(stderr, "\n" );
      
      // turn edid squashing back on
      buffer = read_byte( FPGA_SNOOP_CTL_ADR ) ;
      buffer |= 0x04;
      write_byte( FPGA_SNOOP_CTL_ADR, buffer );

    } else {
      test_file = fopen( test_fname, "r" );
      if( test_file != NULL ) {
	for( i = 0; i < 256; i++ ) {
	  fscanf(test_file, "%x%*[ \n\t]", &temp );
	  edid[i] = temp & 0xFF;
	  if( i % 16 == 0 )
	    fprintf(stderr, "\n" );
	  fprintf(stderr, "%02x ", edid[i] );
	}
      } else {
	fprintf(stderr, "Can't open %s, aborting.\n", test_fname );
	exit(0);
      }
    }
    
    if( parse_edid( edid ) ) {
      fprintf(stderr, "invalid EDID from monitor, big problems\n" );
    }

    // alright, at this point, i need to extract a mode list
    // in a form that's usable so i can process it

    // 0x80 is a magic number that is the offset to
    // HDMI_EXTENSION. this code is in parse-edid.c and
    // should probably be broken into a header
    parse_hdmi_extension_return(edid + 0x80, cea_list);
    
    fprintf(stderr,"modes supported: ");
    num_modes = 0;
    for( i = 0; i < 128; i++ ) {
      if( cea_list[i] != 0 ) {
	fprintf(stderr, "%d ", i );
	num_modes ++;
      }
    }
    fprintf(stderr,"\n" );

    printf( "\n" ); // compatibility with bug in check_edid.c
    if( num_modes == 0 ) {
      fprintf(stderr, "No overlapping modes supported between NeTV and monitor. Forcing 720p and praying...\n" );
      cea_list[4] = 1;
    }

    // now build a new EDID based upon the supported modes from the monitor
    new_edid = make_edid(cea_list);
    memcpy( netv_edid, new_edid, sizeof(unsigned char) * 128);
    free(new_edid);

    hdmi = make_hdmi(cea_list);
    memcpy( &(netv_edid[128]), hdmi, sizeof(unsigned char) * 128);
    free(hdmi); // HDMI is calloc'd inside make_hdmi

    if( cache_valid ) {
      for( i = 0; i < 256; i++ ) {
	if( netv_edid[i] != cached_edid[i] )
	  cache_valid = 0;
      }
    }

    if( !cache_valid ) {
      fprintf( stderr, "EDID cache is invalid, committing to disk.\n" );
      // if cache is invalid...
      // write the EDID to a file
      for( i = 0; i < 256; i++ ) {
	if( (i % 16 == 0) && (i != 0) ) {
	  fprintf(edid_file, "\n" );
	}
	fprintf(edid_file, "%02x ", netv_edid[i] );
      }
      fprintf(edid_file, "\n" );
      fclose( edid_file );
      system("sync"); // force it to commit to flash
    } else {
      fprintf( stderr, "EDID cache was valid, not touching the disk.\n" );
      fclose( edid_file );
    }

    if( test_fname == NULL ) {
      fprintf(stderr, "Pulling HPD to re-load the spoofed NeTV EDID.\n");
      // edid squashing was turned off way above

      // load data into the modeline ROM
      write_modeline(netv_edid);

      // initiate a quick HPD to re-load the new EDID to the source
      snoopctl = read_byte(0x0);
      snoopctl |= (0x08 | 0x04);  // force HPD, edid squash, leave all other bits intact
      write_byte( 0x0, snoopctl ); // hpd is now forcing
      
      deepsleep(0, 150); // 150 ms sleep
      
      snoopctl &= 0xF7;
      snoopctl |= 0x04; // make sure edid squash stays on
      write_byte( 0x0, snoopctl ); // hpd is now releasing
    }

    if( test_fname == NULL ) {
      fprintf(stderr, "\nDone. Releasing semaphore.\n" );
      // then release semaphore
      compctl = read_byte(0x3); // refresh compctl in case other bits changed
      
      compctl &= 0x7F; // release semaphore
      write_byte( 0x3, compctl );
    }

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
