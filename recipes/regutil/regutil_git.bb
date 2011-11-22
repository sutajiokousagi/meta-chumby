inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby register reader and writer"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "BSD"
PR = "r1"

PACKAGE_ARCH = "${MACHINE}"
SRCREV = "9d4d40e76867cdbe7ba8db5aa96fc7bf4c99b43d"

SRC_URI = "${CHUMBYSG_GIT_HOST}/regutil.git;subpath=;protocol=${CHUMBYSG_GIT_PROTOCOL}"
S = "${WORKDIR}/git"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} regutil.c -o regutil -DCNPLATFORM_${CNPLATFORM}
}

do_install() {
    install -d ${D}${sbindir}
    install -m 0755 regutil ${D}${sbindir}
}

