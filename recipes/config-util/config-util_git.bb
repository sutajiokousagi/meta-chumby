inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby config block manager"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Henry Groover"
LICENSE = "GPLv2"
PR = "r0"

SRC_URI = "${CHUMBYSG_GIT_HOST}/utils.git;subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "241261ec80f9c8d32ded37971fa7bff002d77bdc"
S = "${WORKDIR}/src"

do_compile() {
    ${CXX} ${CFLAGS} ${LDFLAGS} config_util.cpp -o config_util
}


do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/config_util ${D}${bindir}/config_util
}

