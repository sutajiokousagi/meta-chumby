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
 * Fpga Lowlevel Control Abstraction
 */

#ifndef _LINUX_FPGA_H
#define _LINUX_FPGA_H

#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/notifier.h>

/*
 * Macros to help debugging
 */

#undef PDEBUG             /* undef it, just in case */
#ifdef FPGA_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "fpga: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#ifndef FPGA_MAJOR
#define FPGA_MAJOR 0   /* dynamic major by default */
#endif

#ifndef FPGA_NR_DEVS
#define FPGA_NR_DEVS 1    /* fpga0 only */
#endif

//// put local data to be remembered in the fpga device here
struct fpga_dev {
  int test; // just a test field
	struct semaphore sem;     /* mutual exclusion semaphore     */
	struct cdev cdev;	  /* Char device structure		*/
};

/*
 * Split minors in two parts
 */
#define TYPE(minor)	(((minor) >> 4) & 0xf)	/* high nibble */
#define NUM(minor)	((minor) & 0xf)		/* low  nibble */


/*
 * The different configurable parameters
 */
extern int fpga_major;     /* main.c */
extern int fpga_nr_devs;
extern int fpga_quantum;
extern int fpga_qset;


/*
 * Prototypes for shared functions
 */

ssize_t fpga_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos);

int     fpga_ioctl(struct inode *inode, struct file *filp,
                    unsigned int cmd, unsigned long arg);


/*
 * Ioctl definitions
 */

/* Use 'c' as magic number */
#define FPGA_IOC_MAGIC  'c'

#define FPGA_IOCWTEST    _IOW(FPGA_IOC_MAGIC,  1, int)
#define FPGA_IOCRTEST    _IOR(FPGA_IOC_MAGIC,  2, int)
#define FPGA_IOCRESET    _IOW(FPGA_IOC_MAGIC,  3, int)
#define FPGA_IOCLED0     _IOW(FPGA_IOC_MAGIC,  4, int)
#define FPGA_IOCLED1     _IOW(FPGA_IOC_MAGIC,  5, int)
#define FPGA_IOCDONE     _IOR(FPGA_IOC_MAGIC,  6, int)
#define FPGA_IOCINIT     _IOR(FPGA_IOC_MAGIC,  7, int)

#define FPGA_IOC_MAXNR 7

#endif
