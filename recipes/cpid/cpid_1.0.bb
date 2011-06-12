inherit chumbysg-git-private
DESCRIPTION = "Cryptoprocessor daemon, used in lieu of a real processor"
LICENSE = "BSD"

#inherit update-rc.d

PR = "r9"

SRC_URI = "${CHUMBYSG_GIT_HOST}/chumby-clone/${PN}${CHUMBYSG_GIT_EXTENSION};protocol=${CHUMBYSG_GIT_PROTOCOL}"
SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git/src/cpid"
DEPENDS = "beecrypt"
RDEPENDS_${PN} = "beecrypt"

do_compile() {
    cd ${S}
    ${CC} crypto.c hal.c main.c makePackets.c -o cpid ${CFLAGS} ${LDFLAGS} -lbeecrypt
}

do_install() {
	install -d ${D}/usr/bin
	install -m 0755 cpid ${D}/usr/bin
}

#INITSCRIPT_NAME = "netv_service"
#INITSCRIPT_PARAMS = "defaults 50 50"
