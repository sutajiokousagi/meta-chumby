inherit chumbysg-git chumby-info qt4e

DESCRIPTION = "IR remote plugin for Qt"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "GPLv3"
PR = "r0"
DEPENDS = "qt4-embedded"
RDEPENDS_${PN} = "task-qt4e-base"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"

SRCREV = "49e2a469e0b348bb41e944fb7fec70b8124c6965"
S = "${WORKDIR}/git"

do_install() {
    install -d ${D}/usr/lib/qtopia/plugins/kbddrivers
    install -m 0755 ${S}/libqchumbyirkbddriver.so ${D}/usr/lib/qtopia/plugins/kbddrivers
}

FILES_${PN} += "/usr/lib/qtopia/plugins/kbddrivers/libqchumbyirkbddriver.so"
FILES_${PN}-dbg += "/usr/lib/qtopia/plugins/kbddrivers/.debug/libqchumbyirkbddriver.so"
