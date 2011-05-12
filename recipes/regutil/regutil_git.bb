inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby register reader and writer"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "BSD"
PR = "r1"

PACKAGE_ARCH = "${MACHINE}"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/regutil${CHUMBYSG_GIT_EXTENSION};subpath=;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} regutil.c -o regutil -DCNPLATFORM_${CNPLATFORM}
}

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 regutil ${D}${sbindir}
}

