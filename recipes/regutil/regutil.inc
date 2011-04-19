inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby register reader and writer"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "BSD"
PR = "r0"
PN = "regutil-${CNPLATFORM}"
RPROVIDES = "regutil"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_utils-1.0_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} regutil.c -o regutil -DCNPLATFORM_${CNPLATFORM}
}

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 regutil ${D}${sbindir}
}

