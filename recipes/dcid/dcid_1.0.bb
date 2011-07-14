inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby Daughter Card ID interface"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "bunnie"
LICENSE = "GPLv2"
PR = "r1a"

DEPENDS = "b64 libtomcrypt libtommath tomsfastmath expat"
RDEPENDS_${PN} = "expat"

PACKAGE_ARCH = "${MACHINE}"


# NeTV stores data in the config_block of partition 1
CNPLATFORM_chumby-silvermoon-netv = "netv"

# Wintergrasp on partition 2
CNPLATFORM_chumby-wintergrasp = "wintergrasp"


SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/dcid${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/dcid${CHUMBYSG_GIT_EXTENSION};subpath=include;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           file://chumby_accel.h \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

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
}

FILES_${PN}-write-enabled = "${bindir}/dcid-write-enabled"
