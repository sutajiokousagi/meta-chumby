SRC_URI = "file://poll-update-server.sh"
S = "${WORKDIR}"
PR = "r3"

do_compile() {
	sed -e 's/_MACHINE_/${MACHINE}/g' -i poll-update-server.sh
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 poll-update-server.sh ${D}${bindir}
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
        /etc/init.d/cron restart

        exit 0
}

pkg_postrm() {
        ROOTCRON=/var/cron/tabs/root
	grep -v 'poll-update-server.sh$' < $ROOTCRON > $ROOTCRON.tmp
	mv $ROOTCRON.tmp $ROOTCRON
}

