DESCRIPTION = "Configuration files for online package repositories aka feeds"
LICENSE = "MIT"

RRECOMMENDS_${PN} += "opkg"

PR = "r15"
PACKAGE_ARCH = "${MACHINE}"

do_compile() {
	echo "src/gz sdk http://buildbot.chumby.com.sg/build/${CHUMBY_BUILDER}/LATEST" > ${WORKDIR}/chumby.conf
}


do_install () {
	install -d ${D}${sysconfdir}/opkg
	install -m 0644  ${WORKDIR}/chumby.conf ${D}${sysconfdir}/opkg/
}

FILES_${PN} = "${sysconfdir}/opkg/chumby.conf"
