inherit chumbysg-git chumby-info autotools

DESCRIPTION = "Chumby http daemon library"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Duane Maxwell"
LICENSE = "GPLv2"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_thirdparty_chumbylibs_${RVERSION}_src;subpath=chumbhttpd;protocol=${CHUMBY_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/chumbhttpd/src"

