inherit chumbysg-git chumby-info

require u-boot.inc

PR = "r20"

PROVIDES = "virtual/bootloader virtual/chumby-bootimage"
RPROVIDES_${PN} = "virtual/bootloader virtual/chumby-bootimage"
RREPLACES_${PN} = "u-boot-2009.07-silvermoon"
COMPATIBLE_MACHINE = "chumby-silvermoon-*"
DEPENDS = "virtual/kernel chumby-blobs config-util-native"
RDEPENDS_${PN} = "config-util"


BRANCH_NAME = "ERROR"
BRANCH_NAME_chumby-silvermoon-netv = "netv"
BRANCH_NAME_chumby-silvermoon-chumby8 = "master"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/u-boot-2009.07-silvermoon${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL};branch=${BRANCH_NAME} \
           file://logo.raw.gz \
"

SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

DEFAULT_PREFERENCE = "-1"
DEFAULT_PREFERENCE_chumby-silvermoon-netv = "1"
DEFAULT_PREFERENCE_chumby-silvermoon-chumby8 = "1"

do_compile () {
        unset LDFLAGS
        unset CFLAGS
        unset CPPFLAGS
        unset TOPDIR
	export CROSS_COMPILE="${TARGET_PREFIX}"
        oe_runmake ${UBOOT_MACHINE}
        oe_runmake all
        oe_runmake tools env
        gzip ${WORKDIR}/logo.raw || true
}

do_install () {
        install -d ${D}/boot
        install ${S}/${UBOOT_BINARY} ${D}/boot/${UBOOT_IMAGE}
        ln -sf ${UBOOT_IMAGE} ${D}/boot/${UBOOT_BINARY}

        if [ -e ${WORKDIR}/fw_env.config ] ; then
            install -d ${D}${base_sbindir}
                install -d ${D}${sysconfdir}
                install -m 644 ${WORKDIR}/fw_env.config ${D}${sysconfdir}/fw_env.config
                install -m 755 ${S}/tools/env/fw_printenv ${D}${base_sbindir}/fw_printenv
                install -m 755 ${S}/tools/env/fw_printenv ${D}${base_sbindir}/fw_setenv
        fi
    # Put this here, because the do_install_append doesn't seem to be working
    install -m 0755 ${WORKDIR}/logo.raw.gz ${D}/boot
}

do_deploy () {

    install -d ${DEPLOY_DIR_IMAGE}
    install ${S}/${UBOOT_BINARY} ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}
    install ${WORKDIR}/logo.raw.gz ${DEPLOY_DIR_IMAGE}/logo.raw.gz
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_IMAGE}

    cd ${DEPLOY_DIR_IMAGE}
    rm -f ${UBOOT_SYMLINK}
    ln -sf ${UBOOT_IMAGE} ${UBOOT_SYMLINK}
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/${UBOOT_SYMLINK}
}
addtask deploy_bootimage after do_install

pkg_postinst_${PN}() {
    config_util --cmd=putblock --dev=/dev/mmcblk0p1 --block=u-bt < /boot/u-boot.bin
    
    # Generate the dcid and cpid config_util if they're missing
    if ! config_util --cmd=getblock --block=logo 2>&1 > /dev/null
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
            --blockdef=/dev/null,340992,720p,1,0,0,0 \
            --blockdef=/dev/null,1843200,logo,1,0,0,0 \
        | dd of=/dev/mmcblk0p1 seek=96
        config_util --cmd=putblock --block=720p < /lib/firmware/hdmi_720p.bin
        config_util --cmd=putblock --block=logo < /boot/logo.raw.gz
    fi
}
