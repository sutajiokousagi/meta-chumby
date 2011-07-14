inherit chumbysg-git chumby-info

require u-boot.inc

DEPENDS = "chumby-blobs elftosb-native"
DEPENDS_${PN} = "chumby-blobs elftosb-native"
PROVIDES = "virtual/bootloader virtual/chumby-bootimage"
RPROVIDES = "virtual/bootloader virtual/chumby-bootimage"
COMPATIBLE_MACHINE = "chumby-wintergrasp"

PR ="r9"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/u-boot-2009.08-wintergrasp${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://mk_hdr.sh \
           file://uboot_ivt.bd \
           file://4_1.bin \
           file://4_2.bin \
           file://000-board_setup.patch \
           file://001-compiler_fix.patch \
           file://002-config.patch \
           file://003-setup_eth_addr.patch \
           file://004-fix_mmc_detect_wp.patch \
           file://005-nand_access.patch \
           file://006-ext2_fix.patch \
           file://007-unified_uboot_mmc_nand_ram.patch \
           file://008-load_image.patch \
           file://009-logo_during_boot.patch \
"

SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

FILES_${PN} = "/boot"

do_deploy () {
    install -d ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/mk_hdr.sh ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/uboot_ivt.bd ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/4_1.bin ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/4_2.bin ${DEPLOY_DIR_IMAGE}
    install ${S}/${UBOOT_BINARY} ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}

    cd ${DEPLOY_DIR_IMAGE}
    rm -f ${UBOOT_SYMLINK}
    ln -sf ${UBOOT_IMAGE} ${UBOOT_SYMLINK}
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK}

    elftosb -p ${DEPLOY_DIR_IMAGE} -z -f imx28 -c ${DEPLOY_DIR_IMAGE}/uboot_ivt.bd -o ${DEPLOY_DIR_IMAGE}/imx28_ivt_uboot.sb

    ./mk_hdr.sh ${@(2052 + ((${IMAGE_FATFS_SIZE} + ${IMAGE_CONFIG_BLOCK_SIZE}) / 512))} 1 > sbmagic

    rm -f ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    touch ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    dd if=${DEPLOY_DIR_IMAGE}/sbmagic of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin bs=512 conv=sync
    dd if=${DEPLOY_DIR_IMAGE}/imx28_ivt_uboot.sb of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin bs=512 seek=2049 conv=sync
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
}

addtask deploy after do_install
