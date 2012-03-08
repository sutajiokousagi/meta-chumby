inherit chumbysg-git
DESCRIPTION = "Cryptoprocessor daemon, used in lieu of a real cryptoprocessor"
LICENSE = "BSD"


inherit update-rc.d
INITSCRIPT_NAME = "chumby-cpid"
INITSCRIPT_PARAMS = "defaults 5 95"

PR = "r12"

CNPLATFORM_chumby-wintergrasp = "wintergrasp"

SRC_URI = "${CHUMBYSG_GIT_HOST}/${PN}.git;protocol=${CHUMBYSG_GIT_PROTOCOL} \
          file://chumby-cpid \
"
SRCREV = "c884dafad6b9400cc98030b6a0837d8f5d3809a2"

S = "${WORKDIR}/git/src/cpid"
DEPENDS = "beecrypt"
RDEPENDS_${PN} = "beecrypt"
PACKAGE_ARCH = "${MACHINE_ARCH}"

do_compile() {
    ${CC} -DCNPLATFORM_${CNPLATFORM} crypto.c hal.c main.c makePackets.c -o cpid ${CFLAGS} ${LDFLAGS} -lbeecrypt
}

do_install() {
    install -d ${D}/usr/bin
    install -m 0755 cpid ${D}/usr/bin

    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/chumby-cpid ${D}${sysconfdir}/init.d
}
