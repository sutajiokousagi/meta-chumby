DESCRIPTION = "Chumby boot blobs - miscellaneous binary blobs"
HOMEPAGE = "http://www.chumby.com/"
LICENSE = "BSD"

DEPENDS = "config-util-native"
DEPENDS_${PN} = "config-util-native"

PROVIDES = "chumby-blobs"
COMPATIBLE_MACHINE = "chumby-wintergrasp"
PR = "r4"

SRC_URI = "file://boot_prep \
           file://power_prep \
           file://4_1.bin \
           file://4_2.bin \
           file://updater.glob \
           file://uboot_ivt.bd \
           file://updater_ivt.bd \
           file://mk_hdr.sh \
           file://usr.fat \
"

FILES_${PN} = "/boot"

do_compile() {
    true
}

do_install() {
    true
}

do_runstrip() {
    true
}

# Copy the resulting file to the image directory
do_deploy() {
    install -d ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/boot_prep ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/power_prep ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/4_1.bin ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/4_2.bin ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/updater.glob ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/uboot_ivt.bd ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/updater_ivt.bd ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/mk_hdr.sh ${DEPLOY_DIR_IMAGE}
    install ${WORKDIR}/usr.fat ${DEPLOY_DIR_IMAGE}

    config_util --cmd=create \
        --mbr=/dev/zero \
        --configname=${CNPLATFORM} \
        --build_ver=${CHUMBY_BUILD} --force --pad \
        --blockdef=/dev/null,16384,cpid,1,0,0,0 \
        --blockdef=/dev/null,1024,dcid,1,0,0,0 \
        > ${DEPLOY_DIR_IMAGE}/config_block.bin

    rm -f ${DEPLOY_DIR_IMAGE}/config_block-${MACHINE}.part
    touch ${DEPLOY_DIR_IMAGE}/config_block-${MACHINE}.part
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/config_block-${MACHINE}.part seek=96 count=32 if=${DEPLOY_DIR_IMAGE}/config_block.bin 
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/config_block-${MACHINE}.part seek=128 count=32 if=/dev/null
    dd conv=notrunc of=${DEPLOY_DIR_IMAGE}/config_block-${MACHINE}.part seek=160 count=2 if=/dev/null

    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/boot_prep
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/power_prep
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/4_1.bin
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/4_2.bin
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/updater.glob
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/uboot_ivt.bd
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/updater_ivt.bd
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/mk_hdr.sh
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/usr.fat
    package_stagefile_shell ${DEPLOY_DIR_IMAGE}/config_block-${MACHINE}.part
}

addtask deploy before do_package_stage after do_compile
