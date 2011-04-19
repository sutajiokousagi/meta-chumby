DESCRIPTION = "This package provides the necessary \
infrastructure for basic TCP/IP based networking."
SECTION = "base"
LICENSE = "GPLv2"
PR = "r51"

SRC_URI_OVERRIDES_PACKAGE_ARCH = "1"


SRC_URI = "\
  http://developer.irexnet.com/pub/iOn/Sources/1.0/Third%20party/all/netbase-${PV}.tar.gz \
  file://options \
  file://init \
  file://hosts \
"

do_install () {
	install -d ${D}${sysconfdir}/init.d
	install	-d ${D}${sbindir}
	install -d ${D}${mandir}/man8
	install -d ${D}${sysconfdir}/network/if-pre-up.d
	install -d ${D}${sysconfdir}/network/if-up.d
	install -d ${D}${sysconfdir}/network/if-down.d
	install -d ${D}${sysconfdir}/network/if-post-down.d

	for dir in if-pre-up.d if-up.d if-down.d if-post-down.d
	do
		for script in `ls -1 "${WORKDIR}/${dir}"`
		do		
			install -m 0755 "${WORKDIR}/${dir}/${script}" "${D}${sysconfdir}/network/${dir}"
		done
	done
		   
	install -m 0644 ${WORKDIR}/hosts ${D}${sysconfdir}/hosts
	install -m 0644 etc-rpc ${D}${sysconfdir}/rpc
	install -m 0644 etc-protocols ${D}${sysconfdir}/protocols
	install -m 0644 etc-services ${D}${sysconfdir}/services
        if [ "${ONLINE_PACKAGE_MANAGEMENT}" != "none" ]; then
	install -m 0755 update-inetd ${D}${sbindir}/
	install -m 0644 update-inetd.8 ${D}${mandir}/man8/
        fi
}

CONFFILES_${PN} = "${sysconfdir}/hosts \
                   ${sysconfdir}/rpc \
                   ${sysconfdir}/protocols ${sysconfdir}/services"

SRC_URI[md5sum] = "b91eeb701d1733b3efb174b9463c5875"
SRC_URI[sha256sum] = "cc74bd934a33e80e0218d9b2482d986191879d184ffd03deaefa78b0c8d7a76a"
