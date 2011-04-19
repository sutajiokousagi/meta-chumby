inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby Daughter Card ID interface"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "bunnie"
LICENSE = "GPLv2"
PR = "r0"
PN = "dcid-${CNPLATFORM}"
PROVIDES = "dcid"
RPROVIDES = "dcid"

DEPENDS = "b64 libtomcrypt libtommath tomsfastmath expat"
RDEPENDS = "expat"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_ironforge_dcid_${RVERSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_ironforge_dcid_${RVERSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=include;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_scripts-1.0_${RVERSION}_src${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=scripts;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://chumby_accel.h \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

# libb64.chumby doesn't link properly, so we manually statically link it.
do_compile() {
    ${CC} -Wall -g -DCNPLATFORM_${CNPLATFORM} -I.. -I../include ${CFLAGS} \
          ${LDFLAGS} \
          *.c main/*.c -o dcid
    ${CC} -Wall -g -DCNPLATFORM_${CNPLATFORM} -I.. -I../include ${CFLAGS} \
          -DDCID_ALLOW_WRITE ${LDFLAGS} \
          *.c main/*.c -o dcid-write-enabled
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 dcid ${D}${bindir}
    install -m 0755 dcid-write-enabled ${D}${bindir}

    install -d ${D}/usr/chumby/scripts
    install -m 0755 ../scripts/dcid_getparms ${D}/usr/chumby/scripts
}

FILES_${PN} = "${bindir}/dcid /usr/chumby/scripts"
FILES_${PN}-write-enabled = "${bindir}/dcid-write-enabled"
