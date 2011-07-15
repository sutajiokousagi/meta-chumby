inherit update-rc.d
INITSCRIPT_NAME = "${PN}"
INITSCRIPT_PARAMS = "defaults 3 97"

S = "${WORKDIR}"
PACKAGE_ARCH = "${MACHINE}"

SRC_URI = "file://${PN}.c file://${PN}.sh"
do_compile() {
    ${CC} ${PN}.c -o ${PN} ${CFLAGS} ${LDFLAGS}
}
do_install() {
    install -d ${D}${sysconfdir}/init.d
    install -d ${D}${sbindir}
    install -m 0755 ${PN} ${D}${sbindir}
    install -m 0755 ${PN}.sh ${D}${sysconfdir}/init.d/${PN}
}
