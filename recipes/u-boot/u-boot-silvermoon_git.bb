inherit chumbysg-git chumby-info

require u-boot.inc

PR = "r6"

PROVIDES = "virtual/bootloader virtual/chumby-bootimage"
RPROVIDES_${PN} = "virtual/bootloader virtual/chumby-bootimage"
RREPLACES_${PN} = "u-boot-2009.07-silvermoon"
COMPATIBLE_MACHINE = "chumby-silvermoon-*"
DEPENDS = "virtual/kernel chumby-blobs config-util-native"
RDEPENDS_${PN} = "config-util"


BRANCH_NAME = "master"
BRANCH_NAME_chumby-silvermoon-netv = "netv"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/u-boot-2009.07-silvermoon${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL};branch=${BRANCH_NAME} \
"

SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-silvermoon-netv = "1"
DEFAULT_PREFERENCE_chumby-silvermoon-chumby8 = "1"

do_deploy () {

    install -d ${DEPLOY_DIR_IMAGE}
    install ${S}/${UBOOT_BINARY} ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}

    cd ${DEPLOY_DIR_IMAGE}
    rm -f ${UBOOT_SYMLINK}
    ln -sf ${UBOOT_IMAGE} ${UBOOT_SYMLINK}
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK}


    install -d ${DEPLOY_DIR_IMAGE}

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


    rm -f ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    touch ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=0 count=96 if=${DEPLOY_DIR_IMAGE}/obm.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=96 count=32 if=${DEPLOY_DIR_IMAGE}/config_block.bin 
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=128 count=2944 if=${DEPLOY_DIR_IMAGE}/u-boot-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=3072 count=7680 if=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin seek=10752 count=7680 if=${DEPLOY_DIR_IMAGE}/zImage-${MACHINE}.bin

    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/boot-${MACHINE}.bin
}
addtask deploy_bootimage after do_install

pkg_postinst_${PN}() {
    config_util --cmd=putblock --dev=/dev/mmcblk0p1 --block=u-bt < /boot/u-boot.bin
    
    # Generate the dcid and cpid config_util if they're missing
    if ! config_util --cmd=getblock --block=dcid 2&>1 > /dev/null
    then
        config_util --cmd=create \
            --mbr=/dev/zero \
            --configname=${CNPLATFORM} \
            --build_ver=${CHUMBY_BUILD} --force --pad \
            --blockdef=/dev/null,1507328,u-bt,1,0,0,0 \
            --blockdef=/dev/null,3932160,krnA,1,0,0,0 \
            --blockdef=/dev/null,3932160,krnB,1,0,0,0 \
            --blockdef=/dev/null,16384,cpid,1,0,0,0 \
            --blockdef=/dev/null,1024,dcid,1,0,0,0 \
        | dd of=/dev/mmcblk0p1 seek=96
    fi
}
