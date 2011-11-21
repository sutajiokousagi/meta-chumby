DESCRIPTION = "Firmware for ath9k-based USB wifi adaptors"
LICENSE = "unknown"

PR = "r0"

SRC_URI = "file://htc_7010.fw \
           file://ar9170-1.fw \
           file://ar9170-2.fw \
           file://ar9271.fw \
           file://htc_9271.fw \
           file://htc_7010.fw \
"

S = "${WORKDIR}"

do_install() {
	install -d ${D}/${base_libdir}/firmware
	install -m 0644 htc_7010.fw ${D}/${base_libdir}/firmware/
	install -m 0644 ar9271.fw ${D}/${base_libdir}/firmware/
	install -m 0644 htc_9271.fw ${D}/${base_libdir}/firmware/
	install -m 0644 ar9170-1.fw ${D}/${base_libdir}/firmware/
	install -m 0644 ar9170-2.fw ${D}/${base_libdir}/firmware/
}

FILES_${PN} = "${base_libdir}/firmware/"
PACKAGE_ARCH = "all"
