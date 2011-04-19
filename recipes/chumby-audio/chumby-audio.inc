inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby basic audio setup scripts for ${FNPLATFORM}"
SECTION = "base"
PRIORITY = "required"
LICENSE = "GPL"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-2.0_${RVERSION};subpath=src;protocol=${CHUMBY_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-${CNPLATFORM}_${RVERSION};subpath=src;protocol=${CHUMBY_GIT_PROTOCOL} \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/rfs1/usr/chumby/scripts"

do_install () {
#
# Create directories and install device independent scripts
#
    install -d ${D}/usr/chumby/scripts

    install -m 0755    ${WORKDIR}/src/rfs1/usr/chumby/scripts/enable_audio.sh ${D}/usr/chumby/scripts/enable_audio.sh

#    ln -sf /psp/asound.conf ${D}${sysconfdir}/asound.conf
}

FILES_${PN} = "/usr/chumby/scripts"
