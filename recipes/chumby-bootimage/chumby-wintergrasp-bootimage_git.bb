SECTION = "bootloaders"
PRIORITY = "optional"
LICENSE = "GPLv2"

DEPENDS = "chumby-blobs elftosb-native u-boot-wintergrasp"
DEPENDS_${PN} = "chumby-blobs elftosb-native u-boot-wintergrasp"
PROVIDES = "virtual/bootloader virtual/chumby-bootimage"
RPROVIDES = "virtual/bootloader virtual/chumby-bootimage"
COMPATIBLE_MACHINE = "chumby-wintergrasp"
PR = "r1"

SRC_URI = "file://sbmagic \
           file://uboot_ivt.bd \
"

FILES_${PN} = "/boot"

do_install () {
    install -d ${D}/boot
    install -m 0755 ${WORKDIR}/sbmagic ${D}/boot
    install -m 0755 ${WORKDIR}/uboot_ivt.bd ${D}/boot
}

do_deploy () {
    install -d ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/sbmagic ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/uboot_ivt.bd ${DEPLOY_DIR_IMAGE}

    elftosb -p ${DEPLOY_DIR_IMAGE} -z -f imx28 -c ${DEPLOY_DIR_IMAGE}/uboot_ivt.bd -o ${DEPLOY_DIR_IMAGE}/imx28_ivt_uboot.sb

    rm -f ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    touch ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    dd if=${DEPLOY_DIR_IMAGE}/sbmagic of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin bs=512 conv=sync
    dd if=${DEPLOY_DIR_IMAGE}/imx28_ivt_uboot.sb of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin bs=512 seek=4 conv=sync
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
}

addtask deploy before do_build after do_compile
