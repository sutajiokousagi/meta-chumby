SRC_URI = "file://poll-server.sh"
S = "${WORKDIR}"
PR = "r1"

do_compile() {
	sed -e 's/_MACHINE_/${MACHINE}/g' -i poll-server.sh
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 poll-server.sh ${D}${bindir}
}

pkg_postinst() {
#!/bin/sh -e
        ROOTCRON=/var/cron/tabs/root

        if test -e ${ROOTCRON}
	then
		if grep -q '^[^#].*poll-server.sh' $ROOTCRON
		then
			exit 0
		fi
	fi

	echo "12 * * * * /usr/bin/poll-server.sh" >> $ROOTCRON
        /etc/init.d/cron restart

        exit 0
}

pkg_postrm() {
        ROOTCRON=/var/cron/tabs/root
	grep -v 'poll-server.sh$' < $ROOTCRON > $ROOTCRON.tmp
	mv $ROOTCRON.tmp $ROOTCRON
}

