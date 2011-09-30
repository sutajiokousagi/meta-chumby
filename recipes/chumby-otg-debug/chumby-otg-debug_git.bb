inherit update-rc.d
PR = "r3"

SRC_URI += "file://chumby-otg-debug \
            file://udhcpd.conf \
            file://42-chumby-otg.rules \
"
INITSCRIPT_NAME = "chumby-otg-debug"
INITSCRIPT_PARAMS = "defaults 4 96"

do_compile() {
}

do_install() {
    install -d ${D}${sysconfdir}/init.d
    install -d ${D}/lib/udev/rules.d

    install -m 755 ${WORKDIR}/chumby-otg-debug ${D}${sysconfdir}/init.d/chumby-otg-debug
    install -m 755 ${WORKDIR}/udhcpd.conf ${D}${sysconfdir}

    install -m 755 ${WORKDIR}/42-chumby-otg.rules ${D}/lib/udev/rules.d
}

FILES_${PN} += "/etc /lib/udev/rules.d/42-chumby-otg.rules"
