inherit chumbysg-git chumby-info

DESCRIPTION = "Chumby config block manager"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Henry Groover"
LICENSE = "GPLv2"
PR = "r0"

SRC_URI = "${CHUMBYSG_GIT_HOST}/utils.git;subpath=src;protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "241261ec80f9c8d32ded37971fa7bff002d77bdc"
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

