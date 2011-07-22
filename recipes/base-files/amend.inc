SRC_URI += "file://fstab.chumby"
do_install_append() {
    install -m 0644 ${WORKDIR}/fstab.chumby ${D}${sysconfdir}/fstab
}
