LICENSE = "MIT"

PV = "${DISTRO_VERSION}"
PR = "r${CHUMBY_BUILD}"

SRC_URI = "file://lsb_release"

PACKAGE_ARCH = "${MACHINE_ARCH}"

do_install() {
	install -d ${D}${sysconfdir}
	echo "Angstrom ${DISTRO_VERSION}" > ${D}${sysconfdir}/angstrom-version
	echo "Built from branch: ${METADATA_BRANCH}" >> ${D}${sysconfdir}/angstrom-version
	echo "Revision: ${METADATA_REVISION}" >> ${D}${sysconfdir}/angstrom-version
	echo "Target system: ${TARGET_SYS}" >> ${D}${sysconfdir}/angstrom-version

	echo "${CHUMBY_BUILD}" > ${D}${sysconfdir}/firmware_build
	echo "${CHUMBY_BUILD}_${CHUMBY_BUILDER}" > ${D}${sysconfdir}/software_version
	
	install -d ${D}${bindir}
	if [ -e ${WORKDIR}/lsb_release ]
	then
		install -m 0755 ${WORKDIR}/lsb_release ${D}${bindir}/
	fi
}
