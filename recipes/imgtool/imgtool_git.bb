inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby image drawer"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Henry Groover"
LICENSE = "GPLv2"
PR = "r1"

DEPENDS = "giflib libpng jpeg"

SRC_URI = "${CHUMBYSG_GIT_HOST}/imgtool.git;subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "e944835b70dcc630aef3fff619daeecd9bb5391a"
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

