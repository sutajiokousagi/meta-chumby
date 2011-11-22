inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby image drawer"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Henry Groover"
LICENSE = "GPLv2"
PR = "r0"

DEPENDS = "giflib libpng jpeg"

SRC_URI = "${CHUMBYSG_GIT_HOST}/imgtool.git;subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "9ee0c24c39175ad9199671411b02755d087fb8b6"
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

