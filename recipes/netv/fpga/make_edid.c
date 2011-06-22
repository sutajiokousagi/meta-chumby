#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//// this program is intended to be run on the build host
//// just run gcc make_edid.c -o make_edid

typedef unsigned char byte;
/* byte must be 8 bits */

/* int must be at least 16 bits */

/* long must be at least 32 bits */

typedef unsigned short u16;
typedef unsigned int u32;

struct standardTimings {
  byte horizPixels;
  byte AR_refresh;
};

struct saneDescriptor {
  int pixelclock;
  int hActive;
  int hBlank;
  int vActive;
  int vBlank;
  int hFP;
  int hBP;
  int vFP;
  int vBP;
  int hSync;
  int vSync;
  int hSizemm;
  int vSizemm;
  int hborder;
  int vborder;
  
  int syncCode; // sorry, too lazy to implement this in fields
};

struct detailDescriptors {
  byte pixelclockLSB;    // byte 0   // bytes, not u16, to allow for unaligned structure start
  byte pixelclockMSB;    // byte 1
  byte hActiveLSB;   // byte 2
  byte hBlankLSB;    // byte 3
  byte hActiveBlankMSB;  // byte 4
  byte vActiveLSB;   // byte 5
  byte vBlankLSB;    // byte 6
  byte vActiveBlankMSB;  // byte 7
  byte hFrontPorchLSB;  // byte 8
  byte hSyncWidthLSB;   // byte 9
  byte vFPvSyncWidthLSB; // byte 10
  byte miscMSB; // byte 11
  byte hVidSizeLSB; // byte 12
  byte vVidSizeLSB; // byte 13
  byte hvSizeMSB; // byte 14
  byte hborder; // byte 15
  byte vborder; // byte 16
  byte syncdef; // byte 17
};

struct displayName {
  char tag[5]; // initialize to 00 00 00 fc 00
  char name[13];
};

struct displayDataString {
  char tag[5]; // initialize to 00 00 00 fe 00
  char name[13];
};

struct displayLimits {
  char tag[4]; // initialize to 00 00 00 fd
  byte rangeLimits;
  byte minVert;
  byte maxVert;
  byte minHoriz;
  byte maxHoriz;
  byte maxPclk;
  byte timingSupport;
  byte unused[7]; // sequence is 0a 20 20 20 20 20 20 typically
};

struct edid_ {
  byte header[8];
  u16 IDManufacturerName;
  u16 IDProductCode;
  u32 IDSerialNumber;
  byte WeekOfManufacture;
  byte ModelYear;

  byte VersionNumber;
  byte RevisionNumber;

  byte VideoInputDefinition;
  byte HorizontalSize;
  byte VerticalSize;
  byte DisplayGamma;
  byte FeatureSupport;

  byte ChromaRGLow;
  byte ChromaBWLow;
  byte RedXHigh;
  byte RedYHigh;
  byte GreenXHigh;
  byte GreenYHigh;
  byte BlueXHigh;
  byte BlueYHigh;
  byte WhiteXHigh;
  byte WhiteYHigh;

  byte TimingsI;
  byte TimingsII;
  byte TimingsReserved;

  // standard timings 1-8
  struct standardTimings timings[8];

  // detail timings
  struct detailDescriptors detail;

  struct displayDataString dataString;
  struct displayName name;
  struct displayLimits limits;
  
  byte extensionBlockCount;
  byte checksum;
};

struct videoDataBlock {
  byte videoTagCode; // 0x41
  byte shortDescriptor1; // just one descriptor supported here, should be "0x84"
};

struct audioDataBlock {
  byte audioTagCode; // 0x23
  byte audio[3]; // 09 07 07  fake data: 2 channels, supports 48/44.1/32 and 24-16 bits
};
struct speakerDataBlock {
  byte speakerTagCode; // 0x83
  byte speaker[3]; // 01 00 00 : just fl/fr speakers available
};

struct hdmiBlock {
  byte vendorTagCode; // 0x66
  byte hdmi[6]; // 03 0c 00  10 00  00
};

struct hdmi_ {
  byte tag;  // value: 02
  byte revision; // value: 03
  byte offset; // where detail timing descriptors begin
  byte numDescrips; 
  
  struct videoDataBlock video;

  struct audioDataBlock audio;

  struct speakerDataBlock speaker;

  struct hdmiBlock hdmi;
  // audio data block collection
  // speaker allocation data block
  // vendor-specific datablock: This has the mandatory HDMI vendor block

  struct detailDescriptors detail; // as many of these as you want

  // pad to zero and add checksum
};

byte calcsum(unsigned char *block) {
  int i;
  byte sum = 0;
  for( i = 0; i < 127; i++ ) {
    sum += block[i];
  }
  return( 256 - sum );
}

void make_insane(struct saneDescriptor *sane, struct detailDescriptors *insane) {
  u16 pixclk;
  
  pixclk = (sane->pixelclock / 10000) & 0xFFFF;
  insane->pixelclockLSB = pixclk & 0xFF;
  insane->pixelclockMSB = (pixclk >> 8) & 0xFF;
  insane->hActiveLSB = sane->hActive & 0xFF;
  insane->hBlankLSB = sane->hBlank & 0xFF;
  insane->hActiveBlankMSB = (((sane->hActive >> 8) & 0xF) << 4) | ((sane->hBlank >> 8) & 0xF);
  insane->vActiveLSB = sane->vActive & 0xFF;
  insane->vBlankLSB = sane->vBlank & 0xFF;
  insane->vActiveBlankMSB = (((sane->vActive >> 8) & 0xF) << 4) | ((sane->vBlank >> 8) & 0xF);
  insane->hFrontPorchLSB = sane->hFP & 0xFF;
  insane->hSyncWidthLSB = sane->hSync & 0xFF;
  insane->vFPvSyncWidthLSB = ((sane->vFP & 0xF) << 4) | (sane->vSync & 0xF);
  insane->miscMSB = (((sane->hFP >> 8) & 0x3) << 6) |
    (((sane->hSync >> 8) & 0x3) << 4 ) |
    (((sane->vFP >> 4) & 0x3) << 2 ) |
    (((sane->vSync >> 4) & 0x3) << 0 );
  insane->hVidSizeLSB = sane->hSizemm & 0xFF;
  insane->vVidSizeLSB = sane->vSizemm & 0xFF;
  insane->hvSizeMSB = (((sane->hSizemm >> 8) & 0xF) << 4) | (((sane->vSizemm >> 8) & 0xF) << 0);
  insane->hborder = sane->hborder;
  insane->vborder = sane->vborder;
  insane->syncdef = sane->syncCode;
  
}

#define MODE720P 0
#define MODE1080P24 1
#define MODE1080P30 2
#define MODE1080P25 3

void make_edid(int mode) {
  struct saneDescriptor sane;
  unsigned char *bytes;
  int i;
  struct edid_ edid;
  
  edid.header[0] = 0;
  edid.header[1] = 0xff;
  edid.header[2] = 0xff;
  edid.header[3] = 0xff;
  edid.header[4] = 0xff;
  edid.header[5] = 0xff;
  edid.header[6] = 0xff;
  edid.header[7] = 0;

  edid.IDManufacturerName = 0x0d0d; //chm = 0x3 0x8 0xd -> 0 000 1101 0000 1101
  edid.IDProductCode = 0x654e; // Ne
  edid.IDSerialNumber = 0x1;
  edid.WeekOfManufacture = 0xff;
  edid.ModelYear = 0x15; // 2011

  edid.VersionNumber = 0x1;
  edid.RevisionNumber = 0x3;

  edid.VideoInputDefinition = 0x80;
  edid.HorizontalSize = 16; // bravia does this hack
  edid.VerticalSize = 9;
  edid.DisplayGamma = 0x78; // 2.2 gamma
  edid.FeatureSupport = 0x0a; // no low power modes, RGB & YCrCb, preferred mode is native
  // may want to tweak above to 0x02 if there are compatibility issues.
  
  edid.ChromaRGLow = 0xd; // copied from sony bravia TV
  edid.ChromaBWLow = 0xc9;
  edid.RedXHigh = 0xa0;
  edid.RedYHigh = 0x57;
  edid.GreenXHigh = 0x47;
  edid.GreenYHigh = 0x98;
  edid.BlueXHigh = 0x27;
  edid.BlueYHigh = 0x12;
  edid.WhiteXHigh = 0x48;
  edid.WhiteYHigh = 0x4c;
 
  edid.TimingsI = 0; // let's see if we can get off with supporting no standard timings
  edid.TimingsII = 0;
  edid.TimingsReserved = 0;
  
  for( i = 0; i < 8; i++ ) { // all bogus timings
    if( mode == MODE720P ) {
      edid.timings[i].horizPixels = 0x81; // 1280
      edid.timings[i].AR_refresh = 0xc0; // 16:9, 60 hz
    } else if( (mode == MODE1080P24) || (mode == MODE1080P30 || (mode == MODE1080P25)) ) {
      edid.timings[i].horizPixels = 1; // 1920 @ 24 hz not representable
      edid.timings[i].AR_refresh = 1;
    }
  }

  if( mode == MODE720P ) {
    sane.pixelclock = 74250000;
    sane.hActive = 1280;
    sane.hBlank = 370;
    sane.vActive = 720;
    sane.vBlank = 30;
    sane.hFP = 110;
    sane.hBP = 220;
    sane.vFP = 5;
    sane.vBP = 20;
    sane.hSync = 40;
    sane.vSync = 5;
    sane.hSizemm = 160;
    sane.vSizemm = 90;
    sane.hborder = 0;
    sane.vborder = 0;
    sane.syncCode = 0x1e; // digital separate sync, vertical sync is positive; hsync is positive
  } else if( mode == MODE1080P24 ) {
    sane.pixelclock = 74250000;
    sane.hActive = 1920;
    sane.hBlank = 830;
    sane.vActive = 1080;
    sane.vBlank = 45;
    sane.hFP = 638;
    sane.hBP = 140;
    sane.vFP = 4;
    sane.vBP = 36;
    sane.hSync = 44;
    sane.vSync = 5;
    sane.hSizemm = 160;
    sane.vSizemm = 90;
    sane.hborder = 0;
    sane.vborder = 0;
    sane.syncCode = 0x1e; // digital separate sync, vertical sync is positive; hsync is positive
  } else if( mode == MODE1080P30 ) {
    sane.pixelclock = 74176000;
    sane.hActive = 1920;
    sane.hBlank = 280;
    sane.vActive = 1080;
    sane.vBlank = 45;
    sane.hFP = 88;
    sane.hBP = 148;
    sane.vFP = 4;
    sane.vBP = 36;
    sane.hSync = 44;
    sane.vSync = 5;
    sane.hSizemm = 160;
    sane.vSizemm = 90;
    sane.hborder = 0;
    sane.vborder = 0;
    sane.syncCode = 0x1e; // digital separate sync, vertical sync is positive; hsync is positive
  } else if( mode == MODE1080P25 ) {
    sane.pixelclock = 74250000;
    sane.hActive = 1920;
    sane.hBlank = 720;
    sane.vActive = 1080;
    sane.vBlank = 45;
    sane.hFP = 528;
    sane.hBP = 148;
    sane.vFP = 4;
    sane.vBP = 36;
    sane.hSync = 44;
    sane.vSync = 5;
    sane.hSizemm = 160;
    sane.vSizemm = 90;
    sane.hborder = 0;
    sane.vborder = 0;
    sane.syncCode = 0x1e; // digital separate sync, vertical sync is positive; hsync is positive
  }

  make_insane(&sane, &(edid.detail));

  edid.dataString.tag[0] = 0x0;
  edid.dataString.tag[1] = 0x0;
  edid.dataString.tag[2] = 0x0;
  edid.dataString.tag[3] = 0xfe;
  edid.dataString.tag[4] = 0x0;
  strncpy(edid.dataString.name, "chumby NeTV  ", 13);

  edid.name.tag[0] = 0;
  edid.name.tag[1] = 0;
  edid.name.tag[2] = 0;
  edid.name.tag[3] = 0xfc;
  edid.name.tag[4] = 0;

  strncpy(edid.name.name, "chumby NeTV  ", 13);

  edid.limits.tag[0] = 0x0;
  edid.limits.tag[1] = 0x0;
  edid.limits.tag[2] = 0x0;
  edid.limits.tag[3] = 0xfd;

  edid.limits.rangeLimits = 0;
  if( mode == MODE720P ) {
    edid.limits.minVert = 0x17;
    edid.limits.maxVert = 60; // 60 hz
    edid.limits.minHoriz = 37; // constrain to 720p
    edid.limits.maxHoriz = 46;
    edid.limits.maxPclk = 8; // 80 MHz max
  } else if ((mode == MODE1080P24) || (mode == MODE1080P30) || (mode == MODE1080P25) ) {
    edid.limits.minVert = 0x17;
    edid.limits.maxVert = 35; 
    edid.limits.minHoriz = 26;
    edid.limits.maxHoriz = 35;
    edid.limits.maxPclk = 8; // 80 MHz max
  } 
  edid.limits.timingSupport = 0;
  edid.limits.unused[0] = 0xa;
  edid.limits.unused[1] = 0x20;
  edid.limits.unused[2] = 0x20;
  edid.limits.unused[3] = 0x20;
  edid.limits.unused[4] = 0x20;
  edid.limits.unused[5] = 0x20;
  edid.limits.unused[6] = 0x20;

  edid.extensionBlockCount = 1; // for the HDMI block

  edid.checksum = calcsum((unsigned char *) &edid);

  bytes = (char *) &edid;
  for( i = 0; i < 128; i++ ) {
    if( ((i % 16) == 0) ) {
      printf( "\n" );
    }
    printf( "%02x ", bytes[i] );
  }
  printf( "\n" );
}

void make_hdmi(int mode) {
  struct saneDescriptor sane;
  struct hdmi_ hdmi;
  int i;
  unsigned char *bytes;

  hdmi.tag = 0x2;
  hdmi.revision = 0x3;
  hdmi.numDescrips = 0x41; // sink deice supports basic audio, 1 native DTD
  hdmi.offset = 4 + sizeof(struct videoDataBlock) +
    sizeof(struct audioDataBlock) + 
    sizeof(struct speakerDataBlock) +
    sizeof(struct hdmiBlock);
  
  hdmi.video.videoTagCode = 0x41;
  if( mode == MODE720P ) {
    hdmi.video.shortDescriptor1 = 0x84; // native (0x80) | mode 4
  } else if( mode == MODE1080P24) {
    hdmi.video.shortDescriptor1 = 0xA0; // native (0x80) | mode 32
  } else if( mode == MODE1080P30) {
    hdmi.video.shortDescriptor1 = 0xA2; // native (0x80) | mode 34
  } else if( mode == MODE1080P25) {
    hdmi.video.shortDescriptor1 = 0xA1; // native (0x80) | mode 33
  }

  hdmi.audio.audioTagCode = 0x23;
  hdmi.audio.audio[0] = 0x09;
  hdmi.audio.audio[1] = 0x07;
  hdmi.audio.audio[2] = 0x07;

  hdmi.speaker.speakerTagCode = 0x83;
  hdmi.speaker.speaker[0] = 0x1;
  hdmi.speaker.speaker[1] = 0x0;
  hdmi.speaker.speaker[2] = 0x0;
  
  hdmi.hdmi.vendorTagCode = 0x66;
  hdmi.hdmi.hdmi[0] = 0x3;
  hdmi.hdmi.hdmi[1] = 0xc;
  hdmi.hdmi.hdmi[2] = 0x0;
  hdmi.hdmi.hdmi[3] = 0x10;
  hdmi.hdmi.hdmi[4] = 0x0;
  hdmi.hdmi.hdmi[5] = 0x0;

  if( mode == MODE720P ) {
    sane.pixelclock = 74250000;
    sane.hActive = 1280;
    sane.hBlank = 370;
    sane.vActive = 720;
    sane.vBlank = 30;
    sane.hFP = 110;
    sane.hBP = 220;
    sane.vFP = 5;
    sane.vBP = 20;
    sane.hSync = 40;
    sane.vSync = 5;
    sane.hSizemm = 160;
    sane.vSizemm = 90;
    sane.hborder = 0;
    sane.vborder = 0;
    sane.syncCode = 0x1e; // digital separate sync, vertical sync is positive; hsync is positive
  } else if( mode == MODE1080P24 ) {
    sane.pixelclock = 74250000;
    sane.hActive = 1920;
    sane.hBlank = 830;
    sane.vActive = 1080;
    sane.vBlank = 45;
    sane.hFP = 638;
    sane.hBP = 140;
    sane.vFP = 4;
    sane.vBP = 36;
    sane.hSync = 44;
    sane.vSync = 5;
    sane.hSizemm = 160;
    sane.vSizemm = 90;
    sane.hborder = 0;
    sane.vborder = 0;
    sane.syncCode = 0x1e; // digital separate sync, vertical sync is positive; hsync is positive
  } else if( mode == MODE1080P30 ) {
    sane.pixelclock = 74176000;
    sane.hActive = 1920;
    sane.hBlank = 280;
    sane.vActive = 1080;
    sane.vBlank = 45;
    sane.hFP = 88;
    sane.hBP = 148;
    sane.vFP = 4;
    sane.vBP = 36;
    sane.hSync = 44;
    sane.vSync = 5;
    sane.hSizemm = 160;
    sane.vSizemm = 90;
    sane.hborder = 0;
    sane.vborder = 0;
    sane.syncCode = 0x1e; // digital separate sync, vertical sync is positive; hsync is positive
  } else if( mode == MODE1080P25 ) {
    sane.pixelclock = 74250000;
    sane.hActive = 1920;
    sane.hBlank = 720;
    sane.vActive = 1080;
    sane.vBlank = 45;
    sane.hFP = 528;
    sane.hBP = 148;
    sane.vFP = 4;
    sane.vBP = 36;
    sane.hSync = 44;
    sane.vSync = 5;
    sane.hSizemm = 160;
    sane.vSizemm = 90;
    sane.hborder = 0;
    sane.vborder = 0;
    sane.syncCode = 0x1e; // digital separate sync, vertical sync is positive; hsync is positive
  }

  make_insane(&sane, &(hdmi.detail));

  bytes = (char *) &hdmi;
  for( i = 0; i < sizeof(hdmi); i++ ) {
    if( ((i % 16) == 0) && (i != 0) ) {
      printf( "\n" );
    }
    printf( "%02x ", bytes[i] );
  }

  for( ; i < 127; i++ ) {
    if( (i % 16) == 0 ) {
      printf( "\n" );
    }
    printf( "%02x ", 0 );
  }
  bytes = calloc(128, sizeof(unsigned char));
  if( sizeof(hdmi) > 128 ) {
    printf( "why is the hdmi record so large?\n" );
    return; // silently abort
  }
  if( bytes == NULL ) {
    printf( "can't allocate 128 bytes. Man, you are screwed!\n" );
    return;
  }
  
  memcpy( bytes, &hdmi, sizeof(hdmi) );
  printf( "%02x ", calcsum(bytes) );
  
  printf( "\n" );
}

int main( int argc, char** argv ) {
  int mode = MODE720P;
  int mode_arg;

  if( argc != 2 ) {
    printf( "%s: make_edid [mode]\n", argv[0] );
    return 0;
  }
  
  mode_arg = strtoul(argv[1], NULL, 0);
  switch( mode_arg ) {
  case 4:
    mode = MODE720P;
    break;
  case 32:
    mode = MODE1080P24;
    break;
  case 34:
    mode = MODE1080P30;
    break;
  case 33:
    mode = MODE1080P25;
    break;
  default:
    printf( "CEA mode number %d not recognized, aborting\n", mode_arg );
    printf( "Recognized modes are 4 (720p/60), 32 (1080p/24), 34 (1080p/30), and 33 (1080p/25)\n" );
    return 0;
  }
  
    make_edid(mode);
    make_hdmi(mode);
    return 0;
}
