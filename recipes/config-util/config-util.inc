inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby config block manager"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Henry Groover"
LICENSE = "GPLv2"
PR = "r0"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/https_internal.chumby.com_firmware_utils-1.0_${RVERSION};subpath=src;protocol=${CHUMBY_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/src"

do_compile() {
    ${CXX} ${CFLAGS} ${LDFLAGS} config_util.cpp -o config_util
}

# Empty stage_native.
# native.bbclass will try to call this and run oe_runmake.  That doesn't work
# for our packages, so just make it a NO-OP.
# Only called for -native flavors.
do_stage_native() {
    true
}

do_install() {
    install -d ${bindir}
    install -m 0755 ${S}/config_util ${bindir}/config_util
}

