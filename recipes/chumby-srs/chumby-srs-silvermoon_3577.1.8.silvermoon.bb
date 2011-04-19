inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby SRS binaries for Silvermoon"
SECTION = "base"
PRIORITY = "required"
LICENSE = "GPL"
PR = "r0"

COMPATIBLE_MACHINE = "chumby-silvermoon"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_srs-wowhd-1.0_${RVERSION};subpath=src;protocol=${CHUMBY_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/rfs1/usr/chumby/scripts"

do_install () {
    install -d ${D}/usr/share/SRS/WowHDMaxV/libs
    install -m 0755    ${WORKDIR}/src/WowHDMaxV/SRSMasterEnable ${D}/usr/share/SRS/WowHDMaxV
    install -m 0755    ${WORKDIR}/src/WowHDMaxV/initSrsIpc ${D}/usr/share/SRS/WowHDMaxV
    install -m 0644    ${WORKDIR}/src/WowHDMaxV/presets_chumbydpf.xml ${D}/usr/share/SRS/WowHDMaxV

    install -m 0755    ${WORKDIR}/src/WowHDMaxV/libs/srswowhdmaxv_maxv.so ${D}/usr/share/SRS/WowHDMaxV/libs
    install -m 0755    ${WORKDIR}/src/WowHDMaxV/libs/srswowhdmaxv_wowhd.so ${D}/usr/share/SRS/WowHDMaxV/libs

    install -d ${D}${libdir}/alsa-lib
    install -m 0755    ${WORKDIR}/src/alsa-lib/* ${D}${libdir}/alsa-lib
}

FILES_${PN} = "/usr/share/SRS ${libdir}/alsa-lib"
INSANE_SKIP_${PN} = True
