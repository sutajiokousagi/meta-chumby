inherit chumbysg-git chumby-info qt4e

DESCRIPTION = "Chromeless web browser for NeTV with some added sugar"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Torin"
LICENSE = "GPLv3"
PR = "r2"
DEPENDS = "qt4-embedded"
#RDEPENDS_${PN} = "qt4-embedded"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"
# add SRC_URIs here for more files

SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"

#
# Create copy files to /usr/bin
#
do_install() {
    install -d ${D}${bindir}

    install -m 0755 ${WORKDIR}/git/bin/NeTVBrowser		${D}${bindir}
    install -m 0755 ${WORKDIR}/git/start_netvbrowser.sh		${D}${bindir}
    install -m 0755 ${WORKDIR}/git/stop_netvbrowser.sh		${D}${bindir}

    # Should not perform a normal qmake install
    #export INSTALL_ROOT=${D}
    #make install
}

# this puts it into a tidy package
#FILES_${PN}-dbg += "/psp/netvbrowser/.debug"
#FILES_${PN} += "/psp/netvbrowser"

