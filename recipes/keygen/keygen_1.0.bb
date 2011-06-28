inherit chumbysg-git-private
DESCRIPTION = "Cryptoprocessor daemon, used in lieu of a real processor"
LICENSE = "BSD"

PR = "r2"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/src/keygen"
DEPENDS = "beecrypt"
RDEPENDS_${PN} = "beecrypt"

PACKAGE_ARCH = "${MACHINE}"

CHUMBY_CFLAGS = ""
CHUMBY_CFLAGS_chumby-silvermoon-netv = "-DUSE_FPGA_ENTROPY"
CHUMBY_CFLAGS_chumby-falconwing = "-DUSE_ACCEL_ENTROPY"

do_compile() {
    ${CC} keygen.c -o keygen ${CFLAGS} ${CHUMBY_CFLAGS} ${LDFLAGS} -lbeecrypt
}

do_install() {
	install -d ${D}/usr/bin
	install -m 0755 keygen ${D}/usr/bin
}
