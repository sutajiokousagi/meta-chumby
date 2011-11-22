inherit chumbysg-git chumby-info qt4e

DESCRIPTION = "Reset button keyboard plugin for Qt"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "GPLv3"
PR = "r1"
DEPENDS = "qt4-embedded"
RDEPENDS_${PN} = "task-qt4e-minimal"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN}.git;protocol=${CHUMBYSG_GIT_PROTOCOL}"

SRCREV = "0b633b5cf8153429b93b8d9b7756ba2a67f1ee76"
S = "${WORKDIR}/git"

do_install() {
    install -d ${D}/usr/lib/qtopia/plugins/kbddrivers
    install -m 0755 ${S}/libqchumbyresetkbddriver.so ${D}/usr/lib/qtopia/plugins/kbddrivers
}

FILES_${PN} += "/usr/lib/qtopia/plugins/kbddrivers/libqchumbyresetkbddriver.so"
FILES_${PN}-dbg += "/usr/lib/qtopia/plugins/kbddrivers/.debug/libqchumbyresetkbddriver.so"
