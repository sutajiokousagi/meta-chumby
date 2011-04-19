inherit chumbysg-git chumby-info module

DESCRIPTION = "Driver for block memory manager for video in Silvermoon"
HOMEPAGE = "http://www.chumby.com"
SECTION = "kernel/modules"
LICENSE = "GPL"

COMPATIBLE_MACHINE = "chumby-silvermoon"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_gst_pxa168-1.0_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/bmm-lib/drv"

MAKE_TARGETS = "compile"

do_install() {
        install -d ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra
        install -m 0644 bmm${KERNEL_OBJECT_SUFFIX} ${D}${base_libdir}/modules/${KERNEL_VERSION}/extra/
}

