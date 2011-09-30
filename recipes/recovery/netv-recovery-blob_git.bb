PR = "r2"

RDEPENDS_${PN} = "config-util"

SRC_URI = "file://recovery-blob-netv"
S = "${WORKDIR}"

do_compile () {
    true
}

do_install () {
    install -d ${D}/boot
    install ${S}/recovery-blob-netv ${D}/boot/recovery-mode
    install -d ${DEPLOY_DIR_IMAGE}
    install ${S}/recovery-blob-netv ${DEPLOY_DIR_IMAGE}/recovery-mode
}

pkg_postinst_${PN}() {
    config_util --cmd=putblock --dev=/dev/mmcblk0p1 --block=krnB < /boot/recovery-mode
}

FILES_${PN} += "/boot/recovery-mode"
