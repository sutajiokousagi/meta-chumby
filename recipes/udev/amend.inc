SRC_URI += "file://cache.chumby"
PR .= "-amend-r1"
do_install_append() {
	if [ "${PN}" = "udev" ]; then
		install -m 0755 ${WORKDIR}/cache.chumby ${D}${sysconfdir}/init.d/udev-cache
	fi
}
