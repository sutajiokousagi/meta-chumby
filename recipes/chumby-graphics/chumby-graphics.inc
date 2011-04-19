inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby userspace graphics"
SECTION = "base"
PRIORITY = "optional"
LICENSE = "GPL"
PR = "r0"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-2.0_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
           ${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_partitions-${CNPLATFORM}_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/rfs1/etc"

do_install () {
    install -d ${D}/bitmap
    cp -r ${WORKDIR}/src/rfs1/bitmap/* ${D}/bitmap/
}

FILES_${PN} = "/bitmap"
