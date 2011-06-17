inherit chumbysg-git chumby-info

require u-boot.inc

DEPENDS = "chumby-blobs elftosb-native"
DEPENDS_${PN} = "chumby-blobs elftosb-native"
PROVIDES = ""
RPROVIDES = ""

PR ="r4b"

SRC_URI = "${CHUMBYSG_GIT_HOST}/wintergrasp/u-boot-2009.08${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://sbmagic \
           file://uboot_ivt.bd \
           file://000-board_setup.patch \
           file://001-compiler_fix.patch \
           file://002-config.patch \
           file://003-setup_eth_addr.patch \
           file://004-fix_mmc_detect_wp.patch \
           file://005-nand_access.patch \
           file://006-ext2_fix.patch \
"

SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

FILES_${PN} = "/boot"

do_deploy () {
    install -d ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/sbmagic ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/uboot_ivt.bd ${DEPLOY_DIR_IMAGE}
    install ${S}/${UBOOT_BINARY} ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}

    cd ${DEPLOY_DIR_IMAGE}
    rm -f ${UBOOT_SYMLINK}
    ln -sf ${UBOOT_IMAGE} ${UBOOT_SYMLINK}
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK}

    elftosb -p ${DEPLOY_DIR_IMAGE} -z -f imx28 -c ${DEPLOY_DIR_IMAGE}/uboot_ivt.bd -o ${DEPLOY_DIR_IMAGE}/imx28_ivt_uboot.sb
}

addtask deploy after do_install
