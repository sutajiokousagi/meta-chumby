inherit chumbysg-git chumby-info qt4e
inherit update-rc.d

DESCRIPTION = "Hardware bridge for NeTV"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Torin"
LICENSE = "GPLv3"
PR = "r187"
DEPENDS = "qt4-embedded fastcgi"
RDEPENDS_${PN} = "task-qt4e-minimal curl fastcgi"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN};protocol=${CHUMBYSG_GIT_PROTOCOL}"

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
    install -d ${D}/usr/share/netvserver/docroot/html_motor
    install -d ${D}/usr/share/netvserver/docroot/tests
    install -d ${D}/usr/share/netvserver/docroot/tmp

#   Symlink to /tmp for caching downloaded thumbnails
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
    cp -rf ${DOCROOT}/html_motor                             ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/tests	                             	 ${D}/usr/share/netvserver/docroot
    cp -rf ${DOCROOT}/scripts                                ${D}/usr/share/netvserver/docroot
    chmod +x ${D}/usr/share/netvserver/docroot/scripts/*
}

# Cron job to check network condition every 30 minutes
pkg_postinst() {
#!/bin/sh -e

	# Create a symlink for lighttpd to point to
	if [ ! -e /www/netvserver ]; then
		ln -s /usr/share/netvserver/docroot /www/netvserver
	fi

	# Cron job: Check for valid Internet connection & otherwise respawn wlan interface
    ROOTCRON=/var/cron/tabs/root

    if test -e ${ROOTCRON}
	then
		if grep -q '^[^#].*check_network.sh' $ROOTCRON
		then
			exit 0
		fi
	fi

	echo "05,35 * * * * /usr/share/netvserver/docroot/scripts/check_network.sh" >> $ROOTCRON
    /etc/init.d/cron restart
    exit 0
}


# this puts it into a tidy package
FILES_${PN}-dbg += "/usr/share/netvserver/.debug"
FILES_${PN} += "/usr/share/netvserver"
FILES_${PN} += ${bindir}
FILES_${PN} += ${sysconfdir}
FILES_${PN} += ${sysconfdir}/init.d

