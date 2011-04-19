inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby /psp defaults"
SECTION = "base"
PRIORITY = "required"
LICENSE = "GPL"
PR = "r0"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-2.0_${RVERSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-${CNPLATFORM}_${RVERSION}${CHUMBYSG_GIT_EXTENSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

do_compile() {
    true
}

do_install () {
#
# Create directories and install device independent scripts
#
    install -d ${D}/usr/share/defaults
    cd ${WORKDIR}/src; tar cvzf ${D}/usr/share/defaults/psp.tar.gz --exclude=.svn psp/
    cd ${D}/usr/share/defaults; md5sum psp.tar.gz > psp.tar.gz.md5
}

FILES_${PN} = "/usr/share/defaults"
