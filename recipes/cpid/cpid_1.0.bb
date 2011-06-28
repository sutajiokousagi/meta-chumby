inherit chumbysg-git-private
DESCRIPTION = "Cryptoprocessor daemon, used in lieu of a real cryptoprocessor"
LICENSE = "BSD"


inherit update-rc.d
INITSCRIPT_NAME = "chumby-cpid"
INITSCRIPT_PARAMS = "defaults 5 95"

PR = "r10"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL} \
          file://chumby-cpid \
"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/src/cpid"
DEPENDS = "beecrypt"
RDEPENDS_${PN} = "beecrypt"

do_compile() {
    ${CC} crypto.c hal.c main.c makePackets.c -o cpid ${CFLAGS} ${LDFLAGS} -lbeecrypt
}

do_install() {
    install -d ${D}/usr/bin
    install -m 0755 cpid ${D}/usr/bin

    install -d ${D}${sysconfdir}/init.d
    install -m 0755 ${WORKDIR}/chumby-cpid ${D}${sysconfdir}/init.d
}
