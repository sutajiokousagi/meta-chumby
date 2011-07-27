SRC_URI += "file://checkroot.chumby \
            file://populate-volatile.sh.chumby \
            file://finish.sh.chumby \
            file://bootmisc.sh.chumby \
"
PR .= "-amend-r2"

do_install_append() {
        install -m 0755 ${WORKDIR}/checkroot.chumby ${D}${sysconfdir}/init.d/checkroot
	install -m 0755 ${WORKDIR}/populate-volatile.sh.chumby ${D}${sysconfdir}/init.d/populate-volatile.sh
	install -m 0755 ${WORKDIR}/finish.sh.chumby ${D}${sysconfdir}/init.d/finish.sh
        install -m 0755 ${WORKDIR}/bootmisc.sh.chumby ${D}${sysconfdir}/init.d/bootmisc.sh
}

