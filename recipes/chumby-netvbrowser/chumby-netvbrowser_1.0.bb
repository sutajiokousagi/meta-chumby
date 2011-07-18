inherit chumbysg-git chumby-info qt4e
inherit update-rc.d

DESCRIPTION = "Chromeless web browser for NeTV with some added sugar"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Torin"
LICENSE = "GPLv3"
PR = "r20"
DEPENDS = "qt4-embedded"
RDEPENDS_${PN} = "task-qt4e-base"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"

SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"

INITSCRIPT_NAME = "chumby-netvbrowser"
INITSCRIPT_PARAMS = "defaults 60 40"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/bin/NeTVBrowser ${D}${bindir}

    install -d ${D}${sysconfdir}/init.d/
    install -m 0755 ${S}/chumby-netvbrowser.sh ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME}
}
