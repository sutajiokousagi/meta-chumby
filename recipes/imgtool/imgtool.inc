inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby image drawer"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Henry Groover"
LICENSE = "GPLv2"
PR = "r0"

DEPENDS = "giflib libpng jpeg"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_ironforge_chumbytools_${RVERSION}_src_imgtool;subpath=src;protocol=${CHUMBY_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

do_compile() {
    ${CXX} ${CFLAGS} ${LDFLAGS} imgtool.cpp -o imgtool \
           -DCNPLATFORM_${CNPLATFORM} -DCNPLATFORM=\"${CNPLATFORM}\" \
           -lpng -lgif -ljpeg
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 imgtool ${D}${bindir}
}

