SECTION = "bootloaders"
PRIORITY = "optional"
LICENSE = "GPLv2"

DEPENDS = "u-boot-wintergrasp"
DEPENDS_${PN} = "u-boot-wintergrasp"
PROVIDES = "virtual/bootloader virtual/chumby-bootimage"
RPROVIDES = "virtual/bootloader virtual/chumby-bootimage"
COMPATIBLE_MACHINE = "chumby-wintergrasp"
PR = "r3"

SRC_URI = ""

do_deploy () {
    rm -f ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    touch ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    dd if=${DEPLOY_DIR_IMAGE}/sbmagic of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin bs=512 conv=sync
    dd if=${DEPLOY_DIR_IMAGE}/imx28_ivt_uboot.sb of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin bs=512 seek=2049 conv=sync
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
}

addtask deploy before do_build after do_compile
