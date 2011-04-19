inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby control panel interface"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "bunnie"
LICENSE = "GPLv2"
PR = "r0"
PN = "cpi-${CNPLATFORM}"

PROVIDES = "cpi"
RPROVIDES = "cpi"

DEPENDS = "b64 libtomcrypt libtommath tomsfastmath expat"
RDEPENDS = "expat"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_ironforge_cpi_${RVERSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_ironforge_cpi_${RVERSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=include;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_scripts-1.0_${RVERSION}_src${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=scripts;protocol=${CHUMBYSG_GIT_PROTOCOL} \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

# libb64.chumby doesn't link properly, so we manually statically link it.
do_compile() {
    ${CC} -Wall -g -DLTM_DESC -DTFM_DESC -DCNPLATFORM_${CNPLATFORM} -I../include ${CFLAGS} \
          ${LDFLAGS} -pthread -lb64.chumby -lexpat -ltomcrypt -ltommath -ltfm \
           *.c cmdline/*.c ${STAGING_LIBDIR}/libb64.chumby.a -o cpi
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 cpi ${D}${bindir}

    install -d ${D}/usr/chumby/scripts
    install -m 0755 ../scripts/cpi.sh ${D}/usr/chumby/scripts
    install -m 0755 ../scripts/guidgen.sh ${D}/usr/chumby/scripts
    install -m 0755 ../scripts/chumby_version ${D}/usr/chumby/scripts
}

FILES_${PN} = "${bindir} /usr/chumby/scripts"
