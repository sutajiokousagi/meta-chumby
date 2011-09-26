inherit chumbysg-git
DESCRIPTION = "Cryptoprocessor daemon, used in lieu of a real cryptoprocessor"
LICENSE = "BSD"


inherit update-rc.d
INITSCRIPT_NAME = "chumby-cpid"
INITSCRIPT_PARAMS = "defaults 5 95"

PR = "r11a"

CNPLATFORM_chumby-wintergrasp = "wintergrasp"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-sg/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL} \
          file://chumby-cpid \
"
SRCREV = "84fbc563a761ddfa18f108ed13227f8414ae5b5f"

S = "${WORKDIR}/git/src/cpid"
DEPENDS = "beecrypt"
RDEPENDS_${PN} = "beecrypt"

do_compile() {
    ${CC} -DCNPLATFORM_${CNPLATFORM} crypto.c hal.c main.c makePackets.c -o cpid ${CFLAGS} ${LDFLAGS} -lbeecrypt
}

do_install() {
    install -d ${D}/usr/bin
    install -m 0755 cpid ${D}/usr/bin

    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/chumby-cpid ${D}${sysconfdir}/init.d
}
