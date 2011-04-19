inherit chumbysg-git chumby-info autotools

DESCRIPTION = "Chumby ipod web server"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Duane Maxwell"
LICENSE = "GPLv2"

DEPENDS = "chumbhttpd libipod"
RDEPENDS = "chumbhttpd"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_thirdparty_chumbylibs_${RVERSION}_src${CHUMBYSG_GIT_EXTENSION};subpath=chumbipodd;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/chumbipodd/src"

