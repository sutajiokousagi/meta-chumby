inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby Daughter Card ID interface"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "bunnie"
LICENSE = "GPLv2"
PR = "r3"

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
          -DDCID_ALLOW_WRITE ${LDFLAGS} \
          *.c main/*.c -o dcid
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 dcid ${D}${bindir}
}

pkg_postinst_${PN}() {
    if test "x$D" != "x"; then exit 1; fi     # Don't do postinst on build system
    if dcid -o 2> /dev/null; then exit 0; fi  # Don't overwrite existing DCIDs.

    # Program default dcid
    echo '<?xml version="1.0"?><chum><vers>0002</vers><rgin>0001</rgin><skin>0001</skin><part>1000</part><camp>0006</camp></chum>' | dcid -i
}
