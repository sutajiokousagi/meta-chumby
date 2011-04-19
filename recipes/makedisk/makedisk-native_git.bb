inherit chumbysg-git chumby-info native

DESCRIPTION = "Disk image creator"
HOMEPAGE = "http://www.chumby.com/"
AUTHOR = "Sean Cross"
LICENSE = "BSD"
PR = "r1"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/makedisk;protocol=${CHUMBY_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"
S = "${WORKDIR}/git"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} makedisk.c -o makedisk
}

# Empty stage_native.
# native.bbclass will try to call this and run oe_runmake.  That doesn't work
# for our packages, so just make it a NO-OP.
do_stage_native() {
    true
}

do_install() {
    install -d ${bindir}
    install -m 0755 ${S}/makedisk ${bindir}/makedisk
}

