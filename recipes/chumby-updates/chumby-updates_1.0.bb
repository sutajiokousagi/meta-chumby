RDEPENDS_${PN} = "opkg"
DEPENDS = "opkg"
SRC_URI = "file://poll-update-server.sh \
           file://secring.gpg \
           file://trustdb.gpg \
           file://trusted.gpg \
           file://opkg-chumby-upgrade.c \
"
S = "${WORKDIR}"
PR = "r12"

do_compile() {
	sed -e 's/_MACHINE_/${MACHINE}/g' -i poll-update-server.sh
	${CC} ${LDFLAGS} ${CFLAGS} -lopkg opkg-chumby-upgrade.c -o opkg-chumby-upgrade -Wall
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 poll-update-server.sh ${D}${bindir}
	install -m 0755 opkg-chumby-upgrade ${D}${bindir}

	install -d ${D}/etc/opkg
	install -m 0600 secring.gpg ${D}/etc/opkg
	install -m 0600 trustdb.gpg ${D}/etc/opkg
	install -m 0600 trusted.gpg ${D}/etc/opkg
}

pkg_postinst() {
#!/bin/sh -e
        ROOTCRON=/var/cron/tabs/root

        if test -e ${ROOTCRON}
	then
		if grep -q '^[^#].*poll-update-server.sh' $ROOTCRON
		then
			exit 0
		fi
	fi

	echo "12 * * * * /usr/bin/poll-update-server.sh" >> $ROOTCRON
# secondary cron entries to be removed post-beta
	echo "27 * * * * /usr/bin/poll-update-server.sh" >> $ROOTCRON
	echo "42 * * * * /usr/bin/poll-update-server.sh" >> $ROOTCRON
	echo "57 * * * * /usr/bin/poll-update-server.sh" >> $ROOTCRON
        /etc/init.d/cron restart

        exit 0
}

pkg_postrm() {
        ROOTCRON=/var/cron/tabs/root
	grep -v 'poll-update-server.sh$' < $ROOTCRON > $ROOTCRON.tmp
	mv $ROOTCRON.tmp $ROOTCRON
}

