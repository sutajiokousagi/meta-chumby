#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

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

int main(int argc, char **argv)
{
  int file_desc, retval;
  char code;
  
  if(argc < 2) {
    fprintf(stderr, "Usage: %s <op>\n", argv[0]);
    return 1;
  }
  code = *(argv[1]);
  
  file_desc = open("/dev/fpga", 0);
  if (file_desc < 0) {
    printf("Can't open device file: %s\n", "/dev/fpga");
    exit(-1);
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
  case 'd':
    ioctl_done(file_desc, &retval);
    printf( "Done pin state: %d\n", retval );;
    break;
  case 'i':
    ioctl_init(file_desc, &retval);
    printf( "Init pin state: %d\n", retval );;
    break;
  default:
    printf( "Command character %c not recognized.\n", code );
  }

  close(file_desc);
  
  return 0;
}
