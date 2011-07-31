inherit chumbysg-git chumby-info autotools

DESCRIPTION = "kobs-ng - tool to burn image to nand"
HOMEPAGE = "http://www.chumby.com/"
LICENSE = "GPLv2"
PR = "r0"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/kobs-ng-10.12.01${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "5814412eb4b61abc31bd706f2063957306266056"
S = "${WORKDIR}/git"

autotools_do_configure() {
  oe_runconf $@
}
