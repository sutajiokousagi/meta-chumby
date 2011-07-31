inherit chumbysg-git chumby-info autotools

DESCRIPTION = "Chumby radio web server"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Duane Maxwell"
LICENSE = "GPLv2"
PR = "r0"

RDEPENDS = "chumbhttpd"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_thirdparty_chumbylibs_${RVERSION}_src${CHUMBYSG_GIT_EXTENSION};subpath=chumbradiod;protocol=${CHUMBYSG_GIT_PROTOCOL}"
S = "${WORKDIR}/chumbradiod/src"

