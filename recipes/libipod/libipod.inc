inherit chumbysg-git chumby-info autotools

DESCRIPTION = "Basic no-frills ipod library"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Duane Maxwell"
LICENSE = "GPLv2"
PR = "r0"

DEPENDS = "expat jpeg libpng tiff zlib"

CONFIGUREOPTS += " \
  --enable-plist \
  --enable-jpeg \
  --enable-png \
  --enable-tiff \
"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_thirdparty_chumbylibs_${RVERSION}_src${CHUMBYSG_GIT_EXTENSION};subpath=libipod;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/libipod/src"

