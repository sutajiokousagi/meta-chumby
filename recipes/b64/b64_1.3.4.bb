DESCRIPTION = "Base64 decoding library"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Duane Maxwell"
LICENSE = "GPLv2"
PR = "r0"

SRC_URI = "file://b64-${PV}.zip"

GCC_VER=gcc43

do_compile() {
    make -C build/${GCC_VER} \
        -f makefile.unix \
        CC="${CC}" CPPC="${CXX}" LD="${LD}" AR="${AR}" \
        COMP_TAG=chumby \
        CC_F_WARNING_ERR= \
        CPPC_F_WARNING_ERR= \
        build.libs.core
}

do_install() {
    install -d ${D}${includedir}
    cp -a include/* ${D}${includedir}

    install -d ${D}/${libdir}
    install -d ${D}/${libdir}/.debug
    install -m 0644 lib/libb64.chumby.a ${D}${libdir}
    install -m 0644 lib/libb64.chumby.debug.a ${D}${libdir}/.debug/libb64.chumby.a
}

#S = "${WORKDIR}/libipod/src"

FILES_${PN}-dev = "${libdir}/libb64.chumby.a  \
                   ${includedir}              \
"

FILES_${PN}-dbg = "${libdir}/.debug/libb64.chumby.a  \
"
