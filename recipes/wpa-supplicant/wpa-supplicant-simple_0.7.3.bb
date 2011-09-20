DESCRIPTION = "A Client for Wi-Fi Protected Access (WPA)."
HOMEPAGE = "http://hostap.epitest.fi/wpa_supplicant/"
BUGTRACKER = "http://hostap.epitest.fi/bugz/"
SECTION = "network"
LICENSE = "GPLv2 | BSD"
LIC_FILES_CHKSUM = "file://../COPYING;md5=c54ce9345727175ff66d17b67ff51f58 \
                    file://../README;md5=54cfc88015d3ce83f7156e63c6bb1738 \
                    file://wpa_supplicant.c;beginline=1;endline=17;md5=acdc5a4b0d6345f21f136eace747260e"
DEPENDS = "libnl libtommath"
INC_PR = "r7"

SRC_URI = "http://hostap.epitest.fi/releases/wpa_supplicant-${PV}.tar.gz \
           file://0001-flush-output.patch \
           file://simple-defconfig \
"

S = "${WORKDIR}/wpa_supplicant-${PV}/wpa_supplicant"

do_configure () {
	install -m 0755 ${WORKDIR}/simple-defconfig .config
}

do_compile () {
	make
}

do_install () {
	install -d ${D}${sbindir}
	install -m 755 wpa_supplicant ${D}${sbindir}
	install -m 755 wpa_passphrase ${D}${sbindir}
	install -m 755 wpa_cli        ${D}${sbindir}
}

PR = "${INC_PR}.0"

SRC_URI[md5sum] = "f516f191384a9a546e3f5145c08addda"
SRC_URI[sha256sum] = "d0cd50caa85346ccc376dcda5ed3c258eef19a93b3cade39d25760118ad59443"
