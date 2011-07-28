inherit chumbysg-git chumby-info

DESCRIPTION = "Bluetune music player"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Henry Groover"
LICENSE = "GPLv2"

DEPENDS = "python-scons-native alsa-lib"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_thirdparty_bluetune_${RVERSION}${CHUMBYSG_GIT_EXTENSION};subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL} \
	file://0001-fix-scons-build.patch \
"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src/BlueTune"

do_compile() {
    TARGET=arm-linux scons target=arm-chumby-linux build_config=Debug,Release
}

do_install() {
    install -d ${D}${bindir}
    install -d ${D}${bindir}/.debug
    install -d ${D}${libdir}
    install -d ${D}${libdir}/.debug
    install -d ${D}${includedir}
    install -d ${D}${includedir}/BlueTune
    install -d ${D}${includedir}/Core
    install -d ${D}${includedir}/Decoder
    install -d ${D}${includedir}/Player

    install -m 0755 Build/Targets/arm-chumby-linux/Release/btplay ${D}${bindir}
    ln -sf ${bindir}/btplay ${D}${bindir}/btplayd
    install -m 0755 Build/Targets/arm-chumby-linux/Debug/btplay ${D}${bindir}/.debug

    install -m 0644 Build/Targets/arm-chumby-linux/Debug/*.a ${D}${libdir}

    # Older Makefile packaged header files, too.  Why rock the boat?
    install -m 0644 Source/BlueTune/*.h ${D}${includedir}/BlueTune
    install -m 0644 Source/Core/*.h ${D}${includedir}/Core
    install -m 0644 Source/Decoder/*.h ${D}${includedir}/Decoder
    install -m 0644 Source/Player/*.h ${D}${includedir}/Player
}

FILES_${PN}-dev = "${libdir}/libAtomix.a      \
                   ${libdir}/libBento4.a      \
                   ${libdir}/libBlt*.a        \
                   ${libdir}/libBlueTune.a    \
                   ${libdir}/libNeptune.a     \
                   ${includedir}/BlueTune/*.h \
                   ${includedir}/Core/*.h     \
                   ${includedir}/Decoder/*.h  \
                   ${includedir}/Player/*.h   \
"
FILES_${PN}-dbg = "${bindir}/.debug/btplay"

FILES_${PN} = "${bindir}/btplay ${bindir}/btplayd"

