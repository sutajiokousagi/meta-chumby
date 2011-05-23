inherit chumbysg-git chumby-info

DESCRIPTION = "udev rules for ${CNPLATFORM}"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "BSD"
PR = "r2"

SRC_URI = "file://40-chumby-early.rules \
           file://98-chumby-late.rules \
           file://50-firmware-compat.rules \
"

S = "${WORKDIR}"

do_compile() {
    true
}

do_install() {
    install -d ${D}/lib/udev/rules.d
    install -m 0644 40-chumby-early.rules ${D}/lib/udev/rules.d/40-chumby-early.rules
    install -m 0644 98-chumby-late.rules ${D}/lib/udev/rules.d/98-chumby-late.rules
    install -m 0644 50-firmware-compat.rules ${D}/lib/udev/rules.d/50-firmware-compat.rules
}

FILES_${PN} = "/lib/udev/rules.d/ /usr/chumby/scripts/"
