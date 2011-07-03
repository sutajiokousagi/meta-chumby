SRC_URI = "file://poll-server.sh"
S = "${WORKDIR}"

do_compile() {
	true
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 poll-server.sh ${D}${bindir}
}

pkg_postinst() {
#!/bin/sh -e
        ROOTCRON=/var/cron/tabs/root

        test -e ${ROOTCRON} \
         && grep '^[^#].*poll-server.sh' $ROOTCRON > /dev/null \
         || echo "12 * * * * /usr/bin/poll-server.sh" >> $ROOTCRON

        /etc/init.d/cron restart
        exit 0
}

pkg_postrm() {
        ROOTCRON=/var/cron/tabs/root
	grep -v 'poll-server.sh$' < $ROOTCRON > $ROOTCRON.tmp
	mv $ROOTCRON.tmp $ROOTCRON
}

