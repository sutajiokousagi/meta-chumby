inherit update-rc.d
PR = "r2"

SRC_URI += "file://chumby-otg-debug file://udhcpd.conf"
INITSCRIPT_NAME = "chumby-otg-debug"
INITSCRIPT_PARAMS = "defaults"

do_compile() {
}

do_install() {
    install -d ${D}${sysconfdir}/init.d
    install -m 755 ${WORKDIR}/chumby-otg-debug ${D}${sysconfdir}/init.d/chumby-otg-debug
    install -m 755 ${WORKDIR}/udhcpd.conf ${D}${sysconfdir}
}

FILES_${PN} += "/etc"
