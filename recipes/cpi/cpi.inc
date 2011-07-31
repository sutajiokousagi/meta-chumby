inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby control panel interface"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "bunnie"
LICENSE = "GPLv2"
PR = "r3"
PACKAGE_ARCH = "${MACHINE}"

DEPENDS = "libtomcrypt libtommath tomsfastmath b64"
RDEPENDS_${PN} = "expat perl expat"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_ironforge_cpi_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_ironforge_cpi_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=include;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://guidgen.sh \
           file://chumby_version \
"
S = "${WORKDIR}/src"

# Use Falconwing-style daemon for silvermoon
CNPLATFORM_chumby-silvermoon-netv = "falconwing"

# libb64.chumby doesn't link properly, so we manually statically link it.
do_compile() {
    ${CC} -Wall -g -DLTM_DESC -DTFM_DESC -DCNPLATFORM_${CNPLATFORM} -I../include ${CFLAGS} \
          ${LDFLAGS} -pthread -lb64.chumby -lexpat -ltomcrypt -ltommath -ltfm \
           *.c cmdline/*.c ${STAGING_LIBDIR}/libb64.chumby.a -o cpi
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 cpi ${D}${bindir}
    install -m 0755 ${WORKDIR}/guidgen.sh ${D}${bindir}
    install -m 0755 ${WORKDIR}/chumby_version ${D}${bindir}
}
