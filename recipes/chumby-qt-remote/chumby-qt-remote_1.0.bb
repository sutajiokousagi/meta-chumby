inherit chumbysg-git chumby-info qt4e

DESCRIPTION = "IR remote plugin for Qt"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "GPLv3"
PR = "r2"
DEPENDS = "qt4-embedded"
RDEPENDS_${PN} = "task-qt4e-minimal"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN}.git;protocol=${CHUMBYSG_GIT_PROTOCOL}"

SRCREV = "9558aa973f5feba262f1218cb79612870d597dca"
S = "${WORKDIR}/git"

do_install() {
    install -d ${D}/usr/lib/qtopia/plugins/kbddrivers
    install -m 0755 ${S}/libqchumbyirkbddriver.so ${D}/usr/lib/qtopia/plugins/kbddrivers
}

FILES_${PN} += "/usr/lib/qtopia/plugins/kbddrivers/libqchumbyirkbddriver.so"
FILES_${PN}-dbg += "/usr/lib/qtopia/plugins/kbddrivers/.debug/libqchumbyirkbddriver.so"
