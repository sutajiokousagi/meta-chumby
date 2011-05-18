inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby bootloader graphics files"
SECTION = "base"
PRIORITY = "optional"
LICENSE = "GPL"
PR = "r0"

COMPATIBLE_MACHINE = "chumby-silvermoon-*"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-2.0_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-silvermoon_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/rfs1/etc"

do_install () {
#
# Create directories and install device independent scripts
#
    install -d ${D}/boot
    install -m 0644    ${WORKDIR}/src/rfs1/boot/loading_chumby.gz ${D}/boot
    install -m 0644    ${WORKDIR}/src/rfs1/boot/logo_chumby.gz ${D}/boot
    install -m 0644    ${WORKDIR}/src/rfs1/boot/recovery_1_chumby.gz ${D}/boot
    install -m 0644    ${WORKDIR}/src/rfs1/boot/recovery_2_chumby.gz ${D}/boot
    install -m 0644    ${WORKDIR}/src/rfs1/boot/loading_insignia.gz ${D}/boot
    install -m 0644    ${WORKDIR}/src/rfs1/boot/logo_insignia.gz ${D}/boot
    install -m 0644    ${WORKDIR}/src/rfs1/boot/recovery_1_insignia.gz ${D}/boot
    install -m 0644    ${WORKDIR}/src/rfs1/boot/recovery_2_insignia.gz ${D}/boot
}

FILES_${PN} = "/boot"
