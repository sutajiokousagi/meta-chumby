#@TYPE: Machine
#@Name: Silvermoon Armada 100 Development Platform
#@DESCRIPTION: Machine configuration for chumby Silvermoon board

CNPLATFORM = "${@bb.data.getVar('MACHINE', d, 1).replace('chumby-', '')}"

IMAGE_FSTYPES = "ext3"
IMAGE_CMD_ext3 = "genext2fs -N 50000 -b ${ROOTFS_SIZE} -d ${IMAGE_ROOTFS} ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext3 ${EXTRA_IMAGECMD}; tune2fs -j ${DEPLOY_DIR_IMAGE}/${IMAGE_NAME}.rootfs.ext3"
EXTRA_IMAGEDEPENDS += "makedisk-native config-util-native virtual/kernel virtual/bootloader chumby-blobs virtual/chumby-bootimage"
MACHINE_POSTPROCESS_COMMAND = "chumby_make_rom"

chumby_make_rom () {

    # Make full boot ROM
    install -d ${DEPLOY_DIR_IMAGE}
    touch ${DEPLOY_DIR_IMAGE}/dummy
    makedisk -o ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img \
             -a ${@(${IMAGE_FATFS_SIZE} / 1024)}k:0x0b:usr.fat \
             -a 16M:0x53:${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin \
             -a 0M:0x05:dummy \
             -a 0M:0x00:dummy \
             -a ${ROOTFS_SIZE}k:0x83:${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.ext3 \
             -a ${ROOTFS_SIZE}k:0x83:${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}.ext3
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/rom-${MACHINE}-${IMAGE_BASENAME}.img
}
