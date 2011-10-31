inherit chumbysg-git chumby-info qt4e
inherit update-rc.d

DESCRIPTION = "Hardware bridge for NeTV"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Torin"
LICENSE = "GPLv3"
PR = "r157"
DEPENDS = "qt4-embedded"
RDEPENDS_${PN} = "task-qt4e-minimal curl"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"

SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git/src"
DOCROOT = "${WORKDIR}/git/etc/docroot"

INITSCRIPT_NAME = "chumby-netvserver"
INITSCRIPT_PARAMS = "defaults 30 70"

do_install() {
    install -d ${D}${bindir}
    install -d ${D}${sysconfdir}/init.d
    install -d ${D}/usr/share/netvserver
    install -d ${D}/usr/share/netvserver/cookies
    install -d ${D}/usr/share/netvserver/docroot/scripts
    install -d ${D}/usr/share/netvserver/docroot/html_test
    install -d ${D}/usr/share/netvserver/docroot/html_remote
    install -d ${D}/usr/share/netvserver/docroot/html_config
    install -d ${D}/usr/share/netvserver/docroot/html_update
    install -d ${D}/usr/share/netvserver/docroot/tests
    install -d ${D}/usr/share/netvserver/docroot/tmp

#   Cache for downloaded thumbnails
    ln -sf /tmp ${D}/usr/share/netvserver/docroot/tmp/netvserver

    install -m 0755 ${S}/bin/NeTVServer                      ${D}${bindir}
    install -m 0755 ${WORKDIR}/git/etc/NeTVServer.ini        ${D}${sysconfdir}
    install -m 0755 ${WORKDIR}/git/etc/chumby-netvserver.sh  ${D}${sysconfdir}/init.d/chumby-netvserver
    install -m 0755 ${DOCROOT}/favicon.ico                   ${D}/usr/share/netvserver/docroot
    install -m 0755 ${DOCROOT}/html_remote.html              ${D}/usr/share/netvserver/docroot

    cp -rf ${DOCROOT}/html_test                              ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/html_remote                            ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/html_config                            ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/html_update                            ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/tests	                             ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/scripts                                ${D}/usr/share/netvserver/docroot
    chmod +x ${D}/usr/share/netvserver/docroot/scripts/*
}

# this puts it into a tidy package
FILES_${PN}-dbg += "/usr/share/netvserver/.debug"
FILES_${PN} += "/usr/share/netvserver"
FILES_${PN} += ${bindir}
FILES_${PN} += ${sysconfdir}
FILES_${PN} += ${sysconfdir}/init.d

