inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby basic time setup scripts for ${CNPLATFORM}"
SECTION = "base"
PRIORITY = "required"
LICENSE = "GPL"
PR = "r0"

RDEPENDS = "cpi-${MACHINE} ntp"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_scripts-1.0_${RVERSION}_src${CHUMBYSG_GIT_EXTENSION};subpath=scripts;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/scripts"

do_install () {
#
# Create directories and install device independent scripts
#
    install -d ${D}/usr/chumby/scripts

    install -m 0755    ${WORKDIR}/scripts/save_time ${D}/usr/chumby/scripts
    install -m 0755    ${WORKDIR}/scripts/restore_time ${D}/usr/chumby/scripts
    install -m 0755    ${WORKDIR}/scripts/sync_time.sh ${D}/usr/chumby/scripts
    install -m 0755    ${WORKDIR}/scripts/sync_time_state.sh ${D}/usr/chumby/scripts
    install -m 0755    ${WORKDIR}/scripts/time_zone.sh ${D}/usr/chumby/scripts
}

FILES_${PN} = "/usr/chumby/scripts"
