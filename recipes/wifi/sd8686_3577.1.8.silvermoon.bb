inherit chumbysg-git chumby-info module

DESCRIPTION = "Driver for sd8686 wifi chipset found in ${FNPLATFORM}"
HOMEPAGE = "http://www.chumby.com"
SECTION = "kernel/modules"
LICENSE = "GPL"
PR = "r0"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_wifi-1.0_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/sd8686/wlan_src/"

EXTRA_OEMAKE = "KERNDIR=${STAGING_KERNEL_DIR}"

PACKAGES += "sd8686-firmware"

do_install() {
        install -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra
        install -m 0644 sd8xxx${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
        install -d ${D}/lib/firmware/mrvl
	install -m 0644 ../FwImage/sd8686.bin ${D}/lib/firmware/mrvl
	install -m 0644 ../FwImage/helper_sd.bin ${D}/lib/firmware/mrvl
}

FILES_${PN}-firmware = "/lib/firmware"
PACKAGES =+ "${PN}-firmware"
RDEPENDS_${PN} = "${PN}-firmware"

