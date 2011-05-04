inherit chumbysg-git chumby-info autotools

DESCRIPTION = "Chumby patched wget code"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Duane Maxwell"
LICENSE = "GPLv2"
PR = "r1"

COMPATIBLE_MACHINE = "chumby-${CNPLATFORM}"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_utils-1.0_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL}"

SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/wget"

autotools_do_configure() {
    oe_runconf $@
}
