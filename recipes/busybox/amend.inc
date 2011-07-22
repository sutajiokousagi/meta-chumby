SRC_URI += "file://syslog.conf.chumby"
PR .= "-amend-pr1"

do_install_append() {
    install -m 644 ${WORKDIR}/syslog.conf.chumby ${D}${sysconfdir}/default/busybox-syslog
}
