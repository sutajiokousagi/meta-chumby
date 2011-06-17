inherit chumbysg-git chumby-info

DESCRIPTION = "Control Panel based on HTML & JavaScript"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Torin"
LICENSE = "GPLv3"
PR = "r2"
DEPENDS = "chumby-netvserver"
RDEPENDS = "chumby-netvserver"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"
# add SRC_URIs here for more files

SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"
DOCROOT = "${WORKDIR}/git"

#
# Create directories and copy files
#
do_install() {

    install -d ${D}/psp/netvserver/docroot

    cp -rf   ${WORKDIR}/git/*		${D}/psp/netvserver/docroot
}

# this puts it into a tidy package
FILES_${PN} += "/psp/netvserver/docroot"

