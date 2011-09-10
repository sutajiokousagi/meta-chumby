SRC_URI_append_chumby-silvermoon-netv = " \
    file://watchdog-silvermoon.conf \
"

do_install_append_chumby-silvermoon-netv() {
        install -m 0644 ${WORKDIR}/watchdog-silvermoon.conf ${D}${sysconfdir}/watchdog.conf
}
