#@TYPE: Machine
#@Name: Silvermoon Armada 100 Development Platform
#@DESCRIPTION: Machine configuration for chumby Silvermoon board

require conf/machine/include/tune-xscale.inc
require conf/machine/include/chumby.inc

TARGET_ARCH = "arm"

CNPLATFORM = "silvermoon"

MACHINE_GUI_CLASS = "bigscreen"
MACHINE_CLASS = "bigscreen"
MACHINE_DISPLAY_WIDTH_PIXELS = "1920"
MACHINE_DISPLAY_HEIGHT_PIXELS = "1080"
MACHINE_FEATURES = "kernel26 usbhost touchscreen ext2 ext3 vfat keyboard alsa iwmmxt wifi"
MACHINE_TASK_PROVIDER 		= "task-base"
MACHINE_ESSENTIAL_EXTRA_RDEPENDS = "u-boot-silvermoon"
MACHINE_HAS_CRYPTOPROCESSOR = "1"

PREFERRED_PROVIDER_virtual/bootloader = "u-boot-silvermoon"
PREFERRED_PROVIDER_chumby-blobs = "chumby-blobs-silvermoon"

UBOOT_MACHINE = "aspenite_config"
BOOTIMAGE_NAME = "u-boot-${MACHINE}.bin"


MACHINE_EXTRA_RDEPENDS += " \
    chumby-udev-rules \
"

MACHINE_EXTRA_RRECOMMENDS = " kernel-modules"

INITRAMFS_IMAGE = ""

module_autoload_silvermoon-tsb = "silvermoon-tsb"

# used by sysvinit_2
SERIAL_CONSOLE = "115200 ttyS0"

# Match current Silvermoon devices
ROOT_FLASH_SIZE = "180"
IMAGE_ROOTFS_SIZE = "180000"


chumby_make_rom () {
    install -d ${DEPLOY_DIR_IMAGE}

    # Create the config block header
    config_util --cmd=create \
        --mbr=/dev/zero \
        --configname=${CNPLATFORM} \
        --build_ver=${CHUMBY_BUILD} --force --pad \
        --blockdef=${DEPLOY_DIR_IMAGE}/u-boot-${MACHINE}.bin,1507328,u-bt,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin,3932160,krnA,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin,3932160,krnB,1,0,0,0 \
        --blockdef=/dev/null,16384,cpid,1,0,0,0 \
        --blockdef=/dev/null,1024,dcid,1,0,0,0 \
        > ${DEPLOY_DIR_IMAGE}/config_block.bin


    # Populate the config block
    rm -f ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    touch ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=0 count=96 if=${DEPLOY_DIR_IMAGE}/obm.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=96 count=32 if=${DEPLOY_DIR_IMAGE}/config_block.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=128 count=2944 if=${DEPLOY_DIR_IMAGE}/u-boot-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=3072 count=7680 if=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=10752 count=7680 if=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin

    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin

    # Make full boot ROM
    rm -f ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img.gz
    makedisk -o ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img \
             -a 16M:0x53:${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin \
             -a ${ROOTFS_SIZE}k:0x83:${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.ext3
    gzip ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img
}


chumby_make_rom_chumby-silvermoon-netv () {
    install -d ${DEPLOY_DIR_IMAGE}

    # Create the config block header
    config_util --cmd=create \
        --mbr=/dev/zero \
        --configname=${CNPLATFORM} \
        --build_ver=${CHUMBY_BUILD} --force --pad \
        --blockdef=${DEPLOY_DIR_IMAGE}/u-boot-${MACHINE}.bin,1507328,u-bt,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin,3932160,krnA,1,0,0,0 \
        --blockdef=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin,3932160,krnB,1,0,0,0 \
        --blockdef=/dev/null,16384,cpid,1,0,0,0 \
        --blockdef=/dev/null,1024,dcid,1,0,0,0 \
        --blockdef=/dev/null,340992,720p,1,0,0,0 \
        --blockdef=/dev/null,1843200,logo,1,0,0,0 \
        > ${DEPLOY_DIR_IMAGE}/config_block.bin


    # Populate the config block
    rm -f ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    touch ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=0 count=96 if=${DEPLOY_DIR_IMAGE}/obm.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=96 count=32 if=${DEPLOY_DIR_IMAGE}/config_block.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=128 count=2944 if=${DEPLOY_DIR_IMAGE}/u-boot-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=3072 count=7680 if=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=18466  count=666 if=${DEPLOY_DIR_IMAGE}/hdmi_720p.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=19132 count=3600 if=${DEPLOY_DIR_IMAGE}/logo-preparing.raw.gz

    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin



    # Make full boot ROM
    rm -f ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img.gz
    makedisk -o ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img \
             -a 16M:0x53:${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin \
             -a ${ROOTFS_SIZE}k:0x83:${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.ext3
    gzip ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img
}
