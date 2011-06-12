PR .= "-amend-r1"

SRC_URI += "file://chumby-otg-debug"
INITSCRIPT_NAME = "chumby-otg-debug"
INITSCRIPT_PARAMS = "defaults"

do_install_append() {
        install -m 755 ${WORKDIR}/chumby-otg-debug ${D}${sysconfdir}/init.d/chumby-otg-debug
}
