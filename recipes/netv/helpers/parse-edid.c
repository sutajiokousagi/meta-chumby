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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: rewrite
// FIXME: cleanup 'static' variables

typedef unsigned char byte;
/* byte must be 8 bits */

/* int must be at least 16 bits */

/* long must be at least 32 bits */

#define VERSION "2.0"

#define DIE_MSG( x ) \
        { MSG( x ); exit( 1 ); }


#define UPPER_NIBBLE( x ) \
        (((128|64|32|16) & (x)) >> 4)

#define LOWER_NIBBLE( x ) \
        ((1|2|4|8) & (x))

#define COMBINE_HI_8LO( hi, lo ) \
        ( (((unsigned)hi) << 8) | (unsigned)lo )

#define COMBINE_HI_4LO( hi, lo ) \
        ( (((unsigned)hi) << 4) | (unsigned)lo )

const byte edid_v1_header[] = { 0x00, 0xff, 0xff, 0xff,
                                0xff, 0xff, 0xff, 0x00 };

const byte edid_v1_descriptor_flag[] = { 0x00, 0x00 };

#define HDMI_EXTENSION                          0x80

#define EDID_LENGTH                             0x80

#define EDID_HEADER                             0x00
#define EDID_HEADER_END                         0x07

#define ID_MANUFACTURER_NAME                    0x08
#define ID_MANUFACTURER_NAME_END                0x09
#define ID_MODEL				0x0a

#define ID_SERIAL_NUMBER			0x0c

#define MANUFACTURE_WEEK			0x10
#define MANUFACTURE_YEAR			0x11

#define EDID_STRUCT_VERSION                     0x12
#define EDID_STRUCT_REVISION                    0x13

#define DPMS_FLAGS				0x18
#define ESTABLISHED_TIMING_1                    0x23
#define ESTABLISHED_TIMING_2                    0x24
#define MANUFACTURERS_TIMINGS                   0x25

#define DETAILED_TIMING_DESCRIPTIONS_START      0x36
#define DETAILED_TIMING_DESCRIPTION_SIZE        18
#define NO_DETAILED_TIMING_DESCRIPTIONS         4



#define DETAILED_TIMING_DESCRIPTION_1           0x36
#define DETAILED_TIMING_DESCRIPTION_2           0x48
#define DETAILED_TIMING_DESCRIPTION_3           0x5a
#define DETAILED_TIMING_DESCRIPTION_4           0x6c



#define PIXEL_CLOCK_LO     (unsigned)dtd[ 0 ]
#define PIXEL_CLOCK_HI     (unsigned)dtd[ 1 ]
#define PIXEL_CLOCK        (COMBINE_HI_8LO( PIXEL_CLOCK_HI,PIXEL_CLOCK_LO )*10000)

#define H_ACTIVE_LO        (unsigned)dtd[ 2 ]

#define H_BLANKING_LO      (unsigned)dtd[ 3 ]

#define H_ACTIVE_HI        UPPER_NIBBLE( (unsigned)dtd[ 4 ] )

#define H_ACTIVE           COMBINE_HI_8LO( H_ACTIVE_HI, H_ACTIVE_LO )

#define H_BLANKING_HI      LOWER_NIBBLE( (unsigned)dtd[ 4 ] )

#define H_BLANKING         COMBINE_HI_8LO( H_BLANKING_HI, H_BLANKING_LO )




#define V_ACTIVE_LO        (unsigned)dtd[ 5 ]

#define V_BLANKING_LO      (unsigned)dtd[ 6 ]

#define V_ACTIVE_HI        UPPER_NIBBLE( (unsigned)dtd[ 7 ] )

#define V_ACTIVE           COMBINE_HI_8LO( V_ACTIVE_HI, V_ACTIVE_LO )

#define V_BLANKING_HI      LOWER_NIBBLE( (unsigned)dtd[ 7 ] )

#define V_BLANKING         COMBINE_HI_8LO( V_BLANKING_HI, V_BLANKING_LO )



#define H_SYNC_OFFSET_LO   (unsigned)dtd[ 8 ]
#define H_SYNC_WIDTH_LO    (unsigned)dtd[ 9 ]

#define V_SYNC_OFFSET_LO   UPPER_NIBBLE( (unsigned)dtd[ 10 ] )
#define V_SYNC_WIDTH_LO    LOWER_NIBBLE( (unsigned)dtd[ 10 ] )

#define V_SYNC_WIDTH_HI    ((unsigned)dtd[ 11 ] & (1|2))
#define V_SYNC_OFFSET_HI   (((unsigned)dtd[ 11 ] & (4|8)) >> 2)

#define H_SYNC_WIDTH_HI    (((unsigned)dtd[ 11 ] & (16|32)) >> 4)
#define H_SYNC_OFFSET_HI   (((unsigned)dtd[ 11 ] & (64|128)) >> 6)


#define V_SYNC_WIDTH       COMBINE_HI_4LO( V_SYNC_WIDTH_HI, V_SYNC_WIDTH_LO )
#define V_SYNC_OFFSET      COMBINE_HI_4LO( V_SYNC_OFFSET_HI, V_SYNC_OFFSET_LO )

#define H_SYNC_WIDTH       COMBINE_HI_4LO( H_SYNC_WIDTH_HI, H_SYNC_WIDTH_LO )
#define H_SYNC_OFFSET      COMBINE_HI_4LO( H_SYNC_OFFSET_HI, H_SYNC_OFFSET_LO )

#define H_SIZE_LO          (unsigned)dtd[ 12 ]
#define V_SIZE_LO          (unsigned)dtd[ 13 ]

#define H_SIZE_HI          UPPER_NIBBLE( (unsigned)dtd[ 14 ] )
#define V_SIZE_HI          LOWER_NIBBLE( (unsigned)dtd[ 14 ] )

#define H_SIZE             COMBINE_HI_8LO( H_SIZE_HI, H_SIZE_LO )
#define V_SIZE             COMBINE_HI_8LO( V_SIZE_HI, V_SIZE_LO )

#define H_BORDER           (unsigned)dtd[ 15 ]
#define V_BORDER           (unsigned)dtd[ 16 ]

#define FLAGS              (unsigned)dtd[ 17 ]

#define INTERLACED         (FLAGS&128)
#define SYNC_TYPE	   (FLAGS&3<<3)  /* bits 4,3 */
#define SYNC_SEPARATE	   (3<<3)
#define HSYNC_POSITIVE	   (FLAGS & 4)
#define VSYNC_POSITIVE     (FLAGS & 2)

#define MONITOR_NAME            0xfc
#define MONITOR_LIMITS          0xfd
#define UNKNOWN_DESCRIPTOR      -1
#define DETAILED_TIMING_BLOCK   -2


#define DESCRIPTOR_DATA         5
#define V_MIN_RATE              block[ 5 ]
#define V_MAX_RATE              block[ 6 ]
#define H_MIN_RATE              block[ 7 ]
#define H_MAX_RATE              block[ 8 ]

#define MAX_PIXEL_CLOCK         (((int)block[ 9 ]) * 10)
#define GTF_SUPPORT		block[10]

#define DPMS_ACTIVE_OFF		(1 << 5)
#define DPMS_SUSPEND		(1 << 6)
#define DPMS_STANDBY		(1 << 7)

char* myname;

byte *base;

void MSG( const char* x )
{
  fprintf(stderr, "%s: %s\n", myname, x ); 
}


int
parse_edid( byte* edid );


int
parse_timing_description( byte* dtd );


int
parse_monitor_limits( byte* block );

int
block_type( byte* block );

char*
get_monitor_name( byte const*  block );

char*
get_vendor_sign( byte const* block );

int
parse_dpms_capabilities( byte flags );

#if 0
int
main( int argc, char** argv )
{
  byte edid[ EDID_LENGTH ];
  FILE* edid_file;

  myname = argv[ 0 ];
  fprintf(stderr, "%s: parse-edid version %s\n", myname, VERSION );
  
  if ( argc > 2 )
    {
      DIE_MSG( "syntax: [input EDID file]" );
    }
  else
    {
      if ( argc == 2 )
	{
	  edid_file = fopen( argv[ 1 ], "rb" );
	  if ( !edid_file )
	    DIE_MSG( "unable to open file for input" );
	}
      
      else
	edid_file = stdin;
    }

  if ( fread( edid, sizeof( byte ), EDID_LENGTH, edid_file )
       != EDID_LENGTH )

    {
      DIE_MSG( "IO error reading EDID" );
    }

  fclose( edid_file );
  
  return parse_edid( edid );
}
#endif

#define NUM_VIDMODES 39
unsigned char modes[39][128] = {
  "640x480p 59.94Hz/60Hz 4:3 1:1",
    "720x480p 59.94Hz/60Hz 4:3 8:9",
    "720x480p 59.94Hz/60Hz 16:9 32:27", 
    "1280x720p 59.94Hz/60Hz 16:9 1:1", 
    "1920x1080i 59.94Hz/60Hz 16:9 1:1",
    "720(1440)x480i 59.94Hz/60Hz 4:3 8:9",
    "720(1440)x480i 59.94Hz/60Hz 16:9 32:27", 
    "720(1440)x240p 59.94Hz/60Hz 4:3 4:9", 
    "720(1440)x240p 59.94Hz/60Hz 16:9 16:27", 
    "2880x480i 59.94Hz/60Hz 4:3 2:9 - 20:93",
    "2880x480i 59.94Hz/60Hz 16:9 8:27 - 80:27",
    "2880x240p 59.94Hz/60Hz 4:3 1:9 - 10:9",
    "2880x240p 59.94Hz/60Hz 16:9 4:27 - 40:27",
    "1440x480p 59.94Hz/60Hz 4:3 4:9",
    "1440x480p 59.94Hz/60Hz 16:9 16:27",
    "1920x1080p 59.94Hz/60Hz 16:9 1:1",
    "720x576p 50Hz 4:3 16:15",
    "720x576p 50Hz 16:9 64:45",
    "1280x720p 50Hz 16:9 1:1",
    "1920x1080i 50Hz 16:9 1:1",
    "720(1440)x576i 50Hz 4:3 16:15",
    "720(1440)x576i 50Hz 16:9 64:45",
    "720(1440)x288p 50Hz 4:3 8:15",
    "720(1440)x288p 50Hz 16:9 32:45",
    "2880x576i 50Hz 4:3 2:15 - 20:15",
    "2880x576i 50Hz 16:9 16:45-160:45",
    "2880x288p 50Hz 4:3 1:15 - 10:15",
    "2880x288p 50Hz 16:9 8:45 - 80:45",
    "1440x576p 50Hz 4:3 8:15",
    "1440x576p 50Hz 16:9 32:45",
    "1920x1080p 50Hz 16:9 1:1",
    "1920x1080p 23.97Hz/24Hz 16:9 1:1",
    "1920x1080p 25Hz 16:9 1:1",
    "1920x1080p 29.97Hz/30Hz 16:9 1:1",
    "2880x480p 59.94Hz/60Hz 4:3 2:9",
    "2880x480p 59.94Hz/60Hz 16:9 8:27",
    "2880x576p 50Hz 4:3 4:15",
    "2880x576p 50Hz 16:9 16:45",
    "1920x1080i (1250 total) 50Hz 16:9 1:1"
    };

void decode_hdmi_videomode(int mode) {
  if( (mode & 0x7f) < NUM_VIDMODES ) {
    fprintf(stderr, "    code %02d: %s", (mode & 0x7f), modes[(mode & 0x7f) - 1] );
    if( mode & 0x80 ){
      fprintf (stderr, " ** NATIVE" );
    }
    fprintf(stderr, "\n" );
  } else {
    fprintf(stderr, "video mode number not recognized: %d\n", mode & 0x7f );
  }
}

void decode_hdmi_videomode_return(int mode, unsigned char *list) {
  if( (mode & 0x7f) < NUM_VIDMODES ) {
    list[mode] = 1;
    fprintf(stderr, "    code %02d: %s", (mode & 0x7f), modes[(mode & 0x7f) - 1] );
    if( mode & 0x80 ){
      fprintf (stderr, " ** NATIVE" );
      list[mode] = 0xFF;
    }
    fprintf(stderr, "\n" );
  } else {
    fprintf(stderr, "video mode number not recognized: %d\n", mode & 0x7f );
  }
}

unsigned int parse_hdmi_datablocks(byte *block) {
  int num_descrips;
  int tag_code;
  int extended_tag_code;
  int i;

  int offset = 0;

  fprintf(stderr, " " );
  num_descrips = block[0] & 0x1F;
  tag_code = block[0] >> 5;
  offset++;

#ifdef DETAILS
  fprintf(stderr, "Offset: %02x\n", (unsigned int) (block - base) );
#endif

  switch(tag_code) {
  case 0: 
    fprintf(stderr, "reserved tag code\n" );
    break;
  case 1:
    fprintf(stderr, "audio data block\n    " );
    for( i = 0; i < num_descrips; i++ ) {
      fprintf(stderr, "%02x ", block[offset++] );
    }
    fprintf(stderr, "\n" );
    break;
  case 2:
    fprintf(stderr, "video data block at %02x\n", offset + 0x80 );
    for( i = 0; i < num_descrips; i++ ) {
      decode_hdmi_videomode(block[offset++]);
    }
    break;
  case 3:
    i = 0;
    fprintf(stderr, "vendor specific data block\n" );
    fprintf(stderr, "    IEEE registration #: 0x%02x%02x%02x\n", block[offset+2], block[offset+1], block[offset] );
    offset += 3; i += 3;
    fprintf(stderr, "    AB CD: %02x %02x\n", block[offset], block[offset+1] );
    offset += 2; i += 2;
    fprintf(stderr, "    undecoded extensions: " );
    for( ; i < num_descrips; i++ ) {
      fprintf(stderr, "%02x ", block[offset++] );
    }
    fprintf(stderr, "\n" );
    break;
  case 4: 
    fprintf(stderr, "speaker allocation data block\n    " );
    for( i = 0; i < num_descrips; i++ ) {
      fprintf(stderr, "%02x ", block[offset++] );
    }
    fprintf(stderr, "\n" );
    break;
  case 5:
    fprintf(stderr, "vesa DTC data block\n    " );
    for( i = 0; i < num_descrips; i++ ) {
      fprintf(stderr, "%02x ", block[offset++] );
    }
    fprintf(stderr, "\n" );
    break;
  case 6:
    fprintf(stderr, "reserved tag code\n" );
    break;
  case 7:
    fprintf(stderr, "using extended tag code\n" );
    extended_tag_code = block[offset++];
    switch(extended_tag_code) {
    case 0:
      fprintf(stderr, "video capability data block\n" );
      break;
    case 1:
      fprintf(stderr, "vendor-specific video data block\n" );
      break;
    case 4:
      fprintf(stderr, "reserved for HDMI video data block\n" );
      break;
    case 18:
      fprintf(stderr, "reserved for HDMI audio data block\n" );
      break;
    default:
      fprintf(stderr, "extended tag code reading not implemented\n" );
      break;
    }
    break;
  default:
    fprintf(stderr, "unrecognized tag\n" );
    break;
  }

  return(num_descrips + 1);
}

// derived from parse_hdmi_datablocks, this will populate list with
// supported CEA modes.
// "list" is a 128-entry table, initialized to zero, and then a non-zero
// value is entered for every index that corresponds to a valid CEA mode.
// if the mode is native, the value will be 255.
unsigned int parse_hdmi_datablocks_return(byte *block, unsigned char *list) {
  int num_descrips;
  int tag_code;
  int extended_tag_code;
  int i;

  int offset = 0;

  fprintf(stderr, " " );
  num_descrips = block[0] & 0x1F;
  tag_code = block[0] >> 5;
  offset++;

#ifdef DETAILS
  fprintf(stderr, "Offset: %02x\n", (unsigned int) (block - base) );
#endif

  switch(tag_code) {
  case 0: 
    fprintf(stderr, "reserved tag code\n" );
    break;
  case 1:
    fprintf(stderr, "audio data block\n    " );
    for( i = 0; i < num_descrips; i++ ) {
      fprintf(stderr, "%02x ", block[offset++] );
    }
    fprintf(stderr, "\n" );
    break;
  case 2:
    fprintf(stderr, "video data block at %02x\n", offset + 0x80 );
    for( i = 0; i < num_descrips; i++ ) {
      decode_hdmi_videomode_return(block[offset++], list);
    }
    break;
  case 3:
    i = 0;
    fprintf(stderr, "vendor specific data block\n" );
    fprintf(stderr, "    IEEE registration #: 0x%02x%02x%02x\n", block[offset+2], block[offset+1], block[offset] );
    offset += 3; i += 3;
    fprintf(stderr, "    AB CD: %02x %02x\n", block[offset], block[offset+1] );
    offset += 2; i += 2;
    fprintf(stderr, "    undecoded extensions: " );
    for( ; i < num_descrips; i++ ) {
      fprintf(stderr, "%02x ", block[offset++] );
    }
    fprintf(stderr, "\n" );
    break;
  case 4: 
    fprintf(stderr, "speaker allocation data block\n    " );
    for( i = 0; i < num_descrips; i++ ) {
      fprintf(stderr, "%02x ", block[offset++] );
    }
    fprintf(stderr, "\n" );
    break;
  case 5:
    fprintf(stderr, "vesa DTC data block\n    " );
    for( i = 0; i < num_descrips; i++ ) {
      fprintf(stderr, "%02x ", block[offset++] );
    }
    fprintf(stderr, "\n" );
    break;
  case 6:
    fprintf(stderr, "reserved tag code\n" );
    break;
  case 7:
    fprintf(stderr, "using extended tag code\n" );
    extended_tag_code = block[offset++];
    switch(extended_tag_code) {
    case 0:
      fprintf(stderr, "video capability data block\n" );
      break;
    case 1:
      fprintf(stderr, "vendor-specific video data block\n" );
      break;
    case 4:
      fprintf(stderr, "reserved for HDMI video data block\n" );
      break;
    case 18:
      fprintf(stderr, "reserved for HDMI audio data block\n" );
      break;
    default:
      fprintf(stderr, "extended tag code reading not implemented\n" );
      break;
    }
    break;
  default:
    fprintf(stderr, "unrecognized tag\n" );
    break;
  }

  return(num_descrips + 1);
}

void parse_hdmi_extension(byte *block) {
  int descriptor_offset;
  int cur_offset;
  int num_descrips;
  int i;
  unsigned char checksum;

#ifdef DETAILS
  fprintf(stderr, "Offset: %02x\n", (unsigned int) (block - base) );
#endif
  if( block[0] != 0x02 ) {
    fprintf(stderr, "not an CEA extension block, aborting.\n" );
    return;
  }
  fprintf(stderr, "CEA extension block revision %d\n", block[1] );

  descriptor_offset = block[2];
  num_descrips = block[3] & 0xf;
  
  cur_offset = 4; // start of data blocks
  
  while( cur_offset < descriptor_offset ) {
    fprintf(stderr, "Parsing datablock at offset %02x\n", cur_offset );
    cur_offset += parse_hdmi_datablocks(&(block[cur_offset]));
  }
  
  fprintf(stderr, "Number of native descriptors: %d; flags: %01x\n", num_descrips, block[3] >> 4 );
  
  num_descrips = 5;
  fprintf(stderr, "Trying to decode up to 5 data blocks\n" );
  for( i = 0; i < num_descrips; i++, cur_offset += DETAILED_TIMING_DESCRIPTION_SIZE ) {
    if ( block_type( &(block[cur_offset]) ) == DETAILED_TIMING_BLOCK ) {
      fprintf(stderr, "Parsing descriptor at %02x\n", cur_offset );
      parse_timing_description( &(block[cur_offset]) );
    }
  }

  checksum = 0;
  for( i = 0; i < 0x80; i++ ) {
    checksum += block[i];
  }
  fprintf(stderr, "Block checksum: %02x; ", block[0x7f] );
  if( checksum == 0 ) {
    fprintf(stderr, "checksum passes\n" );
  } else {
    fprintf(stderr, "checksum fails\n" );
  }
}

// this function is derived from the above, but it will return a list of supported modes
// the list pointer should be an array of at least 128 characters in size as there are 128 CEA modes
// the list is zero'd out, and if the value is non-zero the mode is supported. If it's 255 the mode is native
void parse_hdmi_extension_return(byte *block, unsigned char *list) {
  int descriptor_offset;
  int cur_offset;
  int num_descrips;
  int i;
  unsigned char checksum;

  if(list == NULL) {
    return;
  }
  bzero(list, 128 * sizeof(char));

#ifdef DETAILS
  fprintf(stderr, "Offset: %02x\n", (unsigned int) (block - base) );
#endif
  if( block[0] != 0x02 ) {
    fprintf(stderr, "not an CEA extension block, aborting.\n" );
    return;
  }
  fprintf(stderr, "CEA extension block revision %d\n", block[1] );

  descriptor_offset = block[2];
  num_descrips = block[3] & 0xf;
  
  cur_offset = 4; // start of data blocks
  
  while( cur_offset < descriptor_offset ) {
    fprintf(stderr, "Parsing datablock at offset %02x\n", cur_offset );
    // even though this is a while loop, only one iteration will ever
    // affect "list" so we pass the same structure to every call
    cur_offset += parse_hdmi_datablocks_return(&(block[cur_offset]), list);
  }
  
  fprintf(stderr, "Number of native descriptors: %d; flags: %01x\n", num_descrips, block[3] >> 4 );
  
  num_descrips = 5;
  fprintf(stderr, "Trying to decode up to 5 data blocks\n" );
  for( i = 0; i < num_descrips; i++, cur_offset += DETAILED_TIMING_DESCRIPTION_SIZE ) {
    if ( block_type( &(block[cur_offset]) ) == DETAILED_TIMING_BLOCK ) {
      fprintf(stderr, "Parsing descriptor at %02x\n", cur_offset );
      parse_timing_description( &(block[cur_offset]) );
    }
  }

  checksum = 0;
  for( i = 0; i < 0x80; i++ ) {
    checksum += block[i];
  }
  fprintf(stderr, "Block checksum: %02x; ", block[0x7f] );
  if( checksum == 0 ) {
    fprintf(stderr, "checksum passes\n" );
  } else {
    fprintf(stderr, "checksum fails\n" );
  }
}


int
parse_edid( byte* edid )
{
  unsigned i;
  byte* block;
  char* monitor_name = NULL;
  char monitor_alt_name[100];
  byte checksum = 0;
  char *vendor_sign;
  int ret = 0;

  base = edid;

  for( i = 0; i < EDID_LENGTH; i++ )
    checksum += edid[ i ];

  if (  checksum != 0  ) {
      MSG( "EDID checksum failed - data is corrupt. Continuing anyway." );
      ret = 1;
  } else 
      MSG( "EDID checksum passed." );
  

  if ( strncmp( edid+EDID_HEADER, edid_v1_header, EDID_HEADER_END+1 ) )
    {
      MSG( "first bytes don't match EDID version 1 header" );
      MSG( "do not trust output (if any)." );
      ret = 1;
    }

  fprintf(stderr, "\n\t# EDID version %d revision %d\n", (int)edid[EDID_STRUCT_VERSION],(int)edid[EDID_STRUCT_REVISION] );

  vendor_sign = get_vendor_sign( edid + ID_MANUFACTURER_NAME ); 

  fprintf(stderr, "Section \"Monitor\"\n" );

  block = edid + DETAILED_TIMING_DESCRIPTIONS_START;

  for( i = 0; i < NO_DETAILED_TIMING_DESCRIPTIONS; i++,
	 block += DETAILED_TIMING_DESCRIPTION_SIZE )
    {

      if ( block_type( block ) == MONITOR_NAME )
	{
	  monitor_name = get_monitor_name( block );
	  break;
	}
    }

  if (!monitor_name) {
    /* Stupid djgpp hasn't snprintf so we have to hack something together */
    if(strlen(vendor_sign) + 10 > sizeof(monitor_alt_name))
      vendor_sign[3] = 0;
    
    sprintf(monitor_alt_name, "%s:%02x%02x",
	    vendor_sign, edid[ID_MODEL], edid[ID_MODEL+1]) ;
    monitor_name = monitor_alt_name;
  }

  fprintf(stderr, "\tIdentifier \"%s\"\n", monitor_name );
  fprintf(stderr, "\tVendorName \"%s\"\n", vendor_sign );
  fprintf(stderr, "\tModelName \"%s\"\n", monitor_name );

  block = edid + DETAILED_TIMING_DESCRIPTIONS_START;

  for( i = 0; i < NO_DETAILED_TIMING_DESCRIPTIONS; i++,
	 block += DETAILED_TIMING_DESCRIPTION_SIZE )
    {

      if ( block_type( block ) == MONITOR_LIMITS )
	parse_monitor_limits( block );
    }

  parse_dpms_capabilities(edid[DPMS_FLAGS]);

  block = edid + DETAILED_TIMING_DESCRIPTIONS_START;

  for( i = 0; i < NO_DETAILED_TIMING_DESCRIPTIONS; i++,
	 block += DETAILED_TIMING_DESCRIPTION_SIZE )
    {

      if ( block_type( block ) == DETAILED_TIMING_BLOCK ) {
	//	fprintf(stderr, "parsing at %02x\n", (int)(block - edid) );
	parse_timing_description( block );
      }
    }

  fprintf(stderr, "EndSection\n" );

  fprintf(stderr, "\nParsing HDMI section\n" );
  block = edid + HDMI_EXTENSION;
  parse_hdmi_extension(block);

  return ret;
}


int
parse_timing_description( byte* dtd )
{
  int htotal, vtotal;
  htotal = H_ACTIVE + H_BLANKING;
  vtotal = V_ACTIVE + V_BLANKING;

#ifdef DETAILS
  fprintf(stderr, "Offset: %02x\n", (unsigned int) (dtd - base) );
#endif
  fprintf(stderr, "\tMode \t\"%dx%d\"", H_ACTIVE, V_ACTIVE );
  fprintf(stderr, "\t# vfreq %3.3fHz, hfreq %6.3fkHz\n",
	  (double)PIXEL_CLOCK/((double)vtotal*(double)htotal),
	  (double)PIXEL_CLOCK/(double)(htotal*1000));

  fprintf(stderr, "\t\tDotClock\t%f\n", (double)PIXEL_CLOCK/1000000.0 );

  fprintf(stderr, "\t\tHTimings\t%u %u %u %u\n", H_ACTIVE,
	  H_ACTIVE+H_SYNC_OFFSET, 
	  H_ACTIVE+H_SYNC_OFFSET+H_SYNC_WIDTH,
	  htotal );

  fprintf(stderr, "\t\tVTimings\t%u %u %u %u\n", V_ACTIVE,
	  V_ACTIVE+V_SYNC_OFFSET,
	  V_ACTIVE+V_SYNC_OFFSET+V_SYNC_WIDTH,
	  vtotal );

  if ( INTERLACED || (SYNC_TYPE == SYNC_SEPARATE)) {
    fprintf(stderr, "\t\tFlags\t%s\"%sHSync\" \"%sVSync\"\n",
	INTERLACED ? "\"Interlace\" ": "",
	HSYNC_POSITIVE ? "+": "-",
	VSYNC_POSITIVE ? "+": "-");
  }

  fprintf(stderr, "\tEndMode\n" );

  return 0;
}


int
block_type( byte* block )
{

#ifdef DETAILS
  fprintf(stderr, "Offset: %02x\n", (unsigned int) (block - base) );
#endif
  if ( !strncmp( edid_v1_descriptor_flag, block, 2 ) )
    {
      fprintf(stderr,"\t# Block type: 2:%x 3:%x\n", block[2], block[3]);

      /* descriptor */

      if ( block[ 2 ] != 0 )
	return UNKNOWN_DESCRIPTOR;


      return block[ 3 ];
    } else {

      /* detailed timing block */

      return DETAILED_TIMING_BLOCK;
    }
}

char*
get_monitor_name( byte const* block )
{
  static char name[ 13 ];
  unsigned i;
  byte const* ptr = block + DESCRIPTOR_DATA;


  for( i = 0; i < 13; i++, ptr++ )
    {

      if ( *ptr == 0xa )
	{
	  name[ i ] = 0;
	  return name;
	}

      name[ i ] = *ptr;
    }


  return name;
}


char* get_vendor_sign( byte const* block )
{
  static char sign[4];
  unsigned short h;

  /*
     08h	WORD	big-endian manufacturer ID (see #00136)
		    bits 14-10: first letter (01h='A', 02h='B', etc.)
		    bits 9-5: second letter
		    bits 4-0: third letter
  */
  h = COMBINE_HI_8LO(block[0], block[1]);
  sign[0] = ((h>>10) & 0x1f) + 'A' - 1;
  sign[1] = ((h>>5) & 0x1f) + 'A' - 1;
  sign[2] = (h & 0x1f) + 'A' - 1;
  sign[3] = 0;
  return sign;
}

int
parse_monitor_limits( byte* block )
{
#ifdef DETAILS
  fprintf(stderr, "Offset: %02x\n", (unsigned int) (block - base) );
#endif
  fprintf(stderr, "\tHorizSync %u-%u\n", H_MIN_RATE, H_MAX_RATE );
  fprintf(stderr, "\tVertRefresh %u-%u\n", V_MIN_RATE, V_MAX_RATE );
  if ( MAX_PIXEL_CLOCK == 10*0xff )
    fprintf(stderr, "\t# Max dot clock not given\n" );
  else
    fprintf(stderr, "\t# Max dot clock (video bandwidth) %u MHz\n", (int)MAX_PIXEL_CLOCK );

  if ( GTF_SUPPORT )
    {
      fprintf(stderr, "\t# EDID version 3 GTF given: contact author\n" );
    }
  
  return 0;
}

int
parse_dpms_capabilities(byte flags)
{
  fprintf(stderr,"\t# DPMS capabilities: Active off:%s  Suspend:%s  Standby:%s\n\n",
    (flags & DPMS_ACTIVE_OFF) ? "yes" : "no",
    (flags & DPMS_SUSPEND)    ? "yes" : "no",
    (flags & DPMS_STANDBY)    ? "yes" : "no");
  return 0;
}
    
