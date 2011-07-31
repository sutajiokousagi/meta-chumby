inherit chumbysg-git chumby-info native

DESCRIPTION = "SB boot blob generator"
HOMEPAGE = "http://www.freescale.com/"
AUTHOR = "Freescale"
LICENSE = "Proprietary"

PR ="r1"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/elftosb-10.12.01${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL} \
"
SRCREV = "396e5c3e423609ab7d618882dbed449a012016c4"
S = "${WORKDIR}/git"

# Must do a single-threaded make
do_compile() {
    make
}

do_install() {
    install -d ${bindir}
    install -m 0755 ${S}/bld/linux/elftosb ${bindir}
}

# Empty stage_native.
# native.bbclass will try to call this and run oe_runmake.  That doesn't work
# for our packages, so just make it a NO-OP.
do_stage_native() {
    true
}
