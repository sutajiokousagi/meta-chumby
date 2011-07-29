#@TYPE: Machine
#@Name: chumby Falconwing i.MX233-based boards
#@DESCRIPTION: Machine configuration for chumby Falconwing platform

TARGET_ARCH = "arm"

MACHINE_GUI_CLASS = "smallscreen"
MACHINE_DISPLAY_WIDTH_PIXELS = "320"
MACHINE_DISPLAY_HEIGHT_PIXELS = "240"

PREFERRED_PROVIDER_virtual/kernel = "linux-falconwing"
PREFERRED_PROVIDER_virtual/bootloader = "chumby-bootshell"

IMAGE_DEVICE_TABLES = "files/device_table-minimal-chumby.txt"

require conf/machine/include/tune-arm926ejs.inc

MACHINE_EXTRA_RDEPENDS += " \
    chumby-udev-rules \
    rt73-firmware \
    chumby-startup-hooks \
    chumby-blobs-falconwing \
"

# used by sysvinit_2
SERIAL_CONSOLE = "115200 ttyAM0"

ROOT_FLASH_SIZE = "128"
MACHINE_FEATURES = "kernel26 usbhost touchscreen ext2 ext3 vfat keyboard alsa"

# 128 megabytes, to match released version
IMAGE_ROOTFS_SIZE = "128000"

ANGSTROM_MESA_VERSION = "7.9"

chumby_make_rom() {
    config_util --cmd=create \
        --mbr=/dev/zero \
        --configname=falconwing \
        --build_ver=1000 --force --pad \
        --blockdef=${DEPLOY_DIR_IMAGE}/chumby_shell.bin,215040,boot,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/4_1.bin,153600,img1,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/4_2.bin,153600,img2,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/5_1.bin,153600,img3,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/5_2.bin,153600,img4,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/5_3.bin,153600,img5,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin,4194304,krnA,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin,4194304,krnB,1,0,0,0 \
        > ${DEPLOY_DIR_IMAGE}/config_block.bin


    rm -f ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    touch ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=4 count=64 if=${DEPLOY_DIR_IMAGE}/bootstream-chumby.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=96 count=32 if=${DEPLOY_DIR_IMAGE}/config_block.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=128 count=420 if=${DEPLOY_DIR_IMAGE}/chumby_shell.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=548 count=300 if=${DEPLOY_DIR_IMAGE}/4_1.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=848 count=300 if=${DEPLOY_DIR_IMAGE}/4_2.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=1148 count=300 if=${DEPLOY_DIR_IMAGE}/5_1.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=1448 count=300 if=${DEPLOY_DIR_IMAGE}/5_2.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=1748 count=300 if=${DEPLOY_DIR_IMAGE}/5_3.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=2048 count=8192 if=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=10240 count=8192 if=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin

    # Make full boot ROM
    rm -f ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img.gz
    install -d ${DEPLOY_DIR_IMAGE}
    makedisk -o ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img \
             -a 16M:0x53:${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin \
             -a ${ROOTFS_SIZE}k:0x83:${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.ext3
    gzip ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img
}
